#ifdef _MULTI_UDPS
//---------------------------------------------------------------------------------------
#include "StdAfx.h"
#include <assert.h>
#include "../~preconfig.h"
#include "netio.h"
#include "handler.h"
#include "sock.h"
#include "server.h"

extern BOOL GlobalExit;

//---------------------------------------------------------------------------------------
BOOL    NetIO::Start(UINT port)
{
    BOOL rv = 1;
    if((CFG_SEPARATETHREADS & Srv_Cfg) == CFG_SEPARATETHREADS)
    {
        _recThread._port = port;
        _recThread.Configure(REC_DATA);
        rv &= _recThread.Start();

        if((CFG_SENDQUEUE & Srv_Cfg) == CFG_SENDQUEUE)
        {
            _sendThread._port = port;
            _sendThread.Configure(SEND_DATA);
            rv &=_sendThread.Start();
        }
    }
    else
    {
        _recThread._port = port;
        if((CFG_SENDQUEUE & Srv_Cfg) == CFG_SENDQUEUE)
            _recThread.Configure(REC_DATA|SEND_DATA);
        else
            _recThread.Configure(REC_DATA);
        rv &=_recThread.Start();
    }
    return _listenThread.Start(port) & rv;
}

//---------------------------------------------------------------------------------------
void    NetIO::Stop()
{
    _listenThread.Stop();
    _recThread.Stop();

    if((CFG_SENDQUEUE & Srv_Cfg) == CFG_SENDQUEUE)
    {
        _sendThread.Stop();
    }
}

//---------------------------------------------------------------------------------------
void ReadWrite::ThreadFoo()
{
    MyPrintf(DL, "->READWRITE::THREADFOO()\r\n");

    int  handlers = 0;

    while(!BStop())
    {
        ge_error(0);
        _delay = 128;

        if(PgServer->HandlersCount())
        {
            this->_PoolData(2);
        }
        else // empty server out messages
        {
            SyncQ&  q = PgServer->_outQueueTCP;
            counted_ptr<Buffer>  pB;
            while(q.Get(&pB));
            MyPrintf(DL,".");
            _delay = 1000;
        }
        PgServer->Count();
        PgServer->FlushDeadHandlers();
        ge_sleep(_delay);
    }
    if(!BStop())
    {
        MyPrintf(EL, "<-READWRITE::THREADFOO() EXCEPTION\r\n");
        GlobalExit=1;
    }

    MyPrintf(DL, "<-READWRITE::THREADFOO()\r\n");

}

//---------------------------------------------------------------------------------------
int ReadWrite::_PoolData(int tout)
{
    timeval  tv         = {0,tout};
    int      handlers   = PgServer->_handlers;

    int      nfds       = _Prepare_FdSets();
    fd_set*  preads     = (REC_DATA & _function)  ? &_rdSet : 0;
    fd_set*	 pwrites    = (SEND_DATA & _function) ? &_wrSet : 0;

    size_t sel =::select(nfds, preads, pwrites, 0, &tv);
    if(sel > 0)
        _Check_FDSets(sel);

    return sel;
}


//---------------------------------------------------------------------------------------
int   ReadWrite::_Check_FDSets(int sel)
{
    Handler*    pH = PgServer->GetHandlers();
    while(pH)
    {
        if((_function & SEND_DATA) )
        {
            if(FD_ISSET(pH->_usock.socket(), &_wrSet))
            {
                if(0 == _CanSend(pH, 0))
                {
                    goto CONTINUE;
                }
            }
            if(FD_ISSET(pH->_sock.socket(), &_wrSet))
            {
                if(0 == _CanSend(pH, 1))
                {
                    goto CONTINUE;
                }
            }

        }        

        if((_function & REC_DATA))
        {
            if(FD_ISSET(pH->_usock.socket(), &_rdSet))
            {
                if(!_Receive(pH, 0))
                {
                    PgServer->DisconnectHandler(pH,"udpreceive");
                }
            }

            if(FD_ISSET(pH->_sock.socket(), &_rdSet))
            {
                if(!_Receive(pH, 1))
                {
                    PgServer->DisconnectHandler(pH, "tcpreceive");
                }
            }

        }
CONTINUE:
        pH = pH->Next();
    }
    return 0;
}

//---------------------------------------------------------------------------------------
int    ReadWrite::_Prepare_FdSets()
{
    if(_function & REC_DATA)        FD_ZERO(&_rdSet);
    if(_function & SEND_DATA)       FD_ZERO(&_wrSet);

    int         nfds = 0;
    Handler*    pH = PgServer->GetHandlers();
    while(pH)
    {
        if(_function & REC_DATA){
            FD_SET(pH->_sock.socket(), &_rdSet);
            FD_SET(pH->_usock.socket(), &_rdSet);
        }


        if(_function & SEND_DATA){
            FD_SET(pH->_sock.socket(), &_wrSet);
            FD_SET(pH->_usock.socket(), &_wrSet);
        }

        nfds = max(nfds, (int)pH->_sock.socket());
        nfds = max(nfds, (int)pH->_usock.socket());
        pH=pH->Next();
    };
	return (++nfds);
}

//---------------------------------------------------------------------------------------
// we may assemble here a huge pB with all data to be sent to client pH
// we can send to this PH handler
BOOL ReadWrite::_CanSend(Handler* pH, BOOL btcp) //pH can send
{
    if(!pH->Flush(btcp))
    {
        PgServer->DisconnectHandler(pH,"cansendflush");
        return 0;
    }
    SyncQ*  pq;
    if(btcp)
        pq = &PgServer->_outQueueTCP;
    else
        pq = &PgServer->_outQueueUDP;
    if(pq->size())
    {
        _delay = 0;
        counted_ptr<Buffer>  pB;
        if(pq->Get(&pB))
        {
            if (pB->GetMsg() == MSG_PING)
            {
                pB->pl._U._Ping.sentTime   = ge_gettick();
            }

            //
            // scan all clients we have to send this buffer
            // remove this handler bit that we are going to send to it
            //
            Handler* pTo;
            while(pTo = PgServer->RemHandlerBit(pB->pl._H.whom))
            {
                // if the current buffer belongs to to this handler
                if(pTo->Uid() == pH->Uid()) // pH has a buffer to send to it
                {
                    if(!pH->SendBuffer(pB, SEND_NOW, btcp))                             
                    {
                        PgServer->DisconnectHandler(pH, "cansend");
                    }
                }
                else    // store the buff in the pTo and when that one is can sent sent it there
                {
                    pTo->SendBuffer(pB, CACHE_IT, btcp);
                }
            }
        }
    }
    return 1;
}

//---------------------------------------------------------------------------------------
BOOL    ReadWrite::_Receive(Handler* pH, BOOL btcp)
{
    BYTE*   pWalkPtr   = _workingBuff;
	int		nPrevcount = 0;
    int&    nPrevBytes = btcp ? pH->_nRecBytesT : pH->_nRecBytesU;
    BYTE*   byPrevBuff = btcp ? pH->_pRecBuffT  : pH->_pRecBuffU;


	if(nPrevBytes) // bring on the last framgmented recevied buffer
	{ 
		::memcpy(pWalkPtr, byPrevBuff, nPrevBytes);
		pWalkPtr    += nPrevBytes;
		nPrevcount  = nPrevBytes;
		nPrevBytes  = 0;
	}
    
    int ncount   = pH->SockReceive(pWalkPtr, REC_BF_SIZE-nPrevcount, btcp);
	pWalkPtr     = _workingBuff; //rebase
    if(ncount == 1)
        return 0;        
    else if(ncount < 0)
        return 1;        

    if(PgServer->IsBlocked())
    {
        return 1;
    }

    _delay = 0;
	ncount += nPrevcount;
    while(ncount)
	{
        if(ncount < sizeof(PL::H))
        {
			nPrevBytes = ncount;
			::memcpy(byPrevBuff, pWalkPtr, ncount);
            return 1;
        }
        PL::H*  pHdr    = (PL::H*)pWalkPtr;
        UINT    blockSZ = pHdr->size + sizeof(PL::H);
        if(blockSZ > sizeof(PL))
        {
            return 0; // wrong buff. cannot be
        }
		if(ncount < blockSZ) // buffer have been fragmented ovet NET
		{
			nPrevBytes = ncount;
			::memcpy(byPrevBuff, pWalkPtr, ncount);
            return 1;
		}

        if(((Buffer*)pWalkPtr)->GetMsg() == MSG_SOCK_PING)
        {
            _Pong(pH, pWalkPtr, blockSZ, btcp);
        }
        else
        {
            counted_ptr<Buffer> pB(new Buffer());
            pB->Transfer(pWalkPtr, blockSZ);
            Handler* pH2  = PgServer->GetHandler(pB->GetClientIndex());
            if(!pH2)
            {
                pH2 = pH;
                pB->SetOwner(pH2->Uid());
            }
            if(pH2)
            {
                PgServer->QueueIn(pB);
            }
        }
        ncount   -= blockSZ;
        pWalkPtr += blockSZ;
    }
    assert (ncount==0);
    return 1; 
}


//---------------------------------------------------------------------------------------
void    ReadWrite::_Pong(Handler* pH, const BYTE* pWalkPtr, int blockSZ, BOOL btcp)
{
    int wtd = PgServer->OnLetDirectPing(pH->Rsin());
    if(wtd==1)
    {
        if(!pH->SockSend(pWalkPtr, blockSZ, btcp))
        {
            PgServer->DisconnectHandler(pH, "pong");
        }
    }
    else if(wtd < 0)
    {
        PgServer->DisconnectHandler(pH, "pong");
        if(wtd==-2)
        {
            PgServer->Ban(pH->Rsin());
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void    Listener::ThreadFoo()
{
    MyPrintf(DL, "->Listener::ThreadFoo()\r\n");

    if(_sock.create(_port) != 0)
    {
         MyPrintf(EL, "CANNOT CREATE SOCKET ON PORT %d. ERROR:%d", _port, _sock.error());
         return;
     }
    _sock.listen(_port);

    while(!BStop())
    {
        PoolConnections(64);
        ge_sleep(64);
    }

    if(!BStop())
    {
        MyPrintf(EL, "<-Listener::ThreadFoo() EXCEPTION\r\n");
        GlobalExit=1;
    }
    MyPrintf(DL, "<-Listener::ThreadFoo()\r\n");
}

//-----------------------------------------------------------------------------
int Listener::Start(int port)
{
    _port = port;
    return OsThread::Start();
}

//-----------------------------------------------------------------------------
void Listener::ServerIsFull()
{
    tcp_cli_sock  s;

    if(_sock.accept(s) > 0)
    {
        Buffer b;
        b.SetOwner(SERVER_ID);
        b.SetMsg(MSG_MESSAGE);
        b.SetString("Server is Full. Please try later on...<br>\r\n");
        s.send((BYTE*)b.Storage(), b.Length());
        ge_sleep(128);
        s.destroy();
    }
}

int Listener::PoolConnections(int toutms)
{
    timeval  tv     = {0, toutms};

    fd_set		rdSet;
    fd_set		wrSet;
    fd_set		exSet;

    ge_error(0);
    FD_ZERO(&rdSet);
    FD_ZERO(&exSet);
    FD_ZERO(&wrSet);
    FD_SET(_sock.socket(), &rdSet);
    FD_SET(_sock.socket(), &exSet);
    int nfds = _sock.socket() + 1;
    int sel  = ::select(nfds, &rdSet, NULL, &exSet, &tv);
    if(sel > 0)
    {
        if( FD_ISSET( _sock.socket(), &rdSet ) )
        {
            Handler* pH = PgServer->OpenHandler();

            if(pH)
            {
                if(_sock.accept(pH->_sock) > 0)
                {
                    pH->_sock.setblocking(1);//

                    AutoLock    (&PgServer->_mutex);

                    if(PgServer->IsBlocked())
                    {
                        Buffer b;

                        b.SetOwner(SERVER_ID);
                        b.SetMsg(MSG_RETRY);
                        pH->_sock.send(b.Storage(), b.Length());
                        ge_sleep(128);
                        PgServer->CloseHandler(pH);
                        MyPrintf(IL, "SEND RERTY TO PLAYER %d \n", pH->Uid());
                    }
                    else
                    {
                        char sa[64];
                        pH->_sock.getsocketaddr(sa);
                        MyPrintf(IL, "NEW CONNECTION FROM: %s\n", sa);

                        if(!PgServer->IsBanned(pH->Rsin()))
                        {
                            pH->OnCreate();
                            if(FALSE == pH->OnConnect(sa))          /// rejected by handler
                            {
                                PgServer->CloseHandler(pH);      // just dstroy it. has not been added to list
                            }
                            else
                            {
                                PgServer->SetHandlerUID(pH);
                            }
                        }
                        else
                            PgServer->CloseHandler(pH);
                    }
                }
                else
                {
                    PgServer->CloseHandler(pH);
                }
            }
            else
            {
                ServerIsFull();
            }
        }
    }
    else if(sel < 0)
    {
        int errror = ge_error();
        MyPrintf(WL, "TCP_SERVER SELECT() ERROR:%d \n",errror);
        return  errror;
    }
    return 0;
}

#endif // !MULTI UDP

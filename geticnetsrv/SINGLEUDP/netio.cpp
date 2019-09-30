#ifndef _MULTI_UDPS
//---------------------------------------------------------------------------------------
#include "../StdAfx.h"
#include <assert.h>
#include "../~preconfig.h"
#include "handler.h"
#include "sock.h"
#include "netio.h"
#include "server.h"

extern BOOL GlobalExit;
//---------------------------------------------------------------------------------------
BOOL    NetIO::Start(UINT port)
{

    BOOL rv = 1;
    if((CFG_SEPARATETHREADS & Srv_Cfg) == CFG_SEPARATETHREADS)
    {
        _recThread._port = (port+1);
        _recThread.Configure(REC_DATA);
        rv &= _recThread.Start();

        if((CFG_SENDQUEUE & Srv_Cfg) == CFG_SENDQUEUE)
        {
            _sendThread._port = port; // send is done on upper port
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
        rv &=!_recThread.Start();
    }
    return !_listenThread.Start(port) & rv;
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
    MyPrintf(DL, "->ReadWrite::ThreadFoo()\r\n");

    int  handlers = 0;
    _recording    = 0;

    if(_usock.create(_port)!=-1)
    {
        _usock.setoption(SO_SNDBUF, 2048);
        _usock.setoption(SO_RCVBUF, 2048);
        _usock.setblocking(1);

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
    }
    if(!BStop())
    {
        MyPrintf(EL, "<-ReadWrite::ThreadFoo() EXCEPTION\r\n");
        GlobalExit=1;
    }

    MyPrintf(DL, "<-ReadWrite::ThreadFoo()\r\n");

}

//---------------------------------------------------------------------------------------
int ReadWrite::_PoolData(int tout)
{
    timeval  tv         = {0,tout};
    int      handlers   = PgServer->_handlers;
    fd_set*  preads     = (REC_DATA & _function)  ? &_rdSet : 0;
    fd_set*	 pwrites    = (SEND_DATA & _function) ? &_wrSet : 0;

    
    // UDP
    _sendsU=0;
    _recsU=0;
    
    _FdZero();
    size_t nfds       = _Prepare_FdSets();
    int sel =::select(nfds, preads, pwrites, 0, &tv);
    if(sel > 0)
    {
        _CheckUDP_FDSets(sel, handlers);
        _CheckTCP_FDSets(sel);
        
    }

    return sel;
}

//---------------------------------------------------------------------------------------
int   ReadWrite::_CheckTCP_FDSets(int sel)
{
    Handler*    pH = PgServer->GetHandlers();
    while(pH){
        if((_function & SEND_DATA) && 
           FD_ISSET(pH->_sock.socket(), &_wrSet))
        {
            if(0 == _TCP_CanSend(pH))
            {
                goto CONTINUE;
            }
        }        
        if((_function & REC_DATA) && 
           FD_ISSET(pH->_sock.socket(), &_rdSet) )
        {
            if(!_ReceiveFrom_TCP(pH))
            {
                PgServer->DisconnectHandler(pH,"RECEIVE TCP");
            }
        }
CONTINUE:
        pH = pH->Next();
    }
    return 0;
}

//---------------------------------------------------------------------------------------
int   ReadWrite::_CheckUDP_FDSets(int sel, int handlers)
{
    if((_function & SEND_DATA) && 
        FD_ISSET(_usock.socket(), &_wrSet))
    {
        int ql = _UDP_CanSend(0);
        while(ql > 128)
            ql = _UDP_CanSend(0);
    }

    if((_function & REC_DATA) && 
        FD_ISSET(_usock.socket(), &_rdSet) )
    {
        while(--handlers > -1 && _ReceiveFrom_UDP(0))
        {
            ;//nothing
        }
    }
    return 1;
}

//---------------------------------------------------------------------------------------
void   ReadWrite::_FdZero()
{
    if(_function & REC_DATA)
        FD_ZERO(&_rdSet);
    if(_function & SEND_DATA)
        FD_ZERO(&_wrSet);

}

//---------------------------------------------------------------------------------------
int    ReadWrite::_Prepare_FdSets()
{
    int         nfds = 0;
    Handler*    pH = PgServer->GetHandlers();
    while(pH){
        if(_function & REC_DATA)
            FD_SET(pH->_sock.socket(), &_rdSet);
        if(_function & SEND_DATA)
            FD_SET(pH->_sock.socket(), &_wrSet);
        nfds = max(nfds, (int)pH->_sock.socket());
        pH=pH->Next();
    };

    if(_function & REC_DATA)
    {
        FD_SET(_usock.socket(), &_rdSet);// udp
    }

    if(_function & SEND_DATA)
    {
        FD_SET(_usock.socket(), &_wrSet);
    }
    nfds = max(nfds, (int)_usock.socket());

	return (++nfds);
}

//---------------------------------------------------------------------------------------
// we may assemble here a huge pB with all data to be sent to client pH
// we can send to this PH handler
BOOL ReadWrite::_TCP_CanSend(Handler* pH) //pH can send
{
    SyncQ&  q  = PgServer->_outQueueTCP;

    if(q.size()) // out queue
    {
        _delay = 0;
        counted_ptr<Buffer>  pB;

        if(q.Get(&pB))
        {
            _PreProcMsg(pB, 0);//


            // scan all clients we have to send this buffer
            // remove this handler bit that we are going to send to it
            Handler* pTo;
            while(pTo = PgServer->RemHandlerBit(pB->pl._H.whom))
            {
                // if the current buffer belongs to to this handler
                if(pTo->Uid() == pH->Uid()) // pH has a buffer to send to it
                {
                    if(!pH->TCP_SendBuffer(pB, SEND_NOW))                             
                    {
                        PgServer->DisconnectHandler(pH, "SEND TCP");
                    }
                }
                else    // store the buff in the pTo and when that one is can sent sent it there
                {
                    pTo->TCP_SendBuffer(pB, CACHE_IT);
                }
            }
        }
    }
    if(!pH->TCP_Flush())
    {
        PgServer->DisconnectHandler(pH ,"TCP SEND");
    }
    return 1;
}

//---------------------------------------------------------------------------------------
// send all udp to all
int ReadWrite::_UDP_CanSend(int nsock)
{
    SyncQ&      q = PgServer->_outQueueUDP;
    Handler*    pTo;
    int         nWaterMark = 0;

    //only one udp socket send to any
    size_t qsz = q.size();
    if(qsz)
    {

#ifdef _DEBUG
    if(qsz > 50)
        printf("UDP Q: > %d \n", qsz);
#endif //

        _delay = 0;
        counted_ptr<Buffer> pB;
        if(q.Get(&pB))
        {
            // makes no sense to accumulate and send later
            // couse we send one block at a time
            _PreProcMsg(pB, 0);//
        
            // test bit where to send and send it
            while((pTo = PgServer->RemHandlerBit(pB->pl._H.whom)))
            {
                if(pTo->IsConnected())
                {
                    // see prev comment. we force the send
                    if(!pTo->UDP_SendBuffer(pB, _usock, SEND_NOW))
                    {
                        PgServer->DisconnectHandler(pTo,"UDP SEND");
                    }
                }
            }
            ++_sendsU;
        }
    }
    return 1;
}

//---------------------------------------------------------------------------------------


BOOL    ReadWrite::_ReceiveFrom_TCP(Handler* pH)
{
    BYTE*   pWalkPtr   = _maxBuffUdp;
	int		nPrevcount = 0;
	if(pH->_nRecBytes)// bring on the last framgmented recevied buffer
	{ 
		::memcpy(pWalkPtr, pH->_pRecBuff, pH->_nRecBytes);
		pWalkPtr    += pH->_nRecBytes;
		nPrevcount  = pH->_nRecBytes;
		pH->_nRecBytes = 0;
	}
    int ncount   = pH->_sock.receive(pWalkPtr, REC_BF_SIZE-nPrevcount);
	pWalkPtr     = _maxBuffUdp;
    if(ncount < 1)
    {
        const int& err = ge_error();
        if(ncount==0 || IS_SOCKK_ERR(err))
        {
            return 0;        
        }
        if(PgServer->IsBlocked())
        {
            return 1;
        }
    }
    _delay = 0;
	ncount += nPrevcount;
    while(ncount)
	{
        if(ncount < sizeof(PL::H))
        {
			pH->_nRecBytes = ncount;
			::memcpy(pH->_pRecBuff, pWalkPtr, ncount);
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
			pH->_nRecBytes = ncount;
			::memcpy(pH->_pRecBuff, pWalkPtr, ncount);
            return 1;
		}

        if(!_PreProcMsgByte(pWalkPtr, blockSZ, 1, pH))
        {
            counted_ptr<Buffer> pB(new Buffer());
            pB->Transfer(pWalkPtr, blockSZ);
            Handler* pH2  = PgServer->GetHandler(pB->GetClientIndex());
            if(!pH2)
            {
                pH2 = pH;
                pB->SetOwner(pH2->Uid());
            }
            else //if(pH2)
            {
                ////::memcpy(&pH->_remote_usin, &_usock.Rsin(), sizeof(pH->_remote_usin));
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
int    ReadWrite::_ReceiveFrom_UDP(int nsock)
{
    BYTE*   pWalkPtr = _maxBuffUdp;

    int     ncount = _usock.receive(pWalkPtr, REC_BF_SIZE);
    if(PgServer->IsBlocked())
        return -1;
    if(ncount > sizeof(PL) || ncount < sizeof(PL::H)) // protect aganst spoof
    {
        PgServer->PreBan(_usock.Rsin()); // pinging from unknown
        return 0;
    }
    _delay = 0;    
    ++_recsU;
    
    while(ncount > 0)
    {
        UINT    blockSZ = ((Buffer*)pWalkPtr)->Length();

        if(ncount < blockSZ)
        {
            PgServer->PreBan(_usock.Rsin()); // pinging from unknown
            assert(0); // incomplette buffer
            break;//
        }

        if(!_PreProcMsgByte(pWalkPtr, blockSZ, 1, 0))
        {
            counted_ptr<Buffer> pB(new Buffer());
            pB->Transfer(pWalkPtr, blockSZ);
            Handler* pH  = PgServer->GetHandler(pB->GetClientUid());
            if(pH )
            {
                pH->_st_bytesInUdp += ncount;
                ::memcpy(&pH->_remote_usin, &_usock.Rsin(), sizeof(pH->_remote_usin));
                PgServer->QueueIn(pB);
            }
            /*
            else
            {
                PgServer->PreBan(_usock.Rsin()); // from whatever
            }
            */
        }
        ncount  -= blockSZ;
        pWalkPtr+= blockSZ;
    }
    if(ncount!=0) // cant receive invalid blocks
    {
        PgServer->PreBan(_usock.Rsin()); // pinging from unknown
    }
    return 1;
}

//---------------------------------------------------------------------------------------
void    ReadWrite::_UDP_Pong(const sockaddr_in& rsin, const BYTE* pWalkPtr, int blockSZ)
{
    int wtd = PgServer->OnLetDirectPing(rsin);
    if(wtd==1)
    {
        _usock.send(pWalkPtr, blockSZ);
    }
    else if(wtd ==-2)
    {
        PgServer->Ban(_usock.Rsin());
    }
}

//---------------------------------------------------------------------------------------
void    ReadWrite::_TCP_Pong(Handler* pH, const BYTE* pWalkPtr, int blockSZ)
{
    int wtd = PgServer->OnLetDirectPing(pH->Rsin());
    if(wtd==1)
    {
        if(!pH->TSockSend(pWalkPtr, blockSZ))
        {
            PgServer->DisconnectHandler(pH, "TCP SEND");
        }
    }
    else if(wtd < 0)
    {
        PgServer->DisconnectHandler(pH, "SOCK PING");
        if(wtd==-2)
        {
            PgServer->Ban(pH->Rsin());
        }
    }
}

//-----------------------------------------------------------------------------
BOOL    ReadWrite::_PreProcMsgByte(BYTE* pMaxBuff, int blockSZ, BOOL isIn, Handler* pH)
{
    if(blockSZ >= sizeof(PL::H))
    {
        Buffer* pb = (Buffer*)pMaxBuff;
        switch(pb->GetMsg())
        {
            case MSG_PING:
                if(isIn==0)
                    pb->pl._U._Ping.sentTime   = ge_gettick();
                else
                    pb->pl._U._Ping.inTime     = ge_gettick();
                return 0;
            case MSG_SOCK_PING:
                MyPrintf(DL, "<pong T>\r\n");
                if(pb->IsTCP() && pH)
                    _TCP_Pong(pH, pb->Storage(), pb->Length());
                else
                    _UDP_Pong(_usock.Rsin(), pb->Storage(), pb->Length());
                return 1;
            case MSG_RECORD:
                return 0;
        }
    }
    return 0; //not processed
}

//-----------------------------------------------------------------------------
BOOL    ReadWrite::_PreProcMsg(counted_ptr<Buffer>& rB, BOOL isIn)
{
    switch(rB->GetMsg())
    {
        case MSG_PING:
            if(isIn==0)
                rB->pl._U._Ping.sentTime   = ge_gettick();
            else
                rB->pl._U._Ping.inTime     = ge_gettick();
            return 0;
        case MSG_RECORD:
            return 0;
    }
    return 0;
}


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


#endif //MULTIUDP

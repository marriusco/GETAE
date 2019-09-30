#ifdef _MULTI_UDPS
#include "StdAfx.h"
#include <assert.h>
#include "../pkbuffer.h"
#include "handler.h"
#include "server.h"

//-----------------------------------------------------------------------------
void    Handler::Clean()
{
    _seqS           = 0;
    _nRecBytesU  = 0;
    _nRecBytesT  = 0;
    _pNext          = 0;
    _errors         = 0;
    _closeCount     = 0;  
    _iid            = 0;
    _pNext          = 0;
    _st_bytesInTcp  = 0;
    _st_bytesOutTcp = 0;
    _st_bytesInUdp  = 0;
    _st_bytesOutUdp = 0;
    _queueTCP.clear();
    _queueUDP.clear();
    _espectedSeq   = Seq::GetSeq(_seqS);

}

//-----------------------------------------------------------------------------
BOOL    Handler::OnConnect(const char* addr)
{
    MyPrintf(DL, "STARTING UDP ON PORT %d \n", PgServer->ip_localIp.n_port+_iid);
    
    ::memcpy(&_remote_usin, &_sock.Rsin(), sizeof(_remote_usin));

    if(0 ==_usock.create(PgServer->ip_localIp.n_port+_iid))
    {
        _usock.setoption(SOL_SOCKET, SO_SNDBUF, sizeof(PL)*8);
        _usock.setoption(SOL_SOCKET, SO_RCVBUF, sizeof(PL)*8);
        return 1;
    }
    return 0;
    //return 0==_usock.connect(addr, PgServer->ip_localIp.n_port+_iid);
}

//-----------------------------------------------------------------------------
void    Handler::Disconnect()
{
    MyPrintf(DL, "CLOSSING SOCKETS (TCP/UDP)\r\n");
    this->Clean();
    this->_usock.destroy();
    this->_sock.destroy();
}

//-----------------------------------------------------------------------------
const BOOL    Handler::SendBuffer(const counted_ptr<Buffer>& pB, BOOL when, BOOL btcp)
{
    _seqS = ge_gettick();
    pB->Sequence(_seqS);
    _espectedSeq = Seq::GetSeq(_seqS);
    if(when == SEND_NOW)
    {
        return this->SockSend(pB->Storage(), pB->Length(), btcp);
    }
    if(btcp == 1)
        this->_queueTCP.push_back(pB);
    else 
        this->_queueUDP.push_back(pB);
    return 1;
}

//--------------------------------------------------------------------------------------
const BOOL Handler::Flush(BOOL btcp)
{
    vector<counted_ptr<Buffer> >* plocoQ;
    if(btcp )
        plocoQ = &this->_queueTCP;
    else 
        plocoQ = &this->_queueUDP;
        
    int rv = 1;
    vector<counted_ptr<Buffer> >::iterator ppb = this->_queueTCP.begin();
    for(;ppb!=this->_queueTCP.end();++ppb)
    {
        rv = this->SockSend((*ppb)->Storage(), (*ppb)->Length(), btcp);
        if(!rv)
            break;
    }
    if(btcp)
        this->_queueTCP.clear();
    else 
        this->_queueUDP.clear();
    return rv;
}


//--------------------------------------------------------------------------------------
const int    Handler::SockReceive(BYTE* pb, int len, BOOL btcp)
{
    int snd   = this->NetSock(btcp)->receive(pb, len, _remote_usin);
    if(snd < 1)
    {
        const int& err = ge_error();
        if(snd==0 || IS_SOCKK_ERR(err))
        {
            return 0;        
        }
    }
    if(btcp)
        _st_bytesInTcp += snd;
    else
        _st_bytesInUdp += snd;
    return snd;
}

//--------------------------------------------------------------------------------------
const BOOL    Handler::SockSend(const BYTE* pb, const int len, BOOL btcp)
{
    
    int snd = this->NetSock(btcp)->send(pb, len, _remote_usin);
    if(snd<=0)
    {
        const int& err = this->NetSock(btcp)->error();
        if( IS_SOCKK_ERR(err))
        {
            this->OnDisconnect();
            return 0;
        }
        MyPrintf(WL, "TCP SEND ERROR:%d\r\n",this->NetSock(btcp)->error());
        return 0;
    }
    if(btcp )
        _st_bytesOutTcp += snd;
    else
        _st_bytesOutUdp += snd;
    return 1;
}

//--------------------------------------------------------------------------------------
const BOOL    Handler::AcceptMessage(const counted_ptr<Buffer>& pB)
{
    return 1; 
    /**
    if(pB->Sequence() == _espectedSeq)
    {
        return 1;
    }
    AddError();
    return 0;
    */
}

//--------------------------------------------------------------------------------------
sock* Handler::NetSock(BOOL tcp)
{
    if(tcp == 1 )
        return( sock*)&_sock;
    else 
        return (sock*)&_usock;
}

#endif // !MULTI UDP
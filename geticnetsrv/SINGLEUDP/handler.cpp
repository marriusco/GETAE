#ifndef _MULTI_UDPS

#include "../StdAfx.h"
#include <assert.h>
#include "../pkbuffer.h"
#include "handler.h"
#include "server.h"

//-----------------------------------------------------------------------------
void    Handler::Clean()
{
    _seqS           = 0;
    _nRecBytes      = 0;
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
    _espectedSeq   = Seq::GetSeq(_seqS);

}

//-----------------------------------------------------------------------------
BOOL    Handler::OnConnect(const char* addr)
{
    return 1;
}


//-----------------------------------------------------------------------------
void    Handler::Disconnect()
{
    this->Clean();
    this->_sock.destroy();
}

//-----------------------------------------------------------------------------
const BOOL    Handler::TCP_SendBuffer(const counted_ptr<Buffer>& pB, BOOL when)
{
    _seqS = ge_gettick();
    pB->Sequence(_seqS);
    _espectedSeq = Seq::GetSeq(_seqS);


    if(when == SEND_NOW)
    {
        return this->TSockSend(pB->Storage(), pB->Length());
    }
    this->_queueTCP.push_back(pB);
    return 1;
}

//-----------------------------------------------------------------------------
const BOOL    Handler::UDP_SendBuffer(const counted_ptr<Buffer>& pB, 
                                      const udp_server_sock& us, DWORD when)
{
    BOOL    rv = 1;
    return USockSend(us, pB->Storage(), pB->Length(), this->_remote_usin);
}

//--------------------------------------------------------------------------------------
const BOOL Handler::TCP_Flush()
{
    int rv = 1;
    // see if has something queued
    vector<counted_ptr<Buffer> >::iterator ppb = this->_queueTCP.begin();
    for(;ppb!=this->_queueTCP.end();++ppb)
    {
        rv = this->TSockSend((*ppb)->Storage(), (*ppb)->Length());
        if(!rv)
            break;
    }
    this->_queueTCP.clear();
    return rv;
}

//--------------------------------------------------------------------------------------
const BOOL    Handler::USockSend(const udp_server_sock& us,
                                 const BYTE* pb, const int len, 
                                 const sockaddr_in& rsin)
{
    int snd = ((udp_server_sock&)us).send(pb, len, rsin );
    ge_sleep(1);
    if(snd <=0 )
    {
        const int& err = this->_sock.error();
        if( IS_SOCKK_ERR(err))
        {
            this->OnDisconnect();
            return 0;
        }
    }
    _st_bytesOutUdp += snd;
    return snd;
}

//--------------------------------------------------------------------------------------
const BOOL    Handler::TSockSend(const BYTE* pb, const int len)
{
    int snd = this->_sock.send(pb, len);
    ge_sleep(1);
    if(snd<=0)
    {
        const int& err = this->_sock.error();
        if( IS_SOCKK_ERR(err))
        {
            this->OnDisconnect();
            return 0;
        }
        MyPrintf(WL, "TCP SEND ERROR:%d\r\n",this->_sock.error());
        return 0;
    }
    _st_bytesOutTcp += snd;
    return snd;
}

//--------------------------------------------------------------------------------------
const BOOL    Handler::AcceptMessage(const counted_ptr<Buffer>& pB)
{
    return 1; // not yet 
    if(pB->Sequence() == _espectedSeq)
    {
        return 1;
    }
    AddError();
    return 0;
}

//--------------------------------------------------------------------------------------
const   BOOL    Handler::Ping()
{
    Buffer      b;

    b.SetOwner(SERVER_ID);
    b.SetMsg(MSG_SOCK_PING);
    b.pl._U._Ping.time = ge_gettick();
    return TSockSend(b.Storage(), b.Length());

}

#endif //MULTIUDP

#ifndef _MULTI_UDPS
// game_server.cpp: implementation of the game_server class.
//-----------------------------------------------------------------------------
#include "../StdAfx.h"
#include "sock.h"
#include "server.h"
//-----------------------------------------------------------------------------
server*             PgServer = 0;
PfnEventHandler     server::_pFHs[MSG_LAST] = {0};

//-----------------------------------------------------------------------------
server::server():_outQueueTCP("tcp-out"),_outQueueUDP("udp-out"),_inQueue("in_que")
{
    _handlers = 0;
    PgServer=this;
    for(int i=0 ; i < MSG_LAST;  i++)
        _pFHs[i] = &server::DefaultHandlerFoo;
}

//-----------------------------------------------------------------------------
BOOL    server::Start(int port, int threads)
{
    BOOL rv = 1;
    _state = SRV_STATE_UNDET;
    

    if(AllocateHandlers(MAX_CLIENTS))
    {
        rv &= _procUnit.Start(threads);
        rv &= _ioUnit.Start(port);
    }
    return rv;//return 1 in case of success
}

//-----------------------------------------------------------------------------
void     server::Stop()
{
    _ioUnit.Stop();

    char i = MAX_CLIENTS;
    while(--i>=0)
    {
        if(_clients[i]->IsConnected())
        {
            _clients[i]->Disconnect();
        }
        delete _clients[i];
        _clients[i] = 0;
    }
    _procUnit.Stop(_inQueue);
}


//-----------------------------------------------------------------------------
void server::PostMessage(SyncQ& q, DWORD to, DWORD msg, DWORD owner, counted_ptr<Buffer>& b)
{
    b->SetSendTo(to);
    b->SetMsg(msg);
    b->SetOwner(owner);
    q.Put(b);
}

//-----------------------------------------------------------------------------
Handler* server::RemHandlerBit(DWORD& bw)
{
    for(int i=0; i < MAX_CLIENTS; i++)
    {
        DWORD mask = 1 << i;    // bit index
        if(bw & mask)
        {
            bw &= ~mask;    //remove the bit
            if(_clients[i]->IsConnected())
            {
                return _clients[i];
            }
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
Handler* server::GetHandler(DWORD uid, BOOL force)
{
    if(uid < MAX_CLIENTS)
    {
        if(force || _clients[uid]->IsActive() )
        {
            return _clients[uid]->Uid() == uid ? _clients[uid] : 0;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
Handler* server::GetHandlers()
{
    AutoLock    l(&_mutex);

    Handler*    ph = 0;
    long        i  = MAX_CLIENTS;
    while(--i>=0)
    {
        if(_clients[i]->IsConnected() && 0 == _clients[i]->ToClose())
        {
            if(ph==0)
                ph=_clients[i];
            else
                ph->AddNext(_clients[i]);
        }
    }
    return ph;
}

//-----------------------------------------------------------------------------
Handler* server::OpenHandler()
{
    long i = MAX_CLIENTS;
    while(--i >= 0) // spot 0 is reserved
    {
        if(_clients[i]->IsActive()==0)
        {
            _clients[i]->Uid(i);
            ++_handlers;
            return _clients[i];
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
void    server::SetHandlerUID(Handler* pH)
{
    counted_ptr<Buffer> pB(new Buffer());

    pB->pl._H.reserved = 0;
    pB->SetMsg(MSG_SET_ID); // by the id use a udp port between 1-4
    pB->SendTo(pH->Bit());
    pB->SetOwner(pH->Uid());
    SendBuffer(pB);
    MyPrintf(DL, "NEW PLAYER. SETING ID %d     PLAYERS COUNT: %d\n", pH->Uid(), _handlers);
}


//-----------------------------------------------------------------------------
void   server::CloseHandler(Handler* pHin)
{
    AutoLock l(&_mutex);
    --_handlers;
    MyPrintf(DL, "\nOUT: PLAYER %d REMOVED.     PLAYERS COUNT: %d\n", pHin->Uid(), _handlers);

    pHin->Disconnect();
} 

//-----------------------------------------------------------------------------
void    server::QueueIn(const counted_ptr<Buffer>& rB)
{
    if ( rB->GetMsg() == MSG_PING)
    {
        DWORD ct = ge_gettick();
        rB->pl._U._Ping.inTime  = ct;
    }

    this->_inQueue.Put(rB);
}

//-----------------------------------------------------------------------------
void     server::SendBuffer(counted_ptr<Buffer>& pB)
{
    if((Srv_Cfg & CFG_SENDQUEUE) == CFG_SENDQUEUE)
    {
        if(pB->pl._H.istcp)
            _outQueueTCP.Put(pB);
        else
            _outQueueUDP.Put(pB);

        return;
    }

    AutoLock l(&_mutex);

    if (pB->GetMsg() == MSG_PING)
    {
        pB->pl._U._Ping.sentTime   = pB->pl._U._Ping.simOutTime;
    }

    Handler* pTo;
    while(pTo  = this->RemHandlerBit(pB->pl._H.whom))
    {
        if(pB->pl._H.istcp)
        {
            // there are no flush
            if(!pTo->TCP_SendBuffer(pB, SEND_NOW))
            {
                this->DisconnectHandler(pTo, "TCP SEND");
                continue;
            }
        }
        else
        {
            if(!pTo->UDP_SendBuffer(pB, _ioUnit.Udp(), SEND_NOW))
            {
                this->DisconnectHandler(pTo,"UDP SEND");
                continue;
            }
        }
    }
}

//-----------------------------------------------------------------------------
void   server::PreBan(const sockaddr_in& rsin)
{
}

//-----------------------------------------------------------------------------
void   server::Ban(const sockaddr_in& rsin)
{
}


//-----------------------------------------------------------------------------
BOOL    server::IsBanned(const sockaddr_in& rsin)
{
    return FALSE;
}

//-----------------------------------------------------------------------------
void    server::DisconnectHandler(Handler* pH, const char *reason)
{
    if(0==pH->ToClose())
    {
        MyPrintf(DL, "\nIN: CLOSEHANDLER %d REASON: %s\n", pH->Uid(), reason);
        pH->PreClose();
        pH->OnDisconnect();
        counted_ptr<Buffer> pB(new Buffer);
        PostMessage(_inQueue, ALL_CLIS, MSG_LEAVE, pH->Uid(), pB);
    }
}

//-----------------------------------------------------------------------------
void    server::CheckAliveHandlers()
{
    AutoLock l(&_mutex);
    for(int i=0; i < MAX_CLIENTS; i++)
    {
        Handler* pH = _clients[i];
        if(pH->IsConnected())
        {
            // ping here the socket layer
            if(!pH->Ping())
            {
                DisconnectHandler(pH, "CHECK ALIVE");
            }
        }
    }
}

//-----------------------------------------------------------------------------
void    server::FlushDeadHandlers()
{
    AutoLock l(&_mutex);
    for(int i=0; i < MAX_CLIENTS; i++)
    {
        Handler* pH = _clients[i];
        if(pH->ToClose() > 1)
        {
            CloseHandler(pH);
        }
        if(pH->GetErrors() > ERR_TRESHHOLD)
        {
            if(pH->IsConnected())
            {
                this->Ban(pH->Rsin());
                this->DisconnectHandler(pH, "FLUSH");
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void    server::_SendMsg(DWORD to, DWORD from, UINT msg, SpBuffer& pB, BYTE tcpip)
{
    pB->SetMsg(msg,tcpip);
    pB->SetSendTo(to);
    pB->SetOwner(from);
    SendBuffer(pB);

}

//---------------------------------------------------------------------------------------
void    server::_SendSrvMsg(UINT msg, DWORD to, SpBuffer& pB, BYTE tcpip)
{
    pB->SetMsg(msg,tcpip);
    pB->SetSendTo(to);
    pB->SetOwner(SERVER_ID);
    SendBuffer(pB);
}

//---------------------------------------------------------------------------------------
void    server::_SendSrvMsg(UINT msg, DWORD to, BYTE tcpip)
{
    SpBuffer pB(new Buffer());

    pB->SetMsg(msg,tcpip);
    pB->SetSendTo(to);
    pB->SetOwner(SERVER_ID);
    SendBuffer(pB);
}



#endif //MULTIUDP

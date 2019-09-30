//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
// NetClient.cpp: implementation of the NetClient class.
//
//---------------------------------------------------------------------------------------

#include "netclient.h"
#include "system.h"

//---------------------------------------------------------------------------------------
NetClient::NetClient()
{
    _cliID     = -1;
    _sentTCP   = 0;
    b_bloking  = 1;
    _lastRec   = GetTickCount();
    _toglePing = 1;
    _alive    = 0;
}


//---------------------------------------------------------------------------------------
NetClient::~NetClient()
{
    _sock.destroy();
    _usock.destroy();
}

//---------------------------------------------------------------------------------------
BOOL     NetClient::Spin(SystemData* dt)
{
    if(!_alive) return 0;
    int nfds = 0;
    if(nfds = _PrepareFdSets())
    {
        timeval  tv   = {0,10};
        int sel       = ::select(nfds, &_rdSet, &_wrSet, &_exSet, &tv);
        if(sel > 0)
        {
            int error = 0;
            if(FD_ISSET(_usock.socket(), &_wrSet))
            {
                if(send_U.Ready())
                {
                    error = _usock.send(send_U.Storage(),send_U.Length(), _usock.Rsin());
                    send_U.Reset();
                    if(-1==error)
                    {
                        if(_ProcessError(_usock.error()))
                            return 0;
                    }
                }
            }

            if(FD_ISSET(_usock.socket(), &_rdSet) )
            {
                error = _usock.receive(rec_U,sizeof(Buffer)*8, _usock.Rsin());
                if(-1==error)
                {
                    if(_ProcessError(_usock.error()))
                        return 0;
                }
                else
                    _ProcessRecieve(rec_U, error);
            }

            if (FD_ISSET(_usock.socket(), &_exSet))
            {
                _ProcessError(-1);
                 return 0;
            }

            if(FD_ISSET(_sock.socket(), &_wrSet))
            {
                if(send_T.Ready())
                {
                    error = _sock.send(send_T.Storage(),send_T.Length());
                    send_T.Reset();
                    if(-1==error)
                    {
                        if(_ProcessError(_usock.error()))
                            return 0;
                    }
                    _sentTCP = 1;
                }
            }

            if(FD_ISSET(_sock.socket(), &_rdSet) )
            {
                error = _sock.receive(rec_T, sizeof(Buffer)*8);
                if(-1==error)
                {
                    if(_ProcessError(_usock.error()))
                        return 0;
                }
                else
                    _ProcessRecieve(rec_T, error);
            }

            if (FD_ISSET(_sock.socket(), &_exSet))
            {
                TRACEX("TCP exception on socket\r\n");
                _ProcessError(-1);
                 return 0;
            }
        }
        else
        {
            _lerror = ::ge_error();
            _ProcessError(_lerror);
        }
    }

    if(GetTickCount() - _lastRec  > 15000)
    {
        _lastRec = GetTickCount();
        _Ping();
    }

    return 1;
}

//---------------------------------------------------------------------------------------
int    NetClient::_PrepareFdSets()
{
	int         nfds	= 0;		// num of ds'es

    FD_ZERO(&_rdSet);
    FD_ZERO(&_exSet);
    FD_ZERO(&_wrSet);
    
    FD_SET(_usock.socket(), &_rdSet);// udp
    FD_SET(_usock.socket(), &_wrSet);
    FD_SET(_usock.socket(), &_exSet);
    nfds = max(nfds, (int)_usock.socket()); 

    FD_SET(_sock.socket(), &_rdSet);
    FD_SET(_sock.socket(), &_exSet);
    FD_SET(_sock.socket(), &_wrSet);
    nfds = max(nfds, (int)_sock.socket()); 
    
	return (++nfds);
}

//---------------------------------------------------------------------------------------
int    NetClient::_ProcessError(const int& error)
{
    _alive = 0;
    _sock.destroy();
    _usock.destroy();
    PSystem->SendMessage(SYS_NET_CLOSED, error, 0);
    return 1;
}

//---------------------------------------------------------------------------------------
void   NetClient::_ProcessRecieve(const BYTE* pRecBuff, const int& recbytes)
{
    _lastRec  = GetTickCount();
    int     rest  = recbytes;
    BYTE*   pBuff = (BYTE*)pRecBuff;
    while(rest > 0)
    {
        Buffer* pB = (Buffer*)pBuff;
        pBuff += sizeof(PL::H) + pB->pl._H.size;
        rest  -= sizeof(PL::H) + pB->pl._H.size;
        PSystem->_NPDispatch(pB);
    }
}

//---------------------------------------------------------------------------------------
BOOL     NetClient::SetUniqueID(DWORD id)
{
    _cliID = id;
    return 1;
}

//---------------------------------------------------------------------------------------
void     NetClient::SetProxy(const char* proxy, int port)
{
    _proxyip    = proxy;
    _proxyport  = port;
}

//---------------------------------------------------------------------------------------
int     NetClient::Reconnect()
{
    return Connect(_ip.c_str(), _port);
}

//---------------------------------------------------------------------------------------
int     NetClient::Connect(const char* ip, int port)
{
    _alive = 0;
    _port  = port;
    _ip    = ip;

   if(NO_ERROR ==_sock.connect(ip, port) && 
      NO_ERROR ==_usock.connect(ip, port))
   { 
        _usock.setblocking(1);
        _sock.setblocking(1);
        _alive = 1;
        PSystem->SendMessage(SYS_NET_OPENED,1,0);
         
   }
   return _alive==1;
}

//---------------------------------------------------------------------------------------
void     NetClient::Join()
{
    send_T.SetOwner(_cliID);
    send_T.SetMsg(MSG_JOIN, true);
    ::strcpy(send_T.pl._U._Join.name, _playername);
    send_T.SetSendTo(SERVER_ID);

    _Ping();
    _Ping();
    
}

//---------------------------------------------------------------------------------------
void     NetClient::Leave()
{
    if(!_alive) return;
    send_T.SetMsg(MSG_LEAVE, true);
    send_T.SetOwner(_cliID);
    send_T.SetSendTo(SERVER_ID);
    _sentTCP = 0;
    while(!_sentTCP)
    {
        if(!Spin(0))
        {
            break;
        }
    }
    Disconnect();
}

//---------------------------------------------------------------------------------------
void    NetClient::Disconnect()
{
    _usock.destroy();
    _sock.destroy();
    nrec_U=0;
    nrec_T=0;
    _alive=0;
}

//---------------------------------------------------------------------------------------
void    NetClient::SendPosition(const V3 &pos, const V3 &euler, int leaf)
{
    if(!_alive)        return;

    send_U.SetOwner(_cliID);
    send_U.SetSendTo(SERVER_ID);

    send_U.pl._U._Position.euler[0]  = euler.x; 
    send_U.pl._U._Position.euler[1]  = euler.y; 
    send_U.pl._U._Position.euler[2]  = euler.z; 

    send_U.pl._U._Position.pos[0]  = pos.x;
    send_U.pl._U._Position.pos[1]  = pos.y;
    send_U.pl._U._Position.pos[2]  = pos.z;
    send_U.pl._U._Position.leaf    = leaf;
    send_U.SetMsg(MSG_POSCHANGED, false);

    if( !memcmp(&_cachedPls[MSG_POSCHANGED], &send_U.pl, send_U.Length()) )
    {
        send_U.Reset(); //same buffer dont send
    }
    else
    {
        memcpy(&_cachedPls[MSG_POSCHANGED], &send_U.pl, send_U.Length());
        _positions.Put((V3&)pos);
    }
}


//---------------------------------------------------------------------------------------
void    NetClient::SendStartPos(const Pos& o, int nleaf)
{
    if(!_alive)        return;

    send_T.SetOwner(_cliID);
    send_T.SetSendTo(SERVER_ID);
    send_T.pl._U._Play.pos[0]   = o._pos.x;
    send_T.pl._U._Play.pos[1]   = o._pos.y;
    send_T.pl._U._Play.pos[2]   = o._pos.z;

    send_T.pl._U._Play.euler[0] = o._euler.x;
    send_T.pl._U._Play.euler[1] = o._euler.y;
    send_T.pl._U._Play.euler[2] = o._euler.z;

    send_T.pl._U._Play.leaf     = nleaf;
    send_T.SetMsg(MSG_PLAY, true);

}

//---------------------------------------------------------------------------------------
void    NetClient::Reset()
{
    send_U.Reset();
    send_T.Reset();
}

//---------------------------------------------------------------------------------------
BOOL   NetClient::_Ping()
{
    return 1;//
    int rv = 0;

    if(_toglePing)
    {
        send_U.SetOwner(_cliID);
        send_U.SetMsg(MSG_PING, false);
        send_U.pl._U._Ping.time = ge_gettick();

        rv = _sock.send(send_T.Storage(),send_T.Length());
        send_T.Reset();
        rv = _usock.send(send_T.Storage(),send_T.Length(), _usock.Rsin());
    }
    else
    {
        send_T.SetOwner(_cliID);
        send_T.SetMsg(MSG_PING, true);
        send_T.pl._U._Ping.time = ge_gettick();
        rv = _sock.send(send_T.Storage(),send_T.Length());
        send_T.Reset();
    }
    
    _toglePing=!_toglePing;
    return rv>0;
}

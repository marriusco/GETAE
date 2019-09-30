//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __NETCLI_H__
#define __NETCLI_H__

#include "sock.h"
#include "_isystem.h"
#include "basecont.h"
#include "../geticnetsrv/pkbuffer.h"
#define MAX_UDPS    4

class NetClient
{
public:
	NetClient();
	virtual ~NetClient();
    BOOL     Spin(SystemData* dt);
    void     Disconnect();
    int      Connect(const char* ip, int port);
    int      Reconnect();
    void     SetProxy(const char* proxy, int port);
    BOOL     SetUniqueID(DWORD id);
    DWORD    GetID(){return _cliID;}
    void     SetPlayerName(const char* pname){::strcpy(_playername, pname);}
    const char* PlayerName()const{return _playername;}
    void    SendPosition(const V3& pos, const V3 &euler, int leaf);
    void    Join();
    void    Leave();
    void    SendStartPos(const Pos& o, int nleaf);
    void    Reset();
    DWORD   Id(){return _cliID;};
    void    SetBlock(BOOL b){
        b_bloking = b;
        _sock.setblocking(b);
        _usock.setblocking(b);
    }
private:
    int     _PrepareFdSets();
    int     _ProcessError(const int& error);
    void    _ProcessRecieve(const BYTE* pRecBuff, const int& recbytes);
    BOOL    _Ping();

private:
    BOOL            b_bloking;
    Buffer          send_T;
    Buffer          send_U;

    BYTE            rec_U[sizeof(Buffer)*8];
    BYTE            rec_T[sizeof(Buffer)*8];
    int             nrec_U;
    int             nrec_T;

    DWORD           _lastRec;
    tcp_cli_sock    _sock;
    udp_client_sock _usock;
    fd_set			_rdSet;
    fd_set			_wrSet;
    fd_set			_exSet;
    long            _alive;
    DWORD           _cliID;
    char            _playername[64];
    BOOL            _sentTCP;
    DWORD           _lerror;
    BOOL            _toglePing;
    UINT            _port;
    tstring         _ip;
    tstring         _proxyip;
    UINT            _proxyport;
    PL              _cachedPls[MSG_LAST];
    RawRing<V3, 256> _positions;
};




#endif //__NETWORK_H__
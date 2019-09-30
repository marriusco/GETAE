#ifdef _MULTI_UDPS
#ifndef _RECTHREAD_H_
#define _RECTHREAD_H_

#include "../~preconfig.h"
#include "../pkbuffer.h"
#include "sock.h"

//---------------------------------------------------------------------------------------
#define SEND_DATA       0x1
#define REC_DATA        0x2
#define SENDREC_DATA    0x3

//---------------------------------------------------------------------------------------
class Handler;
class NetIO;
class Statistics;
class ReadWrite : public OsThread
{
public:
    friend class NetIO;
    friend class Statistics;
    ReadWrite(){
        _function = SENDREC_DATA;
        _workingBuff = new BYTE[REC_BF_SIZE];
        _delay      = 128;
        _recsU      = 0,
        _sendsU     = 0;
        _nbytesUDP  = 0;
    };
    ~ReadWrite(){ delete[] _workingBuff;};
    void    ThreadFoo();
    void    Configure(DWORD function){_function = function;}

private:
    int     _PoolData(int tout=10);
    int     _Check_FDSets(int sel);
    int     _Prepare_FdSets();
    void    _Pong(Handler* pH, const BYTE* pMaxBuff, int blockSZ,BOOL);
    
    BOOL    _Receive(Handler* pH, BOOL btcp);
    BOOL    _CanSend(Handler*, BOOL btcp);

private:
    fd_set			_rdSet;
    fd_set			_wrSet;
    fd_set			_exSet;
    int             _error;
    int             _port;
    DWORD           _delay;
    int             _recsU;
    int             _sendsU;
    BYTE            *_workingBuff;
    long            _nbytesUDP;
    DWORD           _function;
};

//-----------------------------------------------------------------------------
class Listener : public OsThread
{
public:
    Listener(){};
    ~Listener(){};
    void    ThreadFoo();
    int     Start(int port);
    int     PoolConnections(int);
    void    ServerIsFull();

private:
    tcp_srv_sock    _sock;
    int             _port;
};


//-----------------------------------------------------------------------------
class NetIO
{
public:
    NetIO(){}
    ~NetIO(){};
    BOOL        Start(UINT port);
    void        Stop();
    const       udp_server_sock& Udpss(){return _susock;}

    ReadWrite  _sendThread;
    ReadWrite  _recThread;
    Listener   _listenThread;
    udp_server_sock _susock;
};

#endif//__RECTHREAD_H_



#endif // !MULTI UDP
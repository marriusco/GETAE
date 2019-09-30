#ifndef _MULTI_UDPS

#ifndef _SRECTHREAD_H_
#define _SRECTHREAD_H_

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
        _maxBuffUdp = new BYTE[REC_BF_SIZE];
        _delay      = 128;
        _recsU      = 0,
        _sendsU     = 0;
        _nbytesUDP  = 0;
    };
    ~ReadWrite(){ delete[] _maxBuffUdp;};
    void    ThreadFoo();
    void    Configure(DWORD function){_function = function;}

private:
    int     _PoolData(int tout=10);
    int     _CheckTCP_FDSets(int sel);
    int     _Prepare_FdSets();
    int     _CheckUDP_FDSets(int sel, int clis);
    void    _TCP_Pong(Handler* pH, const BYTE* pMaxBuff, int blockSZ);
    void    _UDP_Pong(const sockaddr_in& rsin, const BYTE* pMaxBuff, int blockSZ);

    int     _ReceiveFrom_UDP(int nsock);
    BOOL    _ReceiveFrom_TCP(Handler* pH);
    BOOL    _TCP_CanSend(Handler*);
    int     _UDP_CanSend(int nsock);
    BOOL    _ReceivedBuffer(sock* psock, counted_ptr<Buffer>& pB);
    BOOL    _PreProcMsgByte(BYTE* pMaxBuff, int blockSZ, BOOL isIn, Handler* pH);
    BOOL    _PreProcMsg(counted_ptr<Buffer>& buffer, BOOL isIn);
    void    _FdZero();
private:
    udp_server_sock _usock;
    fd_set			_rdSet;
    fd_set			_wrSet;
    fd_set			_exSet;
    int             _error;
    int             _port;
    DWORD           _delay;
    int             _recsU;
    int             _sendsU;
    BYTE            *_maxBuffUdp;
    long            _nbytesUDP;
    DWORD           _function;
    BOOL            _recording;
    
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
    const       udp_server_sock& Udp(){return _recThread._usock;}

    ReadWrite  _sendThread;
    ReadWrite  _recThread;
    Listener   _listenThread;
};

#endif//__SRECTHREAD_H_

#endif //MULTIUDP
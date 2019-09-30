#ifdef _MULTI_UDPS
#ifndef _GAME_SERVER_H__
#define _GAME_SERVER_H__

//-----------------------------------------------------------------------------
#include "../baselib/baselib.h"
#include "../baselib/baseutils.h"
    #ifdef _CFG_WIN32_WIN
        #include "../baselib/WIN_WIN/InetFtpHttp.h"
    #endif
    #ifdef _CFG_GNU_WIN
        #include "../baselib/GNU_WIN/InetFtpHttp.h"
    #endif
    #ifdef _CFG_GNU_LINUX
        #include "../baselib/GNU_LINUX/InetFtpHttp.h"
    #endif
#include "../statistics.h"
#include "../pkbuffer.h"
#include "../~preconfig.h"
#include "../procunit.h"
#include "netio.h"
#include "handler.h"


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#pragma warning (disable: 4786)
typedef enum _SRV_STATE
{
    SRV_STATE_UNDET,
    SRV_STATE_PLAYING,
    SRV_STATE_LOADINGLEVEL,
}SRV_STATE;
class Processor;
typedef counted_ptr<Buffer> SpBuffer;

//-----------------------------------------------------------------------------
class Statistics;
class server
{
public:
    friend class Statistics;
    server();
    virtual~ server(){}
    virtual void    MT_UpdateSimulation(Processor* pt, const Handler*  pH, const SpBuffer& pB)=0;
    virtual void    HearthBeat(DWORD)=0;
    virtual BOOL    AllocateHandlers(int)=0;
    virtual int     OnLetDirectPing(const sockaddr_in& rsin)=0;
    BOOL            Start(int port, int threads=1);
    void            Stop();
    void            CloseHandler(Handler* pc);
    Handler*        OpenHandler();
    void            SetHandlerUID(Handler* pH);
    bool            IsBlocked(){return (_state==SRV_STATE_LOADINGLEVEL||_state==SRV_STATE_UNDET); }
    void            SendBuffer(counted_ptr<Buffer>& buff);
    void            SetState(SRV_STATE state){_state = state;_counter=0;}
    Handler*        GetHandler(DWORD id,BOOL force=0);
    Handler*        RemHandlerBit(DWORD& bw);
    Handler*        GetHandlers();
    void            PostMessage(SyncQ& q, DWORD to, DWORD msg, DWORD owner, counted_ptr<Buffer>& b);
    void            Count(){++_counter;} // loops in thet state
    BOOL            IsInState(){return _counter!=0;}
    void            PreBan(const sockaddr_in& rsin);
    void            Ban(const sockaddr_in& rsin);
    BOOL            IsBanned(const sockaddr_in& rsin);
    void            QueueIn(const counted_ptr<Buffer>& rB);
    int             HandlersCount(){
        AutoLock l(&_mutex);
        return _handlers;
    }
    void            FlushDeadHandlers();
    void            DisconnectHandler(Handler* pH, const char*);
protected:
    
public:
    Statistics           _stats;   
    NetIO                _ioUnit;
    ProcUnit             _procUnit;
    
    SyncQ                _outQueueTCP;
    SyncQ                _outQueueUDP;
    SyncQ                _inQueue;

    Handler*             _clients[MAX_CLIENTS];
    int                  _handlers;
    mutex                _mutex;
    
    long                 _dirty;
    bool                 _blocked;
    SRV_STATE            _state;
    long                 _counter;
    Ip                   ip_localIp;
};

//-----------------------------------------------------------------------------
extern server* PgServer;
#pragma warning (disable: 4786)


#endif // !_GAME_SERVER_H__
#endif // !MULTI UDP



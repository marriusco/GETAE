#ifndef _MULTI_UDPS
#ifndef _CLIHANDLER_H_
#define _CLIHANDLER_H_

#include "../~preconfig.h"
#include "sock.h"
#include "../pkbuffer.h"


//----------------------------------------------------------------------------
#define SEND_NOW    1
#define CACHE_IT    0

//-----------------------------------------------------------------------------
class Statistics;
class Listener;
class ReadWrite;
class Handler
{
public:
    friend class Statistics;
    friend class Listener;
    friend class ReadWrite;
    friend class ListenThread;

    Handler(){
        _iid        = 0;
        _pNext      = 0;
        _closeCount = 0;
        _nRecBytes  = 0;
        _errors     = 0;
        _nSndWaterMark=0;
        _seqS        = 0;
        _nRecBytes   = 0;
        _st_bytesInTcp = 0;
        _st_bytesOutTcp = 0;
        _st_bytesInUdp = 0;
        _st_bytesOutUdp = 0;
        _espectedSeq = Seq::GetSeq(_seqS);
    }
    virtual ~Handler(){
    }
    virtual void    Clean();
    virtual void    Disconnect();
    virtual void    OnCreate(){_seqS = 0; _seqR = 0; _errors=0;_nRecBytes=0;};
    virtual void    OnDestroy(){};
    virtual void    OnDisconnect(){_nRecBytes=0;}
    virtual void    OnError(int error){ error=1;};
    virtual BOOL    OnConnect(const char* addr=0);
    virtual const BOOL AcceptMessage(const counted_ptr<Buffer>& pB);
    void            AddError(){++_errors;}
    void            ClearError(){_errors = 0;}
    void            PreClose(){++_closeCount;};
    const   BOOL    TCP_SendBuffer(const counted_ptr<Buffer>& pB, BOOL when);
    const   BOOL    UDP_SendBuffer(const counted_ptr<Buffer>& pB, const udp_server_sock& us, DWORD when=0);
    const   BOOL    TCP_Flush();
    const   BOOL    Ping();
    const   long    GetErrors(){return _errors;}    
    const   sockaddr_in& Rsin(){return  _remote_usin;}
    Handler* Next(){Handler* pR = this->_pNext; this->_pNext=0; return pR;}
    BOOL    IsConnected()const{return _sock.isopen();}
    const long ToClose()const{return _closeCount;}
    const BOOL IsActive()const{return _closeCount != 0 || IsConnected();}
    void  AddNext(Handler* pH){if(0==_pNext) _pNext=pH; else _pNext->AddNext(pH);}
    const BOOL    USockSend(const udp_server_sock& us, const BYTE* pb, const int len, const sockaddr_in& rsin);
    const BOOL    TSockSend(const BYTE* pb, const int len);
    
    WORD    Counter()const            {return HIWORD(_iid);}
    WORD    Index()const              {return LOWORD(_iid);}
    DWORD   Uid()const                {return _iid;}  // unique Id
    DWORD   Bit()const                {return 1 << LOWORD(_iid);} //return bit index
    void    Uid(DWORD uidh)           {_iid = uidh;}
    void    Uid(WORD uniq, WORD idx)  {_iid = MAKELONG(idx, uniq);}
    
protected:
    struct  sockaddr_in          _remote_usin;      // dest
    tcp_cli_sock                 _sock;
    DWORD                        _iid;              // index/unicid
    vector<counted_ptr<Buffer> > _queueTCP;
    Handler*                     _pNext;
    long                         _closeCount;
    BYTE                        _pRecBuff[sizeof(PL)];
	int							_nRecBytes;
    long                        _nSndWaterMark;
    DWORD                       _seqS;
    DWORD                       _seqR;
    DWORD                       _espectedSeq;
    long                        _errors;

    DWORD                       _st_bytesInTcp;
    DWORD                       _st_bytesOutTcp;
    DWORD                       _st_bytesInUdp;
    DWORD                       _st_bytesOutUdp;

};




#endif//


#endif //MULTIUDP
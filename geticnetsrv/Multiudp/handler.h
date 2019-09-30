#ifdef _MULTI_UDPS
#ifndef _CLIHANDLER_H_
#define _CLIHANDLER_H_

#include "../~preconfig.h"
#include "sock.h"
#include "../pkbuffer.h"


//----------------------------------------------------------------------------
#define SEND_NOW    1
#define CACHE_IT    0
//-----------------------------------------------------------------------------


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

    Handler(){
        Clean();
    }
    virtual ~Handler(){
    }
    virtual void    Clean();
    virtual void    Disconnect();
    virtual void    OnCreate(){_seqS = 0; _seqR = 0; _errors=0;_nRecBytesU=0;_nRecBytesT=0;};
    virtual void    OnDestroy(){};
    virtual void    OnDisconnect(){_nRecBytesU=0;_nRecBytesU=0;}
    virtual void    OnError(int error){ error=1;};
    virtual BOOL    OnConnect(const char* addr);
    virtual const BOOL AcceptMessage(const counted_ptr<Buffer>& pB);
    void            AddError(){++_errors;}
    void            ClearError(){_errors = 0;}
    void            PreClose(){++_closeCount;};
    const   long    GetErrors(){return _errors;}    
    const   sockaddr_in& Rsin(){return  _remote_usin;}
    Handler*       Next(){Handler* pR = this->_pNext; this->_pNext=0; return pR;}
    BOOL          IsConnected()const{return _sock.isopen();}
    const long    ToClose()const{return _closeCount;}
    const BOOL    IsActive()const{return _closeCount != 0 || IsConnected();}
    const DWORD   Bit()const{return (1 << _iid);}
    void          AddNext(Handler* pH){if(0==_pNext)_pNext=pH;else _pNext->AddNext(pH);}
    const DWORD   Uid() const{return _iid;}
    void          Uid(const DWORD uidh){_iid = uidh;}
    const BOOL    SockSend(const BYTE* pb, const int len, BOOL btcp);
    const int     SockReceive(BYTE* pb, int len, BOOL btcp);
    const BOOL    Flush(BOOL btcp);
    sock*         NetSock(BOOL tcp);
    const BOOL    SendBuffer(const counted_ptr<Buffer>& pB, BOOL when, BOOL btcp);
    

protected:
    struct  sockaddr_in          _remote_usin;      // dest
    tcp_cli_sock                 _sock;
    udp_server_sock              _usock;
    DWORD                        _iid;              // index/unicid
    vector<counted_ptr<Buffer> > _queueTCP;
    vector<counted_ptr<Buffer> > _queueUDP;
    Handler*                     _pNext;
    long                         _closeCount;
    long                         _nSndWaterMark;
    DWORD                        _seqS;
    DWORD                        _seqR;
    DWORD                        _espectedSeq;
    long                         _errors;
    BYTE                         _pRecBuffT[sizeof(PL)+1];
    BYTE                         _pRecBuffU[sizeof(PL)+1];
	int							 _nRecBytesU;
    int							 _nRecBytesT;

    DWORD                        _st_bytesInTcp;
    DWORD                        _st_bytesOutTcp;
    DWORD                        _st_bytesInUdp;
    DWORD                        _st_bytesOutUdp;

};




#endif//



#endif // !MULTI UDP
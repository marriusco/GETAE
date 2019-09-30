#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "getae.h"
#ifdef _MULTI_UDPS
    #include "MULTIUDP/handler.h"
    #include "MULTIUDP/server.h"
    #include "MULTIUDP/netio.h"
#else
    #include "SINGLEUDP/handler.h"
    #include "SINGLEUDP/server.h"
    #include "SINGLEUDP/netio.h"
#endif //
#include "pkbuffer.h"

#define FLAG_DIRTY  1
#define CLI_OFFLINE 0
#define CLI_ONLINE  1

struct History
{
    Pos     pos;
    DWORD   time;
    DWORD   seq;
};


class CPlayer : public Handler  , public Pos
{
public:
	CPlayer();
	virtual ~CPlayer();
    virtual void    OnMessage(counted_ptr<Buffer>& rB);
    DWORD   Status(){return _cliState;}
    void    Status(DWORD cs){_cliState = cs; }
    void    Ping(DWORD ms){_ping = ms;}
    BOOL    Dirty(){return _flags & FLAG_DIRTY;}
    void    SetPos(const Pos& pos);
    void    Dirty(BOOL b){_flags |= FLAG_DIRTY;}
    void    SetPosV(const V3& v){this->_pos = v;}
    void    SetPosPoint(REAL x, REAL y, REAL z);
    void    Euler(const V3& v){this->SetAngles(v.x,v.y,v.z);}
    void    SetLeaf(int leaf){_leaf = leaf;}
    int     GetLeaf(){return _leaf;};
    void    GetZone(int leaf){_zone = leaf;}
    int     SetZone(){return _zone;};
    const   V3&  GetPos(){return this->_pos;}
    const   V3&  Euler(){return this->_euler;}
    DWORD   VisFlag(){return _vis;}
    void    VisFlag(int index, int set);
    void    Reinitialize();
private:
    DWORD                   _cliState;    
    DWORD                   _ping;
    DWORD                   _flags;
    DWORD                   _vis;
    int                     _leaf;
    DWORD                   _zone;
    RawRing<History, 128>   _history; // 1 sec history
};


#endif //

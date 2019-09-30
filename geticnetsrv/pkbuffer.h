#ifndef _BUFFER_H__
#define _BUFFER_H__
#pragma warning (disable:4786)

#include "getae.h"

//---------------------------------------------------------------------------------------
#define ALL_CLIS    (0xFF)

//---------------------------------------------------------------------------------------
class Seq
{
    struct Numbs   {
        DWORD  l1,l2,l3,l4;
    };

public:
    static DWORD GetSeq(DWORD sch)	
    {
        int scheme = (sch % 32);
        int y      = LOWORD(HIWORD(sch));
        int x      = HIWORD(HIWORD(sch));
        
        static  Numbs   numbs[] =
        {
            {15731,989221,1176316589,1073741825},
            {15231,779221,1276315589,1072741824},
            {15731,789621,1376314589,1071741821},
            {15711,789251,1476313589,1073741824},
            {15031,981221,1176216589,3073741825},
            {13731,789421,1576312589,1071741822},
            {15731,783221,1676311589,1073241824},
            {25731,729221,1776310589,1073751824},
            {15731,989221,1176316589,1073741825},
            {15231,779221,1276315589,1072741824},
            {15731,789621,1376314589,1071741821},
            {15711,789251,1476313589,1073741824},
            {15731,989221,1176316589,1073741825},
            {15231,779221,1276315589,1072741824},
            {15731,789621,1376314589,1071741821},
            {15711,789251,1476313589,1073741824},
            {15031,981221,1176216589,3073741825},
            {13731,789421,1576312589,1071741822},
            {15731,783221,1676311589,1073241824},
            {25731,729221,1776310589,1073751824},
            {15731,989221,1176316589,1073741825},
            {15231,779221,1276315589,1072741824},
            {15731,789621,1376314589,1071741821},
            {15711,789251,1476313589,1073741824},
            {15031,981221,1176216589,3073741825},
            {13731,789421,1576312589,1071741822},
            {15731,783221,1676311589,1073241824},
            {25731,729221,1776310589,1073751824},
            {15031,981221,1176216589,3073741825},
            {13731,789421,1576312589,1071741822},
            {15731,783221,1676311589,1073241824},
            {25731,729221,1776310589,1073751824},

        };
        return ((x *(x*y*numbs[scheme].l4+numbs[scheme].l3)+numbs[scheme].l2) & 0x7FFFFFFF)/numbs[scheme].l1;
    }
};

//---------------------------------------------------------------------------------------
typedef enum _SRV_MSGS{ // these goes all on tcp
    MSG_NA = 0,           // dont add at the begining .see static int lengths
    MSG_PING,           // 1
    MSG_SOCK_PING,      // 2 // not trough the game status system (handeled by rec thread)
    MSG_JOIN,           // 3
    MSG_AUTH,           // 4
    MSG_PLAY,           // 5
    MSG_LOADLEVEL,      // 6
    MSG_POSCHANGED,     // 7
    MSG_MESSAGE,        // 8
    MSG_SET_ID,         // 9
    MSG_LEAVE,          // 10
    MSG_DELETED,        // 11
    MSG_NEWLEVELBEGIN,  // 12
    MSG_ACTION,         // 13
    MSG_RETRY,
    MSG_RECORD,
    MSG_LAST,
    
}SRV_MSGS;

//---------------------------------------------------------------------------------------
class Handler;
struct PL
{
    struct H
    {
        WORD    size;       // 2    
        BYTE    istcp;      // 1    
        BYTE    msg;        // 1    
        BYTE    count;
        BYTE    reserved;
        DWORD   uid;        // source index    
        DWORD   whom;       // destination bits
        DWORD   seq;        // sequence 
    }_H;

    union U
    {
        BYTE    buff[256-sizeof(H)];

        struct Ping
        {
            long  ping;
            DWORD time;
            DWORD nBuffs;
            DWORD inSz;
            DWORD outTSz;
            DWORD outUSz;
            DWORD nthrDs;
            DWORD nFPS;
            DWORD nClis;
            DWORD inTime;
            DWORD simInTime;
            DWORD simOutTime;
            DWORD sentTime;
            DWORD udpPerSecIn;
            DWORD udpPerSecOut;
            DWORD tcpPerSecIn;
            DWORD tcpPerSecOut;
            DWORD tcpSrvSecIn;
            DWORD tcpSrvSecOut;
            DWORD udpSrvSecIn;
            DWORD udpSrvSecOut;

        }_Ping;

        struct Join
        {
            char  name[32];
            char  passwd[32];
            DWORD os;            // os
            BYTE  fps;           // local fps
            UINT  gameversion;   // version
            BYTE  registered;    // registered version
            DWORD locIP;         // local ip
            DWORD flags;         // WANT_MAPNAME
        }_Join;

        struct Auth
        {
            DWORD dw;
        }_Auth;

        struct  Level //
        {
            char    url[164];   // MSG_LOADLEVEL
        }_Level;
    
        struct Position
        {
            float pos[3];
            float euler[3];
            int   leaf;
        }_Position;

        struct Play: public Position
        {
            char  url[128];
            char  name[32];
            BYTE  character;     // character ID
        }_Play;
        
        struct Action         // action
        {
            float pos[3];
            float euler[3];
            int   hitObj;       // local hit
            DWORD flags;        // action flags
        }_Action;

    }_U;
};

//---------------------------------------------------------------------------------------
#pragma warning (disable:4786)
class Buffer : public DPool<Buffer>
{
public:
    
    Buffer(){::memset(&pl,0,sizeof(pl));_refs=0;Aquire();}
    BYTE*   Storage(){return (BYTE*)&pl;}
    BYTE*   GetBuffer(){return (BYTE*)&pl._U.buff;}
    int     MaxLen(){return sizeof(PL);}
    int     Length(){return pl._H.size + sizeof(pl._H);}
    int     BuffLen(){return pl._H.size;}
    void    Sequence(DWORD seq){pl._H.seq=seq;}
    const DWORD   Sequence()const{ return pl._H.seq;}
    int     HdrLen(){return sizeof(pl._H);}
    BOOL    Ready(){return pl._H.msg != MSG_NA;}
    void    Reset(){pl._H.size=0; pl._H.msg = MSG_NA;}
    void    Count(int counter){pl._H.count=counter;}
    void    Transfer(BYTE* pb, int length){::memcpy(&pl, pb, length); }
    void    SetMsg(BYTE msg, BYTE tcp=1){
            static int lengths[]={0, 
                                  sizeof(PL::U::Ping),
                                  sizeof(PL::U::Ping),
                                  sizeof(PL::U::Join),
                                  sizeof(PL::U::Auth),
                                  sizeof(PL::U::Play),
                                  sizeof(PL::U::Level),
                                  sizeof(PL::U::Position),
                                  sizeof(PL::U),
                                  0,
                                  0,
                                  0,
                                  0,
                                  sizeof(PL::U::Action),
                                  0,
                                };
            ++pl._H.count;
            pl._H.msg   = msg;
            pl._H.istcp = tcp;
            pl._H.size  = lengths[msg];
    }
    BYTE   GetMsg()const{return (BYTE)pl._H.msg;}
    void   SetOwner(DWORD uid){pl._H.uid = uid;}
    DWORD  GetClientUid()const{return pl._H.uid;}
    WORD   GetClientIndex(){return pl._H.uid;}
    BOOL   IsTCP(){return pl._H.istcp;}
    DWORD& GetTo(){return pl._H.whom;}
    void   SendTo(int id){pl._H.whom |= id;}
    void   SetSendTo(DWORD id){pl._H.whom = id;}
    void   Aquire(){++_refs;}
    void   Unaquire(){--_refs;if(0==_refs)delete this;}
    void    SetString(char* s,...){ // test buffer
            va_list args;
            va_start(args, s);
#ifdef _WINDOWS
            ::_vsnprintf((char*)pl._U.buff, 128, s, args);
#else
      sprintf((char*)pl._U.buff, s, args);
#endif //
            pl._H.size = strlen((char*)pl._U.buff);
            va_end(args);
    }
    PL pl; // keep it at 256
private:
    long    _refs;
};


//---------------------------------------------------------------------------------------
//// typedef cs_deque<counted_ptr<Buffer> >  SyncQ;

//---------------------------------------------------------------------------------------
#define  SERVER_ID (0xFF)

//---------------------------------------------------------------------------------------
#define JOIN_GET_MAPNAME         0x1
#define JOIN_MAP_LOADED          0x2
#define JOIN_POSITION            0x4
#define SET_ID_UDP_CONN          0x5  // in reserved
#define SET_SEND_UDP_PORT_PLUS1  0x6  // set on client site a separate udp for send on which the port id PORT+1


#endif //_BUFFER_H__



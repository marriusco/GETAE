#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__
#pragma warning (disable: 4786)
#include <IO.h>
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
#include "MiniBt.h"
#include "../system/levelman.h"


class CPlayer;
class GameServer  : public server
{
public:
    GameServer();
    virtual ~GameServer(){ge_sleep(3000);};

    int     Initialize();
    bool    IsAlive();
    void    Idle();
    void    SetLocalIP(const TCHAR* ip, int port);//ip_localIp
    void    SetMasterServer(const TCHAR* ip, int port);
    void    SetMapsLocation(const TCHAR* ip);
    void    SetProxy(const TCHAR* ip, int port);
    void    SetMaxTimePerLevel(int minutes);
    void    SetMaximFrameRate(int fps);
    void    SetThreadCount(int tc);
    void    SetPoolSize(int psz){n_poolsize=::tmin(psz,4096);};
    int     GetPoolSize(){return n_poolsize;}
    void    Stop();
    void    Uninitialize();
    void    ChangeLevel();
    virtual void MT_UpdateSimulation(Processor* pt,  Handler*  pH, SpBuffer& pB);
    virtual void   HearthBeat(DWORD);
    DWORD    GetCurrentVersion();
    virtual BOOL     AllocateHandlers(int);
    int     OnLetDirectPing(const sockaddr_in &rsin);
    void    ConfigServer(DWORD csrv, DWORD printlevel);
    int     SetScriptProc(const TCHAR* scrProc);
    BOOL    StartServer();
    void    SetWorkingFolder(const TCHAR* mapsfolder);
    LevelMan* GetLevelManager(){
        return &o_levman;
    }
    void     Register2Master(const TCHAR* srvip, int port, BOOL reg);//
    long    SendMessage(long msg, long w, long l);
    template <class WP, class LP>long Dispatch(long msg, WP wp, LP lp){
        return _scrSysProc ? (*_scrSysProc)(this, msg, wp, lp) : 0/*not handeled*/;
    }
private:
    void    _SendData(const SpBuffer& pB);
    void    _SendCurrentMap(Handler* pH);
    
    BOOL    _LoadNewLevel(const TCHAR* levName);
    BOOL    _LoadFileLevel(const TCHAR* levName);
    void    _BroadcastLevelChange() ;
    void    _Clean();

    void    _RunVisibilities();
    void    _RunOwnerShips(CPlayer *p);
    BOOL    _CreatePlayer(CPlayer* pP, SpBuffer& pB);
    void    _SetVisibilities(int set, CPlayer* p, CPlayer* q);

//  handlers
    void    _Client_Join( CPlayer* pH,       SpBuffer& pB);
    void    _Client_Play( CPlayer* pH,       SpBuffer& pB);
    void    _Client_Leave( CPlayer* pH,      SpBuffer& pB);
    void    _Client_PosChanged( CPlayer* pP, SpBuffer& pB);
    void    _Client_Ping( CPlayer* pP,       SpBuffer& pB);
    void    _Client_Act( CPlayer* pP,        SpBuffer& pB);
private:
    Ip                  ip_masterServer;
    Ip                  ip_fileServer;
    Ip                  ip_localIp;
    Ip                  ip_httpproxy;
    int                 n_milisecs;
    int                 n_fps;
    int                 n_threads;
    tstring             s_password;
    
    tstring             s_localfolder;
    tstring             s_maplocation;
    map<tstring, int>   s_playedmaps;
    int                 n_curmapidx;
    tstring             s_curlevel;


    DWORD               n_tickCount;
    long                n_minutes;
    long                n_poolsize;
    mutex               m_mutex;
    MiniBt*             p_tree;
    DWORD               t_statistics;
    long                n_statinproc;
    LevelMan            o_levman;
    SquirrelFunction<int>*  _scrSysProc;
};


//-----------------------------------------------------------------------------
class FileBrowser
{
public:
    FileBrowser(){};
    ~FileBrowser(){};
    int    BroseDir(const TCHAR* dir, const TCHAR* wildchar)
    {
        CDirChange cd(dir);
        struct _finddata_t c_file;
        long    hFile;
        TCHAR   loco[256];
        _tcscpy(loco, wildchar);
        _files.clear();

        TCHAR*   pTok = _tcstok(loco,",");
        while(pTok)
        {
            int found = 0;
            if( (hFile = _findfirst( pTok, &c_file )) != -1L )
            {
                _files << c_file.name;
                while( _findnext( hFile, &c_file ) == 0 )
                {
                    _files << c_file.name;
                }
            }
            _findclose( hFile );

            pTok=_tcstok(0,",");
        }



       return _files.size();
    }
    const TCHAR* GetAt(int index){
        if(index < _files.size())
        {
            return _files[index].c_str();
        }
        return "";
    }

private:
    vvector<t_string> _files;
};

extern GameServer* PSystem;

//---------------------------------------------------------------------------------------
DECLARE_INSTANCE_TYPE(FileBrowser);
DECLARE_INSTANCE_TYPE(GameServer);



//---------------------------------------------------------------------------------------
// messages into the script engine
#define SYS_PROGRESS    99
#define GM_CREATE       100
#define GM_STOP         101


#endif // #ifndef __GAME_SERVER_H__

/*
{
        ip_masterServer.s_ip   = "marius.homeunix.org";
        ip_masterServer.n_port = 27001;
        ip_fileServer.s_ip     = "";
        n_poolsize             = 512;
        p_tree                 = 0; 
        n_fps                  = 30;
        n_threads              = 1;
        n_minutes              = 10;
        n_milisecs             = n_minutes*60*1000;
        char lip[128];
        sock::GetLocalIP(lip);
        ip_localIp.s_ip       = lip;
        ip_localIp.n_port     =  27001;
        _scrSysProc           = 0;
        _Clean();
        n_curmapidx           = 0;
    };
*/

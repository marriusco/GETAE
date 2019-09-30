#pragma warning (disable: 4786)

#include "stdafx.h"
#include "GameServer.h"
#include "pkbuffer.h"
#include "BzipCompressor.h"
#include "getae.h"
#include "Player.h"
/*
    1:
       base::c->connect
       base::c<-setid
       c ->  [join]
       ?     [auth]  <-  s
       ?c -> [join]  ?^
             [map]   <-  s
       c ->  [play]
       ?c -> [leave] (do download the map)

*/
extern BOOL GlobalExit;
GameServer* PSystem;

GameServer::GameServer()
{
    ip_masterServer.s_ip   = "marius.homeunix.org";
    ip_masterServer.n_port = 27001;
    ip_fileServer.s_ip     = "";
    n_poolsize             = 512;
    p_tree                 = 0; 
    n_fps                  = 200;
    n_threads              = 1;
    n_minutes              = 10;
    n_milisecs             = n_minutes*60*1000;
    char lip[128];
    sock::GetLocalIP(lip);
    ip_localIp.s_ip        = lip;
    ip_localIp.n_port      = 27001;
    memset(_clients,0,sizeof(_clients));
    _Clean();
    
    ON_EVENT(MSG_PING,&GameServer::_Client_Ping);
    ON_EVENT(MSG_DELETED,&GameServer::_Client_Leave);
    ON_EVENT(MSG_LEAVE,&GameServer::_Client_Leave);
    ON_EVENT(MSG_JOIN,&GameServer::_Client_Join);
    ON_EVENT(MSG_PLAY,&GameServer::_Client_Play);
    ON_EVENT(MSG_POSCHANGED,&GameServer::_Client_PosChanged);
    ON_EVENT(MSG_ACTION, &GameServer::_Client_Act);
}

//---------------------------------------------------------------------------------------
// update map and client states into the server GameServer
// from pool of threads
void     GameServer::MT_UpdateSimulation(Processor* pt,
                                         Handler*  pH,
                                         SpBuffer& pB)
{
    ++n_statinproc;
    CPlayer* pl =static_cast<CPlayer*>(pH);
    this->_HanndleMessage(pB->GetMsg(), pl, pB);
    --n_statinproc;
}

//---------------------------------------------------------------------------------------
void     GameServer::_Client_Join(CPlayer* pH, SpBuffer& pB)
{
    const PL::U::Join &join = pB->pl._U._Join;
    MyPrintf(IL, "PLAYER %d JOINS THE SERVER\r\n", pH->Uid());

    // if password is set send auth message
    if(s_password.length())
    {
        // if join does not contain password
        if(strcmp(s_password.c_str(), join.passwd))
        {
            // send  auth and return
            _SendSrvMsg(MSG_AUTH, pH->Bit());
            return;
        }
    }
    _SendCurrentMap(pH);
}


//---------------------------------------------------------------------------------------
void     GameServer::_Client_Play(CPlayer* pP, SpBuffer& pB)
{

    MyPrintf(IL, "PLAYER %d STARTS TO PLAY IN SIMULATION\r\n", pP->Uid());
    

    if(_CreatePlayer(pP, pB))
    {
        SpBuffer    pB(new Buffer);
        pB->pl._U._Play.pos[0]   = pP->_pos.x;
        pB->pl._U._Play.pos[1]   = pP->_pos.y;
        pB->pl._U._Play.pos[2]   = pP->_pos.z;
        pB->pl._U._Play.euler[0] = pP->_euler.x;
        pB->pl._U._Play.euler[1] = pP->_euler.y;
        pB->pl._U._Play.euler[2] = pP->_euler.z;
        pB->pl._U._Play.leaf     = pP->GetLeaf();

        // Send to all clients this PLAY
        _SendMsg(ALL_CLIS, pP->Uid(), MSG_PLAY, pB, 1);

        // Sent to this PLay all clients
        for(int i=0; i < MAX_CLIENTS; i++)
        {
            CPlayer* pwp = (CPlayer*)_clients[i];

            if(pwp->Status() != CLI_ONLINE)
                continue;

            if(pwp == pP) // dont sent back to this client it's iD
                continue;

            SpBuffer pB(new Buffer);
            pB->pl._U._Play.pos[0]   = pwp->_pos.x;
            pB->pl._U._Play.pos[1]   = pwp->_pos.y;
            pB->pl._U._Play.pos[2]   = pwp->_pos.z;
            pB->pl._U._Play.euler[0] = pwp->_euler.x;
            pB->pl._U._Play.euler[1] = pwp->_euler.y;
            pB->pl._U._Play.euler[2] = pwp->_euler.z;
            pB->pl._U._Play.leaf     = pwp->GetLeaf();

            _SendMsg(pP->Bit(), pwp->Uid(), MSG_PLAY, pB, 1);
        }
    }
}

//---------------------------------------------------------------------------------------
void     GameServer::_Client_Act( CPlayer* pP, SpBuffer& pB)
{
    PL::U::Action& a = (PL::U::Action&)pB->pl._U._Action;

    pP->SetAngles(a.euler[0], a.euler[1], a.euler[2]);
    pP->_pos.x = a.pos[0];
    pP->_pos.y = a.pos[1];
    pP->_pos.z = a.pos[2];

    if(pP->GetLeaf()==-1) 
    {
        pP->SetLeaf(p_tree->GetCurrentleaf(pP->_pos));
    }
    this->_SendMsg(pP->VisFlag(), pP->Uid(), MSG_ACTION, pB, 1);
}

//---------------------------------------------------------------------------------------
void     GameServer::_Client_PosChanged( CPlayer* pP, SpBuffer& pB)
{
    PL::U::Position& f = (PL::U::Position&)pB->pl._U._Position;

    pP->SetAngles(f.euler[0], f.euler[1], f.euler[2]);
    pP->SetPosPoint(f.pos[0],f.pos[1],f.pos[2]);

    //let processor thread to get current leaf !!!
    /*
    if(-1==f.leaf && pP->GetLeaf()==-1) 
    {
        f.leaf = p_tree->GetCurrentleaf(pP->_pos);
    }
    */

    pP->SetLeaf(f.leaf);
    pP->Dirty(1);
    // now send to others that can see me UDP pos changed
    // not including me couse position changed goes on locally for smoothness
    this->_SendMsg(pP->VisFlag(), pP->Uid(), MSG_POSCHANGED, pB, 0);

}

//---------------------------------------------------------------------------------------
BOOL     GameServer::_CreatePlayer(CPlayer* pP, SpBuffer& pB)
{
    const    PL::U::Play& play = pB->pl._U._Play;

    // inform all other about this player
    if(pP->Status() == CLI_ONLINE)
    {
        MyPrintf(IL, "PLAYER [%d] REINITIALIZED\r\n",  pP->Uid());
    }
    pP->Status(CLI_ONLINE);
    pP->_pos.setval( V3(play.pos[0],play.pos[1],play.pos[2]) );
    pP->SetAngles( play.euler[0],play.euler[1],play.euler[2] );

    MyPrintf(IL, "PLAYER [%d] ADDED TO SIMULATION. ALL OTHERS NOTIFYED \r\n",  pP->Uid());
    
    return TRUE;
}

//---------------------------------------------------------------------------------------
void     GameServer::_Client_Leave(CPlayer* pP, SpBuffer& pB)
{

    if(pP->Status() == CLI_OFFLINE)
        return;
    pP->Status(CLI_OFFLINE);

    // leave this client
    SpBuffer b2  = SpBuffer(new Buffer);
    _SendMsg(ALL_CLIS, pP->Uid(), MSG_LEAVE, b2, 1);
    MyPrintf(IL, "\nPLAYER %d DELETED FROM SIMULATION. ALL OTHERS NOTIFYED \n", pP->Uid());
}

DWORD    GameServer::GetCurrentVersion()
{
    return 130;
}


int    GameServer::Initialize()
{
    int sz = sizeof(PL::H);
    sz = sizeof(PL);
    n_tickCount   = 0;
    int err = Dispatch(GM_CREATE, this, 1);
    if(err == 1)
        return NO_ERROR;

    if(err==-1)
        return err;

    Register2Master(ip_masterServer.s_ip.c_str(), ip_masterServer.n_port, TRUE);

    o_levman.SetupAddrs(s_maplocation.c_str(),
                        s_localfolder.c_str(),
                        ip_httpproxy.s_ip.c_str(),ip_httpproxy.n_port);
    o_levman.CheckLocally();
    if(0!=o_levman.Download(1))
    {
        printf("Cannot connect \n");
    }
    if(o_levman.GetResourceCount("")) 
    {
        return this->StartServer();
    }
    printf("cannot start \r\n");
    return -1;
}

BOOL    GameServer::StartServer()
{
    t_statistics = ge_gettick();
    n_statinproc = 0;
    if(this->Start(ip_localIp.n_port, n_threads))
    {
        TRACEX("Server Listerning on port %d\r\n", ip_localIp.n_port);
        return 1;
    }
    return 0;
}

bool    GameServer::IsAlive()
{
    return 1;
}

void     GameServer::SetWorkingFolder(const TCHAR* mapsfolder)
{
    s_localfolder = mapsfolder;
    mkdir(mapsfolder);
}

void    GameServer::SetProxy(const TCHAR* ip, int port)
{
    ip_httpproxy.n_port = port;
    ip_httpproxy.s_ip   = ip;

}

void    GameServer::SetLocalIP(const TCHAR* ip, int port)
{
    ip_localIp.s_ip   = ip;
    ip_localIp.n_port = port;
}

void    GameServer::SetMasterServer(const TCHAR* ip, int port)
{
    ip_masterServer.s_ip   = ip;
    ip_masterServer.n_port = port;
}

// marius/password@ip:port
void    GameServer::SetMapsLocation(const TCHAR* ip)
{
    s_maplocation = ip;
    ip_fileServer.ParseUrl(ip);
}

void    GameServer::SetMaxTimePerLevel(int minutes)
{
    minutes = minutes % 60;
    n_milisecs = minutes * 60*1000;
}

void    GameServer::SetMaximFrameRate(int fps)
{
    n_fps = fps % 400;
}

void    GameServer::SetThreadCount(int tc)
{
    n_threads = tc % 32; //limit it at 32 threads
}

void    GameServer::Stop()
{
    server::Stop();
}



void GameServer::_BroadcastLevelChange()
{
    printf(">Broadcast Level Change On TCP Channel ... ");

    SpBuffer pB(new Buffer());
    
    this->_inQueue.Clean();  
    this->SetState(SRV_STATE_LOADINGLEVEL);
    this->_dirty = 1;
    this->_SendSrvMsg(MSG_NEWLEVELBEGIN, ALL_CLIS, pB, 1);
    DWORD ct = ge_gettick();
    do{
        ge_sleep(512);                     // wait for clis stop 
        ++this->_dirty;
    }while(this->_outQueueTCP.Count() && ge_gettick() - ct < 5000);
    if(this->_outQueueTCP.Count())
    {
        this->_outQueueTCP.Clean();
    }
    this->_outQueueUDP.Clean();
    
    int i = MAX_CLIENTS;
    while(--i>=0)
    {
        if(_clients[i]->IsConnected()==false)
            continue;
        ((CPlayer*)_clients[i])->Reinitialize();
    }
    printf("done....\r\n");
}

//---------------------------------------------------------------------------------------
// send the whole http/or ftp format
void     GameServer::_SendCurrentMap(Handler* pH)
{
    if(pH)
        printf("Send current map %s to %d[%d] \n", s_curlevel.c_str(), pH->Counter(), pH->Index());
    else
        printf("Send current map %s to All \n", s_curlevel.c_str());

    //s_curlevel
    SpBuffer pB(new Buffer());

    string remoteloco;
    if(ip_fileServer.s_ip.empty())
    {
        tstring             s_password;
        tstring             s_curlevel;
        if(s_password.length())
            remoteloco   = MKSTR("ftp://getic:%s@localhost:21/", s_password.c_str());
        else
            remoteloco   = "ftp://localhost:21/";
    }
    else
    {
        size_t lastslash = ip_fileServer.s_url.find_last_of("/");
        if(lastslash)
            remoteloco = ip_fileServer.s_url.substr(0,lastslash+1);
        remoteloco += s_curlevel;
    }

    ::sprintf(pB->pl._U._Level.url, "%s",remoteloco.c_str());


    if(pH)
        this->_SendSrvMsg(MSG_LOADLEVEL, pH->Bit(), pB, 1);
    else
        this->_SendSrvMsg(MSG_LOADLEVEL, ALL_CLIS, pB, 1);

}



BOOL    GameServer::_LoadNewLevel(const TCHAR* levName)
{
    MiniBt*  ptree = new MiniBt();
    try{
        ptree->LoadNewLevel(levName);
        printf("Game server Loading BSP: %s\r\n", levName);
        _Clean();
        p_tree = ptree;
    }catch(int& err)
    {
        printf("? Game server error: %d Loading BSP: %s\r\n", err, levName);
        return 0;
    }
    return 1;
}


BOOL    GameServer::_LoadFileLevel(const TCHAR* slevName)
{
    string levName(slevName);

    if(levName.find(".ml")!=-1)
    {
        tstring   bspname;
        FileWrap fw;

        if(fw.Open(MKSTR("%s/%s",s_localfolder.c_str(),levName.c_str()),"rb"))
        {
            // another ugly parsing
            char enough[512]={0};
            fw.Read(enough, 512);
            fw.Close();
            char* ps = strstr(enough,"file url='");
            if(ps)
            {
                ps += strlen("file url='");
                char* pe  = strstr(ps,"'");
                if(pe)
                {
                    *pe = 0;
                    bspname = ps;
                }
            }
        }

        if(!bspname.empty())
        {
            bspname.replace(bspname.find("gbz"),3,"gbt");
            return _LoadNewLevel( MKSTR("%s/%s",s_localfolder.c_str(), bspname.c_str()) );
        }
    }
    return 0;
}


BOOL     GameServer::AllocateHandlers(int handlers)
{
    for(int i=0; i<handlers; i++)
    {
        _clients[i] = new CPlayer();
    }
    return 1;
}


void  GameServer::_Clean()
{
    delete p_tree;
    p_tree = 0;
}


void    GameServer::Register2Master(const TCHAR* srvip, int port,  BOOL breg)
{

    tcp_cli_sock s;
    tstring      locoip = ip_httpproxy.s_ip.length() ? ip_httpproxy.s_ip : srvip;
    int          locop  = ip_httpproxy.n_port ? ip_httpproxy.n_port : port;
    TCHAR        addrloco[32];
    sock::GetLocalIP(addrloco);

    if(!_tcscmp(srvip,"localhost") || 
        !_tcsncmp(srvip,"127",3) || 
        !_tcsncmp(srvip,"192",3) ||
        !_tcsncmp(srvip,addrloco,3)){
            locoip = srvip;
            locop   = port;
    }
    printf ("accessing master at %s \r\n", srvip);

    if(s.connect(locoip.c_str(), locop) >= 0)
    {
        char request[256];

        sprintf(request, "GET /regserver.php?reg=%d&ip=%s&port=%d HTTP/1.1\r\n"
                         "Host: %s:%d\r\n"
                         "User-Agent: GeticSrv/1.0\r\n\r\n",
                         breg, ip_localIp.s_ip.c_str(), ip_localIp.n_port,
                         srvip, port);
        s.send((BYTE*)request, strlen(request)+1);
        char loco[256];
        int  bytes;
        while(bytes = s.receive((BYTE*)loco,255))
        {
            loco[bytes]=0;
        }
        s.destroy();
    }
}


void    GameServer::Uninitialize()
{
    Register2Master(ip_masterServer.s_ip.c_str(), ip_masterServer.n_port, 0);

    // this->Stop();
};

int    GameServer::SetScriptProc(const TCHAR* script)
{
    if(0 == script)return -1;
    try{
        _scrSysProc = new SquirrelFunction<int>(script);
    }catch(SquirrelError& e)
    {
        TRACEX(e.desc);
        return 1;
    }
    return 0;
}

extern long  Trace_Level;
void    GameServer::ConfigServer(DWORD csrv, DWORD printlevel)
{
    Srv_Cfg     = csrv;
    Print_Level = printlevel;
    Trace_Level = printlevel;
}

int    GameServer::OnLetDirectPing(const sockaddr_in &rsin)
{
    return 1;
}

void    GameServer::Idle()
{
    if(n_tickCount==0 ||
       ge_gettick() - n_tickCount > (DWORD)n_milisecs)
    {
        n_tickCount = ge_gettick();
        ChangeLevel();
    }

    DWORD delay = 1000/n_fps;
    DWORD ct = ge_gettick();
    HearthBeat(delay);
    DWORD dt = ge_gettick()-ct;
    if(0==dt) dt=1;
    if(dt < delay)
    {
        Sleep(delay-dt);
    }
    /*
    if(ge_gettick() - t_statistics > 500)
    {
        _mutex.Lock();
        t_statistics = ge_gettick();
        printf( "Iq = %d oTcp = %d oUdp = %d inproc = %d fps = %d pool=%d\r\n", _inQueue.size(),
                                                                                _outQueueTCP.size(),
                                                                                _outQueueUDP.size(),
                                                                                n_statinproc,
                                                                                1000/dt,
                                                                                Buffer::Size());
        _mutex.Unlock();
    }
    */

}

//---------------------------------------------------------------------------------------
void     GameServer::_RunOwnerShips(CPlayer *p)
{
}

//---------------------------------------------------------------------------------------
void     GameServer::_RunVisibilities()
{
    CPlayer *p,*q;
    V3      pq;

    for(int i=0; i<MAX_CLIENTS; i++)
    {
        p = (CPlayer*)_clients[i];

        if(!p->IsConnected() || !p->Dirty())
            continue;

        _RunOwnerShips(p);

        if(p->GetLeaf()==-1)
            p->SetLeaf(p_tree->GetCurrentleaf(p->_pos));

        for(int j = i+1; j < MAX_CLIENTS; j++)
        {
            if(!p->IsConnected() || !p->Dirty())
                continue;

            q  = (CPlayer*)_clients[j];
            if(q->GetLeaf()==-1)
                q->SetLeaf(p_tree->GetCurrentleaf(q->_pos));

            if(p->GetLeaf()!=-1 && 
               q->GetLeaf()!=-1)
            {
                // det PVS client to client
                if(p_tree->LeafPVSLeaf(q->GetLeaf(), p->GetLeaf()))
                {
                    _SetVisibilities(1,p,q);
                }
                else
                {
                    _SetVisibilities(1,p,q);
                }
            }
            else
            {
                _SetVisibilities(1,p,q);
            }
       }
       
    }
}

//---------------------------------------------------------------------------------------
void GameServer::_SetVisibilities(int set, CPlayer* p, CPlayer* q)
{
    //back one of eachother
    if(set)
    {
        V3  pq = q->_pos - p->_pos;
        V3  qp = p->_pos - q->_pos;
        if(Vdp(pq, q->_fwd) < 0)
        {
            q->VisFlag(q->Uid(), 1);
        }
        else
        {
            q->VisFlag(q->Uid(), 0);
        }
        if(Vdp(qp, p->_fwd) < 0)
        {
            p->VisFlag(p->Uid(), 1);
        }
        else
        {
            p->VisFlag(p->Uid(), 0);
        }
    }
    else
    {
        p->VisFlag(q->Uid(), 0);
        q->VisFlag(p->Uid(), 0);
    }
}

//---------------------------------------------------------------------------------------
void     GameServer::_Client_Ping(CPlayer* pP,  SpBuffer& pB)
{
    SpBuffer& pBW = (SpBuffer&)pB;

    pBW->pl._U._Ping.nClis = this->_handlers;

    pBW->pl._U._Ping.simInTime= ge_gettick();

    if(pBW->pl._U._Ping.ping <= 0)
    {
        pBW->pl._U._Ping.nBuffs = _stats.percBuffs;
        pBW->pl._U._Ping.nFPS   = _stats.fps;
        
        pBW->pl._U._Ping.nthrDs         = _stats.nThreads;
        pBW->pl._U._Ping.inSz           = _stats.percinQ;
        pBW->pl._U._Ping.outTSz         = _stats.percinQTcp;
        pBW->pl._U._Ping.outUSz         = _stats.percinQUdp;
        pBW->pl._U._Ping.udpPerSecIn    = _stats.receivesa[pP->Uid()];
        pBW->pl._U._Ping.udpPerSecOut   = _stats.sendsa[pP->Uid()];
        pBW->pl._U._Ping.tcpPerSecIn    = _stats.receivesUa[pP->Uid()];
        pBW->pl._U._Ping.tcpPerSecOut   = _stats.sendsUa[pP->Uid()];
        pBW->pl._U._Ping.tcpSrvSecIn    = _stats.receives;
        pBW->pl._U._Ping.tcpSrvSecOut   = _stats.sends;
        pBW->pl._U._Ping.udpSrvSecIn    = _stats.receivesU;
        pBW->pl._U._Ping.udpSrvSecOut   = _stats.sendsU;
    }
    
    else
    {
        ((CPlayer*)pP)->Ping(pBW->pl._U._Ping.ping);
    }
    
    pBW->SendTo(pP->Bit());
    pBW->pl._U._Ping.simOutTime = ge_gettick();
    SendBuffer(pBW);
}


//---------------------------------------------------------------------------------------
// run the map, run visibility and so on and decide where to send messages
void     GameServer::HearthBeat(DWORD delay)// from main thread
{
    if(0==p_tree)
    {
        Sleep(delay);
        return;
    }
    _RunVisibilities();
}


void GameServer::ChangeLevel()
{
    static int Gindex   = 0;
    int    curmapidx    = n_curmapidx;

    _BroadcastLevelChange();
    ge_sleep(512);

    int nlevels    = o_levman.GetResourceCount("");
    while(1)
    {
        curmapidx  = (Gindex++) % nlevels;
        tstring  szmap = o_levman.GetResourceAt("",curmapidx);

        if(_LoadFileLevel(szmap.c_str()))
        {
            s_curlevel = szmap;
            s_playedmaps[szmap] = s_playedmaps[szmap]+1;

            this->SetState(SRV_STATE_PLAYING);
            this->_SendCurrentMap(0); 
            this->_dirty = 1;
            ge_sleep(512);
            break;
        }
        else if(curmapidx == n_curmapidx)
        {
            GlobalExit=1; //cannot load any of these maps
            break;
        }
    }
}

long    GameServer::SendMessage(long msg, long w, long l)
{
    if(msg == SYS_PROGRESS)
    {
        if(w<0)
        {
            if(l)
                printf("receciving %s \n",(const TCHAR*)l);
        }
        else if(w>0 && w==l)
        {
            printf(".");
        }
        else if(w==0)
            printf("of %d  bytes\n",l);

    }
    return 0;
}

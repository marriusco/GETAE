// geticnetsrv.cpp : Defines the entry point for the console application

#include "stdafx.h"
#include <conio.h>
#include "getae.h"
#include "../system/levelman.h"
#include "GameServer.h"

//---------------------------------------------------------------------------------------
using namespace std;

//---------------------------------------------------------------------------------------
HINSTANCE       AppInstance;
GameServer      *PGs;
BOOL            GlobalExit=0;
int             serverport = 0;
void            IoExport2Script();
long            Srv_Cfg     = CFG_SENDQUEUE;
long            Print_Level = DL;
long            Trace_Level = DL;

TCHAR* tex_includes[256] ={
                                    {_T("")},
                                    {_T("scripts/")},
                                    {_T("res/")},
                                    {_T("../res/")},
                                    {_T("../scripts/")},
                                   };
//---------------------------------------------------------------------------------------
// modifyed std library to allow include search path in the script 
char* sq_includes[256] = {
        {"scripts/"},
        {"../"},
        {"../scripts/"},
        0,
};
//---------------------------------------------------------------------------------------
class Initializer
{
public:
    static void    printfunc(HSQUIRRELVM v, const SQChar *lpszFormat, ...) 
    { 
        va_list args;
        va_start(args, lpszFormat);
        static char szBuffer[800];
        unsigned int nBuf = ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char), lpszFormat, args);
        va_end(args);

        #ifdef _CONSOLE
            printf(szBuffer);
        #else
            ::OutputDebugStringA(szBuffer);
        #endif //
    } 

    Initializer(){
        SquirrelVM::Init();
        sq_setprintfunc(SquirrelVM::GetVMPtr(), Initializer::printfunc); //sets the print function

    }
    ~Initializer(){
       SquirrelVM::Shutdown();
    }
};

//---------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    if(argc<=1) 
    {
        printf("usage: server script.gs  (eg: server scripts/server/server.gs)\n");
        getch();
        return -1;
    }
    Initializer i;
    AppInstance = ::GetModuleHandle(argv[0]);
    IoExport2Script();
    PGs  = new GameServer(); 
    if(PGs)
    {
        try{
            sock::Init();
            SquirrelObject main = SquirrelVM::CompileScript(_SC(argv[1]));
            SquirrelVM::RunScript(main);
            SquirrelFunction<const SQChar*> mainFunction("main");
            string mf = mainFunction(PGs);
            if(0==PGs->SetScriptProc(mf.c_str()))
            {
                DPool<Buffer>::CreatePool(PGs->GetPoolSize());
                if(0==PGs->Initialize())
                {
                    while(PGs->IsAlive() && !GlobalExit)
                    {
                        PGs->Idle();
                        if(_kbhit())
                        {
                            if(toupper(_getch())=='Q')
                            break;
                        }
                    }
                }
            }
            
        }
        catch(SquirrelError &e)
        {
            Initializer::printfunc(0, e.desc);
        }
        PGs ? PGs->Uninitialize() : 0;
        delete PGs;
        DPool<Buffer>::DestroyPool();
        sock::Uninit();
    }
    if(GlobalExit)
        getch();
    return 0;
}


void IoExport2Script()
{
    SQClassDef<LevelMan>("LevelMan").
                func(&LevelMan::SetupAddrs,"SetupAddrs").
                func(&LevelMan::CheckLocally,"CheckLocally").
                func(&LevelMan::Download,"Download").
                func(&LevelMan::GetResourceCount,"GetResourceCount").
                func(&LevelMan::GetResourceAt,"GetResourceAt").
                func(&LevelMan::QueryMaster,"QueryMaster").
                func(&LevelMan::GetServerAt,"GetServerAt");


    SQClassDef<GameServer>("GameServer").
                        func(&GameServer::IsAlive, "IsAlive").
                        func(&GameServer::SetLocalIP, "SetLocalIP").
                        func(&GameServer::SetMasterServer, "SetMasterServer").
                        func(&GameServer::SetMapsLocation, "SetMapsLocation").
                        func(&GameServer::SetMaxTimePerLevel, "SetMaxTimePerLevel").
                        func(&GameServer::SetMaximFrameRate, "SetMaximFrameRate").
                        func(&GameServer::SetWorkingFolder, "SetWorkingFolder").
                        func(&GameServer::SetThreadCount, "SetThreadCount").
                        func(&GameServer::SetPoolSize, "SetPoolSize").
                        func(&GameServer::SetProxy, "SetProxy").
                        func(&GameServer::Stop, "Stop").
                        func(&GameServer::StartServer, "Start").
                        func(&GameServer::ConfigServer, "ConfigServer").
                        func(&GameServer::ChangeLevel, "ChangeLevel").
                        func(&GameServer::Register2Master,"Register2Master").
                        func(&GameServer::GetLevelManager,"GetLevelManager").
                        func(&GameServer::GetCurrentVersion, "GetCurrentVersion");

}

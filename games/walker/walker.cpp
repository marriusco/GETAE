//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "WalkerGame.h"

char* sq_includes[256] = {
        {"scripts/"},
        {"../"},
        {"../scripts/"},
        0,
};
  TCHAR* tex_includes[256] ={
                                    {_T("")},
                                    {_T("scripts/")},
                                    {_T("res/")},
                                    {_T("../res/")},
                                    {_T("../scripts/")},
                                   };



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if(ul_reason_for_call==DLL_PROCESS_ATTACH)
    {
        
    }
    else if( ul_reason_for_call==DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}

WalkerGame* PGame;



__declspec(dllexport) long  GameProc(ISystem* ps, long m, long wp, long lp)
{
    return PGame->Proc(ps,m, wp, lp);
}


__declspec(dllexport) const NotifyProc  main(ISystem* psystem, 
                                             HINSTANCE appInstance, 
                                             const TCHAR* cmdline)
{
    BeamTree::Export2DLL(appInstance, (System*)psystem);
    PGame = new WalkerGame(psystem, cmdline);
    return GameProc;
}



#pragma comment(lib, "../../_lib/extern/jpg.lbb")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Advapi32.lib") 

// these are the libs
#ifdef _DEBUG
    #pragma comment(lib, "baselibd.lib")
    #pragma comment(lib, "systemd.lib") 
    #pragma comment(lib, "gebsplibd.lib")
    #pragma comment(lib, "sqstdlibd.lib")
    #pragma comment(lib, "squirreld.lib")
    #pragma comment(lib, "sqplusd.lib")
#else
    #pragma comment(lib, "baselib.lib")
    #pragma comment(lib, "system.lib") 
    #pragma comment(lib, "gebsplib.lib")
    #pragma comment(lib, "sqstdlib.lib")
    #pragma comment(lib, "squirrel.lib")
    #pragma comment(lib, "sqplus.lib")
#endif //

int _imp__strtod;
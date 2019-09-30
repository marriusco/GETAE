// stdafx.cpp : source file that includes just the standard includes
//	geticnetsrv.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <stdarg.h>

// TODO: reference any additional headers you need in STDAFX.H

extern  long  Trace_Level; //all levels    

void    MyPrintf(long level, const char* pfmt,...)
{
    if((level & Trace_Level) == level)
    {
	    va_list args;
	    va_start(args, pfmt);
	    char szBuffer[1024];
        ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char), pfmt, args);
        printf(szBuffer);
	    va_end(args);
    }

}


#pragma comment(lib, "../../_lib/extern/jpg.lbb")
//----------------------------------------------------------------------------------------
// these are  vor VC8 express eddition
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Advapi32.lib") 

//
#ifdef _DEBUG
    #pragma comment(lib, "sqstdlibd.lib")
    #pragma comment(lib, "squirreld.lib")
    #pragma comment(lib, "sqplusd.lib")
    #pragma comment(lib, "baselib8d.lib")
    #pragma comment(lib, "systemd.lib") 
    #pragma comment(lib, "gebsplibd.lib")
#else
    #pragma comment(lib, "sqstdlib.lib")
    #pragma comment(lib, "squirrel.lib")
    #pragma comment(lib, "sqplus.lib")
    #pragma comment(lib, "baselib.lib")
    #pragma comment(lib, "system.lib") 
    #pragma comment(lib, "gebsplib.lib")
#endif //

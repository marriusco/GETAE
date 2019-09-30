//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include ".\utils.h"


extern System*     PGs;
extern BOOL        Exception;

//---------------------------------------------------------------------------------------
// modifyed std library to allow include search path in the script 
TCHAR* sq_includes[256] = {
                            {_T("./")},
                            {_T("scripts/")},
                            {_T("../")},
                            {_T("../scripts/")},
                            0,
};

// include search ddir for textures manager
  TCHAR* tex_includes[256] ={
                                    {_T("")},
                                    {_T("scripts/")},
                                    {_T("res/")},
                                    {_T("../res/")},
                                    {_T("../scripts/")},
                                   };

//---------------------------------------------------------------------------------------
void printfunc(HSQUIRRELVM v, const SQChar *lpszFormat, ...) 
{ 
    va_list args;
    va_start(args, lpszFormat);
    static char szBuffer[800];
    unsigned int nBuf = ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char), lpszFormat, args);
    va_end(args);
#ifdef _DEBUG
    printf(szBuffer);
#else
    if(PGs && !Exception)
    {
        PGs->Printf(szBuffer);
    }
    else
    {
    #ifdef _CONSOLE
        printf(szBuffer);
    #else
        ::OutputDebugStringA(szBuffer);
    #endif //
    }
#endif //
} 

//----------------------------------------------------------------------------------------
BOOL    FindFile(const TCHAR* filename)
{
    char    loco[256];
    
    long    hFile;
    struct _finddata_t fileFndData;

    strcpy(loco, filename);

    if(strchr(loco,'\\') || strchr(filename,'/'))
    {
        hFile = _findfirst( loco, &fileFndData );
        if(hFile != -1)
        {
            _findclose(hFile);
            return 1;
        }
        char    temp[256];
        char    fnm[256];
        char    ext[128];
        _splitpath( loco, temp, temp, fnm, ext );
        sprintf(loco,"%s%s",fnm, ext );
    }



    for(int i=0; sq_includes[i]; ++i)
    {
        hFile    = _findfirst( MKSTR("%s%s",sq_includes[i],loco), &fileFndData );
        if(hFile != -1)
        {
            _findclose(hFile);
            return 1;
        }
    }
    return 0;
}
    
                
                


//----------------------------------------------------------------------------------------
string SquirrelExecuteMain(const char* script)
{
    SquirrelObject main = SquirrelVM::CompileScript(_SC(script));
    SquirrelVM::RunScript(main);
    SquirrelFunction<const SQChar*> mainFunction("Main");
    return mainFunction(PGs);
}


//----------------------------------------------------------------------------------------
// these are  vor VC8 express eddition
#pragma comment(lib, "../../_lib/extern/jpg.lbb")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Advapi32.lib") 

// these are the libs
#ifdef _DEBUG
    #pragma comment(lib, "sqstdlibd.lib")
    #pragma comment(lib, "squirreld.lib")
    #pragma comment(lib, "sqplusd.lib")
    #pragma comment(lib, "baselibd.lib")
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

int _imp__strtod;

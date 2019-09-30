//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdarg.h> 
#include <stdio.h>
#include <IO.h>             //findfile
#include <conio.h> 
#include "objbase.h"
#include "baseutils.h"
#include "beamtree.h"
#include "../system/system.h"
#include "../system/levelman.h"

BOOL    FindFile(const TCHAR* filename);
void    ExportFileBrowser();
void    Rebase();
void    printfunc(HSQUIRRELVM v, const SQChar *lpszFormat, ...) ;
string  SquirrelExecuteMain(const char* script);

class Initializer
{
public:
    Initializer(){
        CoInitialize(0);
        SquirrelVM::Init();
        sq_setprintfunc(SquirrelVM::GetVMPtr(), printfunc); //sets the print function

    }
    ~Initializer(){
       //CoUninitialize();      // this causes assertion failure
       SquirrelVM::Shutdown();
    }

    BOOL    Initializer::Rebase()
    {
        char    redBuff[256] = {0};
        HKEY hk=0;
        HKEY hKey=0;


        getcwd(redBuff, 256);
        char* pls = &redBuff[strlen(redBuff)-1];
        pls--;
        while(pls != redBuff){
            if(*pls=='\\')break;
            --pls;
        }
        
        if(!strcmp(pls,"\\_bin"))
        {
            printf("atach extension gs and gll\n");
            if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GETIC_3D_300\\getic\\DIR",0, KEY_READ, &hk))
            {
                AtachGsExtension(".gs");
                AtachGsExtension(".gll");

                if(::RegCreateKey(HKEY_CURRENT_USER, "Software\\GETIC_3D_300\\getic\\DIR", &hKey) == ERROR_SUCCESS)
	            {
		            ::RegSetValueEx(hKey, "HOME", 0, REG_SZ, (CONST BYTE*)redBuff, (lstrlen(redBuff) + 1) * sizeof(TCHAR));
		            ::RegCloseKey(hKey);
	            }
            }
            if(hk)
                ::RegCloseKey(hk);
            return TRUE;
        }
        
        BOOL rv=FALSE;
      
        if(ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GETIC_3D_300\\getic\\DIR",0, KEY_READ, &hk))
        {
            DWORD   cbData = 256;

            ::memset(redBuff,0 , sizeof(redBuff));
            if(::RegQueryValueEx(hk, "HOME", NULL,NULL, (LPBYTE)redBuff, &cbData) ==ERROR_SUCCESS)
            {
                redBuff[cbData]=0;
                chdir(redBuff);
                getcwd(redBuff,256);
                rv = TRUE;
            }
            ::RegCloseKey(hk);
        }
        if(FALSE == rv)
            printf("Run at least once moge.exe from it's home directory \r\n");
        return rv;
    }

    void AtachGsExtension(const TCHAR* extens)
    {
        TCHAR   pathNoSlash[_MAX_PATH];
        getcwd(pathNoSlash, _MAX_PATH);
        tstring  csAppName   = MKSTR("%s\\moge.exe", pathNoSlash) ;
        tstring  icIdx       = MKSTR(",%d", 1);

        tstring defCl = csAppName;
        defCl         += icIdx;

        tstring strFilterExt    = extens, 
                strFileTypeId   = "DGScript", 
                strFileTypeName = "DScript";

        tstring strTemp = MKSTR("%s\\DefaultIcon", strFileTypeId.c_str());
        _RegSetKey(strTemp.c_str(), defCl.c_str());

        LONG lSize = _MAX_PATH * 2;

        strTemp = MKSTR("%s\\Shell", strFilterExt.c_str());
        _RegSetKey(strTemp.c_str(), "", 0);

        strTemp = MKSTR("%s\\Shell\\open", strFilterExt.c_str());
        _RegSetKey(strTemp.c_str(), "", 0);

        strTemp = MKSTR("%s\\Shell\\open\\command\\", strFilterExt.c_str());
        csAppName+=" \"%1\"";
        _RegSetKey(strTemp.c_str(), csAppName.c_str(), 0);

        strTemp = MKSTR("%s\\OpenWithList\\'%s'", strFilterExt.c_str(), "moge.exe");
        _RegSetKey(strTemp.c_str(), "", 0);

        strTemp = MKSTR("%s\\Def", strFilterExt.c_str());
        _RegSetKey(strTemp.c_str(), csAppName.c_str(), 0);



    }

    void _RegSetKey(LPCTSTR lpszKey, 
                            LPCTSTR lpszValue, 
                            LPCTSTR lpszValueName=0)
    {
	    if (lpszValueName == 0)
	    {
		    ::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,lpszValue, lstrlen(lpszValue) * sizeof(TCHAR));
            return ;
	    }
	    HKEY hKey;
	    if(::RegCreateKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS)
	    {
		    ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ,(CONST BYTE*)lpszValue, (lstrlen(lpszValue) + 1) * sizeof(TCHAR));
		    ::RegCloseKey(hKey);
	    }
    }
};




//UINT* URPcmds;
/*
#pragma comment (linker,"/subsystem:windows")
#ifdef _CONSOLE
	#undef _CONSOLE
#endif //
#ifndef _WINDOWS
	#define _WINDOWS
#endif //
*/

#define LOOP()  while(1)
#endif //


// xmoge.cpp : Implementation of DLL Exports.

#ifndef _NIS_

#include "stdafx.h"
#include "resource.h"



// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer 
// to be automatically implemented for you

[ module(dll, uuid = "{F1F221C5-0263-4CC6-BF6D-BDB3DAC0286B}", 
		 name = "xmoge", 
		 helpstring = "xmoge 1.0 Type Library",
		 resource_name = "IDR_XMOGE") ]
class CxmogeModule
{
public:
// Override CAtlDllModuleT members
};
		 


TCHAR* tex_includes[256] ={ _T(""),
                            _T("common/"),
                            0
                          };


#else


#include <stdio.h>
#include <direct.h>
#include "baselib.h"

#include "windows.h"


extern HINSTANCE   G_dllInstance;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    G_dllInstance = (HINSTANCE)hModule;
    return TRUE;
}



#include <atlbase.h>    

static mutex       __mutex;
STDAPI DllRegisterServer()
{
    
    char cwd[_MAX_PATH];
    char sdll[_MAX_PATH];
    char sxpt[_MAX_PATH];

    _getcwd(cwd, sizeof(cwd)-1);
    sprintf(sdll,"%s\\npxmoge.dll",cwd);
    sprintf(sxpt,"%s\\npxmoge.xpt",cwd);

    CRegKey crk;
    if (crk.Open(HKEY_CLASSES_ROOT, "http\\shell\\open\\command") == ERROR_SUCCESS)
    {
        DWORD cbData = 0;
        char  pszValue[_MAX_PATH] = {0};
        char  command[_MAX_PATH] = {0};
        DWORD lchars = _MAX_PATH;
        if (crk.QueryStringValue("", pszValue, &lchars) == ERROR_SUCCESS)
        {
            char* pfireFox = ::strstr(pszValue,"FIREFOX.EXE");
            if(0 == pfireFox)
                pfireFox = ::strstr(pszValue,"firefox.exe");
            if(pfireFox)
            {
                *pfireFox = 0;
                
                char dest[_MAX_PATH];
                sprintf(dest,"%splugins\\npxmoge.dll",pszValue);
                CopyFile(sdll,dest,0);

                sprintf(dest,"%splugins\\npxmoge.xpt",pszValue);
                CopyFile(sxpt,dest,0);
                

            }
        }
        crk.Close();
    }




    if(0 == crk.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins"))
    {
        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net");
        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net\\MogePlayer");

        crk.SetStringValue("Description","MOGE Player 1.0");
        crk.SetStringValue("Path", sdll);
        crk.SetStringValue("ProductName","MOGE Player");
        crk.SetStringValue("Vendor","Zalsoft Inc.");
        crk.SetStringValue("Version","1.0");
        crk.SetStringValue("XPTPath", sxpt);



        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net\\MogePlayer\\MimeTypes");

        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net\\MogePlayer\\MimeTypes\\application\\xmoge-gbt");

        crk.SetStringValue("Description","XMoge BSP Tree");
        crk.SetStringValue("Suffixes","gbt");


        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net\\MogePlayer\\MimeTypes\\application\\xmoge-ml");
        crk.SetStringValue("Description","XMoge BSP XML Tree");
        crk.SetStringValue("Suffixes","ml");


        crk.Create(HKEY_LOCAL_MACHINE,"SOFTWARE\\MozillaPlugins\\@getic.net\\MogePlayer\\Suffixes");
        crk.SetStringValue("gbt","");
        crk.SetStringValue("ml","");

        crk.Close();
        return 0;
    }

    



    

    return -1;
}


STDAPI DllUnregisterServer()
{
    return -1;
}


TCHAR* tex_includes[256] ={ (""),
                            ("common/"),
                            0
                          };



#endif //




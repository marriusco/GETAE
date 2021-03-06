//============================================================================
// Author: Octavian Marius Chincisan 2003 - 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "stdafx.h"
#include "dxsound.h"

#pragma comment(lib, "dxguid.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C"
{
    EX_IN Isound*	CreateInstance()
    {
	    return new DxSound(); 
    }

    EX_IN void DestroyInstance(Isound* pm)
    {
        delete pm;
    }

    EX_IN const char* GetCatName(void)
    {
        return "sound,DirectX8 Sound";
    }
};
//============================================================================
// Author: Octavian Marius Chincisan 2003 - 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "stdafx.h"
#include "alsound.h"


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
	    return new AlSound(); 
    }

    EX_IN void DestroyInstance(Isound* pm)
    {
        delete pm;
    }

    EX_IN const char* GetCatName(void)
    {
        return "sound,OpenAL Sound";
    }
};
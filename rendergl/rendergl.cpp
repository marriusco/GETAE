//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "openglrender.h"



//---------------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
    return TRUE;
}


extern "C"
{
    EX_IN Irender*	CreateInstance()
    {
	    return new OpenglRender(); 
    }

    EX_IN  void	DestroyInstance(Irender* pr)
    {
	    delete pr;
    }

    EX_IN const char* GetCatName(void)
    {
        return "render,Open GL";
    }

};

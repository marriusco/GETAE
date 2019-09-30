//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "_isystem.h"
#include "rendersys.h"


BOOL RenderSys::Create(ISystem* pe, const TCHAR* plugname, HINSTANCE hi)
{
	if(p_render.Load(plugname)){
		return p_render->CreateRender(pe, &s_rndStruct, a_prims);
	}
	return 0;
}

void    RenderSys::Destroy()
{
    if(p_render.Interface()){
        p_render->Destroy();
        p_render.Free();
    }
}


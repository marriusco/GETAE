//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __RENDER_SYS_H__
#define	__RENDER_SYS_H__

#include "_irender.h"
#include "_isystem.h"
class ISystem;

//------------------------------------------------------------------------------------------
typedef UINT    PRIM_TYPE[256];
class NO_VT RenderSys
{
public:
	RenderSys(){TRACEX("RenderSys()");}
	~RenderSys(){TRACEX("~RenderSys()");}

    int 		 Create(Irender* pRender){p_render.SetIface(pRender);return 0;}
	BOOL		 Create(ISystem* pe, const TCHAR* plugname, HINSTANCE hi);
	void		 Destroy();	
	Irender*	 GetRender(){return p_render.Interface();}
    UINT         Prims(int i){return a_prims[i];}
    BOOL         IsLoaded(){return p_render.Interface()!=0;}
public:
    RndStruct	 s_rndStruct;                 
    PRIM_TYPE	 a_prims;
private:	

	PlugInDll<Irender>  p_render;					// plugin
};


//------------------------------------------------------------------------------------------

#endif // 

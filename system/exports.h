//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef _EXPORTS_H_
#define _EXPORTS_H_

    #include "sqplus.h"
    using namespace SqPlus;
#include "system.h"



DECLARE_ARRAY_TYPE(char,64,VAR_TYPE_STRING); // StartStruct

DECLARE_INSTANCE_TYPE(UV)
static int UVCtor(REAL x, REAL y, HSQUIRRELVM v)
{
    return PostConstruct<UV>(v, new UV(x,y), ReleaseClassPtr<UV>::release);
}


DECLARE_INSTANCE_TYPE(V3)
static int V3Ctor(REAL x, REAL y, REAL z,  HSQUIRRELVM v)
{
    return PostConstruct<V3>(v, new V3(x,y,z), ReleaseClassPtr<V3>::release);
}

DECLARE_INSTANCE_TYPE(StartStruct);
DECLARE_INSTANCE_TYPE(CLR)
DECLARE_INSTANCE_TYPE(M4)
DECLARE_INSTANCE_TYPE(Vtx)

DECLARE_INSTANCE_TYPE(RenderVx)
static int VxCtor(int cap, int what,  HSQUIRRELVM v)
{
    return PostConstruct<RenderVx>(v, new RenderVx(cap, what), ReleaseClassPtr<RenderVx>::release);
}

DECLARE_INSTANCE_TYPE(RndStruct);



DECLARE_INSTANCE_TYPE(Box)
DECLARE_INSTANCE_TYPE(Pos)
DECLARE_INSTANCE_TYPE(Plane)
DECLARE_INSTANCE_TYPE(UIInputMap)
DECLARE_INSTANCE_TYPE(Htex)
DECLARE_INSTANCE_TYPE(Itape)
DECLARE_INSTANCE_TYPE(Irender)
DECLARE_INSTANCE_TYPE(SystemData);
DECLARE_INSTANCE_TYPE(RenderLight);
DECLARE_INSTANCE_TYPE(RenderMaterial);
DECLARE_INSTANCE_TYPE(RenderFog);



DECLARE_INSTANCE_TYPE(Camera);
DECLARE_INSTANCE_TYPE(ICamera);
DECLARE_INSTANCE_TYPE(System);
DECLARE_INSTANCE_TYPE(RECT);
DECLARE_INSTANCE_TYPE(POINT);
DECLARE_INSTANCE_TYPE(SIZE);
DECLARE_INSTANCE_TYPE(UIMan);
DECLARE_INSTANCE_TYPE(UiControl);
DECLARE_INSTANCE_TYPE(GServer);
DECLARE_INSTANCE_TYPE(LevelMan);
DECLARE_INSTANCE_TYPE(FileBrowser);


void S_Export2Script();

#endif ////_EXPORTS_H_
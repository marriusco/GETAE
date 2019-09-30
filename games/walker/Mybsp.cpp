//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include ".\mybsp.h"
#include ".\entity.h"
#include ".\scene.h"
#include ".\entactor.h"
#include ".\entlhallo.h"
#include ".\walkergame.h"


long    MyBSP::_BspProc(long m, long w, long l)
{
    switch(m)
    {
        case BM_COLLIDEMODEL://moving model into scene
            PScene->RunModelCollision((BspModel*) w, (SystemData*) l);
            break;
        case BM_READITEM:
            _HandleReadItem((const BTF_Item*)w);
            break;

        case BM_READCATEGORY:
            break;
    }
    return 0;
}

void    MyBSP::_HandleReadItem(const BTF_Item* pitem)
{
    switch(pitem->_type)
    {
        case ITM_LIGTBULB:
            _CreateLightHallo(pitem);
            break;
        case ITM_TRIGER: 
            //coming next build
            break;
        case ITM_SNDSOURCE:
            //coming next build
            break;
        case ITM_STARTPLAY:
            _CreateActor(pitem);
            break;
    }
}

void    MyBSP::_CreateActor(const BTF_Item* pActor)
{
    static UINT NActors = 0;
    EntActor* pe = new EntActor(pActor);
    if(pe)
    {
        if(0==NActors)
        {
            pe->SetCamera(Psys->GetCamera());
            pe->PsyFlags() |= ACK_CAM|ACK_INPUT;
            PScene->SetMe(pe);
        }
        ++NActors;
    }
}

void    MyBSP::_CreateLightHallo(const BTF_Item* pLight)
{
    EntLHallo* pe = new EntLHallo(pLight);
}

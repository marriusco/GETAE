//============================================================================
// Author: Octavian Marius Chincisan March 2007
// Zalsoft Inc 1999-2007
//============================================================================

#include "stdafx.h"
#include ".\entity.h"
#include ".\meshes.h"
#include ".\scene.h"
#include ".\walkergame.h"
#include ".\entlhallo.h"


EntLHallo::EntLHallo(const BTF_Item* pItem):Entity(pItem, ENT_LIGHTS)
{
    const BTF_LightBulb& lb = pItem->_ligtbulb;

    _radius         = lb._radius;            
    _halloRadMin    = lb._halloRadMin;   
    _halloRadMax    = lb._halloRadMax;   
    _mimDist        = lb._mimDist;
    _maxDist        = lb._maxDist;
    _lmIntensity    = lb._lmIntensity;
    _color          = CLR(lb._colorD[0],lb._colorD[1],lb._colorD[2],128);
}

EntLHallo::~EntLHallo()
{
}


void    EntLHallo::Render(Scene* pscene, const SystemData* psy)
{
    this->_scale = V3(100,100,100);
    this->_trFlags = OBJ_BB_SPH|OBJ_SCALE|OBJ_TRANSLATE;
    Irender* pr  = Psys->Render();
    Vtx*     pvx = MakeQuad(V0, 1, 1, 'z');
   
    //pr->DisableTextures(1);
    
    
    DWORD howwhat = Z_POLYGON|VX_TX1|DS_BL;
    
    //howwhat |= pr->BindAllTextures(_textures, _texCombine);
    pr->Color(this->_color);
    pr->Render(this, _trFlags, DS_BLEND_LIGHT, &_textures[0], _texCombine, pvx, 4, howwhat);


}

void    EntLHallo::Animate( Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx)
{

}



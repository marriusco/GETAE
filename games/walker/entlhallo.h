//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once
#include "entity.h"

class EntLHallo : public Entity
{
public:
    
    EntLHallo(const BTF_Item* pItem);
    virtual ~EntLHallo(void);

    void    Animate( Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx);
    void    Render(Scene* pscene, const SystemData* psy);
private:
    int  _radius;        
    int  _halloRadMin;   
    int  _halloRadMax;   
    int  _mimDist;       
    int  _maxDist;       
    int  _lmIntensity;
    CLR  _color;

};

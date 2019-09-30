#pragma once

#include "entity.h"
#include "beamtree.h"

class MovingLight :  public Entity//, public DPool<MovingLight>
{
public:
    MovingLight(){}
    MovingLight(Scene* pscene, PointLight* ppl, V3& pos, V3& dir, REAL sp, REAL rad, CLR& color, REAL intens, REAL ttl);
    virtual ~MovingLight(void);

    void    Animate( Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx);
    void    Render(Scene* pscene, const SystemData* psy);
    PointLight* p_pl;
    int         _ttl;
};

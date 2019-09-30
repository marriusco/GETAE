
#include "system.h"
#include "scene.h"
#include "movinglight.h"

MovingLight::MovingLight(Scene* pscene, 
                         PointLight* ppl, V3& pos, V3& dir, REAL sp, REAL rad, CLR& color, REAL intens, REAL ttl):Entity(0,ENT_OBJECTS)
{
    p_pl          = ppl;
    _pos = ppl->_pos     = pos;
    this->_fwd   = dir;
    this->_speed = sp;
    ppl->_radius  = rad;
    ppl->_color   = color;
    _ttl     = ttl;
    ppl->_flags   = LIGHT_POINT|LIGHT_STEADY;
}

MovingLight::~MovingLight(void)
{
    
}


void    MovingLight::Animate( Scene* pscene, const Camera* pov, 
                              const SystemData* psy, int camLeaf, int thisIdx)
{
    _leaf = pscene->Tree()->GetCurrentLeaf(_pos);

    _visible = pscene->Tree()->IsLeafInPVS(_leaf) && 
               ((Camera*)pov)->CanSeeCamera(this->_pos, p_pl->_radius);
    if(_visible)
    {
         pscene->Tree()->SetDynaLightAttrib(p_pl, _pos, p_pl->_radius,  p_pl->_intens, p_pl->_color);
    }
    _pos   = _pwpos + (_fwd*_speed) * 0.4;
    _pwpos = _pos;
    _ttl   -= 1;
    Impact ip;
    V3 df = _pos + _fwd * 16;
    if(pscene->Tree()->SegmentIntersect(ip, _pos, df))
    {
        ReflectVector(_fwd, ip._plane._n,1,_fwd);
    }


    if(this->_ttl <= 0)
    {
        pscene->Tree()->ReleaseDynaLight(p_pl);
        delete this;
    }
}

void    MovingLight::Render(Scene* pscene, const SystemData* psy)
{
    
}


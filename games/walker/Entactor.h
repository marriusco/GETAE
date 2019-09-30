//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once
#include "entity.h"
#include "Meshes.h"
//#include "../../an8loader/An8Model.h"

class EntActor : public Entity
{
public:
    
    EntActor(const BTF_Item* pItem);
    virtual ~EntActor(void);

    void    Animate( Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx);
    void    Render(Scene* pscene, const SystemData* psy);
    BOOL    OnCollided(Impact*);
    void    Respawn(){_pos = _spawnpos;}
    void    SetRespawnPos(const V3& v){_spawnpos = v;}
    void    RunCollision(Scene* pscene, const SystemData* psy, DWORD leafcontent, Impact& ip);
private:
    void    _Collide(Scene* pscene, const SystemData* psy, DWORD leafcontent, Impact& ip);
    BOOL    _ClimbStep(Scene* pscene,const V3& upgrav, REAL,REAL, V3&, int);
    void    _ReadInput(Scene* pscene, const SystemData* psy);
    V3      _spawnpos;


    //An8Model*   p_an9model;
    Mesh    *p_mesh;
};

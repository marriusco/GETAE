//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once

#include "_irender.h"
#include "system.h"
#include "beamtree.h"
//#include "../../an8loader/An8Model.h"

typedef enum _ENT_TYPES
{
    ENT_ACTORS  = 0,    // 
    ENT_OBJECTS,           // 
    ENT_SMOKES,         // ONE TEXTURE, ONE BLEND
    ENT_LIGHTS,         // ONE TEXTURE, ONE BLEND
    ENT_LAST,
}ENT_TYPES;
struct EntRndStruct
{
    DWORD       blend;
    Htex        texId;
};
class Entity;
class Scene
{
public:
    typedef PtrArray<Entity*>             PEntities;

    Scene(void);
    ~Scene(void);

    virtual int     Animate(Camera* pov, SystemData* psy);
    virtual int     Render(SystemData* psy, DWORD how);
    virtual int     LoadLevel(const TCHAR* );
    virtual void    RunModelCollision(BspModel* pModel, SystemData* sd);
    virtual void    OnKey(int keyupdn, DWORD keycode);
    virtual Entity* Insert(const BTF_Item*,Entity*);
    virtual void    Remove(const EKEY&);
    const   PEntities*   Entities(){return  _onscreen;}
    BeamTree* Tree(){return _tree;}
    static long     BspProc(BeamTree*, long m, long w, long l);
    void            SetMe(Entity* pme){_pMe = pme;}
    BeamTree*       BspTree(){return _tree;}
    int             FillLights(RenderLight** pl, int nLeaf, const V3& vpos);
#ifdef _DEBUG
    void    AddIpoint(const V3& ip, const V3& nrm);//_ipoints
    void    RenderIpoints();
#endif //
private:
    
    void        _RenderGroup(const ENT_TYPES& grp, PEntities* pes, SystemData* psy);
    void        _DeleteEntities();
    long        _BspProc(long m, long w, long l);

private:
    Entity*       _pMe;
    BeamTree*     _tree;
    PEntities     _onscreen[ENT_LAST];
    V3            _segs;
public:
    V3            _gravacc;
    V3            _segFired[2];

    //PlugInDll<An8Model>     _anmodel;
    vvector<RenderLight*>   _lights;

#ifdef _DEBUG
    vvector<V3>   _ipoints;          
#endif //
};

extern Scene*  PScene;
extern System* Psys;


DECLARE_INSTANCE_TYPE(Scene);

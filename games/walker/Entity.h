//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once

#include "baselib.h"
#include "scene.h"

//---------------------------------------------------------------------------------------
// flags
#define ACK_INPUT   0x1     // input atached to this entity
#define ACK_CAM     0x2     // camera atached to this entity

//---------------------------------------------------------------------------------------
struct Impact;
class Scene;
class Entity : public Pos 
{
public:
    friend class Scene;
    typedef enum _POSTURE
    {
        POSTURE_FLOATING,
        POSTURE_ONFLOOR,
    }POSTURE;
public:
    Entity(){}
    Entity(const BTF_Item* pItem, ENT_TYPES e);
    ~Entity();

    virtual void    Animate( Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx);
    virtual void    Render(Scene* pscene, const SystemData* psy);
    virtual BOOL    OnCollided(Impact*);
    virtual void    Respawn(){};
    virtual void    PushByModel(Scene* pscene, BspModel* pModel, SystemData* sd);
    virtual void    RunCollision(Scene* pscene,const SystemData* psy, DWORD leafcontent, Impact& bi);

    BOOL    IsVisible(){return _visible;}
    EKEY    Key(){return _uid;}

    Camera*  GetCamera()const {return _pcam;}
    void     SetCamera( Camera* pc);
    void     IncrementError(){++_err;}
    V3&      Velocity(){return _velocity;}
    V3&      Rotacity(){return _rotacity;}
    REAL&    Speed(){return _speed;}
    V3&      PreviosPos(){return _pwpos;}
    int&     Leaf(){return _leaf;}
    BOOL&    Visible(){return _visible;}
    long&    FrameCount(){return  _frame;}
    DWORD&   CollisionBits(){return _colbitset;}
    DWORD&   PsyFlags(){return  _psysflags;}
    DWORD&   GlFlags(){return  _trFlags;}
    Entity::POSTURE& Posture(){return _posture;}
    Box&     AaBox() {return _bbox;}
    int&     Model(){return _nbspmodel;}
    DWORD&   Environment(){return _env;}
    REAL     Mass(){return _mass;}
    ENT_TYPES Type(){return _type;}
    void     ColideandSlide(Scene* pscene, V3& a, V3& b, const Box& box);
    void     KickSpeed(const V3& speed);
protected:
    ENT_TYPES   _type;
    POSTURE     _posture;
    REAL        _speed;
    REAL        _mass;
    int         _leaf; 
    BOOL        _visible;
    long        _frame;
    DWORD       _colbitset;
    DWORD       _psysflags;
    DWORD       _trFlags;
    int         _nbspmodel;
    DWORD       _env;
    Camera*     _pcam;
    int         _err;
    EKEY        _uid;
    V3          _velocity;
    V3          _rotacity;
    V3          _pwpos;
    Box         _bbox;
    Htex        _textures[4];
    DWORD       _texCombine;

};

DECLARE_INSTANCE_TYPE(Entity);
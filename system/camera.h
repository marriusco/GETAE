//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once

#include "_isystem.h"
#include "baselib.h"

#define CAM_ATTACHED 1
class Entity;
class Camera : public ICamera
{
public:
    Camera(){_ud=0;}
    int         Animate(SystemData*);
    BOOL		CanSee(const Box& box, int useIntersection=-1)const;
    BOOL		CanSeeCamera(const V3& center,  REAL rad);
    REAL        Fov(){return _fov;}
    void        SetPos(const V3& eye, const V3& euler);
    void        SetPosMat(const M4& vmatrix);
    void        LookAt(const V3& eye, const V3& up, const V3& fwd, const V3& rght);
    void        SetFarAndFov(REAL rfar, REAL rfov);
    const M4&	ViewMatrix();
    REAL        GetFinderHeight();
    REAL        GetFinderWidth();
    REAL        GetFinderDepth();
    void        SetUserData(void* p){_ud = p;};
    void*       GetUserData(){return _ud;};
    
private:
    void        _Attach2Entity(Entity* ud){_ud=ud;};
    void		_AddFrustrumPlan(const V3& a,const V3& b,const V3& c){_hulls[_ihulls++].CalcNormal(a,b,c);}
    void		_AddFrustrumPlan(const V3& norm, const V3& ponpl){_hulls[_ihulls++].Set(norm,ponpl);}
    void		_AddFrustrumPlan(const Plane& plane){_hulls[_ihulls++]=plane;}
    void        _Update(REAL farPlane);

private:
    M4          _matrix;
    REAL        _farp;
    REAL        _fov;
    Plane		_hulls[32];		
    int			_ihulls;		
    V3			_viewfdots[5];	
    V3          _finderDim;
    void       *_ud;
};

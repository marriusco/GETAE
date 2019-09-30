//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "system.h"
#include ".\camera.h"



//---------------------------------------------------------------------------------------
void  Camera::SetPos(const V3& pos, const V3& ypr)
{
    _euler = ypr;
    ROLLPI(_euler.y);
    BLOCKRANGE(_euler.x);	
    Euler2Pos();
    _pos  = pos;
    _Update( _farp);
}

//---------------------------------------------------------------------------------------
void  Camera::SetPosMat(const M4& vm)
{
    this->_pos.x   = -vm._m[12] ; this->_pos.y  = -vm._m[13];    this->_pos.z  =-vm._m[14];
    this->_fwd.x   = -vm._m[12] ; this->_fwd.y  = -vm._m[6] ;    this->_fwd.z  =-vm._m[10] ;
    this->_up.x    =  vm._m[1]  ; this->_up.y   = vm._m[5];      this->_up.z   = vm._m[9];
    this->_right.x =  vm._m[0]  ; this->_right.y= vm._m[4];      this->_right.z= vm._m[8];
    _Update( _farp);
}

//---------------------------------------------------------------------------------------
void  Camera::LookAt(const V3& eye,const  V3& up,const  V3& fwd, const V3& rght)
{
    M4 mla = MLook(eye, fwd, up, rght);
    SetPosMat(mla);
    _Update(_farp);
}

//---------------------------------------------------------------------------------------
void  Camera::SetFarAndFov(REAL rfar, REAL rfov)
{
    _fov  = rfov;
    _farp = rfar;

    PSystem->DrawProps()->fFar = rfar;
    PSystem->GetRenderSys()->GetRender()->RefreshModelView(rfov);
}

//---------------------------------------------------------------------------------------
void Camera::_Update( REAL farPlane)
{
	_ihulls=0;
    _viewfdots[0] = _pos;

#pragma message ("make this by aspect")
    REAL heightFar = farPlane * 0.6713f;  
    REAL widthFar  = farPlane * 0.8820f;   

    // far translation matrix. where to translate far dots
    const REAL tfx = farPlane * _fwd.x;
    const REAL tfy = farPlane * _fwd.y;
    const REAL tfz = farPlane * _fwd.z;
    const REAL wfx = widthFar*_right.x;
    const REAL wfy = widthFar*_right.y;
    const REAL wfz = widthFar*_right.z;
    const REAL hfx = heightFar*_up.x;
    const REAL hfy = heightFar*_up.y;
    const REAL hfz = heightFar*_up.z;
    
    _viewfdots[4].x = _pos.x + tfx + wfx + hfx;
    _viewfdots[4].y = _pos.y + tfy + wfy + hfy;
    _viewfdots[4].z = _pos.z + tfz + wfz + hfz;
    _viewfdots[3].x = _pos.x + tfx + wfx - hfx;
    _viewfdots[3].y = _pos.y + tfy + wfy - hfy;
    _viewfdots[3].z = _pos.z + tfz + wfz - hfz;
    _viewfdots[2].x = _pos.x + tfx - wfx - hfx;
    _viewfdots[2].y = _pos.y + tfy - wfy - hfy;
    _viewfdots[2].z = _pos.z + tfz - wfz - hfz;
    _viewfdots[1].x = _pos.x + tfx - wfx + hfx;
    _viewfdots[1].y = _pos.y + tfy - wfy + hfy;
    _viewfdots[1].z = _pos.z + tfz - wfz + hfz;

    _AddFrustrumPlan(_fwd, _pos);				                    // near
    _AddFrustrumPlan(_viewfdots[1],_viewfdots[2],_viewfdots[3]); // far
    _AddFrustrumPlan(_viewfdots[1],_viewfdots[0],_viewfdots[2]); // left
    _AddFrustrumPlan(_viewfdots[4],_viewfdots[3],_viewfdots[0]); // right
    _AddFrustrumPlan(_viewfdots[3],_viewfdots[2],_viewfdots[0]); // down
    _AddFrustrumPlan(_viewfdots[1],_viewfdots[4],_viewfdots[0]); // up
}


//---------------------------------------------------------------------------------------
// Test if sphere is inside. Assume sphere is small and frust big so dont do the corners
BOOL	Camera::CanSeeCamera(const V3& point,  REAL rad)
{
    for(int i=0;i<6;++i)
    {
        if(_hulls[0].DistTo(point.x,point.y,point.z) < -rad)
            return 0;
    }
    return 1;
}

//---------------------------------------------------------------------------------------
// Test if bounding box is in viewing fruatrum(dont do the corners)
BOOL	Camera::CanSee(const Box& box, int useIntersection)const
{
    #pragma message("tranform box mM into proj matrix and test AABB")
    if(useIntersection==-1) //auto
    {
        useIntersection = (box.GetMaxExtend() > 3200); //32 meters/feets
    }
    useIntersection =0;
    if(useIntersection==1)
    {
        V3 ct = box.GetCenter();
        V3 ex = box.GetExtends() * .50000;
        for(int i=0; i<6; i++)
        {
            const Plane& pl = _hulls[i];
            // find average radius in respect with theplane
            REAL  offset = Rabs(pl._n.x*ex.x) + Rabs(pl._n.y*ex.y) + Rabs(pl._n.z*ex.z);
            REAL  dist   = pl.DistTo(ct) + offset;
            if(dist < 0)return 0;
        }
        return 1;
    }

    const V3& am = box._min;
    const V3& aM = box._max;
    for(int i=0; i<6; i++)
    {
		if(_hulls[i].DistTo(am.x,am.y,am.z) > 0) continue;
        if(_hulls[i].DistTo(aM.x,am.y,am.z) > 0) continue;
        if(_hulls[i].DistTo(am.x,aM.y,am.z) > 0) continue;
        if(_hulls[i].DistTo(aM.x,aM.y,am.z) > 0) continue;
        if(_hulls[i].DistTo(am.x,am.y,aM.z) > 0) continue;
        if(_hulls[i].DistTo(aM.x,am.y,aM.z) > 0) continue;
        if(_hulls[i].DistTo(am.x,aM.y,aM.z) > 0) continue;
        if(_hulls[i].DistTo(aM.x,aM.y,aM.z) > 0) continue;
        return 0;
    }
    return 1;
}

//--------------------------------------------------------------------------------------------
const M4& Camera::ViewMatrix()
{
    return _matrix;
}

//--------------------------------------------------------------------------------------------
int  Camera::Animate(SystemData* psd)
{
    if(0 == _ud)
    {
        this->MoveAndRot(psd->_pMouse,800.0 * psd->_ticktime, psd->_ticktime/4);
    }
    Euler2Pos();
    _Update(_farp);
    _matrix = MLook(_pos, _fwd, _up, _right);
    _finderDim.y = psd->_pRndStruct->fHeight;
    _finderDim.x = psd->_pRndStruct->fWidth;
    _finderDim.z = psd->_pRndStruct->fNear;
    return 1;
}

//--------------------------------------------------------------------------------------------
REAL        Camera::GetFinderHeight()
{
    return _finderDim.y;
}

//--------------------------------------------------------------------------------------------
REAL        Camera::GetFinderWidth()
{
    return _finderDim.x;
}

REAL        Camera::GetFinderDepth()
{
    return _finderDim.z;
}


#include "stdafx.h"
#include "entity.h"
#include ".\scene.h"
#include ".\walkergame.h"


//---------------------------------------------------------------------------------------
Scene*  PScene = 0;
static const REAL LOCOEPS = (1.0000/32.0000);
static const REAL ONEEPS  = (1.0000 + LOCOEPS);
static const REAL  STEPH  =  40.0;

//---------------------------------------------------------------------------------------
static void S_ClipVect(V3&  vin, 
                      V3    vn, 
                      REAL  atten,
                      V3&   vout) 
                     
{
    V3 vvout = vin - vn * (Vdp(vin,vn) * atten);
    vout     = vvout;
    if(Rabs(vvout.x) < LOCOEPS) vvout.x = 0;
    if(Rabs(vvout.y) < LOCOEPS) vvout.y = 0;
    if(Rabs(vvout.z) < LOCOEPS) vvout.z = 0;
}

//---------------------------------------------------------------------------------------
Scene::Scene(void):_tree(0)
{
    assert(0==PScene);
    PScene = this;
    _gravacc = V3(0,1,0) * -981;
}

//---------------------------------------------------------------------------------------
Scene::~Scene(void)
{
    _DeleteEntities();
    delete _tree;
    PScene = 0;
}

//---------------------------------------------------------------------------------------
int Scene::LoadLevel(const TCHAR* pfile)
{
    assert(_tree==0);
    _tree = new MyBSP();
    if(_tree->LoadLevel(pfile)!=NO_ERROR)
    {
        delete _tree;
        _tree = 0;
        return -1;
    }

    return NO_ERROR;
}

//---------------------------------------------------------------------------------------
int Scene::Physics(Camera* pov, SystemData* psy)
{
    int camLeaf = _tree->Physics(pov, psy);
    _PhysicsActors(&_onscreen[ENT_ACTORS], pov, psy, camLeaf);
    return 1;
}

//---------------------------------------------------------------------------------------
int Scene::Render(SystemData* psy, DWORD how)
{
    _tree->Render(psy, Z_POLYGON);
    _RenderActors(&_onscreen[ENT_ACTORS], psy);
    RenderIpoints();
    return 1;
}

//---------------------------------------------------------------------------------------
void    Scene::_PhysicsActors(PEntities* pes, 
                              Camera* pov, 
                              const SystemData* psy, 
                              int camLeaf)
{
    int         elements = pes->Count();
    Entity*     pent;
    while(--elements >= 0)
    {
        pent = pes->At(elements);
        pent->Physics(pov, psy, camLeaf);
    }
}

//---------------------------------------------------------------------------------------
void    Scene::_RenderActors(PEntities* pes, SystemData* psy)
{
    int         elements = pes->Count();
    Entity*     pent;
    while(--elements >= 0)
    {
        pent = pes->At(elements);
        if(pent->IsVisible())
        {
            pent->Render(psy);
        }
    }
}

//---------------------------------------------------------------------------------------
Entity* Scene::Insert(const ENT_TYPES& et, Entity* pe)
{
    pe->_uid._et   = et;
    Entity** ppe   = _onscreen[et].Push(pe);
    pe->_uid._pidx = &pe->_index;
    return *ppe;
}

//---------------------------------------------------------------------------------------
void Scene::Remove(const EKEY& key)
{
    _onscreen[key._et].RemoveIdx(*(key._pidx));
}

//---------------------------------------------------------------------------------------
void Scene::_DeleteEntities()
{
    for(int e = ENT_ACTORS; e != ENT_LAST; ++e)
    {
        int         elements = _onscreen[e].Count();
        Entity*     pent;
        while(--elements >= 0)
        {
            pent = _onscreen[e].At(elements);
            delete pent;
        }
    }
}

//---------------------------------------------------------------------------------------
BOOL Scene::_ApplySceneDynamics(Entity* pe, const SystemData* psy)
{
    
    pe->_rotacity    *=.5;
    pe->_velocity.x  *= .8;
    pe->_velocity.z  *= .8;

    if(::IsZero(PScene->_gravacc))
        pe->_velocity.y  *= .8;

    else if(pe->_posture == Entity::POSTURE_FLOATING)
        pe->_velocity  += (PScene->_gravacc * psy->_ticktime);

    BOOL nullit = 1;
   
    if(pe->_velocity.len() < 1.0 )
    {
        pe->_velocity.reset();
        return 0;
    }
   if(pe->_posture == Entity::POSTURE_ONFLOOR)
            pe->_posture = Entity::POSTURE_FLOATING;    
    return 1;
}

//---------------------------------------------------------------------------------------
BOOL Scene::_TryMoveUpStep(const V3& n, Entity* pe, const V3& upgrav, REAL time)
{
    V3      prevPos = pe->_pos;
    
    Box     pb      = pe->_bbox;
    Impact* ip2;

    //
    // rise it up and mve it forward. as \
    //
    V3      dirvel   = pe->_velocity; dirvel.norm();
    V3      uppos    = pe->_pos + upgrav * STEPH;
    V3      fwdpos   = pe->_pos + (dirvel * 8.0); 
    fwdpos  -= (VY*4);                    
    ip2 = _tree->ObjectIntersect(uppos, fwdpos, pe->_bbox);
    if(ip2->_hit && Vdp(n, upgrav)>.7)
    {
        pe->_pos     = ip2->_ip + n; // 1 unit up
        pe->_posture = Entity::POSTURE_ONFLOOR;
        return 1;
    }
    return 0;

}

//---------------------------------------------------------------------------------------
void Scene::RunCollision(BspModel* pModel, SystemData* sd)
{
    Box  eb;
    V3   entityPos;
    V3   moddepl = pModel->_speed * sd->_ticktime;
    REAL depl    = min(8.0, moddepl.len());

    for(int e = ENT_ACTORS; e != ENT_LAST; ++e)
    {
        int         elements = _onscreen[e].Count();
        Entity*     pent;
        while(--elements >= 0)
        {
            pent = _onscreen[e][elements];
            
            if(!pent->_visible) continue;

            entityPos = pModel->ToModelSpace(pent->_pos);
            eb = pent->_bbox;
            eb.MakeMoveBbox(entityPos, entityPos, depl);
            
            if(eb.IsTouchesBox(pModel->_bx) && pent->_nmodel==0)
            {
                pent->_nmodel = pModel->_index;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
BOOL Scene::RunCollision(Entity* pe, const SystemData* psy, DWORD onleaf)
{
    if(!_ApplySceneDynamics(pe, psy))
    {
        return 0;
    }
    if(onleaf==0)    
    {
        return _BasicColidePlanes(pe, psy, onleaf);
    }
    return _ColideWithBSPFaces(pe, psy, onleaf);
}

//---------------------------------------------------------------------------------------
BOOL Scene::_BasicColidePlanes(Entity* pe, const SystemData* psy, DWORD w2c)
{
    Impact* pi;
    V3      planes[5];
    V3      grvect = _gravacc;
    int     planesCount = 0;
    V3&     vel = pe->_velocity;
    V3&     pos = pe->_pos;
    V3      savedpos = pe->_pos;
    int     tests = 5;

    pe->_nmodel  = 0;

    while(--tests)
    {
        V3  vStart = pos;

        V3   vEnd = vStart + psy->_ticktime * vel;
        pi    = _tree->ObjectIntersect(vStart, vEnd, pe->_bbox);
        
        if(pe->_nmodel==0)
            pe->_nmodel  = pi->_model;

        if(pi->_hit==0)
        {
            pos = pi->_ip;
            break;
        }
#ifdef _DEBUG
        AddIpoint(pi->_ip, pi->_plane._n);
#endif //_DEBUG

        // floor V
        if(Vdp(pi->_plane._n, -grvect)>.7f)
        {
            pe->_posture = Entity::POSTURE_ONFLOOR;
        }
        //  /_  \_ |_ are steps
        if(grvect.len2() && Rabs(Vdp(pi->_plane._n, grvect)) < .2)
        {
            if(_TryMoveUpStep(pi->_plane._n, pe, -grvect, psy->_ticktime))
            {
                continue;
            }
        }
        planes[planesCount] = pi->_plane._n;
        ++planesCount;
        for(int i=0; i< planesCount;i++)
        {
            ::S_ClipVect(vel, planes[i], 1.0, vel);

            for(int j=0;j<planesCount;j++)
            {
                if(j!=i)
                {
                    if(Vdp(vel, planes[j]) < 0)
                    {
                        break;
                    }
                }
                if(j == planesCount)
                    break;
            }
            if(i != planesCount)
            {
                if(i)
                    S_ClipVect(vel, planes[0], 1.0, vel);
            }
            else
            {
                if(planesCount != 2)
                {
                    vel.reset();
                    pos = savedpos;
                    break;
                }
		        V3   vdir = Vcp(planes[0], planes[1]);
		        REAL dist = Vdp(vdir, vel);
		        vel = vdir * dist;
            }
        }
    }

    pe->_env     = pi->_env;
    pe->_visible = _tree->IsLeafInPVS(pi->_leaf);

    return 1;
}

//---------------------------------------------------------------------------------------
BOOL Scene::_ColideWithBSPFaces(Entity* pe, 
                                const SystemData* psy, 
                                DWORD w2c)
{
    V3   vStart   = pe->_pos;
    V3   vEnd     = vStart + psy->_ticktime * pe->_velocity;
    V3   fwd      = vEnd - vStart;  fwd.normalize();
    Box  bx       = pe->_bbox;
    REAL extend   = vdist(vStart, vEnd);

    bx.MakeMoveBbox(vStart, vEnd, extend);

    Impact2* pi  = _tree->GetPotentialyColidingFaces(fwd, bx, w2c);
    if(pi->_faces.Size())
    {
        _ColideFaces(pe, pi, fwd, psy->_ticktime);
    }
    else if(pi->_planes.Size())
    {
        _ColidePlanes(pe, pi, fwd, psy->_ticktime);
    }
    else
    {
        pe->_pos = vEnd;
    }
    
    DWORD content = _tree->GetContent(pe->_pos,0);
    if(content & CONT_CANTGO)
    {
        pe->_pos = pe->_pwpos;
        pe->_velocity.reset();
    }
    else
    {
        pe->_pwpos = pe->_pos;
    }
    

    return 1; 
}

//---------------------------------------------------------------------------------------
BOOL Scene::_ColideFaces(Entity* pe, Impact2* pi, const V3& fwd, REAL time)
{
    return 1;
}

//---------------------------------------------------------------------------------------
BOOL Scene::_ColidePlanes(Entity* pe, Impact2* pi, const V3& fwd, REAL time)
{
    V3    grvect = _gravacc;
    int   check  = 5;
    V3    vSafe  = pe->_pos;
    V3    vEnd;
    V3    vStart = pe->_pos;
    V3    exBox  = pe->_bbox.GetExtends() * .50000;
    BOOL  hit = 0;
    REAL  bestDepl =INFINIT;
    V3    bestIP;
    V3    velocity = pe->_velocity;
    Plane bestPlane;
    BspModel* pModelT=0;

    do{
        hit      = 0;
        vEnd     = pe->_pos + velocity * time;
        bestDepl = INFINIT;

        for(int i=0 ;i< pi->_planes.Size();i++)
        {
            Plane  plane = pi->_planes[i];
            int    model = plane._u;

            pModelT = 0;
            if(model)
            {
                BspModel* pModel =  _tree->GetModel(model);
                if(pModel->_props & MODEL_DYNAMIC)
                {
                    vEnd    = pModel->ToModelSpace(vEnd   );
                    vStart  = pModel->ToModelSpace(vStart);
                    pModelT = pModel;
                }
            }
            BOOL   ipSet = 0;
            REAL   mr = fabs(exBox.x*plane._n.x)+fabs(exBox.y*plane._n.y)+fabs(exBox.z*plane._n.z);
            REAL   dS = plane.DistTo(vStart)- mr;
            REAL   dE = plane.DistTo(vEnd)  - mr;
            V3     iP;  // intersection point 
            REAL   depl;
            if(dS < 0 && dE < dS) // behind should not happen
            {
                //ip is on normal dir at mr
                iP   = vStart + (plane._n * dS * ONEEPS);
                depl = 0;
                ipSet = 1;
            }
            else if(dS >= 0 && dE < 0)
            {
                REAL ratio  = (dS-LOCOEPS)/(dS-dE);
                CLAMPVAL(ratio, 0.0000, 1.0000);
                iP     = vStart + (vEnd-vStart) * ratio;
                depl   = vdist(vStart, iP);
                ipSet = 1;
            }
            if(0==ipSet)
                continue;
                
#ifdef _DEBUG
             AddIpoint(iP, plane._n);
#endif //
            // floor V
            if(Vdp(plane._n, -grvect) > .7000f)
            {
                pe->_posture = Entity::POSTURE_ONFLOOR;
            }

            //  /_  \_ |_ are steps
            if(grvect.len2() && Rabs(Vdp(plane._n, grvect)) < .2)
            {
                if(_TryMoveUpStep(plane._n, pe, -grvect, time))
                {
                    continue;
                }
            }
            if(depl < bestDepl)
            {
                bestPlane   = plane;
                bestDepl    = depl;
                bestIP      = iP;
                if(pModelT)
                {
                    pModelT->UnTransformVertex(bestIP);
                    pModelT->UnTransformPlane(bestPlane, bestIP);
                }
                hit  = 1;
            }
        }
        if(hit)
        {
            S_ClipVect(velocity, bestPlane._n, 1, velocity) ;
        }
    }while(hit && --check);

    if(0==check) //all slides happen
    {
        pe->_velocity =- pe->_velocity;
        pe->_pos = vSafe;
        return 0;
    }
    pe->_velocity = velocity;
    if(hit)    // hit set
        pe->_pos      = bestIP;
    else    
        pe->_pos      = vEnd;

    return 0;
}

//=======================================================================================
#ifdef _DEBUG
void Scene::AddIpoint(const V3& ip, const V3& nrm)
{
    static V3 prev;
    if(prev == ip)return;
    prev=ip;
    _ipoints.push_back(ip);
    _ipoints.push_back(ip+nrm*16);
    if(_ipoints.size()>128)
    {
        _ipoints.erase(_ipoints.begin());
        _ipoints.erase(_ipoints.begin());
    }
}

//=======================================================================================
void Scene::RenderIpoints()
{
    Camera* pc = Psys->GetCamera();
    _segFired[0]=pc->_pos + pc->_up * (-pc->GetFinderHeight()) +
                 pc->_right * pc->GetFinderWidth();

    _segFired[1]=pc->_pos + pc->_fwd * 32000;
    Impact* pi = _tree->SegmentIntersect(_segFired[0], _segFired[1]); 
    Irender* pr  = Psys->Render();
    pr->EnableBlend(1);

    if(pi->_hit)
    {
        _segFired[1]=pi->_ip;
        pr->Color4(255,255,0,200);
        
        pr->BeginPrim(Z_LINES);
        V3 fromn = _segFired[1] + pi->_plane._n *4;
        pr->Vertex3(fromn.x, fromn.y, fromn.z);
        fromn = _segFired[1] + pi->_plane._n *64;
        pr->Vertex3(fromn.x, fromn.y, fromn.z);
        pr->End();

    }

    pr->Color4(255,0,255,200);
    vvector<V3>::iterator pb  = _ipoints.begin();
    vvector<V3>::iterator ep = _ipoints.end();  
    pr->BeginPrim(Z_LINES);
    for(;pb!=ep;)
    {
        pr->Vertex3(pb->x, pb->y, pb->z);        ++pb;
        pr->Vertex3(pb->x, pb->y, pb->z);        ++pb;
    }
    pr->End();

    pr->EnableBlend(0);
    
}
#endif//

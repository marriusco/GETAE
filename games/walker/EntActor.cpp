//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "entity.h"
#include "meshes.h"
#include "scene.h"
#include "walkergame.h"
#include "entactor.h"
#include "an8model.h"
#include "MovingLight.h"

extern DWORD __HowToCollide;

static const REAL LOCOEPS = (1.0000/32.0000);
static const REAL ONEEPS  = (1.0000 + LOCOEPS);
static const REAL  STEPH  =  64.0;
DWORD              __HowToCollide = COL_BASIC; 


EntActor::EntActor(const BTF_Item* pItem):Entity(pItem,ENT_ACTORS)
{
    this->_spawnpos = pItem->_pos;
    this->_speed = 200; //2 m/s
    this->_mass  = 100; //100k
    p_mesh = new Cylinder(64,180,8,8); 

   _bbox.AddPoint(V3(-32,-90,-32));
   _bbox.AddPoint(V3(32,90,32));

}

EntActor::~EntActor()
{
    delete p_mesh;
}

void    EntActor::Animate( Scene* pscene,
                          const Camera* pov, 
                          const SystemData* psy, 
                          int camLeaf, int thisIdx)
{
    Impact im; 
    if(_psysflags & ACK_INPUT)
    {
         this->_ReadInput(pscene, psy);
    }
    this->_rotacity    *=.5;
    this->_velocity.x  *= .8;
    this->_velocity.z  *= .8;
    this->_nbspmodel   = 0;

    if(::IsZero(pscene->_gravacc))
    {
        this->_velocity.y  *= .8;   // 
    }

    else if(this->_posture == Entity::POSTURE_FLOATING)
    {
        this->_velocity  += (pscene->_gravacc * psy->_ticktime);
    }
    BOOL nullit = 1;
    if(this->_velocity.len2()<.2)
    {
        this->_velocity.reset();
        return ;
    }
    
    int nmodel = this->_nbspmodel;

    RunCollision(pscene, psy, __HowToCollide, im);

    if(this->_nbspmodel && nmodel != this->_nbspmodel)
    {
        BspModel* pModel = pscene->Tree()->GetModel(this->_nbspmodel);
        pscene->Tree()->TriggerMove(pModel);
    }
}
 

void EntActor::RunCollision(Scene* pscene, const SystemData* psy, DWORD leafcontent, Impact& ip)
{
    if(this->_posture == Entity::POSTURE_ONFLOOR)
    {
        this->_posture = Entity::POSTURE_FLOATING;    
    }

    this->_Collide(pscene, psy, __HowToCollide, ip);

    if(this->_colbitset & CONT_CANTGO)
    {
        this->_pos = this->_pwpos;
        this->_velocity.reset();
    }
    else 
    {
        this->_pwpos = this->_pos;
    }
}

void    EntActor::Render(Scene* pscene, const SystemData* psy)
{
    if(_psysflags & ACK_CAM)
        return;//
    //this->_scale = V3(30,160,30);
    this->_trFlags = OBJ_ROTATE_EX|OBJ_TRANSLATE|OBJ_SCALE;

    Irender* pr  = Psys->Render();
       

    RenderLight* plghts[8];
    int nlights = pscene->FillLights((RenderLight**)&plghts, this->_leaf, this->_pos);
    if(nlights)
    {
        pr->EnableRenderMaterialLighting(0x303030,1);
        pr->SetLights((const RenderLight**)&plghts, nlights);
    }

    
    pr->Push();
        pr->Translate(this->_pos);
        pr->BindTexture(this->_textures[0],0);
        
        pr->Render(p_mesh->i_strps, 
                   p_mesh->i_grps, 
                   p_mesh->p_prims, 
                   p_mesh->p_strips, 
                   p_mesh->p_pnorms,
                   p_mesh->p_uvs,
                   0,
                   p_mesh->p_idxes);
        
    pr->Pop();

    if(nlights)
        pr->EnableRenderMaterialLighting(0,0);

    //Entity::Render(psy);
}

BOOL    EntActor::OnCollided(Impact* ip)
{
    return Entity::OnCollided(ip);
}

void EntActor::_ReadInput(Scene* pscene, const SystemData* psy)
{
    if(_psysflags & ACK_CAM)
    {

        const int* pr   = (psy->_pMouse);
        V3         fwd  = _fwd;

        if(pscene->_gravacc.len())
        {
            fwd.y   = 0;
            fwd.norm();
        }

        if(pr[2]) 
            _velocity += fwd   * _speed * pr[2];
        if(pr[0]) 
            _velocity += _right * _speed * pr[0];

        if(pscene->_gravacc.len()==0)
        {
            if(pr[1]) 
                _velocity += _up    * _speed * pr[1];
        }

        if(psy->_pKeys[KA_JUMP])
        {
            if(_posture == POSTURE_ONFLOOR)
                _velocity.y = 800;
            //_velocity += fwd * 1000;
        }
/*
        if(psy->_pKeys[KA_FIRE1])
        {
            PointLight* ppl = pscene->Tree()->GetDynaLight();
            if(ppl)
            {
                new MovingLight(pscene, ppl, _pos, this->_fwd, 130.0, 90, CLR(rand()%255,rand()%255,255), 50, 250);
            }
        }
  */

        
        if(pr[3]) 
            _rotacity.x = pr[3] * .01;
        if(pr[4]) 
            _rotacity.y = pr[4] * .01;
        if(pr[5]) 
            _rotacity.z = pr[5] * .01;

        Rotate( _rotacity.x, _rotacity.y, _rotacity.z);

        Pos p = (const Pos&)*this;
        (Pos&)*(Psys->GetCamera()) = p;
        Psys->GetCamera()->_pos.y += 65;
        Psys->GetCamera()->Euler2Pos();
    }

}


//---------------------------------------------------------------------------------------
void EntActor::_Collide(Scene* pscene, const SystemData* psy, DWORD leafcontent, Impact& ip)
{
    REAL ratio = 1.0;
    V3   vStart = this->_pos;
    V3   vEnd;
    V3   pVs = vStart;
    V3   grvect = pscene->_gravacc;
    grvect.norm();
    int i;

    this->_nbspmodel = 0;

    for( i=0;i<4;i++)
    {
        vEnd = vStart + psy->_ticktime * this->_velocity;
        REAL   dmax = vdist(vEnd, vStart);
    
        if(pscene->BspTree()->ObjectIntersect(ip, vStart, vEnd, 0,1, this->_bbox))
        {
            //pscene->AddIpoint(vStart, ip._ip);
            ratio       = (dmax - ip._dist) / dmax;
            vStart      = ip._ip; // + (ip._plane._n * .2);

            if(this->_nbspmodel==0)
            {
                this->_nbspmodel  = ip._model;
            }

            if(Vdp(ip._plane._n, -grvect) > .7f) // pretty horizontal
            {
                this->_posture = Entity::POSTURE_ONFLOOR;
            }

            //  /_  \_ |_ are steps. we hit a vetical. see if is a step
            // better be a detail model marked as a step
            if(grvect.len2() && Rabs(Vdp(ip._plane._n, grvect)) < .2 && ratio)
            {
                if(_ClimbStep(pscene, -grvect, psy->_ticktime, ratio, vStart, ip._nplane))
                {
                    continue;
                }
            }
            ClipVector(this->_velocity, ip._plane._n, ratio, this->_velocity);
            if(this->_velocity.len2() == 0)
            {
                vEnd = vStart;
                break;
            }
        }
        else break;
    }
    if(4==i)
    {
        this->_velocity.reset();
        vEnd =  pVs;
    }
    this->_pos = vEnd;

#ifdef _DEBUG
    int leaf1 = pscene->BspTree()->GetCurrentLeaf(vStart);
    int leaf2 = pscene->BspTree()->GetCurrentLeaf(vEnd);
    if(leaf1==-1 && leaf2==-1)
    {
        TRACEX("");
    }
     //pscene->AddIpoint(vStart, this->_pos);
#endif //_DEBUG
    this->_env       = ip._env;
    this->_visible   = pscene->BspTree()->IsLeafInPVS(ip._leaf);
    this->_colbitset = pscene->BspTree()->GetLeafContent(ip._leaf);

    return ;
}

//---------------------------------------------------------------------------------------
BOOL EntActor::_ClimbStep(Scene* pscene, const V3& upgrav, 
                          REAL time, REAL ratio, V3& newPos, int iplane)
{
    V3      prevPos = this->_pos;
    Box     pb      = this->_bbox;

    //
    // see if can nove up ^^^^
    //
    Impact  ip2;
    V3      dirvel   = this->_velocity; dirvel.norm();
    V3      nextPos    = this->_pos + upgrav * STEPH;

    if(0==pscene->BspTree()->ObjectIntersect(ip2, prevPos, nextPos, 0, 1, this->_bbox))// ^
    {
        //
        // now forward  >>>>>>
        //
        //pscene->AddIpoint(prevPos,nextPos );

        prevPos =  nextPos;
        nextPos += this->_velocity * time * ratio + this->_fwd;

        if(0==pscene->BspTree()->ObjectIntersect(ip2, prevPos, nextPos, 0, 1, this->_bbox))
        {
            //
            // place it down VVVVVVV
            //
            //pscene->AddIpoint(prevPos,nextPos );
            prevPos =  nextPos;
            nextPos += upgrav * -(STEPH + 8.0);
            if(pscene->BspTree()->ObjectIntersect(ip2, prevPos, nextPos, 0, 1, this->_bbox)) /// VVVV
            {
                newPos         = ip2._ip;// + ip2._plane._n; // 1 unit up

                //pscene->AddIpoint(prevPos,newPos );

                if(Vdp(ip2._plane._n, upgrav) > .7)
                {
                    this->_posture = Entity::POSTURE_FLOATING;
                    
                    return 1;
                }
            }
        }
    }
    return 0;
}



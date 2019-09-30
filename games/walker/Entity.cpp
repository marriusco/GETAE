//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include ".\entity.h"
#include ".\meshes.h"
#include ".\scene.h"


//---------------------------------------------------------------------------------------
Entity::Entity(const BTF_Item* pe, ENT_TYPES e):_type(e),
                                        _posture(POSTURE_FLOATING),
                                        _speed(0),
                                        _mass(1),
                                        _leaf(-1), 
                                        _visible(1),
                                        _frame(-1),
                                        _colbitset(0),
                                        _psysflags(0),
                                        _trFlags(OBJ_ROTATE_EX|OBJ_TRANSLATE),
                                        _nbspmodel(0),
                                        _env(0),
                                        _pcam(0),
                                        _err(0)
{
    _bbox.AddPoint(V3(-32,-80,-32));
    _bbox.AddPoint(V3(32,80,32));
    this->_bbox.AddPoint(V3(-32,-80,-32));
    this->_bbox.AddPoint(V3(+32,+80,+32));

    if(pe)
    {
        this->_pos    = pe->_pos;
        this->_pwpos  = pe->_pos;
        this->_euler  = pe->_dir;
        Irender* pr  = Psys->Render();
        UINT* prims = pr->GetPrimitives();

        for(int i=0; i < 5 ;i++)
        {
            if(!_Noval(pe->_texp.textures[i]))
            {
                _textures[i].hTex      = pe->_texp.textures[i];
                _textures[i].glTarget  = prims[Z_TEXTURE_2D];//GL_TEXTURE_2D
                _textures[i].envMode   = 0;
                _textures[i].genST     = pe->_texp.texApply[i];
            }
        }
        _texCombine = pe->_texp.combine;
    }
    PScene->Insert(pe, this);
}

//---------------------------------------------------------------------------------------
Entity::~Entity()
{
    if(_pcam)
       _pcam->SetUserData(0);
    PScene->Remove(this->_uid);
}


//---------------------------------------------------------------------------------------
void    Entity::Animate( Scene* pscene,
                        const Camera* pov, 
                        const SystemData* psy, 
                        int camLeaf, int thisIdx)
{
     if(_frame == psy->_physFrame)
        return;
    _frame = psy->_physFrame;
 
}

//---------------------------------------------------------------------------------------
void    Entity::Render(Scene* pscene, const SystemData* psy)
{
    if(_psysflags & ACK_CAM)
        return;

    Irender* pr  = Psys->Render();
    Vtx*     pvx = MakeQuad(V0, 1, 1, 'z');
    
    pr->DisableTextures(1);
    //pr->Scale(V3(30,160,30));

    pr->Color4(255,255,255,255);
    
    pr->Render(this, _trFlags, pvx, 4, Z_POLYGON|VX_TX1);

    pr->BeginPrim(Z_LINES);
    pr->Vertex3(_pos.x, _pos.y, _pos.z);
    V3 fw = _pos + _fwd*120;
    pr->Vertex3(fw.x, fw.y, fw.z);
    pr->End();

}

//---------------------------------------------------------------------------------------
BOOL    Entity::OnCollided(Impact* pi)
{
    return 0;
}

//---------------------------------------------------------------------------------------
void     Entity::SetCamera( Camera* pc)
{
    Camera* poc = _pcam;
    if(_pcam = pc) 
    {
        _psysflags |= ACK_CAM ;
        pc->SetUserData(this);
    }
    else
    {
        if(poc)
            poc->SetUserData(0);
        _psysflags &= ~ACK_CAM; 
    }
}

//---------------------------------------------------------------------------------------
void    Entity::PushByModel(Scene* pscene, BspModel* pModel, SystemData* sd)
{
    REAL fdepl = pModel->_depl.len2();
    if(!IsZero(fdepl))
    {

        Impact ip;
        // model moves the entity from this->_pos to pModel->_depl
        // and also vel's it
        V3 from = this->_pos;
        V3 to = from + pModel->_depl;
        
        ColideandSlide(pscene, from, to, this->_bbox);
        this->_pos = to;

        ip.Reset();
        pscene->Tree()->SegmentIntersect(ip,this->_pos, this->_pos+(VY*-this->_bbox.GetMaxExtend()));
        if(ip._hit && ip._model==pModel->_index && this->_nbspmodel)
        {
            this->_nbspmodel = pModel->_index;
            //KickSpeed(pModel->_depl / sd->_ticktime);
        }

        ip.Reset();
        RunCollision(pscene, sd, COL_BASIC, ip);
    }
}

void    Entity::RunCollision(Scene* pscene,const SystemData* psy, DWORD leafcontent, Impact& ip)
{
    V3 vStart = this->_pos;
    V3 vEnd   = vStart + psy->_ticktime * this->_velocity;
    
    this->_pwpos = this->_pos;
    pscene->Tree()->ObjectIntersect(ip, vStart, vEnd, 0,1, this->_bbox);
    this->_pos = ip._ip;
}


void     Entity::ColideandSlide(Scene* pscene, V3& a, V3& b, const Box& box)
{
    V3      s = a;
    V3      e = b;
    V3      v = b - a;
    REAL    dmax,ratio;
    Impact  ip;
 
    for(int i=0;i<4;i++)
    {
        dmax = vdist(a, b);
    
        if(pscene->BspTree()->ObjectIntersect(ip, s, e, 0,1, box))
        {
            ratio  = (dmax - ip._dist) / dmax;
            s      = ip._ip; 

            ClipVector(v, ip._plane._n, ratio, v);

            if(v.len2()<0.002)
            {
                e = s;
                break;
            }
            e = s + v * ratio;
        }
        else break;
    }
    return ;
}

void Entity::KickSpeed(const V3& speed)
{
    this->_velocity += speed;
}

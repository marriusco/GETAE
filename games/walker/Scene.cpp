//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "entity.h"
#include ".\scene.h"
#include ".\mybsp.h"
using namespace baseg;

//---------------------------------------------------------------------------------------
Scene*  PScene = 0;

//---------------------------------------------------------------------------------------
Scene::Scene(void):_tree(0)
{
    assert(0==PScene);
    PScene = this;
    _pMe   = 0;
    _gravacc = V3(0,1,0) * -981;

    //_anmodel.Load("an8loader.dll");
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
    Psys->SetHudMode(HUD_TRACE|HUD_HELP);
    return NO_ERROR;
}


//---------------------------------------------------------------------------------------
int Scene::Animate(Camera* pov, SystemData* psy)
{
    //
    // apply BSP phisics . moving models to entities
    //
    int camLeaf = _tree->Animate(pov, psy);
    
    //
    // apply objs to actors
    //
    Entity*    pent;
    int        index = _onscreen[ENT_OBJECTS].Count();
    while(--index>-1)
    {
        pent = _onscreen[ENT_OBJECTS].At(index);
        pent->Animate(this, pov, psy, camLeaf, index);
    }
    
    index = _onscreen[ENT_SMOKES].Count();
    while(--index>-1)
    {
        pent = _onscreen[ENT_SMOKES].At(index);
        pent->Animate(this, pov, psy, camLeaf, index);
    }

    index = _onscreen[ENT_LIGHTS].Count();
    while(--index>-1)
    {
        pent = _onscreen[ENT_LIGHTS].At(index);
        pent->Animate(this, pov, psy, camLeaf, index);
    }

    index = _onscreen[ENT_ACTORS].Count();
    while(--index>-1)
    {
        pent = _onscreen[ENT_ACTORS].At(index);
        pent->Animate(this, pov, psy, camLeaf, index);
    }
    
    return 1;
}

//---------------------------------------------------------------------------------------
int Scene::Render(SystemData* psy, DWORD how)
{
    _tree->Render(psy, Z_POLYGON);
    for(int i=0; i < ENT_LAST; i++)
    {
        if(_onscreen[i].Count())
        {
            _RenderGroup((ENT_TYPES&)i,&_onscreen[i], psy);
        }
    }

#ifdef _DEBUG
    RenderIpoints();
#endif
    return 1;
}


//---------------------------------------------------------------------------------------
void    Scene::_RenderGroup(const ENT_TYPES& grp, PEntities* pes, SystemData* psy)
{
    int         index = pes->Count();
    Entity*     pent;
    while(--index >= 0)
    {
        pent = pes->At(index);
        if(pent->IsVisible())
        {
            pent->Render(this, psy);
        }
    }
}

//---------------------------------------------------------------------------------------
Entity* Scene::Insert(const BTF_Item* pbi, Entity* pe)
{
    pe->_uid._et   = pe->Type();

    if(pe->_uid._et == ENT_LIGHTS)
    {
        BTF_LightBulb* plb = (BTF_LightBulb*)pe;
        RenderLight* prr = new RenderLight();
        ::memset(prr,0,sizeof(RenderLight));
        
        prr->_colorAmbient      = ZBLACK;
        prr->_colorDiffuse.setval(pbi->_ligtbulb._colorD);
        prr->_colorSpecular.setval(pbi->_ligtbulb._colorD);
        prr->_constAttenuation  = 0.001;
        prr->_flags             = LIGHT_POINT;
        prr->_radius            = pbi->_ligtbulb._radius;
        prr->_userdata          = _tree->GetCurrentLeaf(pbi->_pos);
        _lights << prr;
    }


    Entity* ppe   = _onscreen[pe->Type()].Push(pe);
    pe->_uid._pidx = &pe->_index;
    return ppe;
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
        int         index = _onscreen[e].Count();
        Entity*     pent;
        while(--index >= 0)
        {
            pent = _onscreen[e].At(index);
            delete pent;
        }
    }

    FOREACH(vvector<RenderLight*>, _lights, ppl)
        delete *ppl;
}



//---------------------------------------------------------------------------------------
void Scene::RunModelCollision(BspModel* pModel, SystemData* sd)
{
    Box  eb;
    V3   entityPos;
    V3   moddepl = pModel->_speed * sd->_ticktime;
    REAL depl    = min(8.0, moddepl.len());

    for(int e = ENT_ACTORS; e != ENT_LAST; ++e)
    {
        int         index = _onscreen[e].Count();
        Entity*     pent;
        while(--index >= 0)
        {
            pent = _onscreen[e][index];
            
            if(!pent->_visible) continue;

            entityPos = pModel->ToModelSpace(pent->_pos);
            eb = pent->_bbox;
            eb.MakeMoveBbox(entityPos, entityPos, depl);
            eb.Expand(32);
            if(eb.IsTouchesBox(pModel->_bx))
            {
                pent->_nbspmodel = pModel->_index;
                pent->PushByModel(this, pModel, sd);
            }
        }
    }
}


//--------------------------------------------------------------------------------------
void   Scene::OnKey(int keyupdn, DWORD keycode)
{
    static int iindexp = 0;
    if(keyupdn)
    {
        if(keycode == DIK_H)
        {
           if(_pMe)
               _pMe->_pos = V0;
        }
        if(keycode == DIK_GRAVE)
        {
            static bool togle;
            if(togle=!togle)
                Psys->SetHudMode(HUD_TRACE|HUD_HELP);
        }
        if(keycode == DIK_1)
        {
            if(_pMe)
            {
                if(_pMe->GetCamera())
                {
                    _pMe->SetCamera(0);
                    Psys->GetCamera()->SetPos(V0,V0);
                    iindexp = 0;
                }
                else
                {
                    _pMe->SetCamera(Psys->GetCamera());
                }
            }
        }
        if(keycode == DIK_2)
        {
            if(_pMe)
            {
                if(_pMe->GetCamera())
                {
                    _pMe->SetCamera(0);
                }
#ifdef _DEBUG
                if(iindexp < _ipoints.size())
                {
                    V3 s = _ipoints[iindexp++];
                    V3 e = _ipoints[iindexp++];


                    Impact pi;
                    if(BspTree()->ObjectIntersect(pi,s, e, 0,1,_pMe->_bbox))
                    {
                        TRACEX("");
                    }
                    _pMe->_pos = e;
                }
                else
                    iindexp=0;
#endif //_DEBUG
            } 
        }
    }
}

//--------------------------------------------------------------------------------------
int  Scene::FillLights(RenderLight** pl, int nLeaf, const V3& vpos)
{
    int max8 = 0;
    FOREACH(vvector<RenderLight*>, _lights, ppl)
    {
        if((*ppl)->_userdata != nLeaf) continue;
        (*pl++) = *ppl;
        ++max8;
        if(max8 > 7) break;
    }
    return max8;
}

//=======================================================================================
#ifdef _DEBUG
void Scene::AddIpoint(const V3& ip, const V3& nrm)
{
   // static V3 prev;
   // if(prev == ip)return;
   // prev=ip;
    _ipoints.push_back(ip);
    _ipoints.push_back(nrm);
    if(_ipoints.size()>512)
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

    _segFired[1]=pc->_pos + pc->_fwd * 1600;
    Impact im;
    _tree->SegmentIntersect(im, _segFired[0], _segFired[1]); 
   

    Irender* pr  = Psys->Render();
    pr->EnableBlend(1);

    if(im._hit)
    {
        static REAL sp;
        if(sp != im._plane._c)
        {
            sp = im._plane._c;
            Psys->TextOut(3,ZWHITE,"-1 %2.2f,%2.2f,%2.2f / %3.2f",im._plane._n.x,im._plane._n.y,im._plane._n.z,im._plane._c);
        }   
        
        _segFired[1]=im._ip;
        pr->Color4((char)255,(char)255,0,(char)200);
        
        pr->BeginPrim(Z_LINES);
        V3 fromn = _segFired[1] + im._plane._n *4;
        pr->Vertex3(fromn.x, fromn.y, fromn.z);
        fromn = _segFired[1] + im._plane._n *64;
        pr->Vertex3(fromn.x, fromn.y, fromn.z);
        pr->End();

    }

    pr->Color4((char)255,0,(char)255,(char)200);
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


//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "basecont.h"
#include "system.h"
#include "beamtree.h"

#define _ONE_TEX_FOR_DEFLMAP
//---------------------------------------------------------------------------------------
static  REAL            fff=0;			// fake data to move the doors
static  bool            dir=true;		// fake data to move the doors
#define BITAT(buff_, x_)				buff_[x_>>3] & (1<<(x_&7))
static  UINT            Counter;
static  V3		        Campos;     
extern  RndStruct       DrawProps;

//---------------------------------------------------------------------------------------
void   BeamTree::GetNodeExtend(int n, V3& ex)
{
    assert(n>=0);
    ex = _nodes[n]._bbox.GetExtends();
}

//---------------------------------------------------------------------------------------
DWORD	BeamTree::GetLeafContent(int leaf)
{
    if(leaf==-1)
        return CONT_CANTGO;
    return _leafs[leaf]._content;                   // empty leafs content ????
}

//---------------------------------------------------------------------------------------
DWORD	BeamTree::GetContent(const V3& pt, int node)
{
    DWORD lc =  GetLeafContent(GetCurrentLeaf(pt, node));
    if(_models._size)
    {
        BspModel* pModel =  &_models[1];
	    for(int i=1; i< _models._size; ++i, ++pModel)
	    {
            if(pModel->_flags & MODEL_NOCOLIDE || _frmStatic != pModel->_visFrm)
            {
                continue;
            }

            V3     vc2Model = pt;

            // tr the seg in model space
            if(pModel->_props & MODEL_DYNAMIC)
            {
                vc2Model = pModel->ToModelSpace(pt);
            }
            Box movedBox;
            movedBox.MakeMoveBbox(vc2Model,vc2Model);
            if(pModel->_bx.ContainPoint(vc2Model))
            {
                lc |= GetLeafContent(GetCurrentLeaf(pt, pModel->_rootNode));
            }
        }
    }
    return lc;
}

DWORD  BeamTree::GetContent(const Box& box, int node)
{
    V3      vc = box.GetCenter();
    DWORD   content = 0;
    R_GetContent(box, 0, node, content);
    if(_models._size)
    {
        Box movedBox = box;
	    BspModel* pModel =  &_models[1];
	    for(int i=1; i< _models._size; ++i, ++pModel)
	    {
            if(pModel->_flags & MODEL_NOCOLIDE || _frmStatic != pModel->_visFrm)
            {
                continue;
            }

            V3     vc2Model = vc;

            // tr the seg in model space
            if(pModel->_props & MODEL_DYNAMIC)
            {
                vc2Model = pModel->ToModelSpace(vc);
            }
            movedBox.MakeMoveBbox(vc2Model,vc2Model);
            if(movedBox.IsTouchesBox(pModel->_bx))
            {
                content |= pModel->_flags;
                break;
            }
        }
    }
    return content;
}

void  BeamTree::R_GetContent(const Box& box, int model, int nodeIdx, DWORD& content)
{
    assert(nodeIdx>=0);
    BtNode*  pNode = &_nodes[nodeIdx];
    if (pNode->IsLeaf())
	{
        if(pNode->IsSolid())
        {
            content |=  CONT_CANTGO ;
        }
        else
        {
            content  |= _leafs[pNode->_leafIdx]._content;
        }
        return;
	}
    DWORD side  = S_ClassifyBox(_planes[pNode->_planeIdx], box);
    if(side & ON_FRONT)
        R_GetContent(box, model, pNode->_nodesIdx[1], content);
    if(side & ON_BACK)
        R_GetContent(box, model, pNode->_nodesIdx[0], content);
}


DWORD	    BeamTree::R_GetContent(const V3& pt, const V3& ex, DWORD& content, int node)
{
    assert(node>=0);
    BtNode*  pNode = &_nodes[node];
    if (pNode->IsLeaf())
	{
        if(pNode->IsSolid())
            content |=  CONT_CANTGO ;
        return content;
	}
    const  Plane&  plane  = _planes[pNode->_planeIdx];
    REAL   offset  = Rabs(ex.x*plane._n.x)+Rabs(ex.y*plane._n.y)+Rabs(ex.z*plane._n.z) + .2;
    REAL    dist   = plane.DistTo(pt) - offset;
    if(dist>=0)
        return R_GetContent(pt, ex,  content, pNode->_nodesIdx[1]);
    else
        return R_GetContent(pt, ex, content, pNode->_nodesIdx[0]);
}



//---------------------------------------------------------------------------------------
// returns the current leaf that contain current  point
#pragma optimize( "at", on ) 

int	BeamTree::GetCurrentLeaf(const V3& pov, int nodeIdx)
{
    if(_nodes._size==0)return -1;
    // pass the prev leaf and check only the visited visible nodes(populated by FrameParentNodes)
    REAL rdist;
    nodeIdx = 0;
    if(nodeIdx==-1)nodeIdx=0;
    BtNode* pNode = &_nodes[nodeIdx];
    while(!pNode->IsLeaf())
    {
        rdist = _planes[pNode->_planeIdx].DistTo(pov);
        if (rdist <0 ) 
            nodeIdx = pNode->_nodesIdx[0];
		else
            nodeIdx = pNode->_nodesIdx[1];

        pNode = &_nodes[nodeIdx];
    }
    return _nodes[nodeIdx]._leafIdx;
}
#pragma optimize( "at", off ) 


//---------------------------------------------------------------------------------------
BOOL		BeamTree::IsLeafInPVS(int leaf)
{
    if(leaf >= 0 && !_Noval(leaf))
	    return _leafs[leaf]._visFrame == _frmStatic;
    return TRUE; // make it all visible when we are out of bsp
}

//---------------------------------------------------------------------------------------
BOOL		BeamTree::IsLeafVisFromLeaf(int l1, int l2)
{
    if(l1==-1 || l2==-1)
        return 1;
    if(_pPVS)
    {
        BYTE* pPvs1 = &_pPVS[_leafs[l1]._pvs];
        return (pPvs1[l2 >> 3] & (1<<(l2 & 7)) )!=0;
    }
    return 1;
}


//---------------------------------------------------------------------------------------
#pragma optimize( "at", on ) 
#pragma inline_depth(32)

BOOL BeamTree::ClassifyBox(Plane& plan, Box& box)
{
    V3	  corners[2];
    GetCornersOfPlane(box._min, box._max, plan._n, corners);
    REAL d1 = plan.DistTo(corners[0]);
    REAL d2 = plan.DistTo(corners[1]);
    DWORD side = 0;
    if (d1 >= 0)
        side |= ON_FRONT;

	if (d2 < 0)	
        side |= ON_BACK;
	return side;
}


//---------------------------------------------------------------------------------------
BOOL BeamTree::R_IsBbInVisibleLeaf(int nodeIdx, Box& box)
{
    assert(nodeIdx>=0);
    BtNode* pNode = &_nodes[nodeIdx];

    if(pNode->IsSolid())
        return TRUE;        //out of env allways visible

    if (pNode->IsLeaf())			
	{
        return (_leafs[pNode->_leafIdx]._visFrame == _frmStatic);
	}

    Plane& plane = _planes[pNode->_planeIdx];    
    int side     = ClassifyBox(plane, box);
	if (side & ON_FRONT)						
    {
		if(R_IsBbInVisibleLeaf(_nodes[nodeIdx]._nodesIdx[0], box))
        {
            return TRUE;
        }
    }
	if (side & ON_BACK)
    {
		if(R_IsBbInVisibleLeaf(_nodes[nodeIdx]._nodesIdx[1],box))
        {
            return TRUE;
        }
    }
    return FALSE;
}
#pragma optimize( "at", off ) 

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// render world
int	BeamTree::Render(SystemData* pSd, DWORD howwhat)
{
    //howwhat = _prims[howwhat];
    _pCamera = _pSystem->GetCamera();    // may be chaged
	Counter++;
    
    int leaf = _CuulTree(_pCamera, pSd);

    Irender* pR = pSd->_pSystem->Render();
    pR->CleanTexCache(-1);

    

    pR->EnableFog(_si.fogNear, _si.fogFar, _si.fogDens, _si.fogColor);

    if(_lInfo._count)
        ApplyDynaLights();
    pR->Push();

    if(_pterrain)
    {
        _pterrain->Render(pSd, howwhat);
    }

    if(DrawProps.retainedDraw)	// Set the array pointers
        pR->SetArrayPtrs(_vertexes._ptr, /*VX_COLOR|*/VX_TX1|VX_TX2|VX_TX3);

	//render far away objects first
    RenderSkyDom(pSd, howwhat);

	// classic rendering comented out
    // RenderStatic_R(_models[0]._rootNode, pSd, dwf, howwhat);		// go with the walls
	// Do Back to Front Rendering
    V3& cp = _pCamera->_pos;

    //->RenderStatic_R(_models[0]._rootNode, pSd, dwf, howwhat);		// leaf rendering
    RenderStatic_RB2F(cp, _models[0]._rootNode, pSd, howwhat);	// go with the walls
	// Do models and last do the transparent faces
    if(_models._size > 1)
        RenderDetails(cp, pSd, howwhat);

    RenderBlendFaces(howwhat);

/*
#ifdef _DEBUG
    
    pR->Color4(255,255,255,255);
    pR->DisableTextures();
    pR->Blend(DS_BLEND_LIGHT|DS_DEEPTEST_OFF|DS_NODEEPTEST);	// set tr context
    
    pR->Disable(pR->GetPrimitives()[Z_CULL_FACE]);

    CLR color(128,0,0,22);
    pR->Color(color);

    FOREACH(vector<DbgFace>, _debugFaces, pDbgFace)
    {
        pR->Render(pDbgFace->_vxes, 0, 3, howwhat);
    }
    pR->UnBlend();		

    pR->Color(ZBLUE);
    _FOREACH(vector<DbgFace>, _debugFaces, pDbgFace)
    {
        pR->DbgLine(pDbgFace->_vxes[0]._xyz , pDbgFace->_vxes[1]._xyz);
        pR->DbgLine(pDbgFace->_vxes[1]._xyz , pDbgFace->_vxes[2]._xyz);
        pR->DbgLine(pDbgFace->_vxes[2]._xyz , pDbgFace->_vxes[0]._xyz);
    }


    pR->Enable(pR->GetPrimitives()[Z_CULL_FACE]);
    

#endif //
*/


    pR->Pop();

    if(DrawProps.retainedDraw)
		pR->ResetArrayPtrs(howwhat);

	// clen temp arrays of blended face, mirrors and ...
	_trFaces.clear();
    _trMirrors.clear();
	_trBlackMsk.clear();

    pR->BeginPrim(Z_LINE_STRIP);
    pR->Color(ZWHITE);
    for(int i=0;i<_walkPoints.size();i++)
    {
        pR->Vertex3(_walkPoints[i].x, _walkPoints[i].y, _walkPoints[i].z);
    }
    pR->End();
    pR->EnableFog(0,0,0,ZBLACK);
    return leaf;
}

int	BeamTree::Animate(Pos* pov, SystemData* pSd)
{
    BspModel*	pModel = &_models[1];    
    for(unsigned int i=1 ; i < _models._size; i++,pModel++)
    {
        if(pModel->_props & MODEL_DYNAMIC)
        {
            #pragma message("demo moving model")
            RunMotionPaths(pModel, pSd);                    	// DEMO MODEL MOVING             
		}
    }
    return 1;
}

 int BeamTree::_CuulTree(Pos* pov, SystemData* pSd)
 {
    Campos      = _pSystem->GetCamera()->_pos;            // share it across
    int leafIdx = GetCurrentLeaf(Campos);

    _Perf.visModels  = 0;
    _Perf.visLights  = 0;
    _Perf.visObjects = 0;
    _Perf.visPolys   = 0;
    _Perf.content    = 0;

	_Perf.curLeaf = leafIdx;
    _Perf.content = this->GetLeafContent(leafIdx);

    // out of empty space. 
    if(leafIdx==-1){
        return -1;
    }

	++_frmDynamic;                  // increment each time
	// if leaf has changed refill the vis frame pvs leafs from this leaf 

    if(_pterrain)
        _pterrain->FrameTerrain(_pSystem->GetCamera());
    if(_curLeaf != leafIdx )
    {
        _curLeaf = leafIdx;			// update current leaf where pov has gone into another leaf
        ++_frmStatic;               
        FrameMainModel(leafIdx);
    }
	if(_models._size > 1)
		FrameMovingModels(leafIdx);

    return leafIdx;
}

//---------------------------------------------------------------------------------------
// For back to front painting mark the visible nodes with current frame as well.
// walking up in the tree
#pragma optimize( "at", on )
void BeamTree::FrameNodeParents(int nodeIdx)
{
    do
    {
        assert(nodeIdx>=0);
        if(_nodes[nodeIdx]._visFrame == _frmStatic)
        {
            return;			// node alredy visited from another children
        }
        _nodes[nodeIdx]._visFrame = _frmStatic;
        nodeIdx = _nodes[nodeIdx]._parentIdx;
    }while(nodeIdx >= 0);
}

//---------------------------------------------------------------------------------------
void BeamTree::FrameMainModel(int leafIdx)
{
	if(leafIdx < 0){
		return ;
	}
	
    BtLeaf*  pLeaf  = &_leafs[_models[0]._firstLeaf];
    BYTE*    pPvs   = 0;//
    int      il;

    if(_pPVS)	pPvs = &_pPVS[_leafs[leafIdx]._pvs];

    _models[0]._visFrm  = _frmStatic;       // each model 
    for(int i=0; i<_models[0]._leafs; i++, pLeaf++)
    {
        il = i+_models[0]._firstLeaf;
        if(pPvs == 0 || (BITAT(pPvs,il))!=0) // if no pvs let them visible
        {
            pLeaf->_visFrame = _frmStatic;      // leaf
            FrameNodeParents(pLeaf->_nodeIdx);  // parent nodes

            // frame all details inside this leaf
            for(unsigned int m=0; m < pLeaf->_models._size; m++)
            {
                BspModel& rModel = _models[pLeaf->_models[m]];

                if(rModel._props & MODEL_DYNAMIC)
                {
                    rModel._dirty = 1;
                }
                if( rModel._visFrm  != _frmStatic) // framed by dynamic
                {
                    rModel._visFrm = _frmStatic;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------
// static- dynamic - Frame visible detail(moveable/nonmoveable) models 
#pragma message("adapt moving model from terrain into bsp!!!")
void BeamTree::FrameMovingModels(int leafIdx)
{
    Box			tranBox;
	BspModel*	pModel = &_models[1];
	
    for(unsigned int i=1 ; i < _models._size; i++,pModel++)
    {
        if(pModel->_props & MODEL_DYNAMIC)
        {
            if(pModel->_dirty)
            {
                pModel->_dirty = 0;
                if(pModel->_onterrain)
                {
                    int nl = _pterrain->GetCurrentLeaf(pModel->_trpos);
                    if(pModel->_leaf    != nl || 
                        pModel->_visFrm != _pterrain->p_leafs[nl]._visframe)
                    {
                        pModel->_leaf   = nl;
                        pModel->_visFrm = _pterrain->p_leafs[nl]._visframe;
                    }
                }
                else
                {
                    int nl = GetCurrentLeaf(pModel->_trpos);
                    if(pModel->_leaf   != nl || 
                    pModel->_visFrm != _leafs[nl]._visFrame)
                    {
                        pModel->_leaf = nl;
                        pModel->_visFrm = _leafs[nl]._visFrame;
                    }
                }
            }
		}

        if(pModel->_flags & MODEL_IS_SKYDOM)
		{
			pModel->_visFrm = _frmStatic;
		}
    }
}


//---------------------------------------------------------------------------------------
// Classic rendering of main BSP
void    BeamTree::RenderStatic_R(int nodeIdx, SystemData* pSd, DWORD howwhat)
{
    Irender* pR = _pSystem->Render();

    V3& camPos = _pCamera->_pos;
    pR->Color(ZWHITE);

    BtLeaf*  pLeaf  = &_leafs[_models[0]._firstLeaf];
    for(int i=0; i < _models[0]._leafs; i++, pLeaf++)
    {
		//
		// PVS
		//
        if(pLeaf->_visFrame != _frmStatic)
            continue;

		//
		// FRUSTRUM culling
		//
        assert(pLeaf->_nodeIdx>=0);
        Box& box = _nodes[pLeaf->_nodeIdx]._bbox;
        if(!_pCamera->CanSee(box))
            continue;
		//
		// RENDER the Leaf
		//
        RenderLeaf(camPos, *pLeaf, howwhat);
    }
}


//---------------------------------------------------------------------------------------
// Back2 Front rendering of Main BSP
void    BeamTree::RenderStatic_RB2F(V3& cp, int nodeIdx, SystemData* pSd, DWORD howwhat)
{
    assert(nodeIdx>=0);
    BtNode* pNode = &_nodes[nodeIdx];

	// NODE IS in PVS
    if(pNode->_visFrame != _frmStatic) 
		return;

	// FRUSTUM
    if(!_pCamera->CanSee(pNode->_bbox))
		return;

	// We've reached a leaf. So render the leaf content
    if(pNode->_leafIdx!=-1){

        BtLeaf*  pLeaf  = &_leafs[pNode->_leafIdx];

        if(pLeaf->_visFrame != _frmStatic)
            return; 
        RenderLeaf(cp, *pLeaf, howwhat);
        return;
    }

 
	//	Do the clasique Back to front Tree Walking by camera position
    int nSide = (_planes[pNode->_planeIdx].GetSide(cp) >=0);

    if(pNode->_nodesIdx[nSide]>=0)
        RenderStatic_RB2F(cp, pNode->_nodesIdx[!nSide], pSd, howwhat);

    if(pNode->_nodesIdx[!nSide]>=0)
        RenderStatic_RB2F(cp, pNode->_nodesIdx[nSide], pSd, howwhat);

}

//---------------------------------------------------------------------------------------
// Walk in all leaf polygons and render the leaf polygons
//
void    BeamTree::RenderLeaf(V3& cp, BtLeaf&  pLeaf, DWORD howwhat)
{
    Face* pFace = &_faces[pLeaf._firstPoly];
    for(int j=0;j<pLeaf._polys;j++,pFace++)
    {
        if(SurfaceTest(pFace, TRUE,TRUE))
        {
            // this causes 1 frame delay for lights
			pFace->_visframe1  = (1+_frmDynamic); 
            RenderSurface(pFace, howwhat);
        }
    }
}

//---------------------------------------------------------------------------------------

void	BeamTree::PrepareHardwareLights(const Box& theBox, int nLeaf)
{
#ifdef HL_
    RenderLight* hLights[8];
    int          nLights = 0;

    FOREACH(vvector<HLight>, _hlights, pl)
    {
        if(IsLeafVisFromLeaf(pl->leaf, nLeaf) && ((Box&)theBox).IsTouchesSphere(pl->pos, pl->radius))
        {
            hLights[nLights] = &_hardLights[pl->index];
            ++nLights;
        }
    }
    Irender* pR = _pSystem->Render();
    pR->SetLights((RenderLight**)&hLights, nLights);
#endif
}


//---------------------------------------------------------------------------------------
// Renders models marked by dynamic frame. These models are moving models as lifts, doors, 
// elevators, platforms etc.
// 
void	BeamTree::RenderDetails(V3& cp, SystemData* pSd, DWORD howwhat)
{
    Irender* pR = _pSystem->Render();
#ifdef HL_
    #pragma message ("detail rendering in full hardware lighting")
    _useHwLighting = 1;
    pR->EnableLighting(1, ZGREY);
    pR->CleanTexCache(-1);
#endif //
    
    // see wich models are marked visible. ignore [0]  (main model)
	Box			theBox;
	BspModel*	pModel = &_models[1];
    _fogs.clear();

    for(int i=1; i<_models._size; i++,pModel++) 
    {
		if(pModel->_visFrm != _frmStatic ||                 // MODEL PVS
            pModel->_flags & MODEL_IS_SKYDOM)
		    continue;   

        theBox = pModel->_bx;
        if(pModel->_props & MODEL_DYNAMIC)
        {
            theBox.Transform(pModel->_trmat);
    		if(!_pCamera->CanSee(theBox))
            {
                continue;
            }
#ifdef HL_
            PrepareHardwareLights(theBox, pModel->_leaf);
#endif //
            pR->Push();
                pR->MulMatrix(pModel->_trmat);
	            R_RenderDetails(i, pSd, howwhat);
            pR->Pop();
            ++_Perf.visModels;
        }
        else
        {
    		if(!_pCamera->CanSee(theBox))
            {
                continue;
            }
            /*
            if(GET_CONTENT(pModel->_props) & MODEL_MAT_FOG)
            {
                Fog fog;

                BtLeaf* pLeaf  = &_leafs[_models[i]._firstLeaf];
                Face*   pFace = &_faces[pLeaf->_firstPoly];

                fog.center = pModel->_bx.GetCenter();
                fog.color  = pFace->_color;
                fog.intens = 100;
                fog.radius = pModel->_bx.GetMaxExtend();
                fog.nmodelNode = i;
                _fogs << fog;
                // ADD fogs map on this model
                // pModel->_bbox, pModel->
                continue;   
            }
            */

            
#ifdef HL_ 
            PrepareHardwareLights(theBox, pModel->_leaf);
#endif
            R_RenderDetails(i, pSd, howwhat);
            ++_Perf.visModels;
        }
    }

#ifdef HL_
    _useHwLighting = 0;
    pR->EnableLighting(0, ZGREY);
#endif //
}


//---------------------------------------------------------------------------------------
// Test each sufrface by flags and place it in a separate collection by flags. 
// Hlods the transparent faces to be rendered at the very end.
//
BOOL BeamTree::SurfaceTest(Face* pFace, 
						   BOOL bFrustCull, 
						   BOOL bBackCull)
{
	// Face is SKY means is complette transparent
	if(pFace->_flags & FACE_TRANSPARENT && pFace->_color.a==0)	
	    return FALSE;
    
	// reject back faces
    if(bBackCull && !(pFace->_flags & FACE_SHOWBACK))
    {
        Plane& rPlane = _planes[pFace->_planeidx];
        if(rPlane.GetSide(_pCamera->_pos) < 0)  
        {
            return FALSE;
        }
    }
	
	

    if(pFace->_flags & FACE_TRANSPARENT)
    {
		_trFaces.push_back(pFace);
        return FALSE;
    }

    if(pFace->_flags & FACE_MIRROR)
    {
        _trMirrors.push_back(pFace);
        return FALSE;
    }

    if(pFace->_flags & FACE_BLACKMASK)
    {
	    _trBlackMsk.push_back(pFace);
	    return FALSE;
    }
    /**
    if(bFrustCull && !_pCamera->CanSee(pFace->_box._min, 
					                  pFace->_box._max))
    {
		return FALSE;
    }
    */

    return TRUE; //let go
}

//---------------------------------------------------------------------------------------
// Render dynamic models, 
void  BeamTree::R_RenderDetails(int modelIdx, SystemData* pSd, DWORD howwhat)
{
    Box     trBox;    
	BOOL	cullFrustFace = 0;

    BtLeaf* pLeaf  = &_leafs[_models[modelIdx]._firstLeaf];
    for(int i=0; i < _models[modelIdx]._leafs; i++, pLeaf++)
    {
	    trBox         = _nodes[pLeaf->_nodeIdx]._bbox;
		cullFrustFace = TRUE;

        // dont cull dynamic faces by frustrum
		if(_models[modelIdx]._props & MODEL_DYNAMIC)    
		{
			trBox.Transform(_models[modelIdx]._trmat);
			cullFrustFace = FALSE;
		}
        // leaf bbox if in frustrum
		if(!_pCamera->CanSee(trBox))
        {
			continue;
        }

        Face* pFace = &_faces[pLeaf->_firstPoly];
        for(int j=0;j<pLeaf->_polys;j++,pFace++)
        {
            if(SurfaceTest(pFace, 0, 0)) 
            {		   
                //this causes 1 frame delay for lights
				pFace->_visframe1  = (_frmDynamic+1); 
                RenderSurface(pFace, howwhat);
            }
        }
    }
}


//---------------------------------------------------------------------------------------
void BeamTree::RenderSurface(Face* pFace, DWORD howwhat)
{
    Irender* pR = _pSystem->Render();
    REG Vtx*  pStart = &_vertexes[pFace->_firstvx];
    REG int   count  = pFace->_vertexcount;

	++_Perf.visPolys;

    if(FACE_TAMASK & pFace->_flags)
    {
        AnimateSurfaceTexture(pFace, pStart, count);
    }

    // IF YOU ENABLE THIS YOU SHOULD PRECOMPUTE ALL VERTEXES NORMALS
    if(_useHwLighting) 
    {
        pFace->_textures[1].hTex=_deftexFB;

#ifdef HL_  // hardware lighting is not includded in this demo
        if(pFace->_dlights>=0)
            pR->SetMaterial(_hardMats[pFace->_dlights]);
#endif //
/*
        RenderLight rl[16];
        for(int i=0;i<_items._size;i++)
        {
            if(_items[i]._gameitem)
        }
*/
    }
    else
    if(UINT(pFace->_textures[1])>0)
    {
		pR->CleanTexCache(-1);
        // if touched build the light map texture
        if(pFace->_dlights)
        {
		    BuildDynaLmaps(pFace);  // this can reset pFace->_dlights
        }
                                  
        /*
        if(_fogs.size())
        {
            pFace->_dlights |= BuildFogLmap(pFace);  // this can reset pFace->_dlights
        }
        */
        

        // test again couse prev call to BuildDynaLmaps can remove the flag
		if(pFace->_dlights)         
		{
			pR->AlterTexture(pFace->_textures[1], 
                                  pFace->_pdynalights[LM_DYNA], 
                                  0, 0, 
                                  pFace->_lmapsize.cx, 
                                  pFace->_lmapsize.cy);
		}
        // else if buffer is updated apply original lmap
		else if(pFace->_pdynalights[LM_DYNA]) 
		{
			pR->AlterTexture(pFace->_textures[1], 
                                   pFace->_pdynalights[0], 
                                   0, 
                                   0, 
                                   pFace->_lmapsize.cx, 
                                   pFace->_lmapsize.cy);
			pFace->_pdynalights[LM_DYNA] = 0;
		}
    }

    if(_useHwLighting) howwhat|=VX_NORMAL;
    pR->Color(pFace->_color); 

    howwhat |= pR->BindAllTextures(pFace->_textures, pFace->_texCombine);
    pR->Render( &_vertexes[0], pFace->_firstvx, pFace->_vertexcount, howwhat);

    //render with detail

    if(!_useHwLighting)
    {
    #ifdef _ONE_TEX_FOR_DEFLMAP
	    if((pFace->_dlights &&  
            pFace->_pdynalights[0] == _pLmapRGB) ||
	        pFace->_textures[1] == _deftexLM     ||
	        pFace->_textures[1] == _deftexFB)    
	    {
		    pR->AlterTexture(pFace->_textures[1], 
								    pFace->_pdynalights[0], 
								    0, 0, 
								    pFace->_lmapsize.cx, 
								    pFace->_lmapsize.cy);
		    pFace->_pdynalights[LM_DYNA] = 0;
            pFace->_dlights = 0;
	    }
    #endif // _ONE_TEX_FOR_DEFLMA
    }
}

//---------------------------------------------------------------------------------------
void BeamTree::RenderMirrors(DWORD howwhat)
{
    return;//
}

//---------------------------------------------------------------------------------------
void BeamTree::RenderBlendFaces(DWORD howwhat)
{
    Irender* pR = _pSystem->Render();
	int g=0;
    pR->Color4((char)255,(char)255,(char)255,(char)255);
    pR->DisableTextures();
	
	pR->Blend(DS_BLEND_LIGHT|DS_DEEPTEST_OFF/*|DS_NODEEPTEST*/);	// set tr context
	{
		vvector<Face*>::iterator b = _trFaces.begin();
		vvector<Face*>::iterator e = _trFaces.end();
		for(;b!=e;b++)
		{
			RenderSurface(*b,howwhat);
			g++;
		}
	}
	pR->UnBlendv();				// remove tr context
	
	pR->Blend(DS_BLEND_LIGHT|DS_DEEPTEST_OFF);	// set tr context
	{
		vvector<Face*>::iterator b = _trBlackMsk.begin();
		vvector<Face*>::iterator e = _trBlackMsk.end();
		for(;b!=e;b++)
		{
			RenderSurface(*b,howwhat);
		}
	}
	pR->UnBlendv();				// remove tr context

    if(_trMirrors.size() && (FACE_MIRRORS & howwhat))
    {
        RenderMirrors(howwhat);
    }
    
}
#pragma optimize( "at", off )

//---------------------------------------------------------------------------------------
Vtx*		BeamTree::GetVxPtr(int& length)
{
    length = _vertexes._size;
    return &_vertexes[0];
}

//---------------------------------------------------------------------------------------
void		BeamTree::PreprocessBspData()
{
};


//---------------------------------------------------------------------------------------
void    BeamTree::RenderSkyDom(SystemData* pSd,  DWORD howwhat)
{
    if(_skyModels.size()==0)
        return;
//    _testVisBox=0;
    Irender* pR = _pSystem->Render();

	pR->Push();
	pR->Translate(_pCamera->_pos);
    Box  tranBox;
    FOREACH(vvector<int>, _skyModels, pMIdx)
    {
        int mIdx = *pMIdx;

        BspModel*  pModel = &_models[mIdx];
        
        tranBox = pModel->_bx;
        tranBox.Expand(1600);
        tranBox.Transform(pModel->_trmat);

         //if(_pCamera->CanSee(tranBox._min, tranBox._max))
         //    continue;
		 pR->Push();
            /*
                pR->Rotate(pModel->_rotVx);        // JUST ROTATE SKYDOM FOR DEMO
				pModel->_rotVx.x+=.1;
				pModel->_rotVx.y+=.05;
				pModel->_rotVx.z+=.001;
                */
                pR->MulMatrix(pModel->_trmat);
	            R_RenderDetails(mIdx, pSd, howwhat);

        pR->Pop();
    }
	pR->Pop();

//    _testVisBox=0;
}


//---------------------------------------------------------------------------------------
void    BeamTree::AnimateSurfaceTexture(Face* pFace, Vtx*  pStart, int count)
{
	//if(!((Counter & 0x3) == 0x3))
	//	return;

    int ts = (pFace->_flags2 & 0xF); // animation bits

    Irender* pR = _pSystem->Render();
    for(int texLayer=0;texLayer<4;++texLayer)
    {
        if(ts & 1)
        {
            if(pFace->_flags & FACE_TG_FIRE)
            {
                if(_rTex.Update(TextuerRt::RT_FIRE,Counter))
                    pR->AlterTexture(pFace->_textures[texLayer], 
                                            _rTex.GetFireTex(), 0, 0, 
                                            _rTex.GetFireTexX(), 
                                            _rTex.GetFireTexY());
         
            }
            else if(pFace->_flags & FACE_TG_BLOB)
            {
                if(_rTex.Update(TextuerRt::RT_BLOB,Counter))
                    pR->AlterTexture(pFace->_textures[texLayer], 
                                            _rTex.GetBlobTex(), 0, 0, 
                                            _rTex.GetBlobTexX(), 
                                            _rTex.GetBlobTexY());
         
            }

            else if(pFace->_flags & FACE_TA_FRAMING)
            {
                const  vvector<UV>&    initTc = pFace->_initalUV;
                if(initTc.size() == count)
                {
                    REAL uScale = 1.0 / pFace->_texanim[0].u;
                    REAL vScale = 1.0 / pFace->_texanim[0].v;

                    for(int i=0; i < count; i++, pStart++)
                    {
                        pStart->_uv[texLayer].u = initTc[i].u * uScale;
                        pStart->_uv[texLayer].v = initTc[i].v * vScale;

                        pStart->_uv[texLayer].u += pFace->_sliceidx[0] * uScale;
                        pStart->_uv[texLayer].v += pFace->_sliceidx[1] * vScale;

                    }

                    pFace->_sliceidx[0]+=1.0f;
                    if((pFace->_sliceidx[0]) >= pFace->_texanim[0].u)
                    {
                        pFace->_sliceidx[0] = 0;
                        pFace->_sliceidx[1]+=1.0f;
                        if(pFace->_sliceidx[1] >= pFace->_texanim[0].v)
                        {
                            pFace->_sliceidx[1] = 0;
                        }
                    }
                }
                return;
            }

            // shift texture coord
            else if(pFace->_flags & FACE_TA_RAD)//pFace->_flags & FACE_TA_SHIFT)
            {
                for(int i=0; i < count; i++, pStart++)
                {
                    if(pStart->_uv[texLayer].u < .5)
                    {
                        pStart->_uv[texLayer].u += pFace->_texanim[0].u;

                        if(pStart->_uv[texLayer].u>.5)
                            pStart->_uv[texLayer].u=0;

                    }
                    else
                    {
                        pStart->_uv[texLayer].u -= pFace->_texanim[0].v;
                        if(pStart->_uv[texLayer].u < .5)
                            pStart->_uv[texLayer].u = 1;
                    }

                    if(pStart->_uv[texLayer].v < .5)
                    {
                        pStart->_uv[texLayer].v += pFace->_texanim[0].v;

                        if(pStart->_uv[texLayer].v>.5)
                            pStart->_uv[texLayer].v=0;

                    }
                    else
                    {
                        pStart->_uv[texLayer].v -= pFace->_texanim[0].v;
                        if(pStart->_uv[texLayer].v < .5)
                            pStart->_uv[texLayer].v = 1;
                    }
                }
                return;
            }
            // expand radial tex coord
            else //if(pFace->_flags & FACE_TA_RAD)
            {
                for(int i=0; i < count; i++, pStart++)
                {
                    pStart->_uv[texLayer].u += pFace->_texanim[0].u;
                    pStart->_uv[texLayer].v += pFace->_texanim[0].v;
                }
            }
            ts>>=1;
        }
    }
}       

//---------------------------------------------------------------------------------------

void    BeamTree::RunMotionPaths(BspModel* pModel, SystemData* sd)
{
    if(pModel->_motionIndex<0)
        return;

    MotionPath& mp = _motions[pModel->_motionIndex];        // get the assoc motion path
    
    if(!HandleMovingModel(&mp, pModel, sd))
    {
        V3& depl = mp.MoveModel(pModel, sd->_ticktime);


        HandleModelCollision(pModel, sd);
        
        pModel->_dirty = TRUE;
    }
}

void Tr_TerTree::FrameTerrain(const Camera* pCam)
{
    R_FrameTerrain(0, pCam);
}

//---------------------------------------------------------------------------------------
void Tr_TerTree::R_FrameTerrain(int inode, const Camera* pCam)
{
    Tr_TerNode& node = this->p_nodes[inode];

    if(!pCam->CanSee(node.b_box))   
    {
        return;
    }

    //
    // frame visible models on terrain with dynamic couse there is no PVS
    // and has to be done by frustrum culling
    //
    if(node.l_idx >= 0 && //is leaf
       node.l_idx < this->p_leafs.size()) 
    {
        Tr_TerLeaf& leaf = this->p_leafs[node.l_idx];
        leaf._visframe = p_BspTree->_frmDynamic;

        for(unsigned int m=0; m < leaf._models._size; m++)
        {
            BspModel& rModel = p_BspTree->_models[leaf._models[m]];

            if(rModel._props & MODEL_DYNAMIC)
                rModel._dirty = 1;
            rModel._visFrm = p_BspTree->_frmDynamic;
        }
        return;
    }

    if(node.i_nodes[0]>=0)
        R_FrameTerrain(node.i_nodes[0], pCam);
    if(node.i_nodes[1]>=0)
        R_FrameTerrain(node.i_nodes[1], pCam);
    
}

//---------------------------------------------------------------------------------------
// << 300
void    Tr_TerTree::Render(SystemData* pSd, DWORD howwhat)
{
    if(!p_BspTree->IsLeafInPVS(this->_landingLeaf))
        return ;
    Irender* pR = pSd->_pSystem->Render();
    
    if(this->n_glights)    howwhat |= VX_COLOR;
    if(DrawProps.retainedDraw && v_strips.size())
        pR->SetArrayPtrs((&v_strips[0]), howwhat);

    pR->Color(ZWHITE);
    
    //_texCombine=3;
    howwhat = Z_TRIANGLE_STRIP | pR->BindAllTextures(_textures, _texCombine);
    R_Render(pSd, 0, howwhat);

    if(DrawProps.retainedDraw && v_strips.size()) 
        pR->ResetArrayPtrs(howwhat);

    return ;
}

//---------------------------------------------------------------------------------------
// << 300
#pragma message("rendering terrain")
void    Tr_TerTree::R_Render(SystemData* pSd, int inode, DWORD howwhat)
{
    for(int i=0; i < this->p_leafs.size(); i++)
    {
        if(this->p_leafs[i]._visframe == p_BspTree->_frmDynamic)
        {
            RenderLeaf(pSd, i, howwhat);
        }
    }
}

//---------------------------------------------------------------------------------------
void    Tr_TerTree::AnimateTerrainTextures(Vtx& v0, Vtx& v1)
{
    DWORD   texBits  = _flags>>16;

    UV      osci(_tanim[0].u * Sinr(Counter/16.0), _tanim[0].v * Cosr(Counter/16.0));
    UV      depl(_tanim[0].u * Counter, _tanim[0].v * Counter);


    for(int texLayer=0;texLayer < 4;++texLayer)
    {
        if(texBits & 1)
        {
            if(_flags & TERR_TA_SHIFT)
            {
                v0._uv[texLayer] += depl;
                v1._uv[texLayer] += depl;
            }
            if(_flags & TERR_TA_OSC)
            {
                v0._uv[texLayer] += osci;
                v1._uv[texLayer] += depl;
            }
        }
        texBits>>=1;
    }
}

//---------------------------------------------------------------------------------------
// << 300
void    Tr_TerTree::RenderLeaf(SystemData* pSd, int nleaf, DWORD howwhat)
{
    Irender*            pR   = pSd->_pSystem->Render();
    const   Tr_TerLeaf& leaf = this->p_leafs[nleaf];
    /*
    for(int k=0; k < 32; k++)
    {
        if(leaf.n_strips[k] == 0)
            break;
        pR->Render( &v_strips[k], leaf._stripStart[k], leaf.n_strips[k], howwhat|VX_TX1|VX_TX2);
    }
    */

    const SIZE  t   = {z_tiles.cx,z_tiles.cy};
    const V3&   c   = b_box._min;
    const V3&   ex  = b_box.GetExtends();
    const REAL  xS  = (ex.x) / (REAL)t.cx;
    const REAL  zS  = (ex.z) / (REAL)t.cy;
    const BOOL  bG  = n_glights!=0;
    int   x,z;
    Vtx   v0,v1;  
    UV    lMsz(z_tiles.cx*xS, z_tiles.cy*zS);
    REAL  fx,fx1,fz;
    int   k = 0;
    DWORD   rt = howwhat;

    const SIZE& s   = leaf.s_tiles[0];
    SIZE  e   = leaf.s_tiles[1];

    for(x=s.cx; x < e.cx; x++)
    {
        fx   = x * xS;
        fx1  = (x+1) * xS;
        v0._xyz.x = c.x + (fx);
        v1._xyz.x = c.x + (fx1);
        
        pR->BeginPrim(REND_PRIM(howwhat));

        for(z=s.cy; z <= e.cy; z++)//GL_TRIANGLES
        {
            fz = z * zS;
            v0._xyz.z = c.z + (fz);
            v0._xyz.y = *(element(v_heights, x, z, t.cx));
            v0._xyz.y /= 255.0;
            v0._xyz.y *= ex.y;
            v0._xyz.y += c.y;

            v1._xyz.z = v0._xyz.z;
            v1._xyz.y = *(element(v_heights, (x+1), z, t.cx));
            v1._xyz.y /= 255.0;
            v1._xyz.y *= ex.y;
            v1._xyz.y += c.y;

            if(bG)
            {
                v0._rgb = *(element(g_light, x, z, t.cx));
            }

            if(_flags & TERR_UTT0)
            {
                v0._uv[0] = UV((v0._xyz.x-c.x)/ex.x , 1-(v0._xyz.z-c.z)/ex.z);
            }
            else
            {
                if((z&1)==0)
                {
                    v0._uv[0] = UV(0,0);
                }
                else
                {
                    v0._uv[0] = UV(0,1);
                }
            }
            v0._uv[1] = UV(fx/lMsz.u , 1-fz/lMsz.v);
            if((z&1)==0)
            {
                v0._uv[2] = UV(0,0);
                v0._uv[3] = UV(0,0);
            }
            else
            {
                v0._uv[2] = UV(0,1);
                v0._uv[3] = UV(0,1);
            }

            if(bG)
            {
                v1._rgb = *(element(g_light, x+1, z, t.cx));
            }
            if(_flags & TERR_UTT0)
            {
                v1._uv[0] = UV((v1._xyz.x-c.x)/ex.x , 1-(v1._xyz.z-c.z)/ex.z);
            }
            else
            {
                if((z&1)==0)
                {
                    v1._uv[0] = UV(1,0);
                }
                else
                {
                    v1._uv[0] = UV(1,1);
                }
            }
            v1._uv[1] = UV(fx1/lMsz.u , v0._uv[1].v);

            if((z&1)==0)
            {
                v1._uv[2] = UV(1,0);
                v1._uv[3] = UV(1,0);
            }
            else
            {
                v1._uv[2] = UV(1,1);
                v1._uv[3] = UV(1,1);
            }

            AnimateTerrainTextures(v0, v1);

            pR->RenderVertex(&v0, rt);
            pR->RenderVertex(&v1, rt);
        }

        pR->End();
    }
}


//------------------------------------------------------------------------------------------------
// builds a big polygon
    
void    BeamTree::AddDbg_Polygon(V3& a, V3& b, V3& c)
{
    Plane p(a,b,c);
    AddDbg_Polygon(&p);
  

}

void    BeamTree::AddDbg_Polygon(Plane* p)
{
    
#ifdef _DEBUG

    DbgFace df;
    
	V3		ax = GetMinAx((V3&)p->_n);
    ax.norm();

    // to corner vectors;
	V3		uDr  = Vcp(ax,  p->_n);
	V3		vDr  = Vcp(uDr, p->_n);

    uDr		*= 1600.0;
	vDr		*= 1600.0;
    REAL vdp = Vdp(p->_n, ax);
    V3&  c = V0;

    df._vxes[0]._xyz =   c+(uDr-vDr);
	df._vxes[1]._xyz =   c+(uDr+vDr);
	df._vxes[2]._xyz =   c-(uDr-vDr);
	df._vxes[3]._xyz =   c-(uDr+vDr);

    _debugFaces.push_back(df);

    if(_debugFaces.size() > 16)
    {
        _debugFaces.erase(_debugFaces.begin());
        _debugFaces.erase(_debugFaces.begin());
        _debugFaces.erase(_debugFaces.begin());
        _debugFaces.erase(_debugFaces.begin());

    }

#endif //_DEBUG

}


const int*        BeamTree::GetPortalsIndexes(int leaf, int count)const
{
    return 0;    
}

const BtPortal&    BeamTree::GetPortal(int index)const
{
    return _portals._ptr[0];
}


BOOL	BeamTree::FaceContainPoint(const Face* pFace, const V3& point)
{
	Plane	plane;
	V3		end;
	V3      start  = _vertexes[pFace->_firstvx + pFace->_vertexcount-1]._xyz;
	V3		above;//  = start + (_planes[pFace->_planeidx]._n*32);
    int     count  = pFace->_vertexcount;

    for(int i=0;i<count;i++)
    {
		end  = _vertexes[pFace->_firstvx+i]._xyz;
		above  = end + _planes[pFace->_planeidx]._n * vdist(start,end)/3;

		plane.CalcNormal(start, end, above);

#ifdef _DEBUG
        _ipPoints << above;
        _ipPoints << above + (plane._n * 64);
        AddDbg_Polygon(start, end, above);
#endif //

		REAL fdt = plane.DistTo(point);
		if(fdt < -0.2)
			return FALSE;
		start = end;
    }
	return TRUE;
}


BOOL    BeamTree::TestPolygonEdge(const Face* pFace, 
                                  const V3&   point,
                                  REAL  radius,
                                  V3& ip)
{
	Plane	plane;
	V3		end;
	V3      start  = _vertexes[pFace->_firstvx + pFace->_vertexcount-1]._xyz;
    int     count  = pFace->_vertexcount;
    
    for(int i=0; i < count; i++)
    {
		end        = _vertexes[pFace->_firstvx+i]._xyz;
		REAL d2seg =  DistPointSeg(start, end, point, ip);
		if(d2seg<=radius)
        {
			return TRUE;
        }
		start = end;
    }
	return FALSE;
}


void BeamTree::UpdateValues(V3& pos, int& leaf)
{
    leaf = this->GetCurrentLeaf(pos);
}


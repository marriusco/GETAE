//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "basecont.h"
#include "system.h"
#include "beamtree.h"
#include <set>

//---------------------------------------------------------------------------------------


static REAL EPSIL   = 1.0/32;

//---------------------------------------------------------------------------------------
DWORD BeamTree::S_ClassifyBox(const Plane& plan, const Box& box)
{
    #pragma message("aligned planes are easy implement!!!")
    V3	  corners[2];
    GetCornersOfPlane((V3&)box._min, (V3&)box._max, (V3&)plan._n, corners);
    REAL d1 = plan.DistTo(corners[0]);
    REAL d2 = plan.DistTo(corners[1]);
    DWORD nodeSide = 0;
    if (d1 >= 0)nodeSide |= ON_FRONT;
	if (d2 < 0)	nodeSide |= ON_BACK;
	return nodeSide;
}

const Vtx* BeamTree::GetVertexes(const Face* pFace, int& ncount)
{
    ncount = pFace->_vertexcount;
    return &_vertexes[pFace->_firstvx];
}

//---------------------------------------------------------------------------------------
BOOL   BeamTree::GetPotentialyColidingFaces (Impact& im, const V3& dir, const Box& box, DWORD w2coll)
{
    Box         bbox;

    im.Reset();
    im._extends    = box.GetExtends() * .5;
    im._what2coll  = w2coll;
    im._fwd        = dir;
    im._node       = -1;

    BspModel* pModel =  &_models[0];
	for(int i=0; i< _models._size; ++i, ++pModel)
    {
        // for detail models do this test.
        bbox = box;
        if(i > 0)
        {
            if(pModel->_flags & MODEL_NOCOLIDE) continue;
            if(pModel->_onterrain )
            {
                if(_frmDynamic != pModel->_visFrm)
                    continue;
            }
            else 
            {
                if(_frmStatic != pModel->_visFrm)
                    continue;
            }

            // move the segment into the model space
            if(pModel->_props & MODEL_DYNAMIC)
            {
                bbox._min = pModel->ToModelSpace(box._min);
                bbox._max = pModel->ToModelSpace(box._max);
            }
            if(!bbox.IsTouchesBox(pModel->_bx))
                continue;
        }

        if(pModel->_onterrain)
        {
            //
            // SouroundingPlanes* pLocoBsp = _pterrain->GetPotentiallyHitVertexex(bex);
            // colide with planes
            //
        }
        else
        {
            int nRootNode = pModel->_rootNode;
            if(i==0)
            {
                if(im._node==-1)
                {
                    Box bex = box;
                    bex.Expand(16.0);
                    im._node = GetCurentNode(nRootNode, bex);
                }
            }
            im._model = i;
            _R_GetPotentialyColidingFaces(im, nRootNode, bbox, w2coll );
        }
    }
    return im._hit;
}

//---------------------------------------------------------------------------------------
void   BeamTree::_R_GetPotentialyColidingFaces(Impact& im, int node, const Box& box, DWORD what2collide)
{
    BtNode* pNode  = &_nodes[node];
    if(pNode->IsLeaf())
	{
        if(pNode->IsSolid())
        {
            im._hit = 1;
            if(im._model==0)
                im._env  =  CONT_CANTGO;
            else
            {
                im._env  |= _models[im._model]._flags;
            }
            return;
        }
        if(im._model == 0)
        {
            if(im._leaf==-1)
                im._leaf  = pNode->_leafIdx;
            im._env  = _leafs[pNode->_leafIdx]._content;
        }
        else
        {
            im._env  = _models[im._model]._flags;
        }
        return _GetFacesOrPlanesFromLeaf(im, pNode->_leafIdx, box, what2collide);

    }

    Plane& plane = _planes[pNode->_planeIdx];
    DWORD nodeSide = S_ClassifyBox(plane, box);

    if(nodeSide & ON_FRONT)
        _R_GetPotentialyColidingFaces(im, pNode->_nodesIdx[1], box, what2collide);
    if(nodeSide & ON_BACK)
        _R_GetPotentialyColidingFaces(im, pNode->_nodesIdx[0], box, what2collide);
}

//---------------------------------------------------------------------------------------
void   BeamTree::_GetFacesOrPlanesFromLeaf(Impact& im, int nleaf, const Box& box, DWORD w2c)
{
    /*
    BtLeaf&     leaf   = _leafs[nleaf];
    if(leaf._sides._size==0) 
        return ;
    set<int>    polys;    
    Face*	    pFace  = &_faces[leaf._firstPoly];
    V3          fwd    = im._fwd;
    for(int j=0; j < leaf._polys; j++, pFace++)
    {
        Plane& rplane = _planes[pFace->_planeidx];
        if(Vdp(rplane._n, fwd) > 0) // going into back of th e plane
            continue;
        if(!pFace->_box.IsTouchesBox(box))
            continue;
        
        if(COL_WANT_FACES & w2c)
        {
            pFace->_nmodel = im._model;
           im._faces.Push(pFace);
        }
        else
        {
            if(polys.find(pFace->_planeidx) == polys.end())
            {
                polys.insert(pFace->_planeidx);
                im._planes.Push(rplane);
                im._planes.Last()._u = im._model;
            }
        }
    }
    */

}




void     BeamTree::_ModelHitLeaf(BtNode* pNode, Impact& best, const V3& vStart, 
                                 const V3& vEnd, 
                                 const REAL fStarti, const REAL fEndi)
{

}

/*
void   BeamTree::_R_ModelIntersect(Impact& best, int rootnode, const V3&  vStart, const V3& vEnd, 
                                   const REAL fStart, const REAL fEnd)
{

	BtNode* pNode = &_nodes[rootnode];
    if (pNode->IsLeaf())
	{
        _ModelHitLeaf(pNode, best, vStart, vEnd, fStart, fEnd);
        return ;
	}
    const  Plane&   plane    = _planes[pNode->_planeIdx];
    REG REAL        offset   = ExtendsToPlane (best._extends,plane);
    REG REAL        dS       = plane.DistTo(vStart);
    REG REAL        dE       = plane.DistTo(vEnd);
    
    if(dS>=offset && dE>=offset)
        return _R_ModelIntersect(best, pNode->_nodesIdx[1], vStart, vEnd, fStart, fEnd);
    else if(dS<-offset && dE<-offset)
        return _R_ModelIntersect(best, pNode->_nodesIdx[0], vStart, vEnd, fStart, fEnd);
    else
    {
	    
		int     s1, s2;
		float   f1, f2;
        if(dS < dE)
        {
            s1 = pNode->_nodesIdx[0];
			s2 = pNode->_nodesIdx[1];
			
			float inverseDiff = 1.0 / (dS - dE);

			f1 = (dS - EPSIL - offset) * inverseDiff;
			f2 = (dS + EPSIL + offset) * inverseDiff;			
        }
        else if(dS >dE)
        {
	        s1 = pNode->_nodesIdx[1];
			s2 = pNode->_nodesIdx[0];
			
			float inverseDiff = 1.0f / (dS - dE);

			f1 = (dS + EPSIL + offset) * inverseDiff;
			f2 = (dS - EPSIL - offset) * inverseDiff;			
        }
        else
        {
            s1 = pNode->_nodesIdx[1];
			s2 = pNode->_nodesIdx[0];
			
			f1 = 1.0;
			f2 = 0.0;		
        }
        CLAMPVAL(f1,0.0,1.0);
        CLAMPVAL(f2,0.0,1.0);

        V3      vMid = vStart + (vEnd-vStart)*f1;
        REAL    fMid = fStart + (fEnd-fStart)*f1;

        _R_ModelIntersect(best, s1, vStart, vMid, fStart, fMid);

        vMid = vStart + (vEnd-vStart)*f2;
        fMid = fStart + (fEnd-fStart)*f2;

        _R_ModelIntersect(best, s2, vMid, vEnd, fMid, fEnd);

    }
}
*/

/*
BOOL   BeamTree::_R_ModelIntersect(Impact& best, 
                                   int rootnode, 
                                   const V3& vStart, 
                                   const V3& vEnd)
{
	BtNode* pNode = &_nodes[rootnode];
    if (pNode->IsLeaf())
	{
        return _TestLeaf(pNode, best);
	}
    const  Plane&  plane  = _planes[pNode->_planeIdx];
    REAL   shift     = ExtendsToPlane (best._extends,plane);
    REAL   dS        = plane.DistTo(vStart);
    REAL   dE        = plane.DistTo(vEnd);
    int    nodeSide  = dS >=0;

    if(dS > shift )
    {
        if(dE > shift)
            return _R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], vStart, vEnd);
        
        REAL d2ip  = (dS-shift)/(dS-dE);
        V3 midPct  = vStart + ((vEnd-vStart)*d2ip);

        if(_R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], vStart, midPct))
        {
            return 1;
        }
        if(_R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], midPct, vEnd))
        {
            StoreCollisiondata(best, vStart,midPct,plane);
        }
    }
    else if(dS < -shift )
    {
        if(dE < -shift)
            return _R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vStart, vEnd);
        
        REAL d2ip  = (dS-shift)/(dS-dE);
        V3 midPct  = vStart + ((vEnd-vStart)*d2ip);

        if(_R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], midPct, vEnd))
        {
            return 1;
        }
        if(_R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vStart, midPct));
        {
            StoreCollisiondata(best, vStart,midPct,plane);
        }
    }
    return best._hit;
}

/*
BOOL   BeamTree::_R_ModelIntersect(Impact& best, 
                                   int rootnode, 
                                   const V3& vStart, 
                                   const V3& vEnd)
{
	BtNode* pNode = &_nodes[rootnode];
    if (pNode->IsLeaf())
	{
        return _TestLeaf(pNode, best);
	}
    const  Plane&  plane  = _planes[pNode->_planeIdx];
    REAL   shift     = ExtendsToPlane (best._extends,plane);
    REAL   dS        = plane.DistTo(vStart);
    REAL   dE        = plane.DistTo(vEnd);
    V3     altTo     = vEnd;
    V3     vMid      = vEnd; 
    BOOL   bhit      = FALSE;
    if(dS < dE)
    {
        if(dS < shift)
        {
            bhit = 0;
            if(dE < shift) // totally behind
            {
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vStart, vEnd);
            }
            else            // patially behind
            {
                vMid = vEnd;
                vMid -= vStart;
                vMid *= (dS-shift+EPSIL)/(dS-dE);
                vMid += vStart;
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vStart, vMid);
            }
            if(bhit)
            {
                best._hit    = 1;
                best._ip     = vMid;
                best._node   = rootnode;
                best._plane  = plane;
                best._dist   = vdist(vStart,vMid);

                altTo = best._ip;
                dE    = plane.DistTo(altTo);
            }
        }
        if(dE > -shift) // too far behind, exceed back of shifted plane
        {
            bhit = 0;
            if(dS > -shift)
            {
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], vStart, altTo);
            }
            else
            {
                vMid = altTo;
                vMid -= vStart;
                vMid *= (dS+shift-EPSIL)/(dS-dE);
                vMid += vStart;
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], altTo, vMid);
            }
        }
    }
    else //dS>=dE
    {
        if(dS > -shift)
        {
            bhit =FALSE;
            if(dE > -shift)
            {
               bhit =  _R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], vStart, vEnd);
            }
            else
            {
                vMid = vEnd;
                vMid -= vStart;
                vMid *= (dS+shift-EPSIL)/(dS-dE);
                vMid += vStart;
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_FRONT], vStart, vMid);
            }
            if(bhit)
            {
                altTo = best._ip;
                dE    = plane.DistTo(altTo);
            }
        }

        if(dE < shift)
        {
            bhit =FALSE;
            if(dS < shift)
            {
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vStart, vEnd);
            }
            else //if(dS >= shift)
            {
                vMid = altTo;
                vMid -= vStart;
                vMid *= (dS-shift + EPSIL)/(dS-dE);
                vMid += vStart;
                bhit = _R_ModelIntersect(best, pNode->_nodesIdx[N_BACK], vMid, altTo);
            } //if(dS < shift)

            if(bhit)
            {
                best._hit    = 1;
                best._ip     = vMid;
                best._node   = rootnode;
                best._plane  = plane;
                best._dist   = vdist(vStart,vMid);
            }
        }   //if(dE < shift)
    }
    return bhit;
}


BOOL   BeamTree::_R_ModelIntersect(Impact& best, int rootnode, 
                                   const V3&  vStart, const V3& vEnd, 
                                   const REAL fStart, const REAL fEnd)
{
	BtNode* pNode = &_nodes[rootnode];
    if (pNode->IsLeaf())
	{
        return _TestLeaf(pNode, best, vStart, vEnd);
	}

    const  Plane&   plane    = _planes[pNode->_planeIdx];
    REG REAL        offset   = Rabs(best._extends.x*plane._n.x)+Rabs(best._extends.y*plane._n.y)+Rabs(best._extends.z*plane._n.z);
    REG REAL        dS       = plane.DistTo(vStart);
    REG REAL        dE       = plane.DistTo(vEnd);
    
    
    if(dS < -offset && dE < -offset)
    {
        return _R_ModelIntersect(best, pNode->_nodesIdx[0], vStart, vEnd,1,0);
    }
    else if(dS >= offset && dE >=offset )
    {
        return _R_ModelIntersect(best, pNode->_nodesIdx[1], vStart, vEnd,1,0);
    }

  	int     s1, s2;
	float   f1, f2;
    if(dS < dE)
    {
        s1 = pNode->_nodesIdx[0];
		s2 = pNode->_nodesIdx[1];
		
		float inverseDiff = 1.0 / (dS - dE);

		f1 = (dS - EPSIL - offset) * inverseDiff;
		f2 = (dS + EPSIL + offset) * inverseDiff;			
    }
    else if(dS >dE)
    {
	    s1 = pNode->_nodesIdx[1];
		s2 = pNode->_nodesIdx[0];
		
		float inverseDiff = 1.0f / (dS - dE);

		f1 = (dS + EPSIL + offset) * inverseDiff;
		f2 = (dS - EPSIL - offset) * inverseDiff;			
    }
    else
    {
        s1 = pNode->_nodesIdx[1];
		s2 = pNode->_nodesIdx[0];
		
		f1 = 1.0;
		f2 = 0.0;		
    }
    CLAMPVAL(f1,0.0,1.0);
    CLAMPVAL(f2,0.0,1.0);

    V3      vMid = vStart + (vEnd-vStart)*f1;
    REAL    fMid = fStart + (fEnd-fStart)*f1;
    if(_R_ModelIntersect(best, s1, vStart, vMid, fStart, fMid))
        return 1;
    
    vMid = vStart + (vEnd-vStart)*f2;
    fMid = fStart + (fEnd-fStart)*f2;
    if(_R_ModelInterse0ct(best, s2, vMid, vEnd, fMid, fEnd))
    {
        StoreCollisiondata(best, vMid, vdist(vStart,vMid), plane);
    }

    return best._hit;
}


//
*/

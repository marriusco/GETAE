//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "basecont.h"
#include "system.h"
#include "beamtree.h"

//---------------------------------------------------------------------------------------

static REAL EPSIL     = 1.0/16.0;
static REAL EPSILH    = 1.0/64.0;

//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
BOOL BeamTree::SegmentIntersect(Impact& im, const V3& a, const V3& b, int node, DWORD what)
{
    Impact  best;

    im.Reset();
    best._ip = b;

    if(R_SegmentIntersect(best, 0, node, a, b))
    {
        im = best;
    }
    
    if(_models._size > 1)
    {
        // build a box and test aonly models witch are in this box.
        V3  s = a;
        V3  e = best._ip;
        V3  vStart, vEnd;
        Box segbox;

	    BspModel* pModel =  &_models[1];
	    for(int i=1; i< _models._size; ++i, ++pModel)
	    {
            if(!pModel->IsColidable(_frmStatic))
            {
                continue;
            }

            // tr the seg in model space
            if(pModel->_props & MODEL_DYNAMIC)
            {
                vStart = pModel->ToModelSpace(s);
                vEnd   = pModel->ToModelSpace(e);
            }
            else
            {
                vStart = (s);
                vEnd   = (e);
            }
            segbox.AddPoint(vStart);
            segbox.AddPoint(vEnd);

            if(!segbox.IsTouchesBox(pModel->_bx))
            {
                continue;
            }

            best._hit = 0;
            if(R_SegmentIntersect(best, i, pModel->_rootNode, vStart, vEnd))
            {
                // tr back the collision point and hit plane
                pModel->UnTransformVertex(best._ip); 
                pModel->UnTransformPlane(best._plane, best._ip);

                // keep the closest hit
                REAL closest   = vdist(a, best._ip);
                if(closest < im._dist)
                {
		            best._hit   = 1;
                    im          = best;
                }
            }
        }
    }
    return im._hit;
}

BOOL   BeamTree::SegmentIntersectModel(Impact& im, int model, const V3& a,const  V3& b, int node, DWORD w2coll)
{
    Impact  best;

    im.Reset();
    best._ip = b;

    BspModel* pModel =  &_models[model];
    if(!pModel->IsColidable(_frmStatic))
         return 0;

    V3 vStart = a;
    V3 vEnd   = b;
    // tr the seg in model space
    if(pModel->_props & MODEL_DYNAMIC)
    {
        vStart = pModel->ToModelSpace(a);
        vEnd   = pModel->ToModelSpace(b);
    }
    Box segbox(vStart,vEnd);

    if(!segbox.IsTouchesBox(pModel->_bx))
    {
        return 0;
    }

    if(R_SegmentIntersect(im, model, pModel->_rootNode, vStart, vEnd))
    {
        // tr back the collision point and hit plane
        pModel->UnTransformVertex(best._ip);
        pModel->UnTransformPlane(best._plane, best._ip);

        // keep the closest hit
        REAL closest   = vdist(a, best._ip);
        if(closest < im._dist)
        {
		    best._hit   = 1;
            im          = best;
        }
    }
    return im._hit;
}

//---------------------------------------------------------------------------------------
BOOL    BeamTree::R_SegmentIntersect(Impact& best, int model, int nodeIdx, 
                                     const V3& a, const V3& b)
{
	BtNode* pNode = &_nodes[nodeIdx];
    if (pNode->IsLeaf())
	{
        return _TestLeaf(pNode, best, a, b);
	}

    Plane& plane = _planes[pNode->_planeIdx];
    REAL      dS = plane.DistTo(a);
    REAL      dE = plane.DistTo(b);

    if(dS >= -EPSIL  && dE >= -EPSIL )
        return R_SegmentIntersect(best, model, pNode->_nodesIdx[N_FRONT], a, b);
    else if(dS < EPSIL && dE < EPSIL )
        return R_SegmentIntersect(best, model, pNode->_nodesIdx[N_BACK], a, b);

	int  nodeSide = dS >=0;
    REAL d2ip     = (dS-(nodeSide * EPSIL))/(dS-dE);
    CLAMPVAL(d2ip,0,1);
	V3 midPct  = a + ((b-a)*d2ip);
    if(best._node==-1) best._node = nodeIdx;

	if(R_SegmentIntersect(best, model, pNode->_nodesIdx[nodeSide], a, midPct))
	{
		return TRUE;
	}
	else if (R_SegmentIntersect(best, model, pNode->_nodesIdx[!nodeSide], midPct, b))
	{
        REAL dist = vdist(midPct,a);
		if(!best._hit && dist < best._dist && nodeSide)
		{
			best._hit	  = 1;
			best._ip     = midPct;
			best._plane  = plane;
            best._model  = model;
			best._dist   = dist;
		}
	}
    return best._hit;
}

static DWORD HIT_COUNT = 0;
//---------------------------------------------------------------------------------------
BOOL   BeamTree::ObjectIntersect (Impact &im, const V3& a, const V3& b, const REAL ra, 
                                  const REAL rb, const Box& box, DWORD w2coll)
{
    Impact best;
    V3     vStart, vEnd;
    V3     mod2obj;
    Box    bbox   = box;

    im.Reset();
    best.Reset();
    best._ip      = b;
    best._hitCnt  = ++HIT_COUNT;

    best._hit       = 0;
    best._extends    = bbox.GetExtends() * .5;
    best._what2coll  = w2coll;
    best._fwd       = b-a;
    best._fwd.normalize();

    BspModel* pModel =  &_models[0];
	for(int i=0; i< _models._size; ++i, ++pModel)
    {
        vStart = a;
        vEnd   = b;
        // for detail models do this test.
        if(i > 0)
        {
            if(pModel->_onterrain)
            {
                if(!pModel->IsColidable(_frmDynamic))
                    continue;
            }
            else 
            {
                if(!pModel->IsColidable(_frmStatic))
                    continue;
            }

            // move the segment into the model space
            if(pModel->_props & MODEL_DYNAMIC)
            {
                vStart = pModel->ToModelSpace(a);
                vEnd   = pModel->ToModelSpace(b);
                // modify end as  with the model speed
            }
        }
        // move the box vTo enclose start and end, ad reject
        // non touched models
        best.Reset();
        best._mbox = bbox;
        best._mbox.MakeMoveBbox(vStart, vEnd, 0);
        best._mbox.Expand(32);

        if(i > 0 && !best._mbox.IsTouchesBox(pModel->_bx))
        {
             continue;
        }
        best._model = i;

        if(pModel->_onterrain)
        {
            //
            // SouroundingPlanes* pLocoBsp = _pterrain->GetPlanes(vStart, vEnd, bex);
            // colide with planes
            //
        }
        else
        {
            int nRootNode = pModel->_rootNode;
            if(i==0)
            {
                //
                // find the node then step collision we start it vStart this node down
                // not vStart the top of the tree
                //
                if(best._node==-1)
                {
                    Box bex = best._mbox;
                    bex.Expand(32.0);
                    best._node = GetCurentNode(nRootNode, bex);
                }
            }
            // prepare the hit flag for next round
            best._hit   = 0;
            best._solid = 0;
            best._nplane=-1;
            
            if(_R_ModelIntersect(best, nRootNode,  vStart, vEnd, ra,rb))
            {
                if(best._solid && best._hit==0)
                {
                    //
                    // if gets here somenting is fishy
                    //
                    best._hit   = 1;
                    best._ip    = vStart + (-best._fwd);
                    best._dist  = 0;
                    best._plane = Plane(best._fwd, best._ip);
                    best._nplane=-1;
                    TRACEX("");
                }

			    if(pModel->_props & MODEL_DYNAMIC)
                {
				    pModel->UnTransformVertex(best._ip);
				    pModel->UnTransformPlane(best._plane, best._ip);
			    }
                if(best._dist < im._dist)
                {
                    im._dist  = best._dist; 
                    im._hit   = best._hit;  
                    im._ip    = best._ip;   
                    im._plane = best._plane;
                    im._nplane = best._nplane;
                    im._fwd      = best._fwd;
                    im._model = best._model;
                }
            }
            
        }
        im._env  |= best._env;

        if(i==0)
        {
            im._leaf  = best._leaf;
            im._node  = best._node;
        }
    }
    if(im._hit==0)
        im._ip = b;
    return im._hit;
}

int   BeamTree::GetCurentNode(int nodeIdx, const Box& bbox)
{
    if(_nodes._size==0)
        return -1;
    DWORD   nodeSide;
    BtNode* pNode;
    do
    {
        pNode = &_nodes[nodeIdx];
        nodeSide  = S_ClassifyBox(_planes[pNode->_planeIdx], bbox);
        if(nodeSide == ON_FRONT)
        {
            nodeIdx = pNode->_nodesIdx[1];
        }
        else if(nodeSide == ON_BACK)
        {
            nodeIdx = pNode->_nodesIdx[0];
        }
        
    }while(nodeSide != (ON_FRONT|ON_BACK) && pNode->_leafIdx!=-1);

    return nodeIdx;
}

BOOL BeamTree::_TestLeaf(BtNode* pNode, Impact& best, const V3& vStart, const V3& vEnd)
{
    
    if(pNode->IsSolid())
    {
        if(best._model == 0)
            best._env |=  CONT_CANTGO ;
        else
            best._env |= _models[best._model]._flags;
        best._ip      = vStart;
        best._solid   = TRUE;
        return 1;
    }
    if(best._model == 0)
    {
        if(best._leaf==-1)
            best._leaf  = pNode->_leafIdx;
        best._env  = _leafs[best._leaf]._content;
    }
    else
    {
        best._env  = _models[best._model]._flags;
    }
    return 0;
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

    BOOL            bhit     = 0;
    const  Plane&   plane    = _planes[pNode->_planeIdx];
    REG REAL        offset   = Rabs(best._extends.x*plane._n.x)+Rabs(best._extends.y*plane._n.y)+Rabs(best._extends.z*plane._n.z)-EPSIL;
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
    int nSide = dS > 0;
    dS -= offset * (nSide ? 1 : -1);
    dE -= offset * (nSide ? 1 : -1);
    
    if(dS * dE <= 0)
    {
        REAL t  = (dS-(EPSIL)) / (dS-dE);
        CLAMPVAL(t, 0.00, 1.00);
        V3  vM  = vStart + ((vEnd - vStart) * t);

        if(_R_ModelIntersect(best, pNode->_nodesIdx[nSide], vStart, vM, 0, 1))
        {
            return 1;
        }
        else if(_R_ModelIntersect(best, pNode->_nodesIdx[!nSide], vM, vEnd, 0, 1))
        {
            REAL dist = vdist(vStart, vM);
            if(best._hit == 0 || dist <= best._dist)
            {
                best._dist   = vdist(vStart, vM);
		        best._hit	 = 1;
		        best._ip     = vM;
		        best._plane  = plane;
                best._nplane = pNode->_planeIdx;
	        }
        }
    }
    else
    {
        BOOL bh = _R_ModelIntersect(best, pNode->_nodesIdx[nSide], vStart, vEnd,1,0);
        bh |= _R_ModelIntersect(best, pNode->_nodesIdx[!nSide], vStart, vEnd,1,0);
        return bh;
    }
    return best._solid;
}




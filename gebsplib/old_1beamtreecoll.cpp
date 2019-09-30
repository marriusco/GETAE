/*

'Melax' Sifting Planes collision detection in Getic BSP.

The world ( Getic BSP)is composed from the main BSP pointed by the model 0 and detail models (small bsps)
pointed by the models folowing model at index 0. A detail model (small BSP) can be dynamic so it can move 
from one location to another location from original position by the moving rules defined in the editor. 
The model original position is stored in _origin member and it's curent position in the _pivot position. 
The 2 functions ToModelSpace/FromModelSpace does nothing else than transformig the collision start
and end positions in to the model coordinate space to perform the collision with the moved model. First
I defined a collision structure that carries collision data as follows.

*/


struct Impact
{
    friend class BeamTree;
    Impact(){
        _hit        = 0;
        _stoped     = 0;
        _model      = -1;
        _dist       = INFINIT;
        _env        = 0;
        _leaf       = -1;
    }
    BOOL    _hit;               // set to true if we have a collision point
    BOOL    _stoped;            // set to true if one end of collision data reached a solid leaf
    int     _model;             // set to model number of the model we've hit
    REAL    _dist;              // diatnce deom start to the collision point
    DWORD   _env;               // environment flags we are hovering in (stored in leaf data for main BSP)
                                // and in model data for detail models
    int     _leaf;              // leaf we are in (just main bsp leaf)
    V3      _ip;                // intersection point. set if we have a collision
    Plane   _plane;             // plne set if we have a plane we collided with
};


After I have determined the next desired position where I want to move based on phisic rules I call this 
function to get the BSP intersectio. The movment is defined by the moving AABB box, that moves from point 
a to point b.


BOOL   BeamTree::ObjectIntersect (Impact &im, 
                                  const V3& a, 
                                  const V3& b, 
                                  const Box& box, 
                                  DWORD w2coll)
{

/*
    Declare a local impact data collision packet that is being used to store the best (closest) collision
    we get while testing all the models in the scene. 
*/

    Impact best;
    V3     vStart, vEnd;
    Box    bbox   = box;

    im.Reset();
    best._ip        = b;
    best._hit       = 0;

/*
    For each BSP (model) in the scene. we start testing the collision.
*/

    BspModel* pModel =  &_models[0];
	for(int i=0; i< _models._size; ++i, ++pModel)
    {

/*
    Make a copy of start and end positions due to moving models, so we can transpose these positions into the
    moving model space.
*/
        vStart = a;
        vEnd   = b;
        
        if(i > 0)
        {
/*
    If the model is detail (index not 0) and the model has a dynamic flag (it can move from it's original position)
    get the start end end position into the model space.
*/
            if(pModel->_props & MODEL_DYNAMIC)
            {
                vStart = pModel->ToModelSpace(a);
                vEnd   = pModel->ToModelSpace(b);
            }
        }
/*
    Build a  box around the move embracing the bounding box too. These utility functions are found in baselib.h 
    Test if the moving box touches the model box. If not ignore the model. More tests can be made to avoid
    unncesarelly box-to-box tests by  comparing model leaf and curent object leaf. 
*/

        Box mbox = bbox;
        mbox.MakeMoveBbox(vStart, vEnd, 0);
        mbox.Expand(16);

        if(i > 0 && !mbox.IsTouchesBox(pModel->_bx))
        {
             continue;
        }

/*
    Prepare the best coll packet fo the next model.
*/
        best._model = i;
        best._hit   = 0;
        best._stoped = 0;
        if(_R_ModelIntersect(best, nRootNode,  vStart, vEnd, ra,rb))
        {
/*
    This is a hack and I dont understand how I can reach a solid leaf without hawing at least
    one time the segment split and get a valid intersetion point. I am still investigating and It may
    have to do with the floating point precision on very very small inclined planes, but works and
    holds the boot on surface.
*/
            if(best._stoped && best._hit==0)
            {
                best._hit   = 1;
                best._ip    = vStart + (-best._fwd * EPSILON);
                best._dist  = 0;
                best._plane = Plane(best._fwd, best._ip);
            }

/*
    If the model was dynamic we get back the ip into the real world
*/
			if(pModel->_props & MODEL_DYNAMIC)
            {
				pModel->UnTransformVertex(best._ip);
				pModel->UnTransformPlane(best._plane, best._ip);
			}

/*
    Hold to the closest IP to the start position.
*/
            if(best._dist < im._dist)
            {
                im._dist  = best._dist; 
                im._hit   = best._hit;  
                im._ip    = best._ip;   
                im._model = best._model;
                im._plane = best._plane;
            }
        }
/*
    Hold to allenvironmants we've hit in all leafs and the leaf we are in into the main model.
*/
        im._env  |= best._env;
        if(i==0)
        {
            im._leaf  = best._leaf;
        }
    }
    return im._hit;
}




BOOL BeamTree::_TestLeaf(BtNode* pNode, Impact& best, const V3& vStart, const V3& vEnd)
{
    
    if(pNode->IsSolid())
    {
        if(best._model == 0)
            best._env |=  CONT_CANTGO ;
        else
            best._env |= _models[best._model]._flags;
        best._stoped   = TRUE;
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
                                   const V3&  vStart, const V3& vEnd)
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
        return _R_ModelIntersect(best, pNode->_nodesIdx[0], vStart, vEnd);
    }
    else if(dS >= offset && dE >=offset )
    {
        return _R_ModelIntersect(best, pNode->_nodesIdx[1], vStart, vEnd);
    }
    int nSide = dS > 0;
    dS -= offset * (nSide ? 1 : -1);
    dE -= offset * (nSide ? 1 : -1);
    
    if(dS * dE < 0)
    {
        REAL t  = (dS-(EPSIL)) / (dS-dE);
        CLAMPVAL(t, 0.00, 1.00);
        V3  vM  = vStart + ((vEnd - vStart) * t);

        if(_R_ModelIntersect(best, pNode->_nodesIdx[nSide], vStart, vM))
        {
            return 1;
        }
        else if(_R_ModelIntersect(best, pNode->_nodesIdx[!nSide], vM, vEnd))
        {
            REAL dist = vdist(vStart, vM);
            if(best._hit == 0 || dist <= best._dist)
            {
                best._dist   = vdist(vStart, vM);
		        best._hit	 = 1;
		        best._ip     = vM;
		        best._plane  = plane;
	        }
        }
    }
    else
    {
        BOOL bh = _R_ModelIntersect(best, pNode->_nodesIdx[nSide], vStart, vEnd);
        bh |= _R_ModelIntersect(best, pNode->_nodesIdx[!nSide], vStart, vEnd);
        return bh;
    }
    return best._stoped;
}

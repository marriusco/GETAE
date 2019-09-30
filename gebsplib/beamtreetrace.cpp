//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "basecont.h"
#include "system.h"
#include "beamtree.h"

//---------------------------------------------------------------------------------------

static REAL EPSIL     = 1.0/4;
static REAL EPSILH    = 1.0/64;

REAL outputFraction;
V3   outputEnd;
BOOL outputStartsOut;
BOOL outputAllSolid;



BOOL BeamTree::Trace( Impact& best, int rootnode, const V3&  inputStart, const V3& inputEnd )
{
	outputStartsOut = true;
	outputAllSolid = false;
	outputFraction = 1.0f;

	// walk through the BSP tree
	CheckNode( best, rootnode, 0.0f, 1.0f, inputStart, inputEnd );

	if (outputFraction == 1.0f)
	{	// nothing blocked the trace
		best._ip = inputEnd;
	}
	else
	{	
        best._ip  = inputStart + outputFraction*(inputEnd-inputStart);
        best._hit = 1;
	}
    return best._hit;
}

void BeamTree::CheckNode(Impact& best, int rootnode, REAL startFraction, REAL endFraction, V3 start, V3 end )
{

    BtNode* pNode = &_nodes[rootnode];

    if (pNode->IsLeaf())
	{	// this is a leaf
        if(pNode->IsSolid())
        {
            if(best._model == 0)
                best._env |=  CONT_CANTGO ;
            else
                best._env |= _models[best._model]._flags;
            best._ip = start;
            best._hit= 1;
            return ;
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
		BtLeaf& l =  _leafs[pNode->_leafIdx];
        if(l._sides._size)
        {
            CheckBrush(best, l , start, end);
        }
		return;
	}

	// this is a pNode
	Plane& plane            = _planes[pNode->_planeIdx];
    REAL   startDistance    = plane.DistTo(start);
    REAL   endDistance      = plane.DistTo(end);
    REAL   offset           = Rabs(best._extends.x*plane._n.x)+Rabs(best._extends.y*plane._n.y)+Rabs(best._extends.z*plane._n.z);

	if (startDistance >= offset && endDistance >= offset)
	{	// both points are in front of the plane
		// so check the front child
		CheckNode(best, pNode->_nodesIdx[1], startFraction, endFraction, start, end );
	}
	else if (startDistance < -offset && endDistance < -offset)
	{	// both points are behind the plane
		// so check the back child
		CheckNode(best, pNode->_nodesIdx[0], startFraction, endFraction, start, end );
	}
	else
	{	// the line spans the splitting plane
		int side;
		REAL fraction1, fraction2, middleFraction;
		V3 middle;

		// split the segment into two
		if (startDistance < endDistance)
		{
			side = 0; // back
			REAL inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance - offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance + offset + EPSILON) * inverseDistance;
		}
		else if (endDistance < startDistance)
		{
			side = 1; // front
			REAL inverseDistance = 1.0f / (startDistance - endDistance);
			fraction1 = (startDistance + offset + EPSILON) * inverseDistance;
			fraction2 = (startDistance - offset - EPSILON) * inverseDistance;
		}
		else
		{
			side = 1; // front
			fraction1 = 1.0f;
			fraction2 = 0.0f;
		}

		// make sure the numbers are valid
		if (fraction1 < 0.0f) fraction1 = 0.0f;
		else if (fraction1 > 1.0f) fraction1 = 1.0f;
		if (fraction2 < 0.0f) fraction2 = 0.0f;
		else if (fraction2 > 1.0f) fraction2 = 1.0f;

		// calculate the middle point for the first side
		middleFraction = startFraction + (endFraction - startFraction) * fraction1;
    	middle = start + fraction1 * (end - start);

		// check the first side
		CheckNode(best, pNode->_nodesIdx[side], startFraction, middleFraction, start, middle );

		// calculate the middle point for the second side
		middleFraction = startFraction + (endFraction - startFraction) * fraction2;
        middle = start + fraction2 * (end - start);

		// check the second side
		CheckNode(best, pNode->_nodesIdx[!side], middleFraction, endFraction, middle, end );
	}
}

void BeamTree::CheckBrush(Impact& best, BtLeaf& l, V3 vStart, V3 vEnd )
{
	REAL startFraction = -1.0f;
	REAL endFraction = 1.0f;
	BOOL startsOut = false;
	BOOL endsOut = false;

    for(int s = 0; s < l._sides._size; s++)
	{
		Plane&  plane = _planes[l._sides[s]];

        REAL  offset   = Rabs(best._extends.x*plane._n.x)+
                         Rabs(best._extends.y*plane._n.y)+
                         Rabs(best._extends.z*plane._n.z);

        REAL    startDistance = plane.DistTo(vStart)-offset;
        REAL    endDistance = plane.DistTo(vEnd)-offset;

		if (startDistance > 0)
			startsOut = true;
		if (endDistance > 0)
			endsOut = true;

		// make sure the trace isn't completely on one side of the brush
		if (startDistance > 0 && endDistance > 0)
		{   // both are in front of the plane, its outside of this brush
			return;
		}
		if (startDistance <= 0 && endDistance <= 0)
		{   // both are behind this plane, it will get clipped by another one
			continue;
		}

		if (startDistance > endDistance)
		{   // line is entering into the brush
			REAL fraction = (startDistance - EPSILON) / (startDistance - endDistance);
			if (fraction > startFraction)
				startFraction = fraction;
		}
		else
		{   // line is leaving the brush
			REAL fraction = (startDistance + EPSILON) / (startDistance - endDistance);
			if (fraction < endFraction)
				endFraction = fraction;
		}
	}

	if (startsOut == false)
	{
		if (endsOut == false)
			outputAllSolid = true;
		return;
	}

	if (startFraction < endFraction)
	{
		if (startFraction > -1 && startFraction < outputFraction)
		{
			if (startFraction < 0)
				startFraction = 0;
			outputFraction = startFraction;
		}
	}
}

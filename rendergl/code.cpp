//---------------------------------------------------------------------------------------
BOOL		BeamTree::SegmentIntersect(V3& p1, V3& p2, Impact* pColl)
{
	Best best;

	_hitTest = FALSE;
	if(R_SegmentIntersect(0, p1, p2, best))
	{
        if(_models._size > 1)
        {
            _hitTest = FALSE;
            IntersectModels(p1, best.bestIP, best);
        }

        // check models from p1 best.bestIP
		pColl->_hitpoint = best.bestIP;
		pColl->_hitplane = best.bestPlane;
        pColl->_hitmodel = best.bestModel;
        pColl->_hitdist  = best.bestDist;
        pColl->_valid    = best.valid;
	}
    return pColl->_valid;
}

//---------------------------------------------------------------------------------------
// checks if the segment intersects any model
void BeamTree::IntersectModels(V3& p1, V3& p2, Best& best)
{
    Best    localBest;
    Box     fromSeg;
    V3      a;
    V3      b;

    for(int i=1; i<_models._size; ++i)
    {
        BspModel& model = _models[i];
        
        // fast bbox collision on model space
        a = p1;
        b = p2;
        model.TransformVertex(a);
        model.TransformVertex(b);
        fromSeg.AddPoint(a);
        fromSeg.AddPoint(b);
        if(!fromSeg.IsTouchesBox(model._bbox))
        {
            continue;
        }
        
        // collide in the models space
        if(R_SegmentIntersect(model._rootNode, a, b, localBest))
        {
            REAL dst = vdist(p1, localBest.bestIP); // dist to a in world space

            if(dst < best.bestDist)
            {
                best.bestDist    = dst;
	            best.bestIP      = localBest.bestIP;
		        best.bestPlane   = localBest.bestPlane;
                best.bestModel   = i;
                localBest.valid  = TRUE;
            }
        }
    }

    if(localBest.valid) // transform back on world space
    {
        BspModel& model = _models[best.bestModel];
	    model.UnTransformVertex(best.bestIP);
        model.UnTransformPlane(best.bestPlane, best.bestIP);
    }
}


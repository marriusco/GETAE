//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "motion.h"
#include "beamtree.h"

//---------------------------------------------------------------------------------------
V3    MotionPath::MoveModel(BspModel* pModel, REAL timeSlice)
{
    MotionNode* pMNode = &_moveNodes[_brPos];
    MotionNode* pNextMNode = &_moveNodes[_brPosNext];

    if(_curState == MP_PAUSE) 
    {
        if(pMNode->_pauseTTL==-1)
            return _curState;

		pMNode->_pauseTTL -= timeSlice;
        pModel->_depl.reset();
        if(pMNode->_pauseTTL<=0)
        {
            //
            // each frame step deplasament
            //
            
            CalcNextIndex();
            pMNode	   = &_moveNodes[_brPos];
            pNextMNode = &_moveNodes[_brPosNext];

            // vector betwen 2 points
            REAL trtsec     = pMNode->_trTime/1000.0;
            pModel->_speed  = (pNextMNode->_wpos - pMNode->_wpos);
            pModel->_speed /= trtsec;
            V3      depl   = pModel->_speed * timeSlice;
            //each step translation
            pModel->_trmatStep = MTranslate(depl.x, depl.y, depl.z);
            

            //
            //  position the model in this node
            //
            V3  trOffset       = pMNode->_wpos - pModel->_origin;
            pModel->_trmat     = MTranslate(trOffset.x,trOffset.y,trOffset.z);
            //pModel->_trpos     = pModel->_origin;
            //pModel->_trmat.v3transform(pModel->_trpos);
			_curState          = MP_MOVES;
        }
    }
    else if(_curState == MP_MOVES)
    {
        pModel->_from = pModel->_trpos;

        //move till reached next point
        pModel->_trmat *= pModel->_trmatStep;
        pModel->_trpos = pModel->_origin;

        pModel->_trmat.v3transform(pModel->_trpos);

        pMNode	   = &_moveNodes[_brPos];
        pNextMNode = &_moveNodes[_brPosNext];

        pModel->_depl = pModel->_trpos-pModel->_from;

        REAL d = vdist(pModel->_trpos, pNextMNode->_wpos);

        if(d < (vdist(pModel->_from, pModel->_trpos)+2))
        {
            pModel->_from = pModel->_trpos;
            _brPos     = _brPosNext;
            _curState  = MP_PAUSE;
            pMNode->_pauseTTL = 10.0;
        }
    }
    return _curState;
}

//---------------------------------------------------------------------------------------
BOOL     MotionPath::CalcNextIndex()
{
    if(_flags & M_LOOP)
    {
        _direction = 1;

        if(_brPos == Last())
            _brPosNext = 0;
        else
            _brPosNext = _brPos+1;
    }
    else
    {
        if(_direction==1)
        {
            if(_brPos == Last())
            {
                _brPosNext = _brPos-1;
                _direction = 0;
            }
            else
                _brPosNext = _brPos+1;
        }
        else
        {
            if(_brPos == 0)
            {
                _brPosNext = _brPos+1;
                _direction = 1;
            }
            else
                _brPosNext = _brPos-1;
        }
    }
    return 1;
}




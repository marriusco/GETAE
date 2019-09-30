//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#ifndef __MOTION_H__
#define __MOTION_H__

//---------------------------------------------------------------------------------------
#include "baselib.h"
#include "basecont.h"


//---------------------------------------------------------------------------------------
// node along the path where the brush makes a stop
class NO_VT MotionNode
{
public:
    MotionNode(V3& wpos,V3& a, REAL pt, REAL tt):_wpos(wpos),_angles(a),
                                                 _pauseTime(pt),_trTime(tt),
                                                 _pauseTTL(pt), _transTTL(tt)  
    {
    }
    // movement props
    V3      _wpos;              // world pos
    V3      _angles;            // orientation    
    REAL    _pauseTime;         // pause time
    REAL    _trTime;            // transition time to next Node
    // runtime data
    REAL    _pauseTTL;          // remaining time in this position
    REAL    _transTTL;          // remaining time in transition
};

//---------------------------------------------------------------------------------------
// moving path.
struct BspModel;
class  NO_VT MotionPath
{
public:
    typedef enum _M_PATH{MP_PAUSE, MP_MOVES}M_PATH;
    
public:
    MotionPath(DWORD flags):_flags(flags),_brPos(0),_brPosNext(0),_curState(MP_PAUSE),_direction(1),_vd(INFINIT){};
    ~MotionPath(){
    };
    void    AddNode(V3& vpos, V3& vEuler, REAL pt, REAL tt){
        MotionNode mn(vpos, vEuler, pt,tt);
        _moveNodes.push_back(mn);
    }
    V3    MoveModel(BspModel*, REAL);
private:
    BOOL   CalcNextIndex();
    int    Last(){return _moveNodes.size()-1;}

public:
    // path data
    DWORD               _flags;
    vector<MotionNode>  _moveNodes;
    //  runtime data
    int                 _brPos;          // brush position index alog the path
    int                 _brPosNext;
    M_PATH              _curState;          
    int                 _direction;      // 1 forward, -1 backward, 
	REAL				_vd;
};

#endif // 

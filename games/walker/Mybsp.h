//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================

#pragma once
#include "beamtreesi.h"
#include "beamtree.h"


class MyBSP : public BeamTree
{
public:
    MyBSP():BeamTree(BspProc,0){};
    ~MyBSP(void){};

public:
    static long BspProc(BeamTree* pb,long m, long w, long l){
        return ((MyBSP*)pb)->_BspProc(m,w,l);
    }
private:
    void    _HandleReadItem(const BTF_Item*);
    void    _CreateActor(const BTF_Item* pActor);
    void    _CreateLightHallo(const BTF_Item* pLight);
    long    _BspProc(long m, long w, long l);
};

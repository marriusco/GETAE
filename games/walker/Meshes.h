//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once





Vtx*    MakeQuad(V3& pos, REAL length, REAL height, char ax);


class Mesh
{
public:
    Mesh(){
        p_prims  = 0;
        p_strips = 0;
        p_pnorms = 0;
        i_grps   = 0;
        i_strps   = 0;
        p_idxes   = 0;
    }
    virtual ~Mesh();

    int     i_strps;
    int     i_grps;
    int*    p_prims;
    int*    p_idxes;
    V3*     p_strips;
    V3*     p_pnorms;
    UV*     p_uvs;
    Box     _box;
};


class Cylinder : public Mesh
{
public:
    Cylinder(REAL diam, REAL l, int strps, int stcks);
    ~Cylinder(){}

};


class MSphere : public Mesh
{
public:
    MSphere(REAL diam, REAL l, int strps, int stcks);
    ~MSphere(){}

};



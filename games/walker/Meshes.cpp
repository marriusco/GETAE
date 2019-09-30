//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "StdAfx.h"
#include ".\meshes.h"


void MakeZQuad(V3& pos, REAL length,REAL height, V3* v)
{
	REAL lpe2 = length/2;
    REAL hpe2 = height/2;
    int index = (pos.z>0) ? 3 : 0;
    int inc   = (pos.z>0) ? -1 : 1;

    /*
       0 1
        +
       3 2
    */
	v[index].x = pos.x-lpe2;
	v[index].y = pos.y+hpe2;
	v[index].z = pos.z;

    index+=inc;
	v[index].x = pos.x+lpe2;
	v[index].y = pos.y+hpe2;
	v[index].z = pos.z;

    index+=inc;
	v[index].x = pos.x+lpe2;
	v[index].y = pos.y-hpe2;
	v[index].z = pos.z;

    index+=inc;
	v[index].x = pos.x-lpe2;
	v[index].y = pos.y-hpe2;
	v[index].z = pos.z;
}

Vtx*    MakeQuad(V3& pos, REAL length, REAL height, char ax)
{
    static BOOL  Bdone=0;
    static Vtx  vt[4];

    if(Bdone)
        return vt;

    V3           v[4];

	MakeZQuad(pos, length, height, v);

    vt[0]._xyz = v[3];
    vt[1]._xyz = v[2];
    vt[2]._xyz = v[1];
    vt[3]._xyz = v[0];

	vt[0]._uv[0] = UV(0,0);
	vt[1]._uv[0] = UV(1,0);
	vt[2]._uv[0] = UV(1,1);
	vt[3]._uv[0] = UV(0,1);

   	vt[0]._uv[1] = UV(0,0);
	vt[1]._uv[1] = UV(1,0);
	vt[2]._uv[1] = UV(1,1);
	vt[3]._uv[1] = UV(0,1);


	vt[0]._rgb =  ZWHITE;
	vt[1]._rgb =  ZWHITE;
	vt[2]._rgb =  ZWHITE;
	vt[3]._rgb =  ZWHITE;
    /*
            (x)
            ^
            |
         0  |   1
        00  |   10
    ---------------->(y)(u)
            |
        3   |   2
        01  |   11
            v(v)

    */
    Bdone=1;
    return vt;
}


Mesh::~Mesh()
{
    delete[] p_prims;
    delete[] p_strips;
    delete[] p_pnorms;
    delete[] p_uvs;
    delete[] p_idxes;
}

Cylinder::Cylinder(REAL diam, REAL l, int strps, int grps)
{
//    p_prims  = new int[1];

    p_prims  = new int[grps + 1];
    p_strips = new V3[(grps + 1) * strps];
    p_pnorms = new V3[(grps + 1) * strps];
    p_uvs    = new UV[(grps + 1) * strps];
    p_idxes  = new int[(grps * (strps+2)) * 2];
    int        idxidx = 0;

    REAL    rad  = diam * 0.50000;
    REAL    arc  = DOIPI / strps;
    REAL    sh   = l/grps;
    int     idx;
    for(int y=0;y <= grps;y++)
    {
        if(y<grps)
            p_prims[y]  = Z_TRIANGLE_STRIP;
        
        for(int x=0;x<strps;x++)
        {
            idx = y*strps+x;
            p_strips[idx].x = rad*sinf(arc*x);
            p_strips[idx].z = rad*cosf(arc*x);
            p_strips[idx].y = sh * y;

            p_pnorms[idx] = p_strips[idx] - V3(0,p_strips[idx].y,0);
            p_pnorms[idx].norm();

            p_uvs[idx].u = (REAL)(arc*x) / DOIPI;
            p_uvs[idx].v = (REAL)y/(REAL)grps;

            if(y<grps)
            {
                p_idxes[idxidx++] = idx;
                p_idxes[idxidx++] = (y+1) * strps + x;
            }
        }

        p_idxes[idxidx++] = (y) * strps;
        p_idxes[idxidx++] = (y+1) * strps;

    }
    i_grps  = grps;
    i_strps = strps;
}


MSphere::MSphere(REAL diam, REAL l, int strps, int grps)
{
    //p_prims  = new int[1];

    p_prims  = new int[grps + 1];
    p_strips = new V3[(grps + 1) * strps];
    p_pnorms = new V3[(grps + 1) * strps];
    p_uvs    = new UV[(grps + 1) * strps];
    p_idxes  = new int[(grps * (strps+2)) * 2];
    int        idxidx = 0;

    REAL    radm  = diam * 0.50000;
    REAL    arc   = DOIPI / strps;
    REAL    sh    = l/grps;
    int     idx;
    for(int y=0; y <= grps; y++)
    {
        if(y < grps)
            p_prims[y]  = Z_TRIANGLE_STRIP;
        
        REAL rad = sinf((REAL)y/(REAL)grps * PI) * radm;

        for(int x=0;x<strps;x++)
        {
            idx = y*strps+x;
            p_strips[idx].x = rad*sinf(arc*x);
            p_strips[idx].z = rad*cosf(arc*x);
            p_strips[idx].y = sh * y;

            p_pnorms[idx] = p_strips[idx] - V3(0,p_strips[idx].y,0);
            p_pnorms[idx].norm();

            p_uvs[idx].u = (REAL)(arc*x) / DOIPI;
            p_uvs[idx].v = (REAL)y/(REAL)grps;

            if(y<grps)
            {
                p_idxes[idxidx++] = idx;
                p_idxes[idxidx++] = (y+1) * strps + x;
            }
        }

        p_idxes[idxidx++] = (y) * strps;
        p_idxes[idxidx++] = (y+1) * strps;

    }
    i_grps  = grps;
    i_strps = strps;
}



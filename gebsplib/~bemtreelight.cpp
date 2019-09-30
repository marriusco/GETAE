//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "basecont.h"
#include "system.h"
#include "beamtree.h"


//---------------------------------------------------------------------------------------


static  BYTE   BuffGlob[256*256*3]; //temp buffer to build each face/frame lmap
#pragma optimize( "at", on )

//---------------------------------------------------------------------------------------
BOOL		BeamTree::PushDynaLigt(PointLight* pl)
{
    return (_lInfo.Push(pl) != 0);
}

//---------------------------------------------------------------------------------------
void		BeamTree::PopDynaLight(PointLight* pl)
{
    _lInfo.Remove(pl);
}

//---------------------------------------------------------------------------------------
void	BeamTree::SetDynaLightAttrib(PointLight* pl, V3& pos, REAL radius, CLR& color)
{
    pl->Set(pos, radius, color,1,LIGHT_POINT|LIGHT_STEADY);
}

//---------------------------------------------------------------------------------------
void    BeamTree::ApplyDynaLights()
{
    PointLight* pl;
    for(int i=0;i < _lInfo.Count(); i++, pl++)
    {
        pl = _lInfo[i];

		++_Perf.dynaLights;

		if(!_pCamera->CanSeeCamera(pl->_lpos,pl->_radius))
        {
			continue;
        }

		++_Perf.visLights;

        BspModel* pModel =  &_models[0];

		R_IlumMainModel(0, pl, i);
	    for(int j=1; j < _models._size; j++,pModel++)
        {
            if(pModel->_props & MODEL_DYNAMIC)
            {
                if(pModel->_visFrm != _frmDynamic)
                    continue;
            }
            else
            {
                if(pModel->_visFrm != _frmStatic)
                    continue;
            }

            if(!pModel->_bx.IsTouchesSphere(pl->_lpos, pl->_radius))
                continue;

			if(0!=j)
			{
				if(!_pCamera->CanSee(pModel->_bx))
				   continue;
			}

            if(pModel->_props & MODEL_DYNAMIC)
                pModel->TransformVertex(pl->_lpos);

			R_IlumDetailModel(pModel, pl , i);

            if(pModel->_props & MODEL_DYNAMIC)
                pModel->UnTransformVertex(pl->_lpos);
        }
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::R_IlumDetailModel(BspModel* pModel, PointLight* pl, int lIdx)
{
	BtLeaf* pLeaf = &_leafs[pModel->_firstLeaf];

	for(int i=0; i< pModel->_leafs; i++, pLeaf++)
	{
	    if(!_nodes[pLeaf->_nodeIdx]._bbox.IsTouchesSphere(pl->_lpos, pl->_radius))
        {
               continue;
        }
		CastDynaLightInLeaf(pLeaf, pl, lIdx);
	}
}

//---------------------------------------------------------------------------------------
void    BeamTree::R_IlumMainModel(int idxNode, PointLight* pl, int lIdx)
{
    BtNode& node = _nodes[idxNode];

	if(node._visFrame != _frmStatic)
		return;

    if(node.IsLeaf())
    {
        if(!node.IsSolid())
			CastDynaLightInLeaf(&_leafs[node._leafIdx], pl, lIdx);
        return;
    }

    if(!node._bbox.IsTouchesSphere(pl->_lpos,pl->_radius))
        return;
    if(!_pCamera->CanSee(node._bbox))
        return;

    REAL    rdist = _planes[node._planeIdx].FastDistTo(pl->_lpos);
    if(rdist > pl->_radius)
    {
        R_IlumMainModel(node._nodesIdx[1], pl, lIdx);
        return;
    }else if(rdist < -pl->_radius){
        R_IlumMainModel(node._nodesIdx[0], pl, lIdx);
        return;
    }

    R_IlumMainModel(node._nodesIdx[1], pl, lIdx);
    R_IlumMainModel(node._nodesIdx[0], pl, lIdx);
}

//---------------------------------------------------------------------------------------
void    BeamTree::CastDynaLightInLeaf(BtLeaf* pLeaf, PointLight* pl, int lIdx)
{

    Face* pFace = &_faces[pLeaf->_firstPoly];
    for(int j=0;j<pLeaf->_polys;j++,pFace++)
    {
        if(pFace->_flags & FACE_NOLIGTMAP)
            continue;
        if(pFace->_flags & FACE_TRANSPARENT)
            continue;
        if(pFace->_flags & FACE_MIRROR)
            continue;
		if(pFace->_visframe1!=_frmDynamic)
			continue;
        Plane& rPlane = _planes[pFace->_planeidx];
        if(rPlane.GetSide(pl->_lpos) < 0)            // skip back planes ////???? test if necesarely
            continue;
        if(!pFace->_box.IsTouchesSphere(pl->_lpos, pl->_radius))
                continue;
        pFace->_dlights |= (1<<lIdx);
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::BuildDynaLmaps(Face* pFace)
{
    int             idx = 0;
    PointLight*     pl;
    DWORD           dwal = pFace->_dlights;
    int             blit = 0;

    // copy original kmap info in the temp buffer we alter
    ::memcpy(BuffGlob, pFace->_pdynalights[0], 
                       pFace->_lmapsize.cx * pFace->_lmapsize.cy * 3);
    while(dwal && (pl = _lInfo[idx]))
    {
        if((dwal & 0x1))
        {
            ApplyToFace(pl, pFace, BuffGlob, blit);
        }
        ++idx;
        dwal >>= 1;
    }
    if(blit)
    {
		pFace->_pdynalights[1] = BuffGlob;
    }
	else
    {
		pFace->_dlights   = 0;
        pFace->_pdynalights[1] = pFace->_pdynalights[0];
	}
}

//---------------------------------------------------------------------------------------
BOOL    BeamTree::BuildFogLmap(Face* pFace)
{
    //LM_DYNA+FOG->LM_DYNA
    Fog&    fog    = _fogs[0];
    V3      fpos   = fog.center;
    REAL    fograd = (fog.radius*2);
    CLR     clr    = fog.color;
    int     rgba[3];
    REAL    dx,dy;
    BYTE*   pbyDyna = BuffGlob;
    int     xe   = pFace->_lmapsize.cx;
    int     ye   = pFace->_lmapsize.cy;
    int     xs=0,ys=0;
    V3&     cp   = _pCamera->_pos;
    BOOL    blit = 0;
    V3*   pUV= pFace->_lmapaxes;
    V3      lumelPos;
    Impact im;

    fograd*=fograd;
    
    ::memcpy(BuffGlob, pFace->_pdynalights[0], pFace->_lmapsize.cx*pFace->_lmapsize.cy*3);


    for(int i=xs; i < xe; i++)
    {
        dx = i * pFace->_lmstep.u;
        for(int j=ys; j < ye; j++)
        {
            dy = j * pFace->_lmstep.v;

            lumelPos =  pUV[0];
            lumelPos += (pUV[1] * dx);
            lumelPos += (pUV[2] * dy);

            if(this->SegmentIntersectModel(im, fog.nmodelNode, cp, lumelPos))
            {
                V3 a = im._ip;
                if(this->SegmentIntersectModel(im, fog.nmodelNode,  lumelPos, cp))
                {
                    V3 b = im._ip;
                    REAL vD = Vdp(a,b);

				    rgba[0] = (clr.r * vD/fograd) + pbyDyna[0];
				    rgba[1] = (clr.g * vD/fograd) + pbyDyna[1];
				    rgba[2] = (clr.b * vD/fograd) + pbyDyna[2];

                    if(rgba[0]>255) rgba[0]=255;
                    if(rgba[1]>255) rgba[1]=255;
                    if(rgba[2]>255) rgba[2]=255;

				    pbyDyna[0] = rgba[0];
				    pbyDyna[1] = rgba[1];
				    pbyDyna[2] = rgba[2];   
                    blit=1;
                }
			}
            pbyDyna += 3;
        }
    }
    if(blit)
        pFace->_pdynalights[LM_DYNA] = BuffGlob;

    return blit;
}


//---------------------------------------------------------------------------------------
void    BeamTree::ApplyToFace(PointLight* pl,
                              Face* pFace,
                              BYTE* byDyna,
                              int& blit)
{

    V3    lp  = pl->_lpos;
    CLR*  clr = &pl->_color;
    REAL  intens,d2l,dx,dy;
    BYTE* pbyDyna = byDyna;
    V3    lumelPos,ip;
    int   rgba[3];
    int   xs = 0;//pFace->_lmMm[0].cx;          // range to aplly light on this face
    int   xe = pFace->_lmapsize.cx;//pFace->_lmMm[1].cx;//
    int   ys = 0;//pFace->_lmMm[0].cy;
    int   ye = pFace->_lmapsize.cy;//pFace->_lmMm[1].cy;//
    REAL  r2 = pl->_radius*pl->_radius;
    V3*   pUV= pFace->_lmapaxes;

	for(int i=xs; i < xe; i++)
    {
        dx = i * pFace->_lmstep.u;          // deplasament on U
	    for(int j=ys; j < ye; j++)
        {
            /* try fast check of saturation
            pDw = (DWORD*)pbyDyna;
            if(*pDw& 0x00FFFFFF == 0x00FFFFFF)
                continue;
            */
            if(pbyDyna[0]==255 &&
               pbyDyna[1]==255 &&
               pbyDyna[2]==255)             //light is saturated
            {
                pbyDyna+=3;
                continue;
            }
            dy = j * pFace->_lmstep.v;      //deplasamant on V

            lumelPos =  pUV[0];    // lumel position
            lumelPos += (pUV[1] * dx);
            lumelPos += (pUV[2] * dy);

            d2l = vdist2(lumelPos, lp);
            if(d2l > r2)
            {
                pbyDyna+=3;
                continue;
            }
            /** shadows
            if(NotCanSee(lumelPos+_planes[pFace->_planeIdx]._n,lp))
			{
				pbyDyna+=3;
				continue;
			}
            */
            intens = (1.1 -  (d2l / r2));

            assert(intens>0);
            // add the extra light info at the top of current values
            rgba[0] = (int)(pbyDyna[1] + (intens *  clr->r));
            rgba[1] = (int)(pbyDyna[0] + (intens *  clr->g));
            rgba[2] = (int)(pbyDyna[2] + (intens *  clr->b));

            //clamp them
            if(rgba[0]>255)rgba[0]=255;
            if(rgba[1]>255)rgba[1]=255;
            if(rgba[2]>255)rgba[2]=255;

            pbyDyna[0] = rgba[1];
            pbyDyna[1] = rgba[0];
            pbyDyna[2] = rgba[2];
            pbyDyna += 3;
            blit = 1;               // mark face as lit
        }
    }
}
#pragma optimize( "at", off )


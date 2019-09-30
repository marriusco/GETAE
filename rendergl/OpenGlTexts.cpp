//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "baseutils.h"
#include "OpenglRender.h"

//--------------------------------------------------------------------------------------------
static struct Bppform
{
    int bpp;
    int gl1;
    int gl2;
} GBpp[5] =
{
	{0, -1          ,         -1  },
    {1, GL_LUMINANCE, GL_UNSIGNED_BYTE}, //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    {2, -1          ,         -1  },
    {3, GL_RGB,  GL_UNSIGNED_BYTE },
    {4, GL_RGBA, GL_UNSIGNED_BYTE }
};

//--------------------------------------------------------------------------------------------
void Local_LoadBytes(BYTE* pD, BYTE* pS, int sx, int sy, int x, int y, int lS)
{
	BYTE* pDest = pD;
	for(int j=0;j<y;j++)
	{
		for(int i=0; i < x; i++)
		{
			*pDest = pS[sx+i + (sy +j)*lS];
			++pDest;
		}
	}
}

//--------------------------------------------------------------------------------------------
Htex OpenglRender::Local_SendMessageStore_Tex(TexLparam* pl, DWORD texFlags)
{
    Htex            htex;

    switch(TGET_TARGET(texFlags))
    {
        default:
		case GEN_TEX_3D_MAP:
        case GEN_TEX_2D_MAP:
            htex.glTarget = GL_TEXTURE_2D;
            pl->creaFlag = GL_TEXTURE_2D;
            break;
        case GEN_TEX_CUBE_MAP:
            htex.glTarget = GL_TEXTURE_CUBE_MAP;
            pl->creaFlag = GL_TEXTURE_CUBE_MAP;
            break;
    }
    pl->userFlag = texFlags;
    htex.hTex   = ::SendMessage(Hwnd(), WM_OPENGLNOTY, WM_STORE_TEXTURE, (LPARAM)pl);
    OpenglRender::_maxtexId = tmax((int)htex.hTex, (int)OpenglRender::_maxtexId);
    return htex;
}

//--------------------------------------------------------------------------------------------
Htex  OpenglRender::GenTexture(const char* texFile, DWORD flags)
{
    static Htex h;
    return h;
}

//--------------------------------------------------------------------------------------------
// can be called from different thread
Htex OpenglRender::GenTexture(int x, int y, int bpp, const BYTE* pBuff, DWORD texFlags)
{
    TexLparam      pl;

    pl.ht        = 0;
    pl.x         = x;
    pl.y         = y;
	pl.bpp       = bpp;
    pl.parr      = (BYTE*)pBuff;
    return Local_SendMessageStore_Tex(&pl, texFlags);
}

//--------------------------------------------------------------------------------------------
HTEX OpenglRender::GenTextureMsg(LPARAM lp)
{
    TexLparam* pTf  = (TexLparam*)lp;

    int   x             = pTf->x;
    int   y             = pTf->y;
    int   bpp           = pTf->bpp;
    BYTE* pBuff         = pTf->parr;

    wglMakeCurrent(_hDC, _hRC); 
    if(pTf->ht == 0)
    {
        glGenTextures(1, (UINT*)&pTf->ht);
    }

    glBindTexture(pTf->creaFlag, pTf->ht);

	// decompose the on bitmap (see if 6 bitmaps are there)
    if(TGET_WRAP(pTf->userFlag) == GEN_TEX_HAS_CUBE_T)
	{
		BYTE* lb = new BYTE[(x*y*bpp)/12];

		Local_LoadBytes(lb, pBuff, x/4, 0, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);

		Local_LoadBytes(lb, pBuff, 0, y/3, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, x/4, y/3, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, 2*x/4, y/3, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, 3*x/4, y/3, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, x/4, 2*y/3, x/4, y/3, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/4, y/3, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);
		delete[] lb;

		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP);


	}
	if(TGET_WRAP(pTf->userFlag) == GEN_TEX_HAS_CUBE_M)
	{
		BYTE* lb = new BYTE[(x*y*bpp)/6];

		Local_LoadBytes(lb, pBuff, 0, 0, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);

		Local_LoadBytes(lb, pBuff, x/3, 0, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, 2*x/3, 0, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, 0, y/2, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, x/3, y/2, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);


		Local_LoadBytes(lb, pBuff, 2*x/3, y/2, x/3, y/2, x);
		glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
						0, GL_RGB, x/3, y/2, 0, GL_RGB, GL_UNSIGNED_BYTE, lb);
		delete[] lb;
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP);


	}

    if(TGET_WRAP(pTf->userFlag) == GEN_CLAMP)
    {
		glTexParameteri(pTf->creaFlag, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(pTf->creaFlag, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //glTexParameteri(pTf->creaFlag, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_SGIS);
        //glTexParameteri(pTf->creaFlag, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_SGIS);
	}
	else
    {
		glTexParameteri(pTf->creaFlag,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(pTf->creaFlag,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

    glTexParameteri(pTf->creaFlag, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    switch(TGET_FILTER(pTf->userFlag))
    {
        case GEN_TEX_MM_LINEAR: //lin
            glTexParameteri(pTf->creaFlag, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        default:
        case GEN_TEX_MM_BILINEAR: // 2 lin
            glTexParameteri(pTf->creaFlag, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case GEN_TEX_MM_TRILINEAR: // 3 lin
            glTexParameteri(pTf->creaFlag, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
    }

    if(pTf->userFlag & TPP_MODULATE)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    else
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(pTf->creaFlag,0,bpp,x,y,0,GBpp[bpp].gl1, GBpp[bpp].gl2,pBuff);
	gluBuild2DMipmaps(pTf->creaFlag,bpp,x,y,GBpp[bpp].gl1, GBpp[bpp].gl2,pBuff);

    if(pTf->ht>0)
        ++_enabledTextures;
	return pTf->ht;
}

//--------------------------------------------------------------------------------------------
void OpenglRender::DeleteTextureMsg(LPARAM lp)
{
    TexLparam* pTf  = (TexLparam*)lp;
	if(glIsTexture(pTf->ht))
    {
		glDeleteTextures(1, (UINT*)&pTf->ht);
        --_enabledTextures;
	}
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::RemoveAllTextures
void OpenglRender::RemoveAllTextures()
{
    // bad assumtion
    for(int i=1 ; i < (OpenglRender::_maxtexId+1); i++)
    {
	    if(glIsTexture(i))
        {
		    glDeleteTextures(1, (UINT*)&i);
            --_enabledTextures;
	    }
    }
}

//--------------------------------------------------------------------------------------------
void OpenglRender::RemoveTextures(const UINT* ptex, int count)
{
    glDeleteTextures(count, ptex);
    _enabledTextures-=count;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::RemoveTexture
void OpenglRender::RemoveTexture(UINT index, BOOL fromVector)
{
	if(glIsTexture(index))
    {
		int lt = index;
		glDeleteTextures(1, (UINT*)&lt);
        --_enabledTextures;
	}
}

//--------------------------------------------------------------------------------------------
// can be called from different thread
Htex OpenglRender::ReGenTexture(const Htex& tex, int x, int y, int bpp, const BYTE* pBuff, DWORD texFlags)
{
    TexLparam       pl;

    pl.ht        = tex.hTex;
    pl.x         = x;
    pl.y         = y;
	pl.bpp       = bpp;
    pl.parr      = (BYTE*)pBuff;
	pl.creaFlag  = texFlags;

    ::SendMessage(Hwnd(), WM_OPENGLNOTY, WM_DELETE_TEXTURE, (LPARAM)&pl);
    return Local_SendMessageStore_Tex(&pl, texFlags);
}

//--------------------------------------------------------------------------------------------
// updates .5 already stored driver texture with the passed in psub[u][v]
void OpenglRender::AlterTexture(const Htex& htex, 
                                const BYTE* psub, 
                                int s, int t, int u, int v)
{
#ifdef _DEBUG
    if(!glIsTexture(htex.hTex)) {assert(0); return ;}
#endif //
    glEnable(htex.glTarget);
    glBindTexture(htex.glTarget, htex.hTex);
	glTexSubImage2D( htex.glTarget, 0, s, t, u, v, GL_RGB, GL_UNSIGNED_BYTE, psub);
}

//--------------------------------------------------------------------------------------------
void OpenglRender::UnBlendv()
{
    DWORD dCtx = _ctxStack.Pop();
	if(dCtx & DS_BLEND)
    {
        if(dCtx & DS_DEEPTEST_OFF)
        {
			glDepthMask(1);
		}
        if(dCtx & DS_NODEEPTEST)
        {
			glEnable(GL_DEPTH_TEST);
        }
        glDisable(GL_BLEND);
	}
	if(dCtx & DS_TEXT_BIT)
		glPopAttrib();
}

//--------------------------------------------------------------------------------------------
void OpenglRender::CleanTexCache(int i)
{
    _cachedTex[0].Clear();
    _cachedTex[1].Clear();
    _cachedTex[2].Clear();
    _cachedTex[3].Clear();
}


//---------------------------------------------------------------------------------------
// loBYTE of comb has text count
DWORD OpenglRender::BindAllTextures(const Htex* pTxStg, DWORD comb)
{
	return _PushTextures((Htex*)pTxStg, comb);
}

//---------------------------------------------------------------------------------------
// returns a bit se in the position of i
DWORD OpenglRender::BindTexture(const Htex& txStg,  int i)
{
    if(i > Ext::p_texCount) return 0;

    OpenglRender::_texareOff=0;
    Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+i);
    glEnable(txStg.glTarget);
    glBindTexture(txStg.glTarget, txStg.hTex);

    switch(TGET_GENST(txStg.genST))
    {
        case GEN_TEX_GEN_NA: 
            if(TGET_GENST(_cachedTex[i].genST) != GEN_TEX_GEN_NA)
            {
	            glDisable(GL_TEXTURE_GEN_S);
	            glDisable(GL_TEXTURE_GEN_T);
            }
            break;
        case GEN_TEX_GEN_ST_CUBE:
	        glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glEnable(GL_TEXTURE_GEN_S);
	        glEnable(GL_TEXTURE_GEN_T);
            break;
        case GEN_TEX_GEN_ST_SPH:
	        glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	        glEnable(GL_TEXTURE_GEN_S);
	        glEnable(GL_TEXTURE_GEN_T);
            break;        
    }
    _cachedTex[i] = txStg;
    return (VX_TX1 << i);
}

//---------------------------------------------------------------------------------------
void OpenglRender::UnBindTexture(const Htex& txStg, int i)
{
    i = tmin(i, Ext::p_texCount);
    if(_cachedTex[i].hTex)
    {
	    Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+i);
        if(TGET_GENST(_cachedTex[i].genST) != GEN_TEX_GEN_NA)
        {
            glDisable(GL_TEXTURE_GEN_S);
	        glDisable(GL_TEXTURE_GEN_T);
        }
        glDisable(_cachedTex[i].glTarget);
        _cachedTex[i].Clear();
    }
}

//---------------------------------------------------------------------------------------
void OpenglRender::DisableTextures(BOOL force)
{
    REG int tl = Ext::p_texCount;
    while(--tl>=0)
    {
        Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+tl);
        glDisable(GL_TEXTURE_2D);
        _cachedTex[tl].Clear();
    }
    OpenglRender::_texareOff=1;
}


//--------------------------------------------------------------------------------------------
DWORD OpenglRender::Blend(DWORD dCtx )
{
    if(dCtx & DS_TEXT_BIT)
    {
		glPushAttrib(GL_COLOR_BUFFER_BIT);
    }
	if(dCtx & DS_SHOW_BACK)
    {
		glDisable(GL_CULL_FACE);
	}

	if(dCtx & DS_BLEND)
    {
		glEnable(GL_BLEND);
		if(dCtx & DS_BLEND_LIGHT)
        {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
		else if(dCtx & DS_BLEND_HALLO)
        {
			glBlendFunc(GL_ONE, GL_ONE);
        }
		else if(dCtx & DS_BLEND_HITMARK)
        {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
		if(dCtx & DS_DEEPTEST_OFF)
        {
			glDepthMask(0);
        }
		if(dCtx & DS_NODEEPTEST)
        {
			glDisable(GL_DEPTH_TEST);
        }
	}
    _ctxStack.Push(dCtx);
    return dCtx;
}

//--------------------------------------------------------------------------------------------
void OpenglRender::UnBlend(DWORD dCtx)
{
    DWORD dr = _ctxStack.Pop();
    assert(dCtx == dr);

	if(dCtx & DS_BLEND)
    {
        if(dCtx & DS_NODEEPTEST)
        {
			glEnable(GL_DEPTH_TEST);
        }
        if(dCtx & DS_DEEPTEST_OFF)
        {
			glDepthMask(1);
		}
        glDisable(GL_BLEND);
	}
	if(dCtx & DS_TEXT_BIT)
		glPopAttrib();
}



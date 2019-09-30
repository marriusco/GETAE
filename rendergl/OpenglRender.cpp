//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "OpenglShader.h"
#include "OpenglRender.h"


int                             Ext::p_texCount                      = 0;
PFNGLCLIENTACTIVETEXTUREARBPROC Ext::p_glClientActiveTextureARB      = (PFNGLCLIENTACTIVETEXTUREARBPROC )Ext::DUMMY_PFNGLACTIVETEXTUREARBPROC;
PFNGLACTIVETEXTUREARBPROC       Ext::p_glActiveTextureARB            = (PFNGLACTIVETEXTUREARBPROC       )Ext::DUMMY_PFNGLCLIENTACTIVETEXTUREARBPROC;
PFNGLMULTITEXCOORD2FARBPROC     Ext::p_glMultiTexCoord2fARB          = (PFNGLMULTITEXCOORD2FARBPROC     )Ext::DUMMY_PFNGLMULTITEXCOORD2FARBPROC;
PFNGLMULTITEXCOORD2FVARBPROC    Ext::p_glMultiTexCoord2fvARB         = (PFNGLMULTITEXCOORD2FVARBPROC    )Ext::DUMMY_PFNGLMULTITEXCOORD2FVARBPROC;
PFNGLLOCKARRAYSEXTPROC          Ext::p_glLockArraysExtProc           = (PFNGLLOCKARRAYSEXTPROC          )Ext::DUMMY_PFNGLLOCKARRAYSEXTPROC;  
PFNGLUNLOCKARRAYSEXTPROC        Ext::p_glUnlockArraysExtProc         = (PFNGLUNLOCKARRAYSEXTPROC        )Ext::DUMMY_PFNGLUNLOCKARRAYSEXTPROC;

DWORD                           Ext::p_txCordMask                    = 0;
//--------------------------------------------------------------------------------------------
OpenglRender::OpenglRender()
{
    _pThis=this;
}


//--------------------------------------------------------------------------------------------
REAL*   FloatColor(const CLR& c)
{
    static REAL rarr[4];
    rarr[0]=c.r/255.0;
    rarr[1]=c.g/255.0;
    rarr[2]=c.b/255.0;
    rarr[3]=c.a/255.0;
    return rarr;
}

//--------------------------------------------------------------------------------------------
//    Creates the window, attches the opengl context to it, reads the card extensions
BOOL OpenglRender::CreateRender(const ISystem* pe, RndStruct* prndStates, UINT* pconsts,
                                HWND useThis)
{
    _pengine = (ISystem*)pe;
	_rnds    = prndStates;
    ::memcpy(pconsts, _glConsts, sizeof(_glConsts));

    if(!useThis)
    {
        if(!WndCreate())
            return FALSE;
    }
    else 
    {
        OpenglRender::_externalWnd=1;
        _hWnd = useThis;
        Attach(GetDC(_hWnd));
    }

    if(GetExtensions())
    {
        RECT rt = {0};
        InitialSettMode();
        //TrySettMode();
        GetClientRect(_hWnd,&rt);
        Perspective((REAL)rt.right,(REAL)rt.bottom);
    }
    return TRUE;
}

//--------------------------------------------------------------------------------------------
// this gl mode is only for testing
void OpenglRender::TrySettMode()
{
}


//--------------------------------------------------------------------------------------------
// this mode works . Is been .5 the first mode that worked well
void OpenglRender::InitialSettMode()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glClearDepth( 1 );
    glDisable(GL_FOG);
    glDisable(GL_DITHER);
    glEnable(GL_ALPHA_TEST);
    CLR c(_rnds->bgColor);
	glClearColor(c.r,c.g,c.b,c.a);
	if(_rnds->xPolyMode & SHOW_FRONT)
		glPolygonMode(GL_FRONT,GL_FILL);
	else if(_rnds->xPolyMode & SHOW_BACK)
		glPolygonMode(GL_BACK,GL_FILL);
	if(_rnds->xPolyMode & SHOW_BOTH)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	BYTE by = FALSE;
	glEdgeFlagv(&by);
	if(!(_rnds->xPolyMode & SHOW_BOTH))
	{
		if(_rnds->xPolyMode & USE_CW)
			glFrontFace(GL_CW);
		if(_rnds->xPolyMode & USE_CCW)
			glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
}

//--------------------------------------------------------------------------------------------
// refreshes the view. This is called when some of the viewport fov is changed
// fwd.y  can be changed in view matrix
void OpenglRender::RefreshModelView(REAL hFov)
{
	glViewport(0,0,(int)_rnds->nWidth, (int)_rnds->nHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(hFov, _rnds->fAspect, _rnds->fNear, _rnds->fFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//--------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::Destroy
void OpenglRender::Destroy()
{
	assert(_ctxStack.Size()==0);
    DestroyWnd();
}

//--------------------------------------------------------------------------------------------
BOOL OpenglRender::Has(const char* s)
{
    return strstr(_extensions,s)!=0; //string
}

//--------------------------------------------------------------------------------------------
// reads the extensions from card
BOOL OpenglRender::GetExtensions()
{
    BOOL    br = FALSE;
    char* pszExt  = (char*)glGetString(GL_EXTENSIONS );
	char* vendor  = (char*)glGetString(GL_VENDOR) ;
	char* rendere = (char*)glGetString(GL_RENDERER);

    if (pszExt==0)
    {
		return br;
	}

    int length = strlen(pszExt)+strlen(vendor)+strlen(rendere)+32;
    _extensions=new char[length];
   
	
    
    strcpy(_extensions, vendor);
    strcat(_extensions, "\r\n");
    strcat(_extensions, rendere);
    strcat(_extensions, "\r\n");
    strcat(_extensions, pszExt);
    

    if (Has("GL_ARB_multitexture"))
    {
		Ext::p_texCount = 0;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &Ext::p_texCount);
        Ext::p_texCount = min(Ext::p_texCount, 4);

        for(int j=0; j < Ext::p_texCount;j++) 
            Ext::p_txCordMask |= (VX_TX1 << j);

        if(Ext::p_texCount > 1 )
        {
	    	Ext::p_glActiveTextureARB         = (PFNGLACTIVETEXTUREARBPROC)       wglGetProcAddress("glActiveTextureARB");
		    Ext::p_glClientActiveTextureARB   = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		    Ext::p_glMultiTexCoord2fARB       = (PFNGLMULTITEXCOORD2FARBPROC)     wglGetProcAddress("glMultiTexCoord2fARB");
		    Ext::p_glMultiTexCoord2fvARB      = (PFNGLMULTITEXCOORD2FVARBPROC)    wglGetProcAddress("glMultiTexCoord2fvARB");
           
            br =  TRUE;
        }                                   
    }

    if(Has("GL_EXT_compiled_vertex_array"))
    {
        Ext::p_glLockArraysExtProc   = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArrysEXT");  
        Ext::p_glUnlockArraysExtProc = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArrysEXT");
    }

    return br;
}

//--------------------------------------------------------------------------------------------
PR_C OpenglRender::Gpa(const char* fn)
{
	return wglGetProcAddress(fn);
}


//--------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::BillBoardStart
void OpenglRender::BillBoardStart(DWORD mode, const V3& objectCenter, const V3& scale)
{
    REAL modelview[16];
    if(mode & OBJ_BB_CYL){
        glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
        for(int i=0; i<3; i+=2 ){
            for(int j=0; j<3; j++ ){
                if ( i==j )
                    modelview[i*4+j] = scale.x;
                else
                    modelview[i*4+j] = 0.0;
            }
        }
        glLoadMatrixf(modelview);
        return;
    }
    if(mode & OBJ_BB_SPH){
        glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
		V3 xUp(modelview[1],modelview[5],modelview[9]);
        for(int i=0; i<3; i++ ){
            for(int j=0; j<3; j++ ){
                if ( i==j )
                   modelview[i*4+j] = scale.x;
                else
                    modelview[i*4+j] = 0.0f;
            }
        }
        glLoadMatrixf(modelview);
        return;
    }
}

//--------------------------------------------------------------------------------------------
void OpenglRender::BillBoardEnd(int mode)
{
    glPopMatrix();
}

//--------------------------------------------------------------------------------------------
// creates .5 new list
void OpenglRender::NewList(int list)
{
	glNewList(list, GL_COMPILE);
}

//--------------------------------------------------------------------------------------------
// generates unic ids for the list
int OpenglRender::GenLists(int num)
{
    wglMakeCurrent(_hDC, _hRC); 
	int n = glGenLists(num);
	return n;
}

//--------------------------------------------------------------------------------------------
// finishes .5 list
void OpenglRender::EndNewList()
{
	glEndList();
}

//--------------------------------------------------------------------------------------------
// delets lists
void OpenglRender::DeleteList(UINT id,UINT many)
{
	glDeleteLists(id,many);
};

//--------------------------------------------------------------------------------------------
// draw's the list
void OpenglRender::CallList(UINT nID)
{
	glCallList(nID);
}

//--------------------------------------------------------------------------------------------
// calls more lists
void OpenglRender::CallLists(UINT a, UINT b, const void* ptr)
{
	glCallLists(a, b, ptr);
}


//--------------------------------------------------------------------------------------------
void OpenglRender::RotateTo(const V3& xyz, REAL angle)
{
    glRotatef(angle, xyz.x, xyz.y, xyz.z);
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Rotate(const V3& xyz)
{
    xyz.x ? glRotatef(xyz.x, 1.0,0.0,0.0):0;
     xyz.y ? glRotatef(xyz.y, 0.0,1.0,0.0):0;
     xyz.z ? glRotatef(xyz.z, 0.0,0.0,1.0):0;
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Scale(const V3& xyz)
{
     glScalef(xyz.x,xyz.y,xyz.z);
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Translate(const V3& xyz)
{
     glTranslatef(xyz.x,xyz.y,xyz.z);
}

//--------------------------------------------------------------------------------------------
// prepares the back buffer for new drawing
void OpenglRender::Clear(int trick)
{
    CleanTexCache();
	glClear (trick ? GL_DEPTH_BUFFER_BIT : (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	return;
}

//---------------------------------------------------------------------------------------------
void OpenglRender::SetViewMtx(const M4& m)
{
    //Push();
    glLoadMatrixf((REAL *)&m);
}

//--------------------------------------------------------------------------------------------
void OpenglRender::RenderFace(const Htex*   dwctx,
                              DWORD         combine,
                              const         Vtx*	pvx,
                              int		    count,
                              DWORD		    howWhat)
{
    DWORD texL=0;
    dwctx ? texL=_PushTextures((Htex*)dwctx, combine): 0; //use prev texture
    Render(pvx, 0, count, howWhat);
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Render(const Pos* pOp, DWORD trCtx,  
                          DWORD dCtx, const Htex* txCtx, DWORD combine, 
                          const Vtx* pVtx, int vxCnt, DWORD howWhat)
{
    DWORD texL=0;
    SetTransform(pOp, trCtx);
    dCtx ? Blend(dCtx):0;
    txCtx ? texL=_PushTextures((Htex*)txCtx, combine) : 0;
    Render(pVtx, 0, vxCnt, howWhat);
    dCtx ? UnBlend(dCtx): 0;
    ResetTransform();
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Render(const Pos* pOp, DWORD trCtx,
                              const Htex* txCtx,  DWORD combine,
                              const Vtx* pVtx, int vxCnt, DWORD howWhat)
{
    DWORD texL=0;
    SetTransform(pOp, trCtx);
    txCtx ? texL=_PushTextures((Htex*)txCtx, combine): 0;
    Render(pVtx, 0, vxCnt, howWhat);
    ResetTransform();
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Render(const Pos* pOp, DWORD trCtx,
                          const Vtx* pVtx, int vxCnt, DWORD howWhat)
{
    SetTransform(pOp, trCtx);
    Render(pVtx, 0, vxCnt, howWhat);
    ResetTransform();
}

//--------------------------------------------------------------------------------------------
void OpenglRender::Render(DWORD dCtx, const Htex* txCtx, DWORD comb, const Vtx* pVtx, int vxCnt , DWORD howWhat)
{
    DWORD texL=0;
    dCtx ? Blend(dCtx):0;
    txCtx ? texL=_PushTextures((Htex*)txCtx, comb) : 0;
    Render(pVtx, 0, vxCnt, howWhat);
    dCtx ? UnBlend(dCtx):0;
}

//--------------------------------------------------------------------------------------------
//    draws the vertexes pVx[vxCnt], int* htex, CLR& color, DWORD ctx
void OpenglRender::Render(const Vtx* pVx, int nStart, int  vxCnt, DWORD howWhat)
{
    if(vxCnt == 0)      return;
    
    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // nuyll out the unsup bits TC

    if(!_rnds->retainedDraw)
    {
        const REG Vtx* pwlk = pVx+nStart;
        glBegin( _glConsts[REND_PRIM(howWhat)] );
        while(--vxCnt >= 0)
        {
            if(howWhat & VX_COLOR)	
                glColor4ubv((BYTE*)(CLR&)pwlk->_rgb);
            if(howWhat & VX_NORMAL) 
                glNormal3fv((REAL*)(V3&)pwlk->_nrm);

            if(vxTc & VX_TX1)   
                Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)(UV&)pwlk->_uv[0]); else 
                goto RV;
            if(vxTc & VX_TX2)   
                Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(REAL*)(UV&)pwlk->_uv[1]); else 
                goto RV;
            if(vxTc & VX_TX3)   
                Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(REAL*)(UV&)pwlk->_uv[2]); else 
                goto RV;
            if(vxTc & VX_TX4)   
                Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(REAL*)(UV&)pwlk->_uv[3]);
            RV:
            glVertex3fv((REAL*)(V3&)pwlk->_xyz);
            ++pwlk;
        }
        glEnd();
    }
    else // retained
    {
        if(!_bArraysSet)
        {
            SetArrayPtrs(pVx, howWhat);
            glDrawArrays(_glConsts[REND_PRIM(howWhat)], nStart, vxCnt);
            ResetArrayPtrs(howWhat);
        }
        else
        {
            glDrawArrays((_glConsts[REND_PRIM(howWhat)]), nStart, vxCnt);
        }
    }
}

void OpenglRender::DrawArrays(int nStart, int vxCnt, DWORD howWhat)
{

    glDrawArrays(_glConsts[REND_PRIM(howWhat)], nStart, vxCnt);

}

//--------------------------------------------------------------------------------------------
void OpenglRender::Render(const BYTE* pV,int start, int count, DWORD what)
{
    DWORD  how         = _glConsts[REND_PRIM(what)];
    DWORD  vxTc        = (what & 0xF00000) & Ext::p_txCordMask; // nuyll out the unsup bits TC
    ////....if(!_rnds->retainedDraw)
    {
        glBegin(how);
        if(count < 0)
        {
            count = -count;
            int      block = sizeof(V3);
            if(what & VX_COLOR)block+=sizeof(CLR);
            if(what & VX_NORMAL)block+=sizeof(V3);
            if(vxTc & VX_TX1)block+=sizeof(UV);
            if(vxTc & VX_TX2)block+=sizeof(UV);
            if(vxTc & VX_TX3)block+=sizeof(UV);
            if(vxTc & VX_TX4)block+=sizeof(UV);

            const BYTE*  pWalk = pV + start + ((count-1) * block);
            for(int i=0; i < count; i++)
            {
                if(what & VX_COLOR){glColor3ubv(pWalk); pWalk-=sizeof(CLR);}
                if(what & VX_NORMAL){glNormal3fv((REAL*)pWalk); pWalk-=sizeof(V3);}
                if(vxTc & VX_TX1){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)pWalk);pWalk-=sizeof(UV);}else {
                        goto VR1;}
                if(vxTc & VX_TX2){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(REAL*)pWalk);pWalk-=sizeof(UV);}else {
                        goto VR1;}
                if(vxTc & VX_TX3){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(REAL*)pWalk);pWalk-=sizeof(UV);}else {
                        goto VR1;}
                if(vxTc & VX_TX4){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(REAL*)pWalk);pWalk-=sizeof(UV);};
    VR1:
                glVertex3fv((REAL*)pWalk);
                pWalk-=sizeof(V3);
            }
        }
        else
        {
            const BYTE*  pWalk = pV + start;
            for(int i=0; i < count; i++)
            {
                if(what & VX_COLOR){glColor3ubv(pWalk); pWalk+=sizeof(CLR);}
                if(what & VX_NORMAL){glNormal3fv((REAL*)pWalk); pWalk+=sizeof(V3);}
                if(vxTc & VX_TX1){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)pWalk);pWalk+=sizeof(UV);}else {
                        goto VR;}
                if(vxTc & VX_TX2){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(REAL*)pWalk);pWalk+=sizeof(UV);}else {
                        goto VR;}
                if(vxTc & VX_TX3){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(REAL*)pWalk);pWalk+=sizeof(UV);}else {
                        goto VR;}
                if(vxTc & VX_TX4){
                    Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(REAL*)pWalk);pWalk+=sizeof(UV);};
    VR:
                glVertex3fv((REAL*)pWalk);
                pWalk+=sizeof(V3);
            }
        }
        glEnd();
    }
}

void OpenglRender::Render(int strps, 
                          int grps, 
                          int* prims, 
                          V3* vxes, 
                          V3* norms, 
                          UV* uvs1, 
                          UV* uvs2,
                          int* idxes)
{
    if(0)//_rnds->retainedDraw)
    {

        glEnableClientState(GL_VERTEX_ARRAY);
	    glVertexPointer(3, GL_FLOAT, sizeof(V3), vxes);
        if(norms)
        {
		    glEnableClientState(GL_NORMAL_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(V3), norms);
        }
        if(uvs1)
        {
            Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		    glTexCoordPointer(2, GL_FLOAT, sizeof(UV), uvs1);
        }
        if(uvs2)
        {
            Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		    glTexCoordPointer(2, GL_FLOAT, sizeof(UV), uvs1);
        }

        //Ext::p_glLockArraysExtProc(0,strps * (grps-1) );   

        for(int g=0;g<grps;g++)
        {
            DWORD how   = _glConsts[REND_PRIM(prims[g])];

            glDrawElements( how, 
                            strps * 2,     
                            GL_UNSIGNED_INT, 
                            &idxes[ g * strps * 2]);
        }

        //Ext::p_glUnlockArraysExtProc(); 

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    }
    else
    {
        
        int iidx = 0;
        for(int y=0;y<grps;y++)
        {
            DWORD  how   = _glConsts[REND_PRIM(prims[y])];
            glBegin(how);
                for(int x=0;x<=strps;x++)
                {
                    norms ? glNormal3fv(norms[idxes[iidx]]) : 0;
                    uvs1 ?  Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)uvs1[idxes[iidx]]) : 0;
                    uvs2 ?  Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)uvs2[idxes[iidx]]) : 0;
                    glVertex3fv((REAL*)vxes[idxes[iidx++]]);

                    norms ? glNormal3fv(norms[idxes[iidx]]) : 0;
                    uvs1 ?  Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)uvs1[idxes[iidx]]) : 0;
                    uvs2 ?  Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)uvs2[idxes[iidx]]) : 0;
                    glVertex3fv((REAL*)vxes[idxes[iidx++]]);
                }
            glEnd();
        }
    }
}

//--------------------------------------------------------------------------------------------
// sets the pointers for the array vertexes
void OpenglRender::SetArray(const RenderVx* pxyz)
{
    if(!_rnds->retainedDraw)        return;
    DWORD  howWhat = pxyz->_what;
    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // null out the unsup bits TC
    
    _bArraysSet=1;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(V3), &pxyz->_v[0]);

	if(howWhat & VX_COLOR)
    {
		glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CLR), &pxyz->_c[0]);
	}

    if(howWhat & VX_NORMAL)
    {
		glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3), &pxyz->_n[0]);
    }

    if(vxTc & VX_TX1)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(UV), &pxyz->_u[0][0]);
    }else return;

    if(vxTc & VX_TX2)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(UV), &pxyz->_u[1][0]);
    }else return;

    if(vxTc & VX_TX3)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE2_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(UV), &pxyz->_u[2][0]);
    }else return;

    if(vxTc & VX_TX4)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE3_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(UV), &pxyz->_u[3][0]);
    }
}

//--------------------------------------------------------------------------------------------
// sets the pointers for the array vertexes
void OpenglRender::ResetArray(const RenderVx* pxyz)
{
    if(!_rnds->retainedDraw)
        return;

    DWORD  howWhat = pxyz->_what;
    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // null out the unsup bits TC
    _bArraysSet=0;
	glDisableClientState(GL_VERTEX_ARRAY);
	if(howWhat & VX_COLOR)
    {
		glDisableClientState(GL_COLOR_ARRAY);
	}

    if(howWhat & VX_NORMAL)
    {
		glDisableClientState(GL_NORMAL_ARRAY);
    }

    if(vxTc & VX_TX1)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;
    if(vxTc & VX_TX2)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;

    if(vxTc & VX_TX3)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE2_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;

    if(vxTc & VX_TX4)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE3_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

//--------------------------------------------------------------------------------------------
// sets the pointers for the array vertexes
void OpenglRender::SetArrayPtrs(const Vtx*  xyz, int howWhat)
{
    if(!_rnds->retainedDraw)        return;
    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // null out the unsup bits TC

    _bArraysSet=1;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vtx), &xyz->_xyz);

	if(howWhat & VX_COLOR)
    {
		glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vtx), &xyz->_rgb);
	}

    if(howWhat & VX_NORMAL)
    {
		glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vtx), &xyz->_nrm);
    }

    if(vxTc & VX_TX1)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vtx), &xyz->_uv[0]);
    }else return;

    if(vxTc & VX_TX2)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vtx), &xyz->_uv[1]);
    }else return;

    if(vxTc & VX_TX3)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE2_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vtx), &xyz->_uv[2]);
    }else return;

    if(vxTc & VX_TX4)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE3_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vtx), &xyz->_uv[3]);
    }
}

//--------------------------------------------------------------------------------------------
// resets the array poiners
void OpenglRender::ResetArrayPtrs(int howWhat)
{
    if(!_rnds->retainedDraw)
        return;

    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // null out the unsup bits TC
    _bArraysSet=0;
	glDisableClientState(GL_VERTEX_ARRAY);
	if(howWhat & VX_COLOR)
    {
		glDisableClientState(GL_COLOR_ARRAY);
	}

    if(howWhat & VX_NORMAL)
    {
		glDisableClientState(GL_NORMAL_ARRAY);
    }

    if(vxTc & VX_TX1)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;
    if(vxTc & VX_TX2)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;

    if(vxTc & VX_TX3)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE2_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }else return;

    if(vxTc & VX_TX4)
    {
        Ext::p_glClientActiveTextureARB(GL_TEXTURE3_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

//--------------------------------------------------------------------------------------------
void OpenglRender::RenderVertex(const Vtx* pVx, DWORD howWhat)
{
    
    DWORD  vxTc = (howWhat & 0xF00000) & Ext::p_txCordMask; // null out the unsup bits TC	

    if(howWhat & VX_COLOR)   glColor4ubv((BYTE*)(CLR&)pVx->_rgb);
    if(vxTc & VX_NORMAL)     glNormal3fv((REAL*)(V3&)pVx->_nrm);
    if(vxTc & VX_TX1)   Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,(REAL*)(UV&)pVx->_uv[0]); else goto RV;
    if(vxTc & VX_TX2)   Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,(REAL*)(UV&)pVx->_uv[1]); else goto RV;
    if(vxTc & VX_TX3)   Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,(REAL*)(UV&)pVx->_uv[2]); else goto RV;
    if(vxTc & VX_TX4)   Ext::p_glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,(REAL*)(UV&)pVx->_uv[3]);
RV:    
    glVertex3fv((REAL*)(V3&)pVx->_xyz);
}


//--------------------------------------------------------------------------------------------
UINT OpenglRender::Compile(const Vtx* pm, int vxCnt){
    assert(0);
    return 0;
}


//--------------------------------------------------------------------------------------------
// obsolete function
void OpenglRender::Render(int font, REAL x, REAL y, REAL z, const char* text, const CLR& clr)
{
	glColor4ubv((BYTE*)(CLR&)clr);
	glPushMatrix();
		glListBase(font - 32);
		glTranslated(x,y,z);
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopMatrix();
}

//---------------------------------------------------------------------------------------
// dummy test method
void    OpenglRender::EnableRenderMaterialLighting(DWORD amb, BOOL b)
{
    if(b)
    {
        CLR c(amb);
        REAL r[4] = {c.r/255.0, c.g/255.0, c.b/255.0, 1};
        REAL p[4] = {0,0,0,0};

        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL) ;
        glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, r);

        REAL a[4]={0.2, 0.2, 0.2, 1.0};
        REAL d[4]={0.8, 0.8, 0.8, 1.0};
        REAL s[4]={0.0, 0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
        glMaterialfv(GL_FRONT, GL_SPECULAR, s);

	    glEnable(GL_LIGHTING);
	    glEnable(GL_LIGHT0);
	    glLightfv(GL_LIGHT0, GL_AMBIENT,r);
	    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,0.001);

    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL) ;
    }
}

//---------------------------------------------------------------------------------------

void    OpenglRender::SetLights(const RenderLight** ppl, int ncount)
{
	    //glPushMatrix();
    //glLoadIdentity();
    int lNum = GL_LIGHT0;

    for(int i=0; i<ncount; ++i, ++ppl, ++lNum)
    {
        const RenderLight* pl = *ppl;
        switch(pl->_flags)
        {
            case LIGHT_POINT:
                {
                    float position[4] = {pl->_pos.x,pl->_pos.y,pl->_pos.z,1/*non-directional*/};
                    float cut_off = 360.0;

	                glLightfv(lNum, GL_AMBIENT,  FloatColor(pl->_colorAmbient));
	                glLightfv(lNum, GL_DIFFUSE,  FloatColor(pl->_colorDiffuse));
	                glLightfv(lNum, GL_SPECULAR, FloatColor(pl->_colorSpecular));
	                glLightfv(lNum, GL_POSITION, (float *) position);
	                glLightfv(lNum, GL_CONSTANT_ATTENUATION, &(pl->_constAttenuation));
	                glLightfv(lNum, GL_LINEAR_ATTENUATION, &(pl->_linearAttenuation));
	                glLightfv(lNum, GL_QUADRATIC_ATTENUATION, &(pl->_quadratAttenuation));
	                glLightfv(lNum, GL_SPOT_CUTOFF, &cut_off);
                }
                break;

            case LIGHT_DIRECT:
                {
                    float direction[4] = {pl->_direction.x,pl->_direction.y,pl->_direction.z,0/*directional*/};

	                glLightfv(lNum, GL_AMBIENT,  FloatColor(pl->_colorAmbient));
	                glLightfv(lNum, GL_DIFFUSE,  FloatColor(pl->_colorDiffuse));
	                glLightfv(lNum, GL_SPECULAR, FloatColor(pl->_colorSpecular));
	                glLightfv(lNum, GL_SPOT_CUTOFF, &pl->_cutOff);
	                glLightfv(lNum, GL_POSITION, (float *) direction);
                }
                break;

            case LIGHT_SPOT:
                {
                    float direction[4] = {pl->_direction.x,pl->_direction.y,pl->_direction.z,0/*directional*/};
                    float position[4] = {pl->_pos.x,pl->_pos.y,pl->_pos.z,1/*directional*/};

	                glLightfv(lNum, GL_AMBIENT,  FloatColor(pl->_colorAmbient));
	                glLightfv(lNum, GL_DIFFUSE,  FloatColor(pl->_colorDiffuse));
	                glLightfv(lNum, GL_SPECULAR, FloatColor(pl->_colorSpecular));
            	    glLightfv(lNum, GL_SPOT_DIRECTION, (float *) direction);
            	    glLightfv(lNum, GL_POSITION, (float *) position);
	                //glLightfv(lNum, GL_CONSTANT_ATTENUATION, &(pl->dvAttenuation0));
	                //glLightfv(lNum, GL_LINEAR_ATTENUATION, &(pl->dvAttenuation1));
	                //glLightfv(lNum, GL_QUADRATIC_ATTENUATION, &(pl->dvAttenuation2));
	                glLightfv(lNum, GL_SPOT_CUTOFF, &pl->_cutOff);
	                //glLightfv(lNum, GL_SPOT_EXPONENT, &(pl->_fallOff));

                }
                break;
        }
        glEnable(lNum);
    }
    //glPopMatrix();
}


//---------------------------------------------------------------------------------------
void    OpenglRender::SetMaterial(const RenderMaterial& mat)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT  , FloatColor(mat._colorAmbient));
	glMaterialfv(GL_FRONT, GL_DIFFUSE  , FloatColor(mat._colorDiffuse));
	glMaterialfv(GL_FRONT, GL_EMISSION , FloatColor(mat._colorEmmisive));
	glMaterialfv(GL_FRONT, GL_SPECULAR , FloatColor(mat._colorSpecular));
	glMaterialf (GL_FRONT, GL_SHININESS, mat._shiness);
}

//---------------------------------------------------------------------------------------
void    OpenglRender::Swap()
{
	SwapBuffers(_hDC);

};


//--------------------------------------------------------------------------------------------
//    MEMBER:	 OpenglRender::~OpenglRender
OpenglRender::~OpenglRender()
{
//    delete   _pShader;
    delete[] _extensions;
	_pThis=0;
    RemoveAllTextures();

    if(_enabledTextures!=0)
    {
        Beep(1200,50);
        TRACEX("Not all the textures were deleted. \r\n");
    }
    ClipCursor(0);
}


//--------------------------------------------------------------------------------------------
void OpenglRender::DbgLine(const V3& a, const V3& b)
{
    glColor4ub(255,255,255,255);
    glBegin(GL_LINES);
    glVertex3fv((REAL*)(V3&)a);
    glVertex3fv((REAL*)(V3&)b);
    glEnd();
}

//--------------------------------------------------------------------------------------------
void OpenglRender::ResetTransform()
{
	DWORD dCtx = _ttxStack.Pop();
	if(dCtx & OBJ_TRANSFORM)
		glPopMatrix();
		--_matrixCount;
}

//--------------------------------------------------------------------------------------------
// build the tranformation matrix right into the render(this should grt a matrix ready to mul)
void OpenglRender::SetTransform(const Pos* pos, DWORD dCtx )
{

    if(dCtx & OBJ_USE_SOFT_TR)
    {
        if(dCtx & OBJ_TRANSFORM)
        {
            /*
            _matrixCount++;
            glPushMatrix();
            M4& m  = pos->BuildTrMatrix();
            glMultMatrixf((REAL*)m);
            */
            if(dCtx & OBJ_BILLBOARD)
            {
			    BillBoardStart(dCtx & OBJ_BILLBOARD , pos->_pos, pos->_scale );
		    }
        }
    }
    else
    {
	    if(dCtx & OBJ_TRANSFORM)
        {
		    glPushMatrix();
		    _matrixCount++;
		    if(dCtx & OBJ_LOADIDENTITY)
			    glLoadIdentity();
		    if(dCtx & OBJ_TRANSLATE)
			    glTranslatef(pos->_pos.x, pos->_pos.y, pos->_pos.z);
		    if((dCtx & OBJ_SCALE) && (dCtx & ~OBJ_BILLBOARD)){
                if(pos->_scale.x!=0)
                {
    	            glScalef(pos->_scale.x,pos->_scale.y,pos->_scale.z);
                }
		    }
		    if(dCtx & OBJ_BILLBOARD)
            {            // bilboard the thingd
			    BillBoardStart(dCtx & OBJ_BILLBOARD , pos->_pos, pos->_scale );
		    }
            else
            {
                if(dCtx & OBJ_ROTATE)
                {               // rotate around x,y,z by angle
			        const V3& rv = pos->_pos;
			        glRotatef(pos->_rot, rv.x, rv.y, rv.z);
		        }
		        if(dCtx & OBJ_ROTATE_EX)
                {            // rotate around rv by x,y,z angles
			        const V3& rv = pos->_euler;
			        rv.y ? glRotatef(R2G(rv.y), 0.0,1.0,0.0):(void)0;
			        rv.x ? glRotatef(R2G(rv.x), 1.0,0.0,0.0):(void)0;
			        rv.z ? glRotatef(R2G(rv.z), 0.0,0.0,1.0):(void)0;
		        }
            }
	    }
    }
    _ttxStack.Push(dCtx);
}


//---------------------------------------------------------------------------------------
void OpenglRender::SetCanonicalState()
{
    static UINT Envs[] = {GL_REPLACE, GL_MODULATE};//GL_ENV

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
    glEnable(GL_CULL_FACE);
    int texstages = Ext::p_texCount;
    while(--texstages>=0)
    {
        Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+texstages);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, Envs[texstages & 0x1]);
        glDisable(GL_TEXTURE_2D);
    }

}

void OpenglRender::EnableStencil(long howWhat)
{
    switch(howWhat)
    {
        case -1: //disable
            glClear(GL_DEPTH_BUFFER_BIT/*|GL_STENCIL_BUFFER_BIT*/);
            glDisable(GL_STENCIL_TEST);
            break;
        case 0:
            glClear (/*GL_COLOR_BUFFER_BIT | */GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
            glEnable(GL_STENCIL_TEST);
            glDisable(GL_DEPTH_TEST);
            glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); 
            glStencilFunc(GL_ALWAYS, 0x1, 0x1); 
            glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
            break;
        case 1:
            glEnable(GL_DEPTH_TEST);
            glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
            glDepthMask(GL_TRUE); 
            glStencilFunc(GL_EQUAL,  0x1,       0x1); 
            glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
            break;
    }
}

void OpenglRender::EnableFog(REAL fnear, REAL ffar, REAL dens, const CLR& frgb)
{
    if(dens)
    {
        glEnable(GL_FOG);
        glFogf(GL_FOG_START,   fnear);
        glFogf(GL_FOG_END,    ffar);
        REAL fgc[4]={frgb.r/255.0, frgb.g/255.0, frgb.b/255.0, frgb.a/255.0};
        glFogfv(GL_FOG_COLOR,  fgc);
        glFogf(GL_FOG_DENSITY, dens);
    }
    else
        glDisable(GL_FOG);
}

int OpenglRender::UseFontBitmaps(HFONT hf, int a, int b, int c)
{
	int i =0;
	if(hf)
	{
        wglMakeCurrent(_hDC, _hRC); 
		HFONT hFontOld = (HFONT)::SelectObject(_hDC, hf);
	    i = glGenLists(b);
		assert(i);
		BOOL bb = wglUseFontBitmaps(_hDC, a, b, i);
		assert(bb);
        SelectObject(_hDC, hFontOld);
	}
	else
	{
		DeleteList(c, 255);
	}
	return i;
};

void OpenglRender::TextOut(int fontlist, 
                           REAL xpos, 
                           REAL ypos, const char* text, const CLR& clr)
{
    glPushMatrix();
        glTranslatef(0, 0, -(_rnds->fNear+.1));
        glColor3ub(clr.r,clr.g,clr.b);
        glRasterPos2f(xpos, ypos);
        glPushAttrib(GL_LIST_BIT);
            glListBase(fontlist);
            glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	
        glPopAttrib();
    glPopMatrix();
    
}


void OpenglRender::RenderFontList(REAL x, REAL y, const char* text, int fontlist, const CLR& clr)
{
   	glColor3ub(clr.r,clr.g,clr.b);
    glRasterPos2f( x, y );
    glPushAttrib(GL_LIST_BIT); 
    glListBase( fontlist-32 );
    glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    glPopAttrib();
}

void OpenglRender::RenderFontList3(const V3& v, const char* text, int fontlist, const CLR& clr)
{
    glColor3ub(clr.r,clr.g,clr.b);
    glRasterPos3f(v.x,v.y,v.z);
    glPushAttrib(GL_LIST_BIT); 
        glListBase( fontlist-32 );
        glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    glPopAttrib();

}


//---------------------------------------------------------------------------------------
/*
    if(lmap)   base + lmap   = rez1;
    if(detail) rez1 + detail = rez2;
    if(aux)    rez2 + aux    = rez3'
*/

DWORD    OpenglRender::_PushTextures(Htex* pT, DWORD combine)
{
    if(pT==0 || combine==0) return 0;
    static  DWORD       envModes[]  =   {GL_MODULATE/*GL_REPLACE*/, GL_MODULATE};
    int                 i           =   Ext::p_texCount;
    DWORD   texbits                 =   combine & 0xF;
    OpenglRender::_texareOff        =   0;
    Htex*   pTex                    =   pT;

    for(i=0; i< texbits; i++,pTex++)
    {
        if(texbits & (1<<i) && pTex->hTex)
        {
            Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+i);
            glEnable(GL_TEXTURE_2D/*pTex->glTarget*/);
            glBindTexture( GL_TEXTURE_2D, pTex->hTex); 
            GenST(TGET_GENST(pTex->genST));
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envModes[i & 0x1]);
            _cachedTex[i] = ((Htex&)(*pTex));
        }
        else //if(_cachedTex[i])
        {
            Ext::p_glActiveTextureARB(GL_TEXTURE0_ARB+i);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            _cachedTex[i].Clear();
        }
        
    }
    return (texbits << 20); //pos in VX position;
}

void OpenglRender::Rotatef(REAL a, REAL x, REAL y, REAL z)
{
    glRotatef(a,x,y,z);
}

void OpenglRender::Scalef(REAL x, REAL y, REAL z)
{
    glScalef(x,y,z);
}

void OpenglRender::Translatef(REAL x, REAL y, REAL z)
{
    glTranslatef(x,y,z);
}

BOOL OpenglRender::InitShader()
{
//if(_pShader)
  //      return 1;
   // _pShader = new glShaderManager();
 //   if(_pShader)
  //      return 1;
    return 0;
}


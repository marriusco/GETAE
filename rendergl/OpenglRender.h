//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
//#define __glext_h_
#ifndef __OPENGLRENDER_H__
#define __OPENGLRENDER_H__

#include "_irender.h"
#include "_isystem.h"
#include <stdio.h>
/*
#include "../_include/gl/gl.h"
#include "../_include/gl/glu.h"
#include "../_include/gl/glaux.h"
#include "../_include/gl/glext.h"
*/

#include "glew/include/GL/glew.h"
#include "../_include/extern/glext.h"
//class glShaderManager;

//--------------------------------------------------------------------------------------------
struct TexLparam                                            // caries .5 rgb array
{                                                           // to build .5 texture out of it
    HTEX    ht;
    int     x;
    int     y;
    int     bpp;
    DWORD   creaFlag;
    DWORD   userFlag;
    BYTE*   parr;
};

//---------------------------------------------------------------------------------------
class OpenglRender  : public Irender
{
public:
	OpenglRender();
	~OpenglRender();
    BOOL CreateRender(const ISystem* pe, RndStruct* prndStates, UINT* gRconsts,HWND useThis=0);
    BOOL SetSystem(const ISystem* pe){_pengine=(ISystem*)pe; return 0;};
    void Destroy();
	HDC     Hdc(){return _hDC;};
	HWND    Hwnd(){return _hWnd;};
	void    Swap();
	void    SwitchMode(){SwMode();}
	void    ClearColor(REAL r,REAL g,REAL b,REAL a){glClearColor(r,g,b,a);};

	DWORD Blend(DWORD rCtx);
    void UnBlend(DWORD context);
    void UnBlendv();

	void    SelectTrMattrix(const Pos* pos, DWORD what){SetTransform(pos, what);}
	void    ReleaseTrMattrix(){ResetTransform();}
    void DrawArrays(int nStart, int vxCnt, DWORD howWhat);
    void SetArray(const RenderVx* pxyz);
	void ResetArray(const RenderVx* pxyz);

    void SetArrayPtrs(const Vtx* pxyz, int what);
	void ResetArrayPtrs(int what);
	PR_C Gpa(const char* fn);
    void SetCurrent(){wglMakeCurrent(_hDC, _hRC);}
    void MatrixMode(int m){glMatrixMode(m);};
    void RenderVertex(const Vtx* pvtci, DWORD dwwxF);
	void    Vertex2(REAL x, REAL y){glVertex2f(x,y);};
	void    Vertex3(REAL x, REAL y, REAL z){
        glVertex3f(x,y,z);	
    };
	void    Texture2(REAL u, REAL v){glTexCoord2f(u,v);};
    void DbgLine(const V3& a, const V3& b);
	void    DeepTest(BOOL enable){glDepthMask(enable ? GL_TRUE : GL_FALSE);}
	void EnableBlend(BOOL enable){enable ? glEnable(GL_BLEND) : glDisable(GL_BLEND);};
	void    BlendFunc(int a, int b){glBlendFunc(a,b);};
    void    AlphaFunc(int a, int b){glAlphaFunc(a, b);}
    void    Color4(char r, char g, char b, char a){
        glColor4ub(r,g,b,a);
    }
    void    Color( CLR& r){glColor4ub(r.r,r.g,r.b,r.a);}
    void SetWndMinimizeFlag(BOOL bEnable);
    void SetCanonicalState();
    void RefreshModelView(REAL ffov);
    void Clear(int quakeTrick);
    void SetMaterial(UINT fb, UINT what, REAL r){
        glMaterialf(_glConsts[fb], _glConsts[what], r);
    }
    void SetMaterials(UINT fb, UINT what, REAL* r){
        glMaterialfv(_glConsts[fb], _glConsts[what], r);
    }

    void MulMatrix(const M4& m){glMultMatrixf((REAL *)&m);}


    void SetMatrix(const M4& m){glLoadMatrixf((REAL *)&m);}
    void SetViewMtx(const M4& m);
    void SetGamma(const REAL* gamma, const REAL* contrast, const int* ilum);
    UINT* GetPrimitives(){return _glConsts;};
    void EnableStencil(long how);
    void EnableFog(REAL near, REAL far, REAL dens, const CLR& color);
    void    Push(){glPushMatrix();_matrixCount++;}
    void    Pop(){glPopMatrix();_matrixCount--;}
    void GetViewMatrix(REAL* m){glGetFloatv(GL_MODELVIEW_MATRIX, m);}
	void LoadIdentity(){glLoadIdentity();};
    void PolygonMode(UINT what,UINT mode){glPolygonMode(what, mode);};
    void CullFace(UINT what){glCullFace(what);};
    void Enable(UINT what){glEnable(what);};
    void Disable(UINT what){glDisable(what);};
    void Rotate(const V3& xyz);
    void RotateTo(const V3& xyz, REAL angle);
    void Scale(const V3& xyz);
    void Translate(const V3& xyz);
    void Rotatef(REAL, REAL, REAL, REAL);
    void Scalef(REAL, REAL, REAL);
    void Translatef(REAL, REAL, REAL);

    void Begin(UINT what){glBegin(what);};
    void BeginPrim(UINT what){
        glBegin(_glConsts[what]);
    };
    void End(){glEnd();};
	UINT Compile(const Vtx* pm, int count);
    int  UseFontBitmaps(HFONT hf, int a, int b, int c);
    void NewList(int list);
	int  GenLists(int num);
    void EndNewList();
    void DeleteList(UINT id,UINT many=1);
    void CallList(UINT ID);
    void CallLists(UINT a, UINT b, const void* ptr);// states
    void EnableRenderMaterialLighting(DWORD amb, BOOL b);
    void SetLights(const RenderLight** pl, int ncount);
    void SetMaterial(const RenderMaterial& mat);
    const char*   GetExtensions(DWORD b){b = _caps; return _extensions;};
    BOOL GetExtensions();

    void TextOut(int fontlist, REAL xpos, REAL ypos, const char* text, const CLR& color);
    Htex GenTexture(const char* texFile, DWORD creaFlags);
    Htex ReGenTexture(const Htex& tex, int x, int y, int bpp, const BYTE* pBuff, DWORD creaFlags);
	Htex GenTexture(int x, int y, int bpp, const BYTE* pBuff, DWORD creaFlags);
    void AlterTexture(const Htex& itex, const BYTE* psub, int s, int t, int u, int v);

    DWORD BindAllTextures(const Htex* pTxStg, DWORD comb);
    DWORD BindTexture(const Htex& txStg,  int stage);
	void UnBindTexture(const Htex& txStg, int stage);
    void DisableTextures(BOOL force=FALSE);

    void RemoveAllTextures();
	void RemoveTexture(UINT index,BOOL=TRUE);
    void RemoveTextures(const UINT* ptex, int count);
    void CleanTexCache(int i=-1);

    DWORD BindTex1(const Htex* txStg){return BindTexture(*txStg, 0);}
    DWORD BindTex2(const Htex* t1, const Htex* t2){
        Htex ht[]={*t1, *t2};
        return BindAllTextures(ht,2);
    }
    DWORD BindTex3(const Htex* t1, const Htex* t2, const Htex* t3){
        Htex ht[]={*t1, *t2, *t3};
        return BindAllTextures(ht,3);
    }
    void Render(int font, REAL x, REAL y, REAL z, const char* text, const CLR& clr);
    void Render(const Pos* pOp, DWORD trCtx, DWORD dCtx, const Htex* txCtx, DWORD comb, const Vtx* pVtx, int vxCnt, DWORD how);
    void Render(const Pos* pOp, DWORD trCtx, const Htex* txCtx, DWORD comb, const Vtx* pVtx, int vxCnt , DWORD how);
    void Render(const Pos* pOp, DWORD trCtx, const Vtx* pVtx, int vxCnt , DWORD how);
    void Render(DWORD dCtx, const Htex* txCtx, DWORD comb, const Vtx* pVtx, int vxCnt , DWORD how);
	void Render(DWORD * indexes, int ncount,DWORD how){};
    void Render(const Vtx * pVtx, int nStart, int nCount, DWORD how);//<<<
    void Render(const BYTE* pV, int start, int count, DWORD what);
    void Render(int strps, int grps, int* prims, V3* vxes, V3* norms, UV* uvs1, UV* uvs2, int* idesx);
    void RenderFace(const Htex* dwctx, DWORD comb, const Vtx* pvx, int count, DWORD how);
    void RenderFontList(REAL x, REAL y, const char* text, int fontlist, const CLR& color);
    void RenderFontList3(const V3& pos, const char* text, int fontlist, const CLR& color);
    void SetNormal(const V3& v){glNormal3fv((REAL*)((V3&)v));};
    void EnableLighting(BOOL enable,DWORD amb){}
    int  GetMaxHwLights(void){return 8;}
    RndStruct* GetStruct(){return _rnds;}
    BOOL InitShader();
	void SetTransform(const Pos* pos, DWORD dCtx );
	void ResetTransform();

private:
    DWORD	_PushTextures(Htex* dwctx, DWORD combine);

    Htex   Local_SendMessageStore_Tex(TexLparam* pl, DWORD texFlags);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT	uMsg, WPARAM wP, LPARAM	lP);
    static BOOL WndCreate();
    static long GoWindowedMode();
	static void GoInitialDevMode();
    static long DevModeFullScreen(BOOL bFull=TRUE);
    static long GoFullScreen();
    static void DestroyWnd();
    static HTEX GenTextureMsg(LPARAM lp);
    static void DeleteTextureMsg(LPARAM lp);
    static HWND SwMode();
    static long IsSwitchMode();
    static long Attach(HDC hDC, int bPP = 24, int backbPP = 16);
    static void Detach();
    static void SetupPalette();
    static void PaletteChanged(WPARAM wP);
    static long QueryNewPalette();
    static void Perspective(REAL w, REAL h);
    static void BillBoardStart(DWORD mode, const V3& objectCenter,const  V3& scale);
    static void BillBoardEnd(int mode);
    static void InitialSettMode();
    static void TrySettMode();
    
    BOOL Has(const char* s);
private:
	static  RndStruct*                      _rnds;
	static	OpenglRender*                   _pThis;

    static  BOOL                            _bArraysSet;
    static  HWND		                    _hWnd;
    static  HDC			                    _hDC;
    static  HPALETTE	                    _hPall;
    static  BOOL		                    _bFullScr;
    static  HGLRC	                        _hRC;
    static  HINSTANCE                       _appHinst;
    static  RECT	                        _rectWnd;
    static	HICON	                        _hi;
    static  DEVMODE	                        _defDevMode;
    static  DWORD	                        _caps;
    static  UINT	                        _glConsts[256];
    
    static  RawArray<DWORD,128>             _ctxStack;
    static  RawArray<DWORD,128>             _ttxStack;
    static  int                             _matrixCount;
    static  int                             _curTexUnit;
    static  BYTE                            _ambLight[4];
    static  WORD                            _gamma[3][256];
    static  Htex                            _cachedTex[4];
    static  DWORD                           _cahchedTgts[4];
    static  ISystem*                        _pengine;
    static char                            *_extensions;
    static  int                             _enabledTextures;
    static BOOL                             _texareOff;
    static int                              _maxtexId;
    //static glShaderManager                   *_pShader;
    static BOOL                             _destroyed;
    static BOOL                             _externalWnd;    
};

//--------------------------------------------------------------------------------------------
struct Ext
{
    static int    p_texCount                                   ;
    static DWORD  p_caps1                                      ;
    static DWORD  p_txCordMask                                 ;

    static PFNGLCLIENTACTIVETEXTUREARBPROC  p_glClientActiveTextureARB ;  
    static PFNGLACTIVETEXTUREARBPROC        p_glActiveTextureARB    ;     
    static PFNGLMULTITEXCOORD2FARBPROC      p_glMultiTexCoord2fARB  ;     
    static PFNGLMULTITEXCOORD2FVARBPROC     p_glMultiTexCoord2fvARB ;     
    static PFNGLLOCKARRAYSEXTPROC           p_glLockArraysExtProc   ;  
    static PFNGLUNLOCKARRAYSEXTPROC         p_glUnlockArraysExtProc ;


    static void (DUMMY_PFNGLACTIVETEXTUREARBPROC) (GLenum texture){};
    static void (DUMMY_PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture){};
    static void (DUMMY_PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t){if(target==GL_TEXTURE0_ARB) glTexCoord2f(s,t);};
    static void (DUMMY_PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v){if(target==GL_TEXTURE0_ARB) glTexCoord2f(v[0], v[1]);};
    static void (DUMMY_PFNGLLOCKARRAYSEXTPROC)(GLint first, GLsizei count){};
    static void (DUMMY_PFNGLUNLOCKARRAYSEXTPROC)(){};

};

INLN void GenST(const DWORD genST)
{
    switch(genST)
    {
        case GEN_TEX_GEN_NA: // 0
	        glDisable(GL_TEXTURE_GEN_S);
	        glDisable(GL_TEXTURE_GEN_T);
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
}



#endif __OPENGLRENDER_H__


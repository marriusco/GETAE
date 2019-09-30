//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "OpenglRender.h"

BOOL                            OpenglRender::_externalWnd=0;    
BOOL                            OpenglRender::_destroyed=1;
//glShaderManager*                OpenglRender::_pShader=0;
int                             OpenglRender::_maxtexId=0;
int                             OpenglRender::_enabledTextures=0;
BOOL                            OpenglRender::_bArraysSet=0;
DWORD	                        OpenglRender::_caps=0;  
int                             OpenglRender::_curTexUnit=0;
HWND		                    OpenglRender::_hWnd=0;
HDC			                    OpenglRender::_hDC=0;
HPALETTE	                    OpenglRender::_hPall=0;
BOOL		                    OpenglRender::_bFullScr=FALSE;
HGLRC	                        OpenglRender::_hRC=0;
HINSTANCE                       OpenglRender::_appHinst=0;
RECT	                        OpenglRender::_rectWnd = {0};
RndStruct*                      OpenglRender::_rnds=0;
HICON	                        OpenglRender::_hi=0;
DEVMODE	                        OpenglRender::_defDevMode={0};	
OpenglRender*                   OpenglRender::_pThis=0;
RawArray<DWORD,128>			    OpenglRender::_ctxStack;
RawArray<DWORD,128>             OpenglRender::_ttxStack;
int                             OpenglRender::_matrixCount=0;
BYTE                            OpenglRender::_ambLight[4]={200,200,200,255};
Htex                            OpenglRender::_cachedTex[4];
DWORD                           OpenglRender::_cahchedTgts[4];
ISystem*                        OpenglRender::_pengine = 0;
char*                           OpenglRender::_extensions;
BOOL                            OpenglRender::_texareOff=1;
UINT                            OpenglRender::_glConsts[]={     0,
																GL_TEXTURE_2D, 
																GL_TEXTURE_3D, 
																GL_TEXTURE_CUBE_MAP,
                                                                GL_FRONT_AND_BACK,
	                                                            GL_LINE,
                                                                GL_FILL,
                                                                GL_CULL_FACE,
                                                                GL_FRONT,
                                                                GL_SHININESS,
                                                                GL_BACK,
	                                                            GL_BLEND,
	                                                            GL_POINTS,       //draw mode
	                                                            GL_LINES,
	                                                            GL_LINE_STRIP,
	                                                            GL_LINE_LOOP,
	                                                            GL_TRIANGLES,
	                                                            GL_TRIANGLE_STRIP,
	                                                            GL_TRIANGLE_FAN,
	                                                            GL_QUADS,
	                                                            GL_QUAD_STRIP,
	                                                            GL_POLYGON,
	                                                            GL_ZERO,         //alpha
	                                                            GL_ONE,
	                                                            GL_SRC_COLOR,
	                                                            GL_ONE_MINUS_SRC_COLOR,
	                                                            GL_SRC_ALPHA,
	                                                            GL_ONE_MINUS_SRC_ALPHA,
	                                                            GL_DST_ALPHA,
	                                                            GL_ONE_MINUS_DST_ALPHA,
	                                                            GL_DST_COLOR,
	                                                            GL_ONE_MINUS_DST_COLOR,
	                                                            GL_SRC_ALPHA_SATURATE,
                                                                GL_TEXTURE_GEN_S,
                                                                GL_TEXTURE_GEN_T,
                                                                GL_TEXTURE_GEN_R,
                                                                GL_TEXTURE_GEN_Q,
                                                                GL_TEXTURE_MAG_FILTER,
                                                                GL_TEXTURE_MIN_FILTER,
                                                                GL_TEXTURE_WRAP_S,
                                                                GL_TEXTURE_WRAP_T,
                                                                GL_TEXTURE_ENV_MODE,
                                                                GL_TEXTURE_ENV_COLOR,
                                                                GL_TEXTURE_ENV,
                                                                GL_CLAMP,
                                                                GL_REPEAT,
                												GL_AMBIENT,
		                                                        GL_DIFFUSE,
		                                                        GL_SPECULAR,
		                                                        GL_EMISSION,
                                                                -1
                                                            };




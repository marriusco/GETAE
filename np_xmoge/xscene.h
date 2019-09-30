#ifndef __XENGINE_H__
#define __XENGINE_H__


#include <string>
#include "../_include/_irender.h"
#include "../_include/_isound.h"
#include "../_include/_iinput.h"
#include "../gebsplib/beamtree.h"
#include "../system/system.h"
#include "../rendergl/OpenglRender.h"
#include "dnlthread.h"
#include "messages.h"

using namespace std;
#define     WM_LOADBSP      (WM_USER+4000)


class XScene;
class System;
class X_Gbt ;
//---------------------------------------------------------------------------------------
// the whole rendering and loading context.
class XScene
{
public:
    friend class DnlThread;
    static XScene*   PXEngine;
    XScene(){
        assert(0==PXEngine);
        p_tree   = 0; 
        p_render = 0; 
        p_input  = 0; 
        b_haslevel = 0;
        rgb_color= 0x919191;
        PXEngine = this;
        s_msg[0] = 0;
        n_bytes      = 0;
        n_exbytes    = 0;
        p_thread     = 0;
        s_proxy[0]   = 0; 
        n_port       = 0;

    }
    ~XScene(){
        PXEngine = 0;
        assert(p_tree==0);
        assert(p_render==0);
        assert(p_input==0);
        assert(p_thread==0);
        f_timer  = 0;
        p_system = 0;
        p_tree   = 0;           // bsp tree
        p_render = 0;       // embeded render (not the dll)
        p_input  = 0;        // embeded input
        b_abort  = 0;
    }

    BOOL    Create(HWND h, HINSTANCE hInstance, HWND parentHwnd);
    void    Destroy(BOOL bforce=0);
    static long   SystemProcedure(ISystem* ps, long m, long w, long l){ 
        return PXEngine->Proc(m,w,l); 
    }
    void    SetProxy(const TCHAR* ppx, ULONG port);
    BOOL    HasLevel(){return b_haslevel;}
    mutex&  Mutex(){return    _cs;    }
    int     Download_Proc(UINT msg, long wp, long lp);
    DnlThread*   ThreadActive(){return  p_thread;}
    long    Proc(long m, long w, long l);
    void    MoveWindow(int x, int y, int w, int h, int redraw);
    void    Spin();
    void    Escape();
    void    Focus();
    void    Move(int,int,int,int);
    BOOL    ThreadLoad(const TCHAR* level);
    BOOL    Connect(const TCHAR* level, UINT nPort);
    void    SetBgColor(long rgbcolor){
        rgb_color=rgbcolor;
        if(p_render)
            p_render->ClearColor((REAL)GetRValue(rgb_color)/255.0, (REAL) GetBValue(rgb_color)/255.0,  (REAL)GetGValue(rgb_color)/255.0, 1.0);
    };
    static XScene*   Exist(){return PXEngine;}
	System* Sys(){return     p_system;}
private:
    void    _Abort(BOOL final);
    void    _RunPhisics(Camera* pcam, SystemData* psd);
    void    _RenderScene(SystemData* psd, DWORD howwhat);
    void    _PumpSome(int x=16);
    int     _On_LoadBsp(long);
    void    _DeleteThread();
private:
    BOOL            b_haslevel;
    System*         p_system;
    BeamTree*       p_tree;         // bsp tree
    OpenglRender*   p_render;       // embeded render (not the dll)
    Iinput*         p_input;        // embeded input
    UINT            u_prims[2048];
    
    REAL            f_timer;
    BOOL            b_focus;
    long            rgb_color;
    BOOL            b_abort;
    mutex           _cs;
    DnlThread*       p_thread;
    TCHAR           s_msg[255];
    long            n_bytes;
    long            n_exbytes;
    BOOL            _loading;
    TCHAR           s_proxy[256];
    UINT            n_port;
};




#endif //

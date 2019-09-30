
#include "atlbase.h"
#include ".\gbtview.h"
#include "..\rendergl\OpenglRender.h"
#include ".\xscene.h"

static     RndStruct	__Disp;
XScene*    XScene::PXEngine;
static  mutex       __mutex;
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
long  XBspProc(BeamTree*, long, long, long);

//---------------------------------------------------------------------------------------
BOOL    XScene::Create(HWND h, HINSTANCE hInstance, HWND hwnd)
{
    RECT    rc; 
    ::GetWindowRect(hwnd, &rc);
	rc.right        -= rc.left;	
    rc.bottom       -= rc.top;
	rc.top          = rc.left     = 0;
	
    __Disp.xPolyMode                 = USE_CW|SHOW_BOTH;
	__Disp.dmPelsWidth                = rc.right;        // init dev mode  
	__Disp.dmPelsHeight               = rc.bottom;
	__Disp.dmBitsPerPel               = 16;
	__Disp.dmDisplayFrequency         = 60;
    __Disp.wndParent                  = hwnd;
    __Disp.bsetCapture                = FALSE;
    __Disp.hInstance                  = hInstance;  
    __Disp.wndStyle                   = WS_CHILD|WS_VISIBLE;
    __Disp.retainedDraw               = 1;
    __Disp.clipCursor                 = 0;
    __Disp.bsetCapture                = 0;
    __Disp.hideCurosr                 = 0;
    __Disp.dwVpStyle                  = 0; // no close button or maximize  

    b_focus  = 0;
    p_render = new OpenglRender();
    if(p_render->CreateRender(0, &__Disp, u_prims, h))
    {
        p_render->SetCurrent();
        p_system = System::Create(hInstance, p_render, 0, 0, hwnd);
        if(p_system)
        {
            PSystem = p_system;             // system library (one system)

			p_render->SetSystem(p_system);

            p_system->SetNf(SystemProcedure, 0);
            p_system->SetHudMode(HUD_TRACE);
            
            ::MoveWindow(p_render->Hwnd(), 0, 0, rc.right,  rc.bottom,1);
            ::ShowWindow(p_render->Hwnd(), SW_SHOW);        
			
            p_render->SetCurrent();
            p_render->ClearColor((REAL)GetRValue(rgb_color)/255.0, (REAL) GetBValue(rgb_color)/255.0,  (REAL)GetGValue(rgb_color)/255.0, 1.0);
            b_abort     = 0;
            p_system->SendMessage(SYS_START, (long)p_system, (long)p_system->GetSysData());
            b_haslevel  = 0;
            return 1;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------
void    XScene::Destroy(BOOL force)
{
    _Abort(0);

    AutoLock    al(&__mutex);
    System*   psystem = p_system;
    p_system = 0;
    while(PeekMessage(0,0,WM_LOADBSP,WM_LOADBSP,PM_REMOVE)){;};
    if(p_tree)
    {
        p_tree->Clear();
        delete p_tree;
        p_tree = 0;
    }
    if(psystem)
    {
        System::Destroy((System*)psystem);
        psystem = 0;
    }
    if(p_render)
    {
        delete p_render; 
        p_render=0;
    }
}

//---------------------------------------------------------------------------------------
void    XScene::_DeleteThread()
{
    AutoLock    al(&__mutex);
    if( 0 == p_thread)
    {
        return;
    }

    DWORD tc = GetTickCount();
    while(GetTickCount() - tc < 2000 && p_thread->StillActive())
    {
        strcpy(s_msg, "Overlapping commands... !");
        _PumpSome(2);
    }
    
    if(!p_thread->StillActive())
        goto DONE;

    p_thread->Join(2000);
    tc = GetTickCount();
    while(GetTickCount() - tc < 2000 && p_thread->StillActive())
    {
        strcpy(s_msg, "Overlapping commands... !");
        _PumpSome(2);
    }
    if(!p_thread->StillActive())
        goto DONE;

    assert(0);
    p_thread->Kill();
DONE:    

    delete p_thread;
    p_thread = 0;
}

//---------------------------------------------------------------------------------------
void    XScene::_Abort(BOOL final)
{
    while(PeekMessage(0,0,WM_OPENGLNOTY,WM_LOADBSP,PM_REMOVE)){;};
     b_abort    = 1;
    _DeleteThread();
}

//---------------------------------------------------------------------------------------
void    XScene::Spin()
{   
    p_system->Spin(f_timer);
}

//---------------------------------------------------------------------------------------
void XScene::Focus()
{
    if(p_render)
    {
        ::SetFocus(p_render->Hwnd());
        p_render->ClearColor((REAL)GetRValue(rgb_color)/255.0, 
							 (REAL) GetBValue(rgb_color)/255.0,  
							 (REAL)GetGValue(rgb_color)/255.0, 1.0);
        if(p_tree && b_haslevel)
        {
            
            Camera* pcam = this->p_system->GetCamera();
            int     leaf = p_tree->GetPovCamLeaf();
            if(leaf >= 0)
            {
                V3      vhit   = pcam->_pos+pcam->_fwd * 200.0;
                Impact  i;
                if(p_tree->SegmentIntersect(i, pcam->_pos, vhit) )
                {
                    if(i._model>0)
                    {
                        BspModel* pm  = p_tree->GetModel(i._model);
                        char* pml = strstr(pm->_name,".ml");
                        if(pml)
                        {
                            char level[256] = {0};
                            char fulllevel[256] = {0};
                            strncpy(level, pm->_name, pml - pm->_name + 3);
                            
                            sprintf(fulllevel,"http://linux/levels200/%s",level);
                            ThreadLoad(fulllevel);
                        }
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void XScene::Move(int x,int y,int w,int h)
{
    if(p_render)
        ::MoveWindow(p_render->Hwnd(),x,y,w,h,0);
}


//---------------------------------------------------------------------------------------
// conect to game server
BOOL    Connect(const TCHAR* level, UINT nPort)
{
    return 0;
}

//---------------------------------------------------------------------------------------
void    XScene::_RunPhisics(Camera* pcam, SystemData* psd)
{
    if(p_tree) 
    {
        p_tree->Animate(pcam, psd);

        int leaf = p_tree->GetPovCamLeaf();
        if(leaf >= 0)
        {
            Impact i;
            V3 fwd   = pcam->_pos+pcam->_fwd * 32.0;
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            fwd   = pcam->_pos+pcam->_fwd * (-32.0);
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            fwd    = pcam->_pos+VY * 32.0;
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            fwd    = pcam->_pos+VY * (-32.0);
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            fwd    = pcam->_pos+pcam->_right * (-32);
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            fwd    = pcam->_pos+pcam->_right * 32;
            if(p_tree->SegmentIntersect(i, pcam->_pos, fwd) )
                goto handle_coll;

            return;
handle_coll:
            
            V3 diptc = (pcam->_pos-i._ip);
            diptc.norm();
            if(diptc.len()>0.001)
            {
                while(vdist(pcam->_pos,i._ip) < 32.2)
                {
                    pcam->_pos += diptc;
                }
            }

        }
    }
}

//---------------------------------------------------------------------------------------
void    XScene::_RenderScene(SystemData* psd, DWORD howwhat)
{
    if(p_tree && p_system) 
    {
        p_render->SetCurrent();
        if(p_system->GetMode() == PLAY_MODE)
        {
            if(!this->b_focus){
                p_system->GetCamera()->Rotate(0.01,0,0);
            }
            else
            {
                Camera* pcam = this->p_system->GetCamera();
                int leaf = p_tree->GetCurrentLeaf(pcam->_pos);
                if(leaf >= 0)
                {
                    V3 vhit   = pcam->_pos+pcam->_fwd * 512.0;
                    Impact i;
                    if(p_tree->SegmentIntersect(i, pcam->_pos, vhit) )
                    {
                        if(i._model>0)
                        {
                            BspModel* pm  = p_tree->GetModel(i._model);
                            char* pml = strstr(pm->_name,".ml");
                            if(pml)
                            {
                                char level[256] = {0};
                                strncpy(level, pm->_name, pml - pm->_name + 3);

                                V3 ptext = i._ip + i._plane._n;
                                Pos p; p._pos = ptext;

                                p_render->Push();
                                p_render->RenderFontList3(ptext, level, 1, ZYELLOW);
                                p_render->Pop();
                            }
                        }
                    }
                }

            }

        }
        p_tree->Render(psd, Z_POLYGON);
    }
}

//---------------------------------------------------------------------------------------
void    XScene::_PumpSome(int x)
{
    MSG m;
    while(--x>0)
    {
        if(PeekMessage(&m,0,0,0,PM_REMOVE))
        {
            DispatchMessage(&m);
        }
    }
}


//---------------------------------------------------------------------------------------
BOOL    XScene::ThreadLoad(const TCHAR* level)
{
    AutoLock    al(&__mutex);

    if(p_thread)
    {
        p_system->TextOut(2,ZRED,MKSTR("-2Cannot switch right now to ...'%s'. Please wait !",level+strlen(level)/2));
        return 0; // did not finished prev loading
    }

    b_abort  = 0;
    p_system->SetMode(DOWNLOADING_MODE);

    if(_tcsstr(level,"http://"))
    {
        TCHAR   tp[_MAX_PATH];
        GetTempPath(_MAX_PATH,  tp);
        _tcscat(tp,"gbtx");

        assert(0==p_thread);
        p_thread  = new DnlThread(this);
        if(n_port && s_proxy[0])
            return p_thread->Start(level, tp, s_proxy, n_port);
        else
            return p_thread->Start(level, tp, 0, 0);

    }
    return 0;
}

//---------------------------------------------------------------------------------------
int     XScene::_On_LoadBsp(long ok)
{
    AutoLock    al(&__mutex);

    char level[128] = {0};
    if(p_thread) 
    {
        DWORD tc = GetTickCount();
        while(GetTickCount() - tc < 5000 && p_thread->StillActive())
        {
            strcpy(s_msg, "Overlapping commands... !");
            _PumpSome(2);
        }
       // assert(!p_thread->StillActive());
        if(p_thread->StillActive())
        {
            return 0;
        }
        if(ok)
            strcpy(level, p_thread->_level);
        delete p_thread; 
    }
    else
    {
        strcpy(s_msg, "Thread teminared by the browser to early !!!");
        ge_sleep(32);
        p_thread = 0;
        return 0;
    }
    if(b_abort)  
    {
        ge_sleep(32);
        p_thread = 0;
        return -1;
    }
    
    p_system->TextOut(3,ZRED, "-2Loading BSP File. Don't Navigate !!!");
    p_system->ShowOutTexts(1);
    if(p_tree)
    {
        b_haslevel = 0;
        p_tree->Clear();
        delete p_tree;
        p_tree = 0;
    }
    if(ok)
    {
        PS     = p_system; // beam tree library
        p_tree = new BeamTree(XBspProc, this, 0);
        if(p_tree)
        {
            p_render->SetCurrent();
            if(NO_ERROR == p_tree->LoadLevel(level))
            {
                b_haslevel = 1;
                p_system->GetCamera()->SetPos(V0,V0);
                p_system->SetMode(PLAY_MODE);
                strcpy(s_msg, " BSP Loaded.");
                n_exbytes = 0;
                p_system->ShowOutTexts(1);
                ge_sleep(64);
                p_thread = 0;
                return 1;
            }
            b_haslevel = 0;
            strcpy(s_msg, "Loading BSP File FAILED. Pick another Level!!!");
            p_system->TextOut(3,ZRED,"-2Loading BSP File FAILED. Pick another Level !!!");
            p_system->ShowOutTexts(1);

            n_exbytes = 0;
            p_system->ShowOutTexts(1);
            delete p_tree;
            p_tree = 0;
        }
    }
    else
    {
        strcpy(s_msg, "Loading BSP File FAILED. Pick another Level!!!");
        p_system->TextOut(3,ZRED,"-2Loading BSP File FAILED. Pick another Level !!!");
    }
    ge_sleep(64);
    p_thread = 0;
    return 0;
}


//---------------------------------------------------------------------------------------
void    XScene::MoveWindow(int x, int y, int w, int h, int redraw)
{
    if(p_render && p_render->Hwnd())
        ::MoveWindow(p_render->Hwnd(),x,y,w,h,redraw);
}

//---------------------------------------------------------------------------------------
void  XScene::Escape()
{
    while(GetCapture() == p_render->Hwnd())
        ReleaseCapture();
    ClipCursor(0);
    while (ShowCursor (TRUE) < 0);
    RECT rt; GetWindowRect(p_render->Hwnd(), &rt);
    HWND hP = ::GetParent(p_render->Hwnd());
    char ct[32];
    while(hP)
    {
        GetWindowText(hP, ct, 31);
        if(ct[0])break;
        hP = ::GetParent(hP);
    }
    if(hP)
    {
        ::SetFocus(hP);
        p_render->ClearColor((REAL)0, (REAL)0,  (REAL)0, 1.0);
    }
    POINT ppcp;
    GetCursorPos(&ppcp);
    SetCursorPos(ppcp.x, rt.top-10);
    b_focus = 0;    
    if(hP)
    {
        ::SetFocus(hP);
        p_render->ClearColor((REAL)0, (REAL)0,  (REAL)0, 1.0);
    }
}

//---------------------------------------------------------------------------------------
void  XScene::SetProxy(const TCHAR* ppx, ULONG port)
{
    if(ppx)
    {
        ::_tcscpy(s_proxy, ppx);
        n_port = port;
    }
}




#pragma message ("If microsoft crapy 2008 visual studio fails with ... errors, just clean ad rebuild baselib gebsplibd system ReleaseNoSq.")
#ifndef _NIS_
#include "atlbase.h"
#include ".\gbtview.h"
#endif //_NIS_
#include "..\rendergl\OpenglRender.h"
#include ".\xscene.h"

//---------------------------------------------------------------------------------------
// game procedure
long XScene::Proc(long m, long w, long l)
{
    if(b_abort)
    {
        if(p_thread)
            _DeleteThread();

        TRACEX("Proc Broken()\n");
        return -1; 
    }
    switch(m)
    {
        case WM_LOADBSP:
            _On_LoadBsp(w);
            break;
        case WM_OPENGLNOTY:
            if(w==-1)
                Escape();
            break;
        case SYS_START:
            Escape();
            b_focus = 0;
            break;
        case WM_SETFOCUS:
            b_focus = 1;
            return 0;
        case WM_KILLFOCUS:
            b_focus = 0;
            return 0;
        case WM_LBUTTONUP:
            break;
        case WM_LBUTTONDOWN:
            if(GetFocus()!=p_render->Hwnd())
                ::SetFocus(p_render->Hwnd());
            Focus();
            break;
        case WM_KEYDOWN:
            if(w==VK_ESCAPE)
            {
                Escape();
            }
            break;
        case SYS_ANIMATE:
            _RunPhisics(((Camera*)w), (SystemData*)l);
            break;
        case SYS_RENDER_2D:
            if(s_msg[0])
            {
                if(n_exbytes)
                {
                    int perc = (n_bytes*100)/n_exbytes;
                    p_system->GetUIMan()->TextOut(1,1,1,MKSTR("%s : %d%%", s_msg, perc),ZWHITE);
                }
                else
                {
                    p_system->GetUIMan()->TextOut(1,0,1,s_msg,ZWHITE);
                }
            }
            return HANDELED;
        case SYS_RENDER_3D:
            _RenderScene((SystemData*)l, Z_POLYGON);
            break;
        case SYS_PROGRESS:
        case SYS_PROGRESSPAINT:
            p_system->Default(m,w,l);
            break;
    }
    return DEFAULT;
}
//---------------------------------------------------------------------------------------
long  XBspProc(BeamTree* pt , long msg , long w, long l)
{
    BTF_Item* pbu = (BTF_Item*) l ;
    return 0;
}


//---------------------------------------------------------------------------------------
int     XScene::Download_Proc(UINT msg, long wp, long lp)
{
#ifdef _DEBUG
    ge_sleep(1);
#endif //
    if(b_abort) return -1;
    if(msg == SYS_PROGRESSPAINT)
    {
        if(wp == -1) //start or stop (-1 and file name)
        {
            if(lp)
            {
                sprintf(s_msg,"Please wait. downloading %s", (char*)lp);
            }
            else
            {
                n_bytes   = 100;
                n_exbytes = 100;
            }
        }
        else if(wp == 0) // receiveing
        {
            n_exbytes = (DWORD)lp;
        }
        else // can be -1 then is an error
        {
            n_bytes = wp;
            n_exbytes = (DWORD)lp;
        }
    }
    return 1;
}

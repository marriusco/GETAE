
#include ".\dnlthread.h"
#include ".\xscene.h"


//-----------------------------------------------------------------------------------------
//
DnlThread::DnlThread(XScene* ps):_pscene(ps),_success(0)
{
    _proxy[0] = 0;
    _port = 0;
}

DnlThread::~DnlThread()
{
   
}

int     DnlThread::Start(const TCHAR* level, const TCHAR* tp, const TCHAR* proxy, UINT port)
{
    ::strcpy(_level, level);
    ::strcpy(_tp, tp);
    proxy ? ::strcpy(_proxy, proxy) : 0; 
    _port = port;
    this->_stopped = 1;
    return OsThread::Start();
}

static  int  S_Download_Proc(UINT msg, long wp, long lp, void* pUser)
{
    return ((XScene*)pUser)->Download_Proc(msg,wp,lp);
}

#include "wininet.h"

void DnlThread::ThreadFoo()
{
    this->_stopped = 0;
    TRACEX("ThreadFoo()<-\n");


    //_ResolveProxy();
    LevelMan lm(_level, _tp, _proxy[0] ? _proxy : 0, _port);

    if(_tcsstr(_level, _T("..")))
    {
        this->_stopped = 1;
        for(int i=0;i<8;i++)
        {
            if(PostMessage(_pscene->p_render->Hwnd(), WM_LOADBSP,0,(long)_level))
                break;
            Sleep(0);
        }
        return;
    }

    lm.Notify( S_Download_Proc, _pscene);

    lm.CheckLocally();
    ///if(lm.CheckLocally() != 0)
    
    {
        if(lm.Download()!=0)
        {
            TRACEX("DnlThread::ThreadFoo(fail)->\n");
            this->_stopped = 1;
            for(int i=0;i<8;i++)
            {
                if(PostMessage(_pscene->p_render->Hwnd(), WM_LOADBSP,0,(long)_level))
                    break;
                Sleep(0);
            }
            return;
        }
    }
	lm.FillLocoFile();
    ::strcpy(_level, lm.GetLocoLevel());
    this->_stopped = 1;
    if(_pscene && ::IsWindow(_pscene->p_render->Hwnd()))
    {
        for(int i=0;i<8;i++)
        {
            if(PostMessage(_pscene->p_render->Hwnd(), WM_LOADBSP,1,(long)_level))
                break;
            Sleep(1);
        }
    }
    TRACEX("ThreadFoo(success)->\n");
}



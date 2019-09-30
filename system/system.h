//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __EGENGINE_H__
#define __EGENGINE_H__

#include "_isystem.h"
#include <IO.H>
#include "syserrors.h"
#include "basecont.h"
#include "bspfilestr.h"
#include "../geticnetsrv/pkbuffer.h"
#include <stdarg.h> 
#include <stdio.h>
#include "sndsys.h"
#include "texman.h"
#include "uiman.h"
#include "levelman.h"
#include "netclient.h"
#include "inputsys.h"
#include "rendersys.h"
#include "camera.h"


    #include "sqplus.h"
    using namespace SqPlus;

//---------------------------------------------------------------------------------------
#pragma warning (disable: 4311)
#define MAX_TL 12

//---------------------------------------------------------------------------------------
#define LOG_ERROR      0x1
#define LOG_WARNING    0x2
#define LOG_INFO       0x4

//---------------------------------------------------------------------------------------
typedef long *pfmain(ISystem*, HINSTANCE, const TCHAR*);


//---------------------------------------------------------------------------------------
class CIniReader;
class System : public ISystem
{
public:
    friend class NetClient;

    System();
    ~System();
public:

    static long     DefNotifyProc(ISystem*, long m, long w, long p);

    static System*  LoadScript(HINSTANCE hi, const TCHAR* cmdLine, const TCHAR* procMain,  HWND parent=0);
    static System*  LoadDLL(HINSTANCE hi, const TCHAR* cmdLine, const TCHAR* procMain,  HWND parent=0);
    static System*  Create(HINSTANCE hInstance, Irender* prender, int x, int y, HWND m_hWnd);


    static void     Destroy(System* pS);
    static void     Log(int err, TCHAR* psz,...);
    const TCHAR*    GetComandLine();
    void    RegisterDestructor(pdestruct , void* );
    long   Default(long, long, long);
    void    Lock(){_mutex.Lock();}
    void    Unlock(){_mutex.Unlock();}
    int     TryLock(){return _mutex.TryLock();}
    void    RestoreMode(){
        _mode = _prevMode;
        RECT rt; GetWindowRect(_hwnd, &rt);
        SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
    };
    void    SetMode(long ss){
        _prevMode = _mode; 
        _mode     = (sysMODES)ss;
        RECT rt; GetWindowRect(_hwnd, &rt);
        SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
    };
    long    GetMode(){
        return _mode;
    };
    void      SetHudMode(unsigned long ss){_hudMode=ss;};
    unsigned long   GetHudMode(){return _hudMode;};
    int     SetNf(NotifyProc np, const TCHAR* script=0);
    int     Run();
    int     Stop(void);
    void    Spin(REAL&);
    void    InitVariables();
// camera functions
    void    SetCamera(Camera* pCam){_pCamera = pCam;}
    Camera* GetCamera(){return _pCamera;}
    
// access plugs functions
    Irender* Render();
    Isound*  Sound();
    Iinput*  Input();

// access sub sys functions
    SndSys*    GetSndSys()      {
        return &_sndSys;
    }
    InputSys*  GetInputSys()    {
        return &_keySys;
    }
    RenderSys* GetRenderSys()   {
        return &_rndSys;
    }
    
    UIMan*      GetUIMan()       {return &_uiMan;} // if not ptr script creates an instance
    TexMan*     GetTexMan()      {return &_texMan;}    
    NetClient*  GetNetClient()   {return &_netClient;}
    SystemData* GetSysData()     {return &_sysData;}

    void     TextOut(long id, const TCHAR*,...);
    void     TextOut(long font, const CLR& color, const TCHAR*,...);
    void     Printf(const TCHAR*,...);
    void     Print(const TCHAR* pc){Printf(pc);};
    int      PumpMessage(){MSG m; return _ReadWinMsg(&m);}
    void     Wait(long percent){};

    int      UpdateWindow(BOOL camtoo=0);
    void     SetCamInfo(const BST_SceneInfo* si);
// engineproc 
    long     SendMessage(long a, long b, long c){
        static long p[3];
        p[0] = a;
        p[1] = b;
        p[2] = c;
        return _Dispatch(this, p[0],p[1],p[2]);
        }
    long     PostProc(long a, long b, long c, long d=0);
    virtual SysMsg*   GetMessage(long );

// render texture functions
    void     ToggleFullScreen();
    Htex*    GenTexGetBuff(const TCHAR* , DWORD, char**, int*, int*, int*);
    Htex*    GenTex(const TCHAR* name, int x, int y, int bpp, const char* buff, DWORD mip);
    Htex*    GenTexFile(const TCHAR* , DWORD);
    Htex*    GenTexFont(REAL x, REAL y, REAL scale, const TCHAR* fname);
    void     DeleteTexFont(Htex*, BOOL allrefs);
    void     DeleteTex(Htex*, BOOL allrefs);
    void     DeleteTexts(Htex*,int, BOOL allrefs);
    const UINT*  Primitives(){return _rndSys.a_prims;}
    const UINT   Primitive(int i){return _rndSys.a_prims[i];}
    RndStruct*   DrawProps(){return &_rndSys.s_rndStruct;}
    const StartStruct& GetStartStruct()const {return _startUp;}
//sound functions
    Itape*   AddSoundFile(const TCHAR* pFile);
	void     RemoveSoundFile(const TCHAR* pFile);
	void     RemoveSound(Itape* pt);
	Itape*   GetTape(const TCHAR* fname);
    void     PlayPrimary(const Itape* pt, DWORD mode=1);
   	void     StopPlay(const Itape* pt );
    Itape*   PlayPrimaryFile(const TCHAR* fname, DWORD mode=1);
	void     StopPlayFile(const TCHAR* fname);

//input functions
    void     SetSpeeds(REAL ts, REAL rs);
	BOOL     IsKeyDown(long key);
    int *    Moves();
    BYTE*    Keys();
    BYTE     GetPressedKey();
    void     IgnoreInput(REAL secs);

//object foos
    long            LoadGame(const TCHAR* dllname);


    void     OnSwitchMode();
    void     OnEscape();
    static void  Export2Script();
    static Htex  S_CreateTex(int dx, int dy, int bpp, BYTE* pb, DWORD mips);
    static void  S_DeleteTex(UINT* tex, int ncount);
    void    Interrupt();
    HINSTANCE   Hinstance(){return _hinstance;}
    HWND        Hwnd(){return _hwnd;}
    void        RegisterGc(IGc* pigc);
    void        UnregisterGc(IGc* pigc);
    void        SetResourcesSearchDir(const TCHAR* dircomma);
    void        Delete(Vtx* pvx);
    Vtx*        New(int count);
    void        ShowOutTexts(BOOL swap=0);
private:
    void 	_Clean();
    void    _ShowProgress(long w, long p);
    void    _NPlayLoadLevel(const char* url);
    void    _NPDispatch(const Buffer* pNetBuffer);

    int     _Timers(REAL& );
    void    _Render(UINT mask);
    
    void    _SetText(LPCTSTR text, const CLR* color, const int font);
    int     _Start(void);
    int     _Configure(const TCHAR* iniFile, HWND parent);
    INLN    DWORD _ReadWinMsg(MSG* pmsg);
    void    _CleanGc();
    long    _CreateRender(Irender* prender=0);
    long    _CreateInput(Iinput* psound=0);
    long    _CreateSound(Isound* psound=0);
    void    _InitalizeFromInterior(Irender* prender);
    void    _OnSwitchMode();
    int    _DequeueMessages(BOOL roundUp=TRUE);
    template <class T, class U> long  _Dispatch(System* ps, long msg, T u, U v)
    {
        long rv = DEFAULT;
        if(this->_scrSysProc)
        {
            rv =  (*_scrSysProc)((System*)this, msg, u, v);
        }
        if(0==rv)
        {
            rv = this->_NotyProc(this, msg, (long)u, (long)v);
        }
        return rv;
    }
private:

    StartStruct             _startUp;
    SystemData              _sysData;
    sysMODES                _mode;
    sysMODES                _prevMode;
    BOOL                    _rendered;
    BOOL                    _stopping;
    NetClient               _netClient;
    TexMan		            _texMan;
    UIMan                   _uiMan;
	SndSys		            _sndSys;
	InputSys	            _keySys;
	RenderSys	            _rndSys;
    Timer                   _timer;
    NotifyProc              _NotyProc;
    HINSTANCE               _hDll;
    UINT*                   _primitives;
    Camera*                 _pCamera;
    Camera                  _camera;
    SquirrelFunction<int>*  _scrSysProc;
    HWND                    _hwndParent;
    HWND                    _hwnd;
    HINSTANCE               _hinstance;
    string                  _texts[MAX_TL];
    struct SLine
    {
        char                line[256];
        CLR                 color;
        int                 font;
        REAL                tout;
    }                       _status[4];    
    int                     _statuses;
private:
    map<long, pdestruct>   _destructs;
    RawRing<SysMsg,256>    _msgque;
    vvector<IGc*>          _gcptrs;
    mutex                  _mutex; 
    BOOL                   _cleaned;
    BOOL                   _online;
    DWORD                  _hudMode;
    tstring                _proxyip;   // form ini file
    UINT                   _proxyport;
    DWORD                  _dummy;
    REAL                   _physycsTime;
    REAL                   _accTime;
    TCHAR                  *_resIncludes;
public:
    static TCHAR           _lastError[1024];
    static int             _tracelevel;
    static TCHAR           _logFile[128];
    static TCHAR           _comandLine[_MAX_PATH];
};



//---------------------------------------------------------------------------------------
INLN DWORD System::_ReadWinMsg(MSG* pmsg)
{
	if (PeekMessage(pmsg,NULL,0,0,PM_REMOVE))
	{
		if(pmsg->message == WM_SYSKEYDOWN || pmsg->message == WM_KEYDOWN)
		{
            if(VK_RETURN==pmsg->wParam )
            {
                if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
                {
                    return OS_TOGGLESCR;
                }
            }
            if(pmsg->wParam == VK_ESCAPE)
            {
                if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
                    return OS_EXITAPP;
                return OS_ESCAPE;
            }
        }
		DispatchMessage(pmsg);				
		return OS_MSG;
	}
    return OS_NOMSG;
}

extern System* PSystem;

//-----------------------------------------------------------------------------
class FileBrowser
{
public:
    FileBrowser(){};
    ~FileBrowser(){};
    int    BroseDir(const TCHAR* dir, const TCHAR* wildchar)
    {
        CDirChange cd(dir);
        struct _finddata_t c_file;
        long    hFile;
        TCHAR   loco[256];
        _tcscpy(loco, wildchar);
        _files.clear();

        TCHAR*   pTok = _tcstok(loco,",");
        while(pTok)
        {
            int found = 0;
            if( (hFile = _findfirst( pTok, &c_file )) != -1L )
            {
                _files << c_file.name;
                while( _findnext( hFile, &c_file ) == 0 )
                {
                    _files << c_file.name;
                }
            }
            _findclose( hFile );

            pTok=_tcstok(0,",");
        }



       return _files.size();
    }
    const TCHAR* GetAt(int index){
        if(index < _files.size())
        {
            return _files[index].c_str();
        }
        return "";
    }

private:
    vvector<t_string> _files;
};

/*
    #include "exports.h"
*/
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#define _LOG System::Log
//---------------------------------------------------------------------------------------

#endif // !__EGENGINE_H__

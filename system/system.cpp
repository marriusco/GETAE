//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include <windows.h>
#include <tchar.h>
#include "../../gebsplib/beamtree.h"
#include "system.h"
#include "inireader.h"
#include "levelman.h"
#include "sock.h"

//---------------------------------------------------------------------------------------
ISystem* PISystem = 0;
System*  PSystem  = 0;
TCHAR    System::_lastError[1024]={0};
int      System::_tracelevel = 0;
TCHAR    System::_logFile[128];
TCHAR    System::_comandLine[_MAX_PATH];
const    int Zero = 0;
const    int Unu  = 1;

LONG Win32ExceptionHandler(struct _EXCEPTION_POINTERS* ExInfo);

//---------------------------------------------------------------------------------------
System*  System::Create(HINSTANCE hInstance, Irender* prender, 
                        int x, int y, HWND hWnd)
{
    if(0 == PSystem)
    {
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)Win32ExceptionHandler);
        PSystem = new System();
        PSystem->_InitalizeFromInterior(prender);
    }
    return PSystem;
}



System*  System::LoadDLL(HINSTANCE hi, const TCHAR* cmdLine, const TCHAR* procMain, HWND parent)
{
    _tcscpy(_comandLine, cmdLine);
    if(0 == PSystem)
    {
        TCHAR dllName[128];
        _tcscpy(dllName, procMain);
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)Win32ExceptionHandler);

        TCHAR appName[260];
        PSystem = new System();
        GetModuleFileName(  hi, appName, 260);
        char* pl = &appName[strlen(appName)-1];
        while(pl!=appName && *pl!='.')--pl;
        if(*pl=='.')*pl=0;
        if(*(pl-1)=='d')*(pl-1)=0;

        _tcscpy(System::_logFile,appName);
        _tcscat(System::_logFile,".log");
        _unlink(System::_logFile);
        _tcscat(appName,".ini");
        PSystem->_hinstance = hi;
        
        if(PSystem->_Configure(appName, 0))
        {
            delete PSystem;
            PSystem = 0;
            return 0; 
        }  
        
        char ddllName[128];
        char ccmdLine[128]="";
        if(strchr(_comandLine,'-'))
        {
            char szCmdLin[128];
            strcpy(szCmdLin, _comandLine);
            strcpy(ddllName, strtok(szCmdLin," -"));
            strcpy(ccmdLine,strtok(0," -"));
        }
        else
        {
            strcpy(ddllName, _comandLine);
        }
        strcpy(ccmdLine, _comandLine);
        
        
        PSystem->_hDll = ::LoadLibrary(ddllName);
        if(PSystem->_hDll > HMODULE(32))
        {
            pfmain* pMain = (pfmain*)::GetProcAddress(PSystem->_hDll, _T("main"));
            if(pMain)
            {
                const NotifyProc pFoo = (NotifyProc)(pMain)(PSystem, hi, ccmdLine);
                PSystem->SetNf(pFoo, 0);
            }
        }
        if(0!=PSystem->_Start())
        {
            delete PSystem;
            PSystem = 0;
        }
    }
    return PSystem;
}

//---------------------------------------------------------------------------------------
System*  System::LoadScript(HINSTANCE hi, const TCHAR* cmdLine, const TCHAR* procMain, HWND parent)
{
    _tcscpy(_comandLine, cmdLine);
    if(0 == PSystem)
    {
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)Win32ExceptionHandler);

        TCHAR appName[260];
        PSystem = new System();
        GetModuleFileName(  hi, appName, 260);
        char* pl = &appName[strlen(appName)-1];
        while(pl!=appName && *pl!='.')--pl;
        if(*pl=='.')*pl=0;
        if(*(pl-1)=='d')*(pl-1)=0;

        _tcscpy(System::_logFile,appName);
        _tcscat(System::_logFile,".log");
        _unlink(System::_logFile);
        _tcscat(appName,".ini");
        PSystem->_hinstance = hi;
        
        if(PSystem->_Configure(appName, 0))
        {
            delete PSystem;
            PSystem = 0;
            return 0; 
        }  

        //System::Export2Script();
        BeamTree::Export2Script(PSystem, hi);

        PSystem->SetNf(0, procMain);
        if(0!=PSystem->_Start())
        {
            delete PSystem;
            PSystem = 0;
            return 0; 
        }
    }
    return PSystem;
}

//---------------------------------------------------------------------------------------
// confing engine
System::System():_texMan(&System::S_CreateTex, &System::S_DeleteTex)
{
    assert(0==PSystem);
    TRACEX("System::System(%X)\r\n",this);
    
    memset(&_sysData,0,sizeof(_sysData));
    _resIncludes = 0;
    _sysData._pauseTime = 6.0;
    _NotyProc   = System::DefNotifyProc;
    _scrSysProc = 0;
    _status[0].line[0]=0;
    _status[1].line[0]=0;
    _status[2].line[0]=0;
    _status[3].line[0]=0;

    _status[1].tout = 0;
    _status[2].tout = 0;
    _status[3].tout = 0;
    _status[4].tout = 0;

    _hDll       = 0;
    _pCamera    = 0;
    _cleaned    = 0;
    _stopping   = 0;
    _statuses   = 0;
    _hwndParent = 0;
    _accTime    = 0;
    _physycsTime= 0.01;
    sock::Init();
    
}

//---------------------------------------------------------------------------------------
void   System::Destroy(System* pS)
{
    sock::Uninit();
    delete PSystem;
    PSystem = 0;
}

//---------------------------------------------------------------------------------------
void   System::InitVariables()
{
    _rndSys.s_rndStruct.hInstance  = _hinstance;
    _rndSys.s_rndStruct.wndParent  = _hwndParent;
    _sysData._pSystem = this;
}

//---------------------------------------------------------------------------------------
int    System::_Configure(const TCHAR* iniFile, HWND parent)
{
    _LOG(LOG_INFO, "_Configure(%s) \r\n", iniFile);

    CIniReader ir(iniFile);

    System::_tracelevel = ir.Readf(_T("engine"),_T("trace"), 0x0);
    _tcscpy(_startUp._renderSel, ir.Reads("engine","_renderSel","rendergl.dll"));
    _tcscpy(_startUp._inputSel,  ir.Reads("engine","_inputSel","inputdx.dll"));
    _tcscpy(_startUp._audioSel,  ir.Reads("engine","_audioSel","sounddx.dll"));

    _rndSys.s_rndStruct.hInstance  = _hinstance;
    _rndSys.s_rndStruct.wndParent  = parent;
    
    _physycsTime                      = ir.Readf(_T("render"),_T("fps"), 0.03125);
    _proxyip                          = ir.Reads(_T("proxy"),_T("ip"), "");
    _proxyport                        = ir.Readi(_T("proxy"),_T("port"), 0);

    _rndSys.s_rndStruct.hideCurosr    = ir.Readi(_T("render"),_T("hideCurosr"), 1);
    _rndSys.s_rndStruct.bFullScreen   = ir.Readi(_T("render"),_T("bFullScreen"), 0);
    _rndSys.s_rndStruct.retainedDraw  = ir.Readi(_T("render"),_T("retainedDraw"), 1); 
    _rndSys.s_rndStruct.fNear		  = ir.Readf(_T("render"),_T("fNear"), 8.0); 
    _rndSys.s_rndStruct.fFar		  = ir.Readf(_T("render"),_T("fFar"), 65535.0); 
    _rndSys.s_rndStruct.hFov		  = ir.Readf(_T("render"),_T("hFov"), 64.0); 
    _rndSys.s_rndStruct.xPolyMode     = ir.Readi(_T("render"),_T("xPolyMode"), Z_FRONT_AND_BACK); 
    _rndSys.s_rndStruct.pfDepthBits   = ir.Readi(_T("render"),_T("pfDepthBits"), 32); 
    _rndSys.s_rndStruct.pfStencilBits = ir.Readi(_T("render"),_T("pfStencilBits"), 0); 
    _rndSys.s_rndStruct.bgColor       = ir.Readi(_T("render"),_T("bgColor"), 0); 
    _rndSys.s_rndStruct.clipCursor    = ir.Readi(_T("render"),_T("clipCursor"), 0);    
    _rndSys.s_rndStruct.bsetCapture   = ir.Readi(_T("render"),_T("bsetCapture"), 0);    
    _rndSys.s_rndStruct.dmPelsWidth   = ir.Readi(_T("render"),_T("dmPelsWidth"), 800);   
    _rndSys.s_rndStruct.dmPelsHeight  =	ir.Readi(_T("render"),_T("dmPelsHeight"),600);  
    _rndSys.s_rndStruct.dmBitsPerPel  = ir.Readi(_T("render"),_T("dmBitsPerPel"),32); 

    
    
    return 0;
}


//---------------------------------------------------------------------------------------
int	System::_Start()
{

    this->_Dispatch(this, SYS_CONFIGURE, &_startUp, &_rndSys.s_rndStruct);

    _LOG(LOG_INFO, "_Start() ...loading plugins\r\n");
    if(_CreateRender())
    {
        printf("Cannot create render subsystem. \r\n");
        _LOG(LOG_ERROR, "Cannot create render subsystem. \r\n");
        return -1;
    }
    _uiMan.Create(&_rndSys.s_rndStruct);
    _CreateSound();
    _CreateInput();
    _LOG(LOG_INFO, "Setting render FOV and FAR\r\n");
    _camera.SetPos(V0,V0);
    _camera.SetFarAndFov(_rndSys.s_rndStruct.fFar, _rndSys.s_rndStruct.hFov);
    _pCamera = &_camera;
    _uiMan.Paint(&this->_sysData);
    SetMode(SET_MODE);
    _LOG(LOG_ERROR, "_Start() exits\r\n");


	return 0;
}

//---------------------------------------------------------------------------------------
// unfactory
void	System::_Clean()
{
    _LOG(LOG_INFO, "_Clean() \r\n");

    delete[] _resIncludes;
    _keySys.Unaquire();    
    if(1==_cleaned)
        return;
    _cleaned = 1;
    FreeLibrary(_hDll);
    _hDll = 0;
    _CleanGc();
    _msgque.Reset();
    _netClient.Leave();
    _keySys.Destroy();
    _uiMan.Clear();
    _sndSys.Destroy();
    _texMan.Clear();
	_rndSys.Destroy();

}

//---------------------------------------------------------------------------------------
int     System::SetNf(NotifyProc np, const TCHAR* script)
{
    _LOG(LOG_INFO, "SetNf(%s) \r\n", script);

    if(np)  
        _NotyProc = np;
    if(script && *script)
    {
        try{
            _scrSysProc = new SquirrelFunction<int>(script);
        }catch(SquirrelError& e)
        {
            _tcscpy(_lastError, e.desc);
            _LOG(LOG_ERROR, "Script: %s \r\n", e.desc);
            return 1;
        }
    }
    return 0;
}


//---------------------------------------------------------------------------------------
void System::OnEscape()
{
    for(int i=MAX_TL-1;i>0;i--)
    {
        _texts[i]="";
    }
    switch(_mode)
    {
        case DOWNLOADING_MODE:
        case PLAY_MODE:
            SetMode(SET_MODE);
            break;
        case SET_MODE:
            SetMode(PLAY_MODE);
            break;
    }
    _uiMan.ResetCursor(&_sysData);
}

//---------------------------------------------------------------------------------------
void System::ToggleFullScreen()
{
    DWORD dwr = !_rndSys.s_rndStruct.bFullScreen;
    this->_Dispatch(this, SYS_TOGLESCRREN, dwr, &_sysData);
    _OnSwitchMode();
}

//---------------------------------------------------------------------------------------
void System::_OnSwitchMode()
{
    _keySys.Unaquire();      
    _rndSys.GetRender()->SwitchMode();
    ::UpdateWindow(_hwnd );
    SetForegroundWindow(_hwnd );
    SetFocus(_hwnd );
}

//---------------------------------------------------------------------------------------
int    System::Stop(void)
{
    PostQuitMessage(0);

    if(_stopping)return 0;
    _stopping=1;
    _keySys.Unaquire();   
    _LOG(LOG_INFO, "Stop() \r\n");
	ClipCursor(0);
	if(_rndSys.s_rndStruct.bFullScreen) 
    {
        _LOG(LOG_INFO, "Render()->SwitchMode() \r\n");
        _rndSys.GetRender()->SwitchMode();
        ShowWindow(_rndSys.GetRender()->Hwnd(),SW_HIDE);
        this->_Dispatch(this, SYS_TOGLESCRREN, _rndSys.s_rndStruct.bFullScreen, &_sysData);
    }
    return 0;
}

//---------------------------------------------------------------------------------------
long System::_CreateRender(Irender* prender)
{
    if(prender)
    {
        _rndSys.Create(prender);
        _hwnd  = _rndSys.GetRender()->Hwnd();


        RECT rt; GetClientRect(_hwnd,&rt);
        //::MoveWindow(_hwnd, 1024-rt.right, 792-rt.bottom, rt.right,rt.bottom,0);
        memcpy(&_rndSys.s_rndStruct, prender->GetStruct(), sizeof(RndStruct));
        _sysData._pRndStruct = &_rndSys.s_rndStruct;
        return NO_ERROR;
    }
    else
    {
        _rndSys.s_rndStruct.xPolyMode     =  USE_CCW|SHOW_BOTH;
        _rndSys.s_rndStruct.hInstance     = _hinstance;
        _rndSys.s_rndStruct.clipCursor    = 1;
        if(_rndSys.Create(this, _startUp._renderSel, _hinstance))
        {
            _hwnd       = _rndSys.GetRender()->Hwnd();
            _primitives = _rndSys.a_prims;
            _sysData._pRndStruct = &_rndSys.s_rndStruct;
            return NO_ERROR;
        }
    }
    return -1;
}

//---------------------------------------------------------------------------------------
long   System::_CreateSound(Isound* psound)
{
    if(psound)
    {
        _sndSys.Create(psound);
        return 0;
    }
    return _sndSys.Create(_startUp._audioSel, _hwnd);
}

//---------------------------------------------------------------------------------------
long   System::_CreateInput(Iinput* pinput)
{
    if(pinput)
    {
        return _keySys.Create(pinput);
    }
    else
    {
        if(_keySys.Create(_startUp._inputSel, _hwnd , _hinstance, 0, &_sysData))
        {
            _keySys.Unaquire();
            return NO_ERROR;
        }
    }
    return -1;

}

//---------------------------------------------------------------------------------------
Irender* System::Render()
{
    return _rndSys.GetRender();
}

//---------------------------------------------------------------------------------------    
Isound*  System::Sound()
{
    return _sndSys.GetSound();
}

//---------------------------------------------------------------------------------------
Iinput*  System::Input()
{
    return _keySys.GetIInput();
}


//---------------------------------------------------------------------------------------
int     System::_DequeueMessages(BOOL roundUp)
{
    SysMsg* pMsg;
    size_t  length = _msgque.Size();
    while(length-->0)
    {
        if(pMsg = _msgque.Get())
        {
            if(pMsg->time <= _sysData._curtime*1000)
            {
                if(pMsg->msg == SYS_UICOMMAND && pMsg->pw==-1)//quit
                {
                    if(RET_NOT_PROC==this->_Dispatch(this, SYS_CLOSEAPP, Zero, &_sysData))
                    {
                        this->Stop();
                        return -1;
                    }
                }
                else
                    this->_Dispatch(this, pMsg->msg, pMsg->pw, pMsg->pl);

            }
            else 
                _msgque.Put(*pMsg);
        }else break;
    }
    return DEFAULT;
}

//---------------------------------------------------------------------------------------
void     System::_Render(UINT mask)
{
    Irender* r = _rndSys.GetRender();
    r->Clear();
    
    if(! (mask & NO_RENDER) )
    {
        if(_mode != SET_MODE)
        {
            r->LoadIdentity();
            r->Push();
                r->SetViewMtx(_pCamera->ViewMatrix());
                if(DEFAULT == this->_Dispatch(this, SYS_RENDER_3D, _pCamera, &_sysData))
                {
                    this->Default(SYS_RENDER_3D, (long)_pCamera, (long)&_sysData);
                }
            r->Pop();
        }
    }
    if(! (mask & NO_PAINT))
    {
        if(DEFAULT == this->_Dispatch(this, SYS_RENDER_2D, _pCamera, &_sysData))
        {
            this->Default(SYS_RENDER_2D, (long)_pCamera, (long)&_sysData);
        }
    }
    r->Swap();
#ifdef _DEBUG
    ge_sleep(1);
#endif //
    ge_sleep(0);
}

//---------------------------------------------------------------------------------------
void    System::ShowOutTexts(BOOL swap)
{
    Irender* r = _rndSys.GetRender();

    if(swap)
    {
	    r->SetCurrent();
        r->Clear();
        r->LoadIdentity();
    }
    
    const UV& vp = _uiMan.GetVievPort();
    const UV& cp = _uiMan.GetCharSize(2);
    REAL  ypos = vp.v - vp.v/2;   // top
    REAL  xpos = vp.u /2; // at the half screen
    SystemData* psd = GetSysData();

    Render()->DisableTextures(1);
	Render()->Color(ZWHITE);
    for(int i = MAX_TL-1; i>0; i--)
    {
        if(!_texts[i-1].empty())
        {
            _uiMan.TextOut(2, xpos, ypos, _texts[i-1].c_str(), ZGREEN);
            
        }
        ypos -= cp.v;
    }
    for(int j=0;j<3;j++)
    {
        SLine& sl = PSystem->_status[j];
        if(sl.line[0])
        {
            const UV& cp1 = _uiMan.GetCharSize(sl.font);
            _uiMan.TextOut(sl.font, 0, ypos, sl.line, sl.color);
            ypos += cp1.v;

            sl.tout -= _sysData._ticktime;
            if(sl.tout<0)
                sl.line[0]=0;
        }
    }

    if(swap)
    {
        r->Swap();
    #ifdef _DEBUG
        ge_sleep(1);
    #endif //
    }
}

//---------------------------------------------------------------------------------------
int     System::UpdateWindow(BOOL camtoo)
{
    _Render(0);
    return 0;
}

//---------------------------------------------------------------------------------------
long     System::PostProc(long a, long b, long c, long d)
{
    SysMsg s;
    s.msg  = a;
    s.pw   = b;
    s.pl   = c;
    s.time = d + _sysData._curtime*1000;
    _msgque.Put(s);
    return 0;
}

//---------------------------------------------------------------------------------------
SysMsg*   System::GetMessage(long how)
{
    if(_msgque.Size())
        return _msgque.Get();
    return 0;
}

//---------------------------------------------------------------------------------------
Htex*      System::GenTexGetBuff(const TCHAR* fn, DWORD mip, char** ppout, int* px, 
                                int* py, int* pb)
{
    assert(_rndSys.GetRender());
    return &_texMan.AddTextureFileGetImage(fn, mip, (BYTE**)ppout, px, py, pb);
}

//---------------------------------------------------------------------------------------
Htex*      System::GenTex(const TCHAR* name, int x, int y, int bpp, const char* buff, DWORD mip)
{
    assert(_rndSys.GetRender());
    return &_texMan.GenTexture(name, x, y, bpp, (BYTE*)buff, mip);
}

//---------------------------------------------------------------------------------------
Htex*      System::GenTexFile(const TCHAR* fn, DWORD mip)
{
    assert(_rndSys.GetRender());
    return &_texMan.AddTextureFile(fn, mip);
}


//---------------------------------------------------------------------------------------
//extern unsigned long fontData[];
Htex*      System::GenTexFont(REAL xc, REAL yc, REAL scale, const TCHAR* fname)
{
    return 0;
    Htex& itex = _texMan.GenTexture("$font",256,256, 1, (BYTE*)0, TEX_NORMAL);

    if(itex.hTex == 0)
        return &itex;
	
	if(itex.genST = _rndSys.GetRender()->GenLists(256))
	{
        _rndSys.GetRender()->Color(ZWHITE);
        _rndSys.GetRender()->BindTexture(itex, 0);

		int loop=0;
		for (int y=15; y>=0; y--)					
		{
			for (int x=0; x<16; x++)
			{
				REAL cx = REAL(x)/16.f;	
				REAL cy = REAL(y)/16.f;	

				_rndSys.GetRender()->NewList(itex.genST+loop);
				_rndSys.GetRender()->Begin(Z_QUADS);

                _rndSys.GetRender()->Texture2(cx,cy+0.0625f);
				_rndSys.GetRender()->Vertex2(0.0f,yc);

				_rndSys.GetRender()->Texture2(cx+0.0625f,cy+0.0625f);
				_rndSys.GetRender()->Vertex2(xc,yc);

				_rndSys.GetRender()->Texture2(cx+0.0625f,cy);
				_rndSys.GetRender()->Vertex2(xc,0.0f);
									
				_rndSys.GetRender()->Texture2(cx,cy);
				_rndSys.GetRender()->Vertex2(0.0f,0.0f);

				_rndSys.GetRender()->End();
				_rndSys.GetRender()->Translate(V3(scale,0.0f,0.0f));
				_rndSys.GetRender()->EndNewList();
				loop++;
			}
		}
		_rndSys.GetRender()->UnBindTexture(itex, 0);
	}
	return &itex;
}

//---------------------------------------------------------------------------------------
void      System::DeleteTexFont(Htex* htexfont, BOOL allrefs)
{
   	if(0 == _texMan.RemoveTexture(*htexfont, allrefs))
    {
        if(_rndSys.IsLoaded())
	        _rndSys.GetRender()->DeleteList(htexfont->genST);
    }
}

//---------------------------------------------------------------------------------------
void      System::DeleteTexts(Htex* tex, int ncount, BOOL allrefs)
{
    int lefover = _texMan.RemoveTextures(tex, ncount, allrefs);
}

//---------------------------------------------------------------------------------------
void      System::DeleteTex(Htex* hTex, BOOL allrefs)
{
    int lefover = _texMan.RemoveTexture((Htex&)*hTex, allrefs);
}


//---------------------------------------------------------------------------------------
Htex   System::S_CreateTex(int dx, int dy, int bpp, BYTE* pb, DWORD mips)
{
    return PSystem->Render()->GenTexture(dx, dy, bpp, pb, mips);
}

//---------------------------------------------------------------------------------------
void   System::S_DeleteTex(UINT* utexts, int ncount)
{
    PSystem->Render()->RemoveTextures((UINT*)utexts, ncount);
}



//---------------------------------------------------------------------------------------
void System::IgnoreInput(REAL r)
{
    _keySys.IgnoreInput(r);
}

unsigned char System::GetPressedKey()
{
    return _keySys.GetPressedKey();
}

unsigned char * System::Keys()
{
    return _keySys.Keys();
}

int * System::Moves()
{
    return _keySys.Moves();
}

int System::IsKeyDown(long key)
{
    return _keySys.IsKeyDown(key);
}

void System::SetSpeeds(REAL s,REAL m)
{

}

void System::StopPlayFile(const TCHAR* filename)
{
    _sndSys.Stop(filename);
}

Itape* System::PlayPrimaryFile(const TCHAR* filename, DWORD flags)
{
    return _sndSys.PlayPrimaryFile(filename,flags);
}

void System::StopPlay(const Itape * ptape)
{
    _sndSys.Stop(ptape);
}

void System::PlayPrimary(const Itape * ptape ,DWORD flags)
{
    _sndSys.PlayPrimary(ptape,flags);
}

Itape * System::GetTape(const TCHAR* filename)
{
    return _sndSys.GetTape(filename);
}

void System::RemoveSound(Itape * ptape)
{
    _sndSys.RemoveSound(ptape);
}

void System::RemoveSoundFile(const TCHAR* filename)
{
    _sndSys.RemoveSoundFile(filename);
}

Itape * System::AddSoundFile(const TCHAR* fielname)
{
    return _sndSys.AddSoundFile(fielname);
}

void     System::Printf(const TCHAR* lpszFormat,...)
{
    static CHAR cahced[1024] = {0};
    static CHAR szBuffer[1024] = {0};


    va_list args;
    va_start(args, lpszFormat);
    ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(CHAR), lpszFormat, args);
    va_end(args);
    printf(szBuffer);
    if(!_tcscmp(cahced, szBuffer))
    {
        _tcscpy(cahced, szBuffer);
        _SetText(szBuffer,&ZGREEN,2);
    }
}

//-----------------------------------------------------------------------------
void     System::TextOut(long font, const TCHAR* lpszFormat,...)
{
    CHAR szBuffer[1024] = {0};
    va_list args;
    va_start(args, lpszFormat);
    ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(CHAR), lpszFormat, args);
    va_end(args);
    _SetText(szBuffer, &ZGREEN, 2);
    ////TRACEX(szBuffer);
}


//-----------------------------------------------------------------------------
void     System::TextOut(long font, const CLR& color, const TCHAR* lpszFormat,...)
{
    CHAR szBuffer[1024] = {0};
    va_list args;
    va_start(args, lpszFormat);
    ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(CHAR), lpszFormat, args);
    va_end(args);
    _SetText(szBuffer, &color, font);
    ////TRACEX(szBuffer);
}

//-----------------------------------------------------------------------------
void System::_SetText(LPCTSTR text, const CLR* color, const int font)
{
    if(text[0])
    {
        if(text[0]=='-')//goes in status line
        {
            int ofs = ::atoi(text+1) & 0x3;
            _status[ofs].color = *color;
            _status[ofs].font  = font;
            _status[ofs].tout  = 4.00;
            if(*(text+2))
                strcpy(_status[ofs].line  , (text+2));
            else
                _status[ofs].line[0]  = 0;
        }
        else
        {
            for(int i=MAX_TL-1;i>0;i--)
            {
                if(_texts[i-1].length())
                    _texts[i]=_texts[i-1];
            }
            _texts[0]=text;
        }
    }
}
 
//-----------------------------------------------------------------------------
void   System::RegisterGc(IGc* pigc)
{
    _gcptrs << pigc;
}

//-----------------------------------------------------------------------------
void   System::UnregisterGc(IGc* pigc)
{
    FOREACH(vvector<IGc*>, _gcptrs, ppGc)
        if(*ppGc == pigc)
            _gcptrs.erase(ppGc);

}

//-----------------------------------------------------------------------------
void   System::_CleanGc()
{
    FOREACH(vvector<IGc*>, _gcptrs, ppGc)
    {
        IGc* pGc = (*ppGc);
        pGc->Clear();
    }
}

//-----------------------------------------------------------------------------
void   System::SetCamInfo(const BST_SceneInfo* si)
{
    if(si)
    {
        _camera.SetFarAndFov(si->camFar, si->camFov);
        _camera.SetAngles(si->camOrient1.y, si->camOrient1.x, si->camOrient1.z);
    }
    else
    {
        _camera.SetPos(V0,V0);
        _camera.SetFarAndFov(_rndSys.s_rndStruct.fFar, _rndSys.s_rndStruct.hFov);
    }
}

//---------------------------------------------------------------------------------------
void    System::_NPDispatch(const Buffer* pb)
{
    switch(pb->GetMsg())
    {
        default:
            break;
        case MSG_SET_ID:
            Printf("<- MSG_SET_ID: %d[%d]", HIWORD(pb->GetClientUid()), LOWORD(pb->GetClientUid()));
            this->_online = true;
            _netClient.SetUniqueID(pb->GetClientUid());
            SendMessage(SYS_NET_SETID, pb->GetClientUid(), 0);
            ge_sleep(10);
            _netClient.Join();
            break;
        case MSG_LOADLEVEL:
            this->Printf("<- MSG_LOADLEVEL: %s", pb->pl._U._Level.url);
            _NPlayLoadLevel(pb->pl._U._Level.url);
            break;
        case MSG_PLAY:
            Printf("<- MSG_PLAY %d[%d]",HIWORD(pb->GetClientUid()), LOWORD(pb->GetClientUid()));             
            SendMessage(SYS_NET_PLAYERON, pb->GetClientUid(), 1);
            break;
        case MSG_LEAVE:
            SendMessage(SYS_NET_PLAYEROFF, pb->GetClientUid(), 1);
            Printf("<- MSG_LEAVE %d[%d]",HIWORD(pb->GetClientUid()), LOWORD(pb->GetClientUid()));
            break;
        case MSG_POSCHANGED:
            SendMessage(SYS_NET_POSITION, pb->GetClientUid(), (long)&(pb->pl)._U._Position);
            break;
        case MSG_ACTION:
            SendMessage(SYS_NET_ACTION, pb->GetClientUid(), (long)&(pb->pl)._U._Position);
            break;
        case MSG_AUTH:
            SendMessage(SYS_NET_AUTH, 0, 0);
            break;
        case MSG_PING:
            SendMessage(SYS_NET_PING,ge_gettick()-pb->pl._U._Ping.time, 0);
            Printf("Pong: %d",ge_gettick()-pb->pl._U._Ping.time);
            break;
        case MSG_NEWLEVELBEGIN:
            Printf("<- MSG_NEWLEVELBEGIN. ");
            _netClient.Reset();
            ge_sleep(3000);
            SendMessage(SYS_NET_MAPCHANGE,0,0);
        break;
    }

}

//---------------------------------------------------------------------------------------
void    System::_NPlayLoadLevel(const char* url)
{
    LevelMan lm( url, "imports/");
    
    if(lm.CheckLocally()!=0)
    {
        _netClient.Leave();
        SetMode(DOWNLOADING_MODE);
        if(lm.Download()==0)
        {
            if(_netClient.Reconnect()==0)
            {
                RestoreMode();
                return;
            }
        }
    }
    SendMessage(SYS_NET_LOADMAP, 1, (long)url);
    Printf("<- CANNOT RECONNECT OR DOWNLOAD LEVEL. ");
    SetMode(SET_MODE);
}

//---------------------------------------------------------------------------------------
// wip (work in progress)
void    System::_ShowProgress(long w, long p)
{
#ifdef _DEBUG
    //ge_sleep(32);
#endif //_DEBUG
    static char fname[256] = {0};
    long percentage = 0;
    if(w==-1) //start or stop
    {
        if(p) // filename
        {
            // show progress bar with the file name on it
            strcpy(_status[0].line, (const char*)p);
            strcpy(fname, (const char*)p);
            TextOut(3,ZRED,fname);
            _status[0].font = 2;
            _status[0].color = ZYELLOW;
            _status[0].tout  = 3.3;
            _dummy = ge_gettick();
            this->ShowOutTexts(0);
        }
        else
        {
            // hide progress bar 
            sprintf(_status[0].line ,"%s: %d%%" ,(const char*)p, 100);
            fname[0] = 0;
            _status[0].tout  = 3.3;
            _status[0].line[0] = 0;
            this->ShowOutTexts(0);
        }
    }
    else
    {
        if(p==0)p=10;
        percentage = (w*100)/p;
        if(ge_gettick() - _dummy > 20)
        {
            if(fname[0])
                sprintf(_status[0].line , "%s: %d%%", fname, percentage);
            else
                sprintf(_status[0].line , "%d%%" ,percentage);
            _status[0].tout  = 3.3;
            this->ShowOutTexts(0);
        }
    }
}

void     System::Log(int err, TCHAR* psz,...)
{
    if(err & _tracelevel)
    {
        FILE* pf = fopen(System::_logFile,"ab");
        if(pf)
        {
            va_list args;
            va_start(args, psz);
            static char szBuffer[256];
            unsigned int nBuf = ::_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char), psz, args);
            va_end(args);
            if(err==1)
                fputs("info: ", pf);
            else if(err==2)
                fputs("warn: ", pf);
            else
                fputs("err : ", pf);
            fprintf(pf, szBuffer);
            fclose(pf);
        }
    }
}


long  System::LoadGame(const TCHAR* dllname)
{
    return -1;
}

const TCHAR* System::GetComandLine()
{
    return _comandLine;
}

void    System::SetResourcesSearchDir(const TCHAR* dircomma)
{
    delete[] _resIncludes;
    _resIncludes = new TCHAR[_tcslen(dircomma)+2];
    _tcscpy(_resIncludes, dircomma);
}


// from Dr Dobbs may  2006 Stefan Wörthmüller...
static FILE* GLogFile = 0;
static void LogStackFrame(PVOID FaultAddress, char* enextBP)
{
    char* p,*pBP;
    unsigned int i, x, BpPassed;
    static int CurrentInStackDump = 0;

    BpPassed = (enextBP != 0);
    if(enextBP)
    {
        _asm mov    enextBP,eBp    
    }
    else
        fprintf(GLogFile,"\n Faulr occured at $ADDRESS:%08LX\n",(int)FaultAddress);
    for(i=0; enextBP && i<100;i++)
    {
        pBP = enextBP;
        enextBP = *(char**)pBP;
        p = pBP + 8;

        fprintf(GLogFile, "        with ");
        for(x=0; p < enextBP && x<20; p++, x++)
        {
            fprintf(GLogFile, "%02X ",*(unsigned char*)p);
        }
        fprintf(GLogFile, "\n\n");
        if(i == 1 && !BpPassed)
        {
            fprintf(GLogFile, "        Fault Occured Here:\n");
        }
        fprintf(GLogFile, "*** %2d called from $ADDRESS:%08LX\n", i, *(char**)(pBP+4));
        if(*(char**)(pBP+4)==0)
            break;
    }
}


LONG Win32ExceptionHandler(struct _EXCEPTION_POINTERS* ExInfo)
{
    char *FaulEx="";
    switch(ExInfo->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            FaulEx="ACCESS VIOLATION";
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            FaulEx="DATATYPE MISALIGNMENT";
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            FaulEx="FLT DIVIDE BY ZERO";
            break;
    }

    int     wsfault = ExInfo->ExceptionRecord->ExceptionCode;
    PVOID   CodeAddress = ExInfo->ExceptionRecord->ExceptionAddress;
    GLogFile = ::fopen("exception_log.txt","w");
    if(GLogFile)
    {
        fprintf(GLogFile, "****************************************************\n");
        fprintf(GLogFile, "                   FAULT \n");
        fprintf(GLogFile, "****************************************************\n");
        fprintf(GLogFile, "Address %0X8\n", (int)CodeAddress);
        fprintf(GLogFile, "    Flags %0X8\n", (int)ExInfo->ExceptionRecord->ExceptionFlags);
        LogStackFrame(CodeAddress, (char*)ExInfo->ContextRecord->Ebp);
        ::fclose(GLogFile);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

//---------------------------------------------------------------------------------------
int    System::Run()
{
    MSG		msg		    = {0};
    DWORD   dwr         = 32;
    REAL    acctimer    = 0;
    
   _sysData._pSystem    = this;
   this->_Dispatch(this, SYS_START, (const char*)this->_comandLine, &_sysData);
    try{
        _LOG(LOG_INFO, "Start Spinning() \r\n");
        while(dwr--)
            ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        while(msg.message != WM_QUIT)
        {
            dwr = _ReadWinMsg(&msg); 
            if(dwr & OS_TOGGLESCR)
            {
                _keySys.Unaquire();
                dwr = !_rndSys.s_rndStruct.bFullScreen;
                if(RET_NOT_PROC == this->_Dispatch(this, SYS_TOGLESCRREN, dwr, &_sysData))
                {
                    _OnSwitchMode();
                }
                continue;
            }

            if(dwr & OS_ESCAPE)
            {
                _keySys.Unaquire();
                if(RET_NOT_PROC==this->_Dispatch(this, SYS_ESCAPE, Zero, &_sysData))
                {
                    OnEscape();
                }
                continue;
            }

            if(dwr & OS_EXITAPP)
            {
                _keySys.Unaquire();
                if(RET_NOT_PROC==this->_Dispatch(this, SYS_CLOSEAPP, Zero, &_sysData))
                {
                    Stop();
                    break;
                }
                continue;
            }
            Spin(acctimer);
	    }
        _keySys.Unaquire();
        this->_Dispatch(this, SYS_EXIT, Zero, &_sysData);
        
    }catch(SquirrelError& e)
    {
        _tcscpy(_lastError, e.desc);
        _LOG(LOG_ERROR, "Script: %s \r\n", e.desc);
        _Clean();
        return 1;
    }
    

#ifndef _DEBUG
    catch(...)
    {
        TRACEX("Unknown exception");
        _LOG(LOG_ERROR, "Unknown exception in main Spin()\r\n");
    }
#endif //
    _Clean();
    return 0;
}

//---------------------------------------------------------------------------------------
//http://www.gaffer.org/articles/
int    System::_Timers(REAL& acc)
{
    _sysData._fps       = _timer.Tick(60);
    _sysData._ticktime =  _physycsTime;//
    _sysData._curtime  += _sysData._ticktime;
    _accTime += _timer.GetDeltaTime();
    return 1;
    if(_accTime > _physycsTime)
    {
        _accTime = 0;
        //_accTime -= _sysData._ticktime;
        return 1;
    }
    return 0;
}
//---------------------------------------------------------------------------------------
void    System::Spin(REAL& acctimer)
{
    REG DWORD dwr    = 0;
    REG BOOL  update = 0;

    if(this->_Timers(acctimer))
    {
        dwr = this->_Dispatch(this, SYS_ANIMATE, _pCamera, &_sysData);
        ++_sysData._physFrame;//NO_CAM_SPIN

        if(dwr==0xFFFFFFFF)
        {
            Stop();
            return;
        }
        if(!(dwr & NO_CAM_SPIN)) 
        {
            _pCamera->Animate(&_sysData);
        }
        if(!(dwr & NO_IN_SPIN)) 
        {
            _keySys.Spin(&_sysData);
        }
        if(!(dwr & NO_NET_SPIN))
        {
            _netClient.Spin(&_sysData);
        }
        if(this->_DequeueMessages()==FAILURE) // has stopped
        {
            return ;
        }
    }
    this->_Render(dwr);
    ++_sysData._drawFrame;

    if(DOWNLOADING_MODE!=GetMode() && this->_hudMode & HUD_HELP)
    {
        
        if((_sysData._physFrame&0xF) == 0xF)
        {
            if(_status[0].line[0]==0)
            {
                _status[0].color =ZGREEN;
                _status[0].font = 2;
                _status[0].tout = 3.00;
                
                sprintf(_status[0].line,"FPS: %f   TIME: %f   FREQ: %f  CAM: %.2f,%.2f,%.2f", 
                                        _sysData._fps, 
                                        _sysData._ticktime,
                                        1.0/_sysData._ticktime,
                                        _camera._pos.x,
                                        _camera._pos.y,
                                        _camera._pos.z);
            }
        }
    }            
}



long   System::Default(long m, long w, long l)
{
    return System::DefNotifyProc(this,m,w,l);
}

//---------------------------------------------------------------------------------------
long  System::DefNotifyProc(ISystem* ps, long m, long w, long p)
{
    switch(m)
    {
        case SYS_INPUT:
            if(w)
            {
                switch(p)
                {
                    case DIK_F1:
                        if(PSystem->_hudMode & HUD_HELP)
                            PSystem->_hudMode &= ~HUD_HELP;
                        else
                            PSystem->_hudMode |= HUD_HELP;
                        break;
                    case DIK_GRAVE:
                        if(PSystem->_hudMode & HUD_TRACE)
                            PSystem->_hudMode &= ~HUD_TRACE;
                        else
                            PSystem->_hudMode |= HUD_TRACE;
                        break;
                    case DIK_TAB:
                        if(PSystem->_hudMode & HUD_CONSOLE)
                            PSystem->_hudMode &= ~HUD_CONSOLE;
                        else
                            PSystem->_hudMode |= HUD_CONSOLE;
                        break;
                }
            }
            break;
        case SYS_RENDER_2D:
            PSystem->_uiMan.Paint((SystemData*)p);
            if(PSystem->_hudMode & HUD_TRACE)
            {
                PSystem->ShowOutTexts();
            }
            break;
        case SYS_PROGRESS:
            PSystem->_ShowProgress(w,p);
            return 0;
        case SYS_PROGRESSPAINT:
            if(PSystem->GetMode() == DOWNLOADING_MODE)
            {
                Irender* r = PSystem->Render();

                PSystem->_keySys.Spin(&PSystem->_sysData);
                r->Clear();
                    PSystem->_uiMan.Paint(&PSystem->_sysData);
                    PSystem->_ShowProgress(w,p);
                r->Swap();

                DWORD pm = PSystem->PumpMessage();

                if(OS_ESCAPE & pm || OS_EXITAPP & pm) // escape was pressed
                {
                    PSystem->OnEscape();
                    PSystem->TextOut(3, "Interrupted by user...");
                    return -1; // break download
                }
                return 0;
            }
            return -1;
    }
    return 0;
}



void System::Delete(Vtx* pvx)
{
    delete[] pvx;
}

Vtx*  System::New(int count)
{
    return new Vtx[count];
}


void    System::RegisterDestructor(pdestruct pd, void* pObj)
{
    if(pd) // [object] = ptr_2_destructor
        _destructs[(long)pObj] = pd;
    else
	{
		std::map<long, pdestruct>::iterator fi = _destructs.find((long)pObj);
		if(fi != _destructs.end())
			_destructs.erase(fi);
	}
}

System::~System()
{ 
	if(_destructs.size())
	{
		map<long, pdestruct>::iterator pfn = _destructs.begin();
		for(; pfn!=_destructs.end(); pfn++)
		{ 
			if(pfn->first && pfn->second)
				pfn->second((void*)pfn->first, 0);
		}
	}
    _Clean();
    _mode       = SET_MODE;
    _prevMode   = SET_MODE;
    delete    _scrSysProc;
    TRACEX("System::~System(%X)\r\n",this);
    PS = 0;
};


void System::_InitalizeFromInterior(Irender* prender)
{

    _CreateRender(prender);

    _sysData._pSystem = this;
    _uiMan.Create(&_rndSys.s_rndStruct);
    _CreateSound();
    _CreateInput();

    _uiMan.Paint(&_sysData);

    _camera.SetPos(V0,V0);
    _camera.SetFarAndFov(_rndSys.s_rndStruct.fFar, _rndSys.s_rndStruct.hFov);
    _pCamera = &_camera;

    SetMode(SET_MODE);

}

void System::Interrupt()
{
    ;
}


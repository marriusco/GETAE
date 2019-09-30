//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "OpenglRender.h"

WORD  OpenglRender::_gamma[3][256];
static BOOL CursorOn = 0;
//-------------------------------------------------------------------------------------------------------
//    creates the window
BOOL  OpenglRender::WndCreate()
{

    ::memset(&_defDevMode,0,sizeof(DEVMODE));

	HWND hdsktop = ::GetDesktopWindow();                // read and save curren dev Mode
    HDC hdc      = ::GetDC(hdsktop);
        _defDevMode.dmPelsWidth         = ::GetDeviceCaps(hdc, HORZRES);
        _defDevMode.dmPelsHeight        = ::GetDeviceCaps(hdc, VERTRES);
        _defDevMode.dmBitsPerPel        = ::GetDeviceCaps(hdc, BITSPIXEL);
        _defDevMode.dmDisplayFrequency  = ::GetDeviceCaps(hdc, VREFRESH);
		GetDeviceGammaRamp(hdc, _gamma);
    ::ReleaseDC(hdsktop,hdc);

    _appHinst        = (HINSTANCE)_rnds->hInstance;
    WNDCLASS wc      = {0};
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.hInstance     = (HINSTANCE)_rnds->hInstance;
    wc.hCursor       = ::LoadCursor(NULL,  IDC_CROSS );
    wc.lpszClassName = "_GETAE_";
	_hi              = ::LoadIcon((HINSTANCE)_rnds->hInstance, MAKEINTRESOURCE(101));
    
    RegisterClass(&wc);
    if(_rnds->bFullScreen){
        if(0 == GoFullScreen()){
            Detach();
            DestroyWnd();
            GoWindowedMode();
        }
        else{
			_rnds->bFullScreen = TRUE;
            _bFullScr           = 1L;
        }
    }else{
        _bFullScr           = 0L;
		_rnds->bFullScreen = FALSE;
        GoWindowedMode();
    }
    Attach(GetDC(_hWnd));
    ShowWindow(_hWnd,SW_SHOW);
    SetForegroundWindow(_hWnd);
    SetFocus(_hWnd);

    // trigger a WM_SIZE
    RECT rt = {0};
    GetWindowRect(_hWnd,&rt);
    rt.bottom-=1;
	MoveWindow(_hWnd,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,FALSE);
    rt.bottom+=1;
	MoveWindow(_hWnd,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,TRUE);

    /*
    if(_rnds->wndParent==0) // let the contrller app to hide/show the cursor
	    ShowCursor(0);
    */
	//if(_glAddSwapHintWIn)
	//	_glAddSwapHintWIn(0,0,rt.right-rt.left,rt.bottom-rt.top);

    return (_hWnd !=0);
}

//-------------------------------------------------------------------------------------------------------
// goes in full screen mode
long OpenglRender::DevModeFullScreen(BOOL bfull )
{
	if(bfull){
		DWORD dwAttr        = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		DEVMODE devEnum	    = {0};
		devEnum.dmFields    = dwAttr;

		for (int i = 0; EnumDisplaySettings(NULL, i, &devEnum) ; i++){
		    if ((devEnum.dmFields & dwAttr) == dwAttr                        && 
                devEnum.dmPelsWidth      == _rnds->dmPelsWidth  &&
                devEnum.dmPelsHeight     == _rnds->dmPelsHeight &&
                devEnum.dmBitsPerPel     == _rnds->dmBitsPerPel)
		    {
				if( _rnds->dmDisplayFrequency ){
					if(devEnum.dmDisplayFrequency  != _rnds->dmDisplayFrequency)
						continue;
				}

				if(DISP_CHANGE_SUCCESSFUL==ChangeDisplaySettings(&devEnum, CDS_TEST)){
					ChangeDisplaySettings(&devEnum, CDS_RESET);	
					return 1l;
				}else{
					MessageBox(0, "Cannot change display settings","ZalEngine",MB_ICONHAND|MB_TASKMODAL|MB_OK);
				}
                break;
		    }
		}
	}
	GoInitialDevMode();
    return 0L;
}

//-------------------------------------------------------------------------------------------------------
void OpenglRender::DestroyWnd()
{
    _pengine = 0;
    SetDeviceGammaRamp(_hDC, _gamma);
	Sleep(256);
    if(_bFullScr)
        DevModeFullScreen(FALSE);
    if(_rnds->hideCurosr)
        while (ShowCursor (FALSE) >= 0);
    //if(_rnds->clipCursor)
        ClipCursor(0);

    if (_hPall)        DeleteObject(_hPall);
    if(_hDC)           ReleaseDC(_hWnd, _hDC);
    if(OpenglRender::_externalWnd==0)
    {
        if(_hWnd)        ::DestroyWindow(_hWnd);
        UnregisterClass("_GETAE_",_appHinst);
        _hWnd   = 0;    
    }
    _hDC    = 0;    _hPall  = 0;
}

//-------------------------------------------------------------------------------------------------------
void OpenglRender::SetWndMinimizeFlag(BOOL bEnable)
{
}

//-------------------------------------------------------------------------------------------------------
// switches to full screen mode
long OpenglRender::GoFullScreen()
{
    DWORD  dwExStyle = WS_EX_APPWINDOW;
    DWORD  dwStyle   = WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
    
    if(!DevModeFullScreen()){
        assert(0);
        return 0;
    }
    _hWnd = CreateWindowEx(dwExStyle,"_GETAE_",
                           "", dwStyle ,0, 0,
                           _rnds->dmPelsWidth,
                           _rnds->dmPelsHeight,
                           NULL,NULL,
                           _appHinst,
                           NULL);
    
    if(_hWnd){
        _destroyed =0;
        _hDC = GetDC(_hWnd);
        return 1L;
    }
    return 0L;
}

//-------------------------------------------------------------------------------------------------------
// switches to window mode
long OpenglRender::GoWindowedMode()
{
    DWORD dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle=WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	if(_rnds->wndStyle)
    {
		dwStyle   = _rnds->wndStyle;
		dwExStyle = 0;
	}
    _hWnd = CreateWindowEx(dwExStyle,"_GETAE_",
                           "", dwStyle ,0, 
						   0,//GetSystemMetrics(SM_CYSCREEN)-_rnds->DevMode.dmPelsHeight,
                           _rnds->dmPelsWidth ,
                           _rnds->dmPelsHeight,
                           (HWND)_rnds->wndParent,
						   NULL,
                           _appHinst,
                           NULL);
    if(_hWnd){
	    _destroyed = 0;
		SendMessage(_hWnd, WM_SETICON,ICON_BIG, (long)(_hi) );
		SendMessage(_hWnd, WM_SETICON,ICON_SMALL, (long)(_hi));
        _hDC = GetDC(_hWnd);
        return 1L;
    }
    return 0L;
}

//-------------------------------------------------------------------------------------------------------
// toggles the mode
HWND OpenglRender::SwMode()
{
    if(_bFullScr = !_bFullScr){
        GetWindowRect(_hWnd, &_rectWnd);
        if (DevModeFullScreen()){
            SetWindowLong(_hWnd,GWL_STYLE,WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
            ShowWindow(_hWnd, SW_SHOWMAXIMIZED);
        }
        else{
            _bFullScr = 0L;
		}
    }
    else{
        ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
        SetWindowLong(_hWnd,GWL_STYLE,WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
        ShowWindow(_hWnd,SW_RESTORE);

        SetWindowPos(_hWnd,HWND_TOP,_rectWnd.left,_rectWnd.top,_rectWnd.right-_rectWnd.left, _rectWnd.bottom-_rectWnd.top,SWP_SHOWWINDOW);
		
        SendMessage(_hWnd, WM_SETICON,ICON_BIG, (long)(_hi) );
		SendMessage(_hWnd, WM_SETICON,ICON_SMALL, (long)(_hi));

    }
    _rnds->bFullScreen=_bFullScr;
    return _hWnd ;
}

//-------------------------------------------------------------------------------------------------------
// Window Proc
LRESULT CALLBACK OpenglRender::WndProc(HWND hWnd, UINT uMsg, WPARAM wP, LPARAM lP)
{
    static RECT rt;
    static int loops=0;
    long processed = 0;

    if(uMsg >= WM_USER+4000 && uMsg <= WM_USER+4100)
    {
        return _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);           
    }

    switch (uMsg)
    {
        case WM_CREATE:
            PostMessage(hWnd, WM_OPENGLNOTY,-1,0);
            break;
        case WM_ACTIVATEAPP:
            if(_pengine)
            {
				processed = _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);
			}
            _rnds->bIsActivated=wP;
			if(wP != 0)
            {
				if(_bFullScr)
                {
					DevModeFullScreen();
					SetCursorPos(GetSystemMetrics(SM_CXSCREEN)/2,
								 GetSystemMetrics(SM_CYSCREEN)/2);
				}
				ShowWindow(hWnd, SW_RESTORE);
                if(_rnds->hideCurosr)
                    while (ShowCursor (FALSE) >= 0);

                if(GetCapture()!=hWnd)
                {
                    GetWindowRect(hWnd,&rt);
                    SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
			        InflateRect(&rt,-145,-145);
                    if(_rnds->hideCurosr)
                        while (ShowCursor (FALSE) >= 0);
                    //if(_rnds->clipCursor)
                        ClipCursor(&rt);
                    if(GetCapture()!=hWnd && _rnds->bsetCapture)
                        SetCapture(hWnd);
                }
            	UpdateWindow(hWnd);
			}
            else // minimize it when deactivated
			{
                if(_rnds->hideCurosr)
                    while (ShowCursor (TRUE) < 0);
                if(GetCapture()==hWnd)
                    ReleaseCapture();

                if (_bFullScr){
					DevModeFullScreen(FALSE);
				}
#ifndef _DEBUG
				ShowWindow(hWnd, SW_MINIMIZE);
#endif //
			}
			return 0;
        case WM_SYSCOMMAND:
            if(wP == SC_SCREENSAVE || wP == SC_MONITORPOWER)
                return 0;
            break;
		case WM_SETFOCUS:
            if(_pengine)
                processed = _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);           
            if(!processed)
            {
                GetWindowRect(hWnd,&rt);
                SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
			    InflateRect(&rt,-145,-145);
                if(_rnds->hideCurosr)
                    while (ShowCursor (FALSE) >= 0);
                //if(_rnds->clipCursor)
                    ClipCursor(&rt);
                if(GetCapture()!=hWnd && _rnds->bsetCapture)
                    SetCapture(hWnd);
            }
           
			break;
		case WM_KILLFOCUS:
            if(_pengine)
                _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);
            if(!processed)
            {
                if(_rnds->hideCurosr)
                    while (ShowCursor (TRUE) < 0);
                //if(_rnds->clipCursor)
                    ClipCursor(0);
                if(GetCapture()==hWnd)
                    ReleaseCapture();
            }
			break;
        case WM_CLOSE:
            if(GetCapture()==hWnd)
                ReleaseCapture();
            PostQuitMessage(0);
	        return 0;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if(GetCapture()!=hWnd)
            {
                GetWindowRect(hWnd,&rt);
                SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
			    InflateRect(&rt,-145,-145);
                if(_rnds->hideCurosr)
                    while (ShowCursor (FALSE) >= 0);
                //if(_rnds->clipCursor)
                    ClipCursor(&rt);
                if(GetCapture()!=hWnd && _rnds->bsetCapture)
                    SetCapture(hWnd);
                SetFocus(hWnd);
            }

        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
    	case WM_MOUSEMOVE:
        if(_pengine)
			    _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
            if(_pengine)
			    _pengine->SendMessage((long)WM_KEYDOWN,(long)wP,(long)lP);
            return 0;
        case WM_SIZE:
			if(_hWnd)
			{
				GetWindowRect(_hWnd, &_rectWnd);
				(Perspective((REAL)LOWORD( lP), (REAL) HIWORD(lP)));
                if(_pengine)
			        _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);
			}
            return 0;
        case WM_PALETTECHANGED:
            (PaletteChanged(wP));
            return 0;
        case WM_QUERYNEWPALETTE:
            (QueryNewPalette());
            return 0;
		case WM_OPENGLNOTY:
            if(wP==-1)
            {
                if(GetFocus() == hWnd)
                {
                    GetWindowRect(hWnd,&rt);
                    SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
		    	    InflateRect(&rt,-145,-145);
                    if(_rnds->hideCurosr)
                        while (ShowCursor (FALSE) >= 0);
                    if(_rnds->clipCursor)
                        ClipCursor(&rt);
                    if(_rnds->bsetCapture)
                        SetCapture(hWnd);
                }
                return 0;
            }
            if(_pengine)
			    _pengine->SendMessage((long)uMsg,(long)wP,(long)lP);
            if(wP == WM_STORE_TEXTURE)
                return (GenTextureMsg(lP));
            else if(wP == WM_DELETE_TEXTURE)
                DeleteTextureMsg(lP);

			return 0;
    }
    return DefWindowProc(hWnd,uMsg,wP,lP);
}

//-------------------------------------------------------------------------------------------------------
// attached the opengl hrc's and pixell stuff
long OpenglRender::Attach(HDC hDC, int bPP, int backbPP )
{
    static PIXELFORMATDESCRIPTOR pfd = {0};

    
    _hDC         = hDC;                                         // copy of hdc
    pfd.nSize    = sizeof(PIXELFORMATDESCRIPTOR);			
    pfd.nVersion = 1;									
														
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask     = PFD_MAIN_PLANE;					
    pfd.iPixelType      = PFD_TYPE_RGBA;
    pfd.iLayerType      = PFD_MAIN_PLANE;
    pfd.cColorBits      = _rnds->dmBitsPerPel;
    pfd.cDepthBits      = _rnds->pfDepthBits;
    pfd.cStencilBits    = _rnds->pfStencilBits;
    pfd.cAccumBits      = 0;									

    GLuint pixFmt = ChoosePixelFormat(_hDC, &pfd);
    if(pixFmt){
        if(SetPixelFormat(_hDC,pixFmt,&pfd)){
            SetupPalette();
            _hRC = wglCreateContext(_hDC);
            if(_hRC){
                if(wglMakeCurrent(_hDC,_hRC)){
                    return 1L;
                }
            }
        }
    }
    assert(0);
    return 0L;
}

//-------------------------------------------------------------------------------------------------------
// detaches the opengl from window hdc
void OpenglRender::Detach()
{
    if (_hPall)
        ::DeleteObject(_hPall);
    _hPall = 0;
    if (!wglMakeCurrent(NULL,NULL))
        assert(0);
    if (!wglDeleteContext(_hRC))
        assert(0);
    _hRC = 0;
}

//-------------------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::SetupPalette
void OpenglRender::SetupPalette()
{
    PIXELFORMATDESCRIPTOR pfd = {0};
    LOGPALETTE* pPal          = 0;
    int paletteSize           = 0;
    int pixFmt                = GetPixelFormat(_hDC);
    int rM, gM, bM;
    

    ::DescribePixelFormat(_hDC, pixFmt, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (pfd.dwFlags & PFD_NEED_PALETTE)
       paletteSize = 1 << pfd.cColorBits;
    else
        return;
    pPal = (LOGPALETTE*)LocalAlloc(LPTR, sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = (short)paletteSize;
    rM  = (1 << pfd.cRedBits)   - 1;
    gM  = (1 << pfd.cGreenBits) - 1;
    bM  = (1 << pfd.cBlueBits)  - 1;
    
    for (int i=0; i<paletteSize; ++i)
    {
        pPal->palPalEntry[i].peRed   = (BYTE)((((i >> pfd.cRedShift)   & rM) * 255) / rM);
        pPal->palPalEntry[i].peGreen = (BYTE)((((i >> pfd.cGreenShift) & gM) * 255) / gM);
        pPal->palPalEntry[i].peBlue  = (BYTE)((((i >> pfd.cBlueShift)  & bM) * 255) / bM);
        pPal->palPalEntry[i].peFlags = 0;
    }
    _hPall = CreatePalette(pPal);
    ::LocalFree(pPal);
    if (_hPall)
    {
        DeleteObject(SelectPalette(_hDC, _hPall, FALSE));
        RealizePalette(_hDC);
    }
}

//-------------------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::QueryNewPalette
long OpenglRender::QueryNewPalette()
{
    if (_hPall){
        UnrealizeObject(_hPall);
        SelectPalette(_hDC, _hPall, FALSE);
        RealizePalette(_hDC);
        return 1L;
    }
    return 0L;
}

//-------------------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::PaletteChanged
void OpenglRender::PaletteChanged(WPARAM wP)
{
    if (_hPall && (HWND)wP != _hWnd){
        UnrealizeObject(_hPall);
        SelectPalette(_hDC, _hPall, FALSE);
        RealizePalette(_hDC);
    }
}

//-------------------------------------------------------------------------------------------------------
//    MEMBER:	  OpenglRender::Perspective
void OpenglRender::Perspective(REAL w, REAL h)
{
    if (h<=1.0f) h = 1.0f;
    glViewport(0,0,(int)w, (int)h);
    _rnds->nWidth = (int)w;
    _rnds->nHeight = (int)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    _rnds->fAspect = (REAL)w /(REAL) h;
    _rnds->fHeight = (REAL)(tan(G2R(_rnds->hFov/2.0f)) * (_rnds->fNear)) * 1.90f;
    _rnds->fWidth = _rnds->fAspect * _rnds->fHeight;
    gluPerspective(_rnds->hFov, _rnds->fAspect, _rnds->fNear, _rnds->fFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


//-------------------------------------------------------------------------------------------------------
//    MEMBER:	  GoInitialDevMode()
void OpenglRender::GoInitialDevMode()
{
	DWORD dwAttr        = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	DEVMODE devEnum	    = {0};
	devEnum.dmFields    = dwAttr;

	for (int i = 0; EnumDisplaySettings(NULL, i, &devEnum) ; i++){
        if(_defDevMode.dmDisplayFrequency){
		    if ((devEnum.dmFields & dwAttr) == dwAttr                   && 
			    devEnum.dmPelsWidth         == _defDevMode.dmPelsWidth  &&
			    devEnum.dmPelsHeight        == _defDevMode.dmPelsHeight &&
			    devEnum.dmBitsPerPel        == _defDevMode.dmBitsPerPel &&
			    devEnum.dmDisplayFrequency  == _defDevMode.dmDisplayFrequency)
		    {
			    ChangeDisplaySettings(&devEnum, CDS_RESET);	
			    break;
		    }
        }
        else{
                if ((devEnum.dmFields & dwAttr) == dwAttr                   && 
			    devEnum.dmPelsWidth             == _defDevMode.dmPelsWidth  &&
			    devEnum.dmPelsHeight            == _defDevMode.dmPelsHeight &&
			    devEnum.dmBitsPerPel            == _defDevMode.dmBitsPerPel)
		    {
			    ChangeDisplaySettings(&devEnum, CDS_RESET);	
			    break;
		    }
        }
	}
}

void    OpenglRender::SetGamma(const REAL* gamma, const REAL* contrast, const int* ilum)
{
    REAL   angle          = PI/256.f;
    REAL   sinfval, start = 0.f;
    WORD   gammaarr[3][256];
    REAL   contrastl[3];

    for(int i=0;i<256;i++)
    {
        sinfval = (REAL)sin(start);
        start   += angle;

		contrastl[0] = (i * contrast[0]);
		contrastl[1] = (i * contrast[1]);
		contrastl[2] = (i * contrast[2]);

        gammaarr[0][i] = ilum[0] + /*i*/ + (sinfval * gamma[0]) + contrastl[0];
        gammaarr[1][i] = ilum[1] + /*i*/ + (sinfval * gamma[1]) + contrastl[1];
        gammaarr[2][i] = ilum[2] + /*i*/ + (sinfval * gamma[2]) + contrastl[2];

        CLAMPVAL(gammaarr[0][i],0,255);
        CLAMPVAL(gammaarr[1][i],0,255);
        CLAMPVAL(gammaarr[2][i],0,255);

        gammaarr[0][i]<<=8;
        gammaarr[1][i]<<=8;
        gammaarr[2][i]<<=8;
    }

	BOOL b =  SetDeviceGammaRamp(_hDC, gammaarr);
	if(FALSE == b)
	{
		DWORD dw = GetLastError();
	}
}



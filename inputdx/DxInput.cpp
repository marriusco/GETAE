// DxInput.cpp: implementation of the DxInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_isystem.h"
#include "DxInput.h"
#include "devices.h"

#ifdef MG_WIN32
const GUID IID_IDirectInput8A = { 0xBF798030,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00 };
#endif //

//---------------------------------------------------------------------------------------
DxInput::DxInput()
{
}

//---------------------------------------------------------------------------------------
BOOL DxInput::Create(HWND hwnd,
                     HINSTANCE appInstance,
                     BOOL isExclusive ,
                     DWORD flags )
{
    _pKeyBoard = NULL;
    _pMouse    = NULL;
    _pJoy	   = NULL;
    
#ifdef  WIN32
#ifndef _USE_DXLIB_

	HMODULE hm = LoadLibrary("dinput8.dll");
	if(hm < (HMODULE)HINSTANCE_ERROR)
	{
		gErrLog = "Error: Plugin requires Direct X 8";
		return FALSE;
	}

	DIPRC	Dic = (DIPRC)GetProcAddress(hm, "DirectInput8Create");

	if(0 == Dic)
	{
		gErrLog = "Error: Plugin requires Direct X 8";
		return FALSE;
	}
	
    HRESULT hr = (Dic)(appInstance,DIRECTINPUT_VERSION,IID_IDirectInput8, (void **)&m_pDI,NULL);
#endif //
#endif //

#if DIRECTINPUT_HEADER_VERSION >=0x0800

//adsfasdfasdf;
        HRESULT hr = DirectInput8Create(appInstance,
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(void **)&m_pDI,
										NULL);
    
    #else
        HRESULT hr =DirectInputCreate(appInstance,
										DIRECTINPUT_VERSION,
										(IDirectInput**)&m_pDI,0);
#endif//DX_8



	if(FAILED(hr))
	{
		return FALSE;
	}
    
    if (flags & IS_USEKEYBOARD)//IS_USEMOUSE
    {
        _pKeyBoard = new CKeys((IDirectInput*)m_pDI, hwnd);
		assert(_pKeyBoard);
    }
    if (flags & IS_USEMOUSE)
    {
        _pMouse = new CMouse((IDirectInput*)m_pDI, hwnd, isExclusive);
		assert(_pMouse);
    }
    if (flags & IS_USEJOYSTICK)
    {
        _pJoy = new Joystick((IDirectInput*)m_pDI, appInstance);
		assert(_pJoy);
    }
    return TRUE;
}

//---------------------------------------------------------------------------------------
//    MEMBER:	  DxInput::Destroy
//    description: 
BOOL DxInput::Destroy()
{
    UnacquireAll();
    if (_pKeyBoard)
    {
		delete _pKeyBoard;
		_pKeyBoard = NULL;
    }
    
    if (_pMouse)
    {
		delete _pMouse;
		_pMouse = NULL;
    }
    
    if (_pJoy)
    {
		delete _pJoy;
		_pJoy = NULL;
    }
    if((IDirectInput*)m_pDI)
	{
		if (FAILED(((IDirectInput*)m_pDI)->Release()))
		{
			return FALSE;
		}
	}
    return TRUE;
}




//---------------------------------------------------------------------------------------
//    MEMBER:	  DxInput::UnacquireAll
//    description: 
void DxInput::UnacquireAll(BOOL b)
{
	if (_pKeyBoard)
		_pKeyBoard->Unacquire(b);
	if (_pMouse)
		_pMouse->Unacquire(b);
	if (_pJoy)
		_pJoy->Unacquire(b);
}

BOOL DxInput::AcquireAll()
{
    REG BOOL b = FALSE;
    _pKeyBoard ? (b |= _pKeyBoard->Acquire()):0;
    _pMouse    ? (b |= _pMouse->Acquire())	 :0;
    _pJoy	   ? (b |= _pJoy->Acquire())	 :0;
    return b;
}

	
void  DxInput::Update(SystemData* pd)
{
    _pKeyBoard ?  _pKeyBoard->Update()	: 0;
    _pMouse    ?  _pMouse->Update()		: 0; 
}


long			DxInput::KeyDown(char key) { return (_pKeyBoard ? _pKeyBoard->KeyDown(key):0); }
long			DxInput::KeyUp(char key) { return (_pKeyBoard ? _pKeyBoard->KeyUp(key):0); }
long			DxInput::ButtonDown(int button) { return (_pMouse ? _pMouse->ButtonDown(button):0); }
long			DxInput::ButtonUp(int button) { return (_pMouse ? _pMouse->ButtonUp(button):0); }
void			DxInput::GetMouseMovement(int &dx, int &dy) { if(_pMouse) _pMouse->GetMovement(dx, dy);}
BYTE*		    DxInput::GetKeysStates()const {return (BYTE*)(*_pKeyBoard);}  
MOUSE_INPUT*	DxInput::GetMouseState()const {return (MOUSE_INPUT*)(DIMOUSESTATE*)(*_pMouse);}
CMouse*         DxInput::Mouse()const{ return (_pMouse);}
CKeys*          DxInput::Keys()const{ return (_pKeyBoard);}


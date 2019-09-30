#if !defined(AFX_DEVICES_H__1314A465_FAF4_450F_BF46_2DBECFBCA557__INCLUDED_)
#define AFX_DEVICES_H__1314A465_FAF4_450F_BF46_2DBECFBCA557__INCLUDED_

#include <windows.h>
#include <dinput.h> // install direct X SDK and add the lib and h patch in project
#include <oleauto.h>

//---------------------------------------------------------------------------------------
class CKeys
{
public:
	CKeys(IDirectInput* pDI, HWND hwnd)
	{
        _piDev = 0;
		HRESULT hr;
#ifdef _USE_DXLIB_
		const DIDATAFORMAT* pdf = &c_dfDIKeyboard;
#else
		DIDATAFORMAT* pdf = &my_dfDIKeyboard;
#endif
		if (SUCCEEDED(hr=pDI->CreateDevice(GUID_SysKeyboard, &_piDev, NULL)))
		{
			if (SUCCEEDED(hr=_piDev->SetDataFormat((const DIDATAFORMAT*)pdf)))
			{
				if (SUCCEEDED(hr=_piDev->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
				{
					_piDev->Acquire();
				}
			}
		}
		Clear();
	}

	virtual ~CKeys()
	{
		if (_piDev)
		{
			_piDev->Unacquire();
			_piDev->Release();
		}
	}

	INLN long  KeyDown(char key) 
	{ 
		return (_akeys[key] & 0x80) ? 1 : 0; 
	}

	INLN long  KeyUp(char key) 
	{ 
		return (_akeys[key] & 0x80) ? 1 : 0; 
	}

	INLN BOOL  Update()
	{
		if (FAILED(_piDev->GetDeviceState(sizeof(_akeys), (LPVOID)_akeys)))
		{
			if (FAILED(_piDev->Acquire()))
			{
				return FALSE;
			}
			if (FAILED(_piDev->GetDeviceState(sizeof(_akeys), (LPVOID)_akeys)))
			{
				return FALSE;
			}
		}
		return TRUE;

	}

	INLN void  Clear() 
	{ 
		memset(_akeys, 0, sizeof(_akeys)); 
	}

	INLN BOOL  Acquire()
	{
		Clear();
		HRESULT hr = _piDev->Acquire();
		return S_OK == hr;
	}

	BOOL  Unacquire(BOOL bErase = TRUE)
	{
		if(bErase)
			Clear();
		return (!FAILED(_piDev->Unacquire()));
	}

	INLN operator BYTE*()const{return (BYTE*)_akeys;} ;

private:
	IDirectInputDevice*		_piDev;
	BYTE					_akeys[256];
	static BYTE				_azeroArr[256];
};


//---------------------------------------------------------------------------------------
// CMouse class. from opengl game programming
class CMouse
{
	public:
	CMouse(IDirectInput* pDI, HWND hwnd, BOOL isExclusive = FALSE)
	{
		HRESULT hr;
	#ifdef _USE_DXLIB_
		const DIDATAFORMAT* pdf = &c_dfDIMouse;
	#else
		DIDATAFORMAT* pdf = &my_dfDIMouse;
	#endif
        _piDev = 0;
		if (SUCCEEDED(hr = pDI->CreateDevice(GUID_SysMouse, &_piDev, NULL)))
		{
			if (SUCCEEDED(hr = _piDev->SetDataFormat((const DIDATAFORMAT*)pdf)))
			{
				if (isExclusive)
					Change(hwnd,DISCL_FOREGROUND | DISCL_EXCLUSIVE);
				else
			    	Change(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
				_piDev->Acquire();
			}
		}
	}

	void Change(HWND hwnd, DWORD flags)
	{
		_piDev->SetCooperativeLevel(hwnd, flags);
	}

	virtual ~CMouse()
	{
		if (_piDev)
		{
			_piDev->Unacquire();
			_piDev->Release();
			_piDev=0;
		}
	}
	INLN long  ButtonDown(int button) 
	{ 
		return (_mouseState.rgbButtons[button] & 0x80) ? 1 : 0; 
	}
	INLN long  ButtonUp(int button) 
	{ 
		return (_mouseState.rgbButtons[button] & 0x80) ? 0 : 1; 
	}
	INLN void  GetMovement(int &dx, int &dy) 
	{ 
		dx = _mouseState.lX; dy = _mouseState.lY; 
	}
	INLN BOOL  Update()
	{  
		HRESULT hr = _piDev->GetDeviceState( sizeof(DIMOUSESTATE), &_mouseState );
		if( FAILED(hr)) 
		{
			hr = _piDev->Acquire();
            int ntry = 8;
			while( hr == DIERR_INPUTLOST && --ntry > 0) 
			{
				hr = _piDev->Acquire();
                Sleep(1);
			}
		}
		return (hr ==S_OK);
	}
	INLN BOOL  Acquire()
	{
		return (!FAILED(_piDev->Acquire()));
	}
	BOOL  Unacquire(BOOL bErase = TRUE)
	{
		return (!FAILED(_piDev->Unacquire()));
	}
	INLN operator DIMOUSESTATE*(){return &_mouseState;}
private:
	IDirectInputDevice*  _piDev;
	DIMOUSESTATE		_mouseState;
};

//---------------------------------------------------------------------------------------
// CMouse class. from opengl game programming
class Joystick
{
public:
	Joystick(IDirectInput* pDI, HINSTANCE appInstance){_piDev = 0;}
	virtual ~Joystick()
	{
		if (_piDev)
		{
			_piDev->Unacquire();
			_piDev->Release();
		}
	}

	BOOL  Update()
	{
		return TRUE;
	}

	INLN BOOL  Acquire()
	{
		return (!FAILED(_piDev->Acquire()));
	}

	BOOL  Unacquire(BOOL bErase = TRUE)
	{
		return (!FAILED(_piDev->Unacquire()));
	}


private:
	IDirectInputDevice*  _piDev;
};





#endif // !defined(AFX_DEVICES_H__1314A465_FAF4_450F_BF46_2DBECFBCA557__INCLUDED_)

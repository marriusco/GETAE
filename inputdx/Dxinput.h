// DxInput.h: interface for the DxInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXINPUT_H__C1D9F7E1_A58C_4E19_B3F6_81C457BC4B7B__INCLUDED_)
#define AFX_DXINPUT_H__C1D9F7E1_A58C_4E19_B3F6_81C457BC4B7B__INCLUDED_

#include "_iinput.h"
#include "devices.h"

struct EngineData;
class DxInput : public Iinput  
{
public:
public:

	DxInput() ;
	virtual ~DxInput() 
	{ 
	}
	BOOL  Create(HWND hwnd,
                 HINSTANCE appInstance, 
                 BOOL isExclusive = TRUE, 
				 DWORD flags = IS_USEKEYBOARD|IS_USEMOUSE);
	BOOL  Destroy();
    BOOL  AcquireAll();
	void  UnacquireAll(BOOL b=TRUE);
	void  Update(SystemData* );
    long			KeyDown(char key);
    long			KeyUp(char key);
    long			ButtonDown(int button);
    long			ButtonUp(int button);
    void			GetMouseMovement(int &dx, int &dy);
    BYTE*		    GetKeysStates()const;
    MOUSE_INPUT*    GetMouseState()const;
    CMouse*         Mouse()const;
    CKeys*          Keys()const;

private:
	CKeys*		    _pKeyBoard;
	CMouse*			_pMouse;
	Joystick*		_pJoy;
	void*			m_pDI;   //IDirectInput

};

#endif // !defined(AFX_DXINPUT_H__C1D9F7E1_A58C_4E19_B3F6_81C457BC4B7B__INCLUDED_)

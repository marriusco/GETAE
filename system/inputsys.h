//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __INPUT_SYS_H__
#define __INPUT_SYS_H__

#pragma warning (disable: 4006)
#include "_iinput.h"
#include "_isystem.h"


//--------------------------------------------------------------------------------------
// interface wrapper for input system
class NO_VT InputSys 
{
public:
    InputSys(){
        _loaded = 0;
        _cAppKeys = 0;
        memset(_prevMvs,0,sizeof(_prevMvs));
        memset(_actions,0,sizeof(_actions));
    }
	virtual     ~InputSys(){}
	void    Spin(SystemData* pData);
    int     Create(Iinput* pinput);
    BOOL    Create(const TCHAR* dll, HWND hWnd, HINSTANCE hInst, BOOL exclusive, SystemData* pData);
    int     OneKey(SystemData* pData);
    void    Destroy();
    void    Unaquire();
	BOOL	IsKeyDown(long key){return KEY_DOWN(key, _pDeviceKeys);}
    int*    Moves(){ return _moves; }
    BYTE*   Keys(){return _actions;}
    BYTE    GetPressedKey(){return _actions[KA_ONEKEY];}
	Iinput*	GetIInput(){return _inputPlug.Interface();} 
    void    IgnoreInput(REAL secs){_oneKeyRepeat = secs; };
    void    IgnoreAction(REAL secs){_oneActionRepeat = secs; };
    BOOL    SetMappedKey(const UIInputMap*);
    BOOL    AddMappedKey(const UIInputMap*);
    BOOL    GetMappedKey(UIInputMap*);
    BOOL    RemoveMapedKey(int keyAction);

private:
    void    RemapMouseButs(MOUSE_INPUT* pm, BYTE* pK);
    void    StoreInput(BYTE* pK, MOUSE_INPUT* pMI, DWORD*, REAL time);
    void    _NoPlugReadInput(SystemData* pData);
public:
    REAL              _prevMvs[MR_LAST];
	int               _moves[MR_LAST];
    BYTE              _actions[64];
    BYTE              _lastKey;
    BYTE*		      _pDeviceKeys;
    MOUSE_INPUT*      _pDeviceMouse;       
    PlugInDll<Iinput> _inputPlug;

public:
	static BOOL		  _bMouseMapped;
	static BOOL		  _bDamp;
	static REAL		  _rDampAttn[2];	

    static REAL	      _f,_s,_u,_x,_y;
    static int        _cAppKeys; 
	static DWORD      _inputflags;
    static REAL       _oneKeyRepeat; 
    static REAL       _oneActionRepeat;
	static DWORD      _cahcedPip;
    static BYTE       _downKeys[256]; 
    static BYTE       _keysmap[256]; 
    static UIInputMap _iMap[64];
    static int        _imaps;
    static POINT      _pcur;
    BOOL              _loaded;
};




#endif //__INPUT_SYS_H__

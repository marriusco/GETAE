//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "inputsys.h"
#include "system.h"

//--------------------------------------------------------------------------
#define KP_NUMKEY_MSK    0x0F00000   

//--------------------------------------------------------------------------
//default keys mapping
POINT   InputSys::_pcur;
REAL	InputSys::_f;
REAL	InputSys::_s;
REAL	InputSys::_u;
REAL	InputSys::_x;
REAL	InputSys::_y;
DWORD   InputSys::_inputflags		=	0;
DWORD   InputSys::_cahcedPip		=	0;
REAL    InputSys::_oneKeyRepeat     = 0.2f; 
REAL    InputSys::_oneActionRepeat  = 0.2f; 
BYTE    InputSys::_downKeys[256]    = {0}; 
BYTE    InputSys::_keysmap[256]     = {0}; 
int     InputSys::_cAppKeys; 
BOOL	InputSys::_bDamp			= 0;
BOOL	InputSys::_bMouseMapped		= FALSE;
REAL	InputSys::_rDampAttn[2]		= {1.f,1.f};
//BSTR
UIInputMap InputSys::_iMap[64] =
{
//  code      mouse  //index in this    //name
    {  K_LEFT,      DIK_A,	    -1,          "Slide Left"},
    {  K_RIGHT,     DIK_D,	    -1,          "Slide Right"},
    {  K_UP,        DIK_UP,	    -1,          "Climb/Swim Up"},
    {  K_DOWN,      DIK_X,	    -1,          "Climb/Swim Down"},
    {  K_FRONT,     DIK_W,	     1,          "Go Front"},
    {  K_BACK,      DIK_S,	    -1,          "Go back"},
    {  K_FIRE1,     DIK_RETURN,  0,          "Fire 1"},
    {  K_FIRE2,     DIK_P,	     2,          "Fire 2"},
    {  K_JUMP,      DIK_SPACE,  -1,          "Jump"},
    {  K_CRUNCH,    DIK_C,	    -1,          "Crunch"},
    {  K_NEXTW,     DIK_N,	    -1,          "Next Weapon"},
    {  K_ATTACH,    DIK_K,      -1,          "Use Item"},
    {  K_MINUS1,    -1    ,     -1,           0},
};		



//--------------------------------------------------------------------------
// search for keys in mause member and set the key val in the read buffer
// then let go
void    InputSys::RemapMouseButs(MOUSE_INPUT* pM, BYTE* pK)
{
    UIInputMap* pMap = _iMap;
    
    while(pMap->dikKey != -1)
    {
        if(pMap->mouseBut == -1)
        {
            pMap++;
            continue;
        }
            
        // test if desired button is down
	    if(pM->rgbButtons[pMap->mouseBut] & 0x80)
        {
            // press associated key and let go for key processing
		    pK[pMap->dikKey] |= 0x80;
			_bMouseMapped = TRUE;
	    }
        pMap++;
    }
}

//--------------------------------------------------------------------------
void    InputSys::StoreInput(BYTE* pK, MOUSE_INPUT* pMI, DWORD* pIp, REAL time)
{
    REAL    moves[MR_LAST] = {0};
	int*   pMoves    = _moves;

    CLAMPVAL(time,0.01,0.04);

		
    *pIp   	    = 0;
    pMoves[B_1]=pMI->rgbButtons[0];
    pMoves[B_2]=pMI->rgbButtons[1];
    pMoves[B_3]=pMI->rgbButtons[2];
    pMoves[B_4]=pMI->rgbButtons[3];
    pMoves[B_5]=pMI->rgbButtons[4];
    pMoves[B_6]=pMI->rgbButtons[5];
    pMoves[B_7]=pMI->rgbButtons[6];
    pMoves[B_8]=pMI->rgbButtons[7];

    // read the key buffer and update local buffers
    // read the moving
    if(KEY_DOWN(pK, _iMap[K_FRONT].dikKey))
    {
	    pMoves[M_Z]  = 1;
    }
    else if(KEY_DOWN(pK, _iMap[K_BACK].dikKey))
    {
	    pMoves[M_Z] = -1;
    }

    if(KEY_DOWN(pK, _iMap[K_LEFT].dikKey))
    {
	    pMoves[M_X] = -1;
    }
    else  if(KEY_DOWN(pK, _iMap[K_RIGHT].dikKey))
    {
	    pMoves[M_X] = 1;
    }

    if(KEY_DOWN(pK, _iMap[K_UP].dikKey))
    {
	    pMoves[M_Y] = 1;
    }
    else if(KEY_DOWN(pK, _iMap[K_DOWN].dikKey))
    {
	    pMoves[M_Y] = -1;
    }

    //read the rotations
    pMoves[R_X] = pMI->lX;

    if(pMoves[R_Y] = pMI->lY)
    {
	    if(InputSys::_inputflags & INSYS_INVUPDN)
        {
		    pMoves[R_Y]=-pMoves[R_Y];
        }
    }

    InputSys::_oneActionRepeat -= time;
    memset(_actions,0,sizeof(_actions));
    
    if(_oneActionRepeat < 0)
    {
        InputSys::_oneActionRepeat = 0.2f;

        // fill up the action structure preserving one key at a time
        if(KEY_DOWN(pK, _iMap[K_JUMP].dikKey))
        {
            _actions[KA_JUMP]|=1;
        }

        if(KEY_DOWN(pK, _iMap[K_FIRE2].dikKey))
        {
	        _actions[KA_FIRE2]|=1;
        }

        if(KEY_DOWN(pK, _iMap[K_CRUNCH].dikKey))
        {
	        _actions[KA_CRUNCH]|=1;
        }

        if(KEY_DOWN(pK, _iMap[K_NEXTW].dikKey))
        {
	        _actions[KA_NEXTW]|=1;
        }

        if(KEY_DOWN(pK, _iMap[K_FIRE1].dikKey))
        {
	        _actions[KA_FIRE1]|=1;
        }

	    for(int i = DIK_1; i <= DIK_0; i++)
	    {
		    if(KEY_DOWN(_pDeviceKeys, i))
    	    {
                _actions[KA_NUMKEY] = (i - DIK_1);
                break;
		    }
	    }
    }
}

//--------------------------------------------------------------------------
//    description: 
void InputSys::Spin(SystemData* pData)
{
    if(!_loaded)
    {
        _NoPlugReadInput(pData);
        return;
    }
	BOOL _inDirty = FALSE;
    memset(_moves,0,sizeof(_moves));
    if(_inputPlug->AcquireAll())
    {
        _inputPlug->Update(pData);
		_bMouseMapped = FALSE;
        RemapMouseButs(_pDeviceMouse, _pDeviceKeys);
        DWORD ip;
        StoreInput(_pDeviceKeys, _pDeviceMouse, &ip, pData->_ticktime);
        InputSys::_oneKeyRepeat-=pData->_ticktime;
 
        if(InputSys::_oneKeyRepeat < 0.f)
        {
            InputSys::_oneKeyRepeat = 0.2f;
			OneKey(pData);
        }
    }
    if(pData->_pauseTime)pData->_pauseTime -= pData->_ticktime;
}

//--------------------------------------------------------------------------
int InputSys::OneKey(SystemData* pData)
{
	// increment by DWORD up to 64 not by bytw up to 255
	// test 4 bytes in one shot
    REG DWORD* pddk = (DWORD*)&_downKeys[0];
	REG DWORD* pdw  = (DWORD*)&_pDeviceKeys[0];
    REG DWORD  j;

	for(int i=0; i < 64; i++)
	{
		j = (i << 2);                         // * 4

		if(*pdw)                            // if the DWORD has a value text each byte
		{
			if(_downKeys[j]==0 && KEY_DOWN(_pDeviceKeys, j))
			{
                _downKeys[j]=1;
                PSystem->SendMessage(SYS_INPUT, 1, j);
				break;
			}
            ++j;

			if(_downKeys[j]==0 && KEY_DOWN(_pDeviceKeys, j))
			{
                _downKeys[j]=1;
                PSystem->SendMessage(SYS_INPUT, 1, j);
				break;
			}
            ++j;

			if(_downKeys[j]==0 && KEY_DOWN(_pDeviceKeys, j))
			{
                _downKeys[j]=1;
                PSystem->SendMessage(SYS_INPUT, 1, j);
				break;
			}
            ++j;

			if(_downKeys[j]==0 && KEY_DOWN(_pDeviceKeys, j))
			{
                _downKeys[j]=1;
				PSystem->SendMessage(SYS_INPUT, 1, j);
				break;
			}
		}
        else if(*pddk)
        {
            *pddk = 0;
            PSystem->SendMessage(SYS_INPUT, 0, j);
        }
		++pdw;
        ++pddk;
	}
    return  j;
}

//--------------------------------------------------------------------------
int     InputSys::Create(Iinput* pinput)
{
    memset(_moves,0,sizeof(_moves));
    memset(_downKeys,0,sizeof(_downKeys));
    _loaded = 0;
    _inputPlug.SetIface(pinput);
    _pDeviceKeys = _inputPlug->GetKeysStates();
    _pDeviceMouse = (MOUSE_INPUT*)_inputPlug->GetMouseState();
    _loaded = 1;
    return 0;
}

//--------------------------------------------------------------------------
BOOL   InputSys::Create(const TCHAR* dll, HWND hWnd, HINSTANCE hInst, BOOL exclusive,
                        SystemData* pData)
{
	TRACEX("InputSys::Create\r\n");
    memset(_downKeys,0,sizeof(_downKeys));
    memset(_moves,0,sizeof(_moves));

    SetFocus(hWnd);
    RECT rt; GetWindowRect(hWnd, &rt);
    SetCursorPos(rt.left+(rt.right-rt.left)/2, rt.top+(rt.bottom-rt.top)/2);
    
	////_pCallBack = pCallBack;
    _loaded = 0;
	HINSTANCE hm = LoadLibrary("dinput8.dll");
	if(hm > (HINSTANCE)HINSTANCE_ERROR)
	{
	    FreeLibrary(hm);
        if(_inputPlug.Load(dll))
	    {
		    if(!_inputPlug->Create(hWnd, hInst, exclusive, IS_USEKEYBOARD|IS_USEMOUSE))
		    {
			    Destroy();
			    return FALSE;
		    }

            _pDeviceKeys = _inputPlug->GetKeysStates();
            _pDeviceMouse = _inputPlug->GetMouseState();

    	    pData->_pKeys  = Keys();
	        pData->_pMouse = Moves();
            _loaded = 1;
            return TRUE;
	    }
    }
    PSystem->Printf("Input plugin not found.! Requires DX8 and Up\n");
    PSystem->Printf("                 ...running with basic input\n");
    _LOG(LOG_WARNING,"Cannot load input plugin %s.! (Requires DX8 and up)\n", dll);
    //GetCursorPos(&_pcur);
    _pcur.x = -1;
    _pcur.y = -1;
    pData->_pKeys = _actions;
    pData->_pMouse = (int*)_moves;

	return FALSE;
}

//--------------------------------------------------------------------------
void   InputSys::Unaquire()
{
    if(!_loaded)return;
    if(_inputPlug.IsValid())
        _inputPlug->UnacquireAll();
}

//--------------------------------------------------------------------------
void   InputSys::Destroy()
{
    if(!_loaded)return;
	TRACEX("InputSys::Destroy\r\n");
	
    if(_inputPlug.IsValid())
    {
        _inputPlug->Destroy();
        _inputPlug.Free();
    }
}

//--------------------------------------------------------------------------
void   InputSys::_NoPlugReadInput(SystemData* pData)
{
    static BYTE mappedDiks[]=
    {
        VK_ESCAPE, DIK_ESCAPE,  VK_SPACE,   DIK_SPACE,
        'H',   DIK_H,           
        'F',   DIK_F,           
        'P',   DIK_P,           
        'B',   DIK_B,           
        'h',   DIK_H,           
        'f',   DIK_F,           
        'p',   DIK_P,           
        'b',   DIK_B,           
        '1',   DIK_1,           '2',   DIK_2,        '3',   DIK_3,
        '4',   DIK_4,           '5',   DIK_5,        '6',   DIK_6,
        '7',   DIK_7,           '8',   DIK_8,        '9',   DIK_9,
        'w',   DIK_W,           'a',   DIK_A,        's',   DIK_S,
        'd',   DIK_D,        
        'W',   DIK_W,           'A',   DIK_A,        'S',   DIK_S,
        'D',   DIK_D,           VK_UP,  DIK_UP,       VK_LEFT,   DIK_LEFT,
        VK_OEM_3,   DIK_GRAVE,  VK_TAB,   DIK_TAB,
        VK_F1, DIK_F1,        VK_F10, DIK_F10, 
        VK_RIGHT,   DIK_RIGHT,VK_DOWN,   DIK_DOWN,  VK_F1,   DIK_F1,0,0
    };

    MOUSE_INPUT mi;
    
    memset(_moves,0,sizeof(_moves));
    memset(_keysmap,0,sizeof(_keysmap));
    memset(&mi,0,sizeof(mi));

    _pDeviceKeys   = _keysmap;
    _pDeviceMouse  = (MOUSE_INPUT*)_moves;
    pData->_pKeys = _actions;
    pData->_pMouse = (int*)_moves;
    REAL dx = 0;
	REAL dy = 0;

    mi.rgbButtons[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 0x80 : 0;
    if(mi.rgbButtons[0])
    {
        GetCursorPos(&_pcur);
    }

	if(PSystem->Render()->Hwnd()==GetFocus())
    {
        POINT pct;
	    GetCursorPos(&pct);
        if(_pcur.x==-1)
        {
            _pcur = pct;
        }
	    dx = pct.x  - _pcur.x;
	    dy = pct.y  - _pcur.y;
    
        SetCursorPos(_pcur.x,_pcur.y);
    }
    //else return;
   
    mi.lX = dx ;
    mi.lY = dy ;
    mi.lZ = 0;
    mi.rgbButtons[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? 0x80 : 0;
    mi.rgbButtons[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ? 0x80 : 0;

    BYTE b=0;
    for(int k=0;k<sizeof(mappedDiks);k+=2)
    {
        if(_keysmap[mappedDiks[k+1]]==0)
            _keysmap[mappedDiks[k+1]] = (GetAsyncKeyState(mappedDiks[k]) & 0x8000) ? 0x80 : 0;
        b|=_keysmap[mappedDiks[k+1]];
        if(b)
        {
                mi.lZ = 0;
        }
    }
    _bMouseMapped = FALSE;
    RemapMouseButs(&mi, _pDeviceKeys);
    DWORD ip;
    StoreInput(_keysmap, &mi, &ip, pData->_ticktime);        
    InputSys::_oneKeyRepeat-=pData->_ticktime;
    
    if(b && InputSys::_oneKeyRepeat < 0.f)
    {
        InputSys::_oneKeyRepeat = 0.08f;
		OneKey(pData);              // sense a key that is not mapped on mouse butt
    }
    if(pData->_pauseTime>0)
        pData->_pauseTime -= pData->_ticktime;
    

}


BOOL    InputSys::SetMappedKey(const UIInputMap* setValPh)
{
    UIInputMap* pMap = _iMap;
    while(pMap->dikKey != -1)
    {
        if(pMap->keyAction==setValPh->keyAction)
        {
            ::memcpy(pMap, setValPh, sizeof(UIInputMap));
            return TRUE; //found
        }
        ++pMap;
    }
    return FALSE;//not found
}

BOOL    InputSys::AddMappedKey(const UIInputMap* newKey)
{
    UIInputMap* pMap = _iMap;
    int index = 0;
    while(pMap->dikKey != -1){++pMap;++index;}
    if(index < 64){
        ::memcpy(pMap, newKey, sizeof(UIInputMap));
        ++pMap;
        pMap->keyAction = K_MINUS1; //place end of map values
        pMap->name[0]   = 0;
        pMap->dikKey    = 0;
        pMap->mouseBut  = -1;
    }
    return FALSE; // map is full
}


BOOL    InputSys::GetMappedKey(UIInputMap* retValPh)
{
    UIInputMap* pMap = _iMap;
    while(pMap->dikKey != -1)
    {
        if(pMap->keyAction==retValPh->keyAction)
        {
            ::memcpy(retValPh, pMap, sizeof(UIInputMap));
            return TRUE; //found
        }
        ++pMap;
    }
    return FALSE;//not found
}

BOOL    InputSys::RemoveMapedKey(int keyAction)
{
    BOOL    rv = FALSE;
    UIInputMap* pMap2;
    UIInputMap* pMap = _iMap;
    while(pMap->dikKey != -1)
    {
        if(pMap->keyAction==keyAction)
        {
            pMap2 = pMap;
            ++pMap;
            rv = TRUE;
            break;
        }
        ++pMap;
    }

    while(pMap->dikKey != -1)
    {
        ::memcpy(pMap2++, pMap++, sizeof(UIInputMap));
    }

    return rv;
}

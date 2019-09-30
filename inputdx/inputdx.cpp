//---------------------------------------------------------------------------------------
#include "stdafx.h"
#include "dxinput.h"

//---------------------------------------------------------------------------------------
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//---------------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
    return TRUE;
}


extern "C"
{
    EX_IN Iinput*	CreateInstance()
    {
	    return new DxInput(); 
    }

    EX_IN  void	DestroyInstance(Iinput* pr)
    {
	    delete pr;
    }

    EX_IN const char* GetCatName(void)
    {
        return "input,DirectX";
    }

};

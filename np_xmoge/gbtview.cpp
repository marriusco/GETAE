
#ifndef _NIS_
// gbtview.cpp : Implementation of Cgbtview
#include "stdafx.h"
#include "gbtview.h"
#include ".\gbtview.h"

#pragma message ("building XMOGE.DLL ACTIVEX")


//---------------------------------------------------------------------------------------
// modifyed std library to allow include search path in the script 
char* sq_includes[256] = {
        {"scripts/"},
        {"../"},
        {"../scripts/"},
        0,
};


// Cgbtview
LRESULT Cgbtview::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    TRACEX("X::OnCreate()<-\n");
    USES_CONVERSION;
	_destroing = 0;
	p_scene = new XScene();
	if(!p_scene->Create(0, _AtlBaseModule.m_hInst, m_hWnd))
	{
		delete p_scene;
		p_scene = 0;
	}
    TrysetProxy();
    //LoadLevel(CComBSTR("http://linux/levels200/default.ml"));
    TRACEX("X::OnCreate()->\n");
	return 0;
}

//DllRegisterServer

LRESULT Cgbtview::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE("X::OnDestroy()<- \n");
   _destroing = 1;
   if(p_scene)
   {
		p_scene->Destroy();
		delete p_scene;
   }
   p_scene = 0;
   ATLTRACE("On Destroy() ->\n");
   return 0;
}


LRESULT Cgbtview::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	static DWORD pt= 0;
	
    if(_destroing) return 0;
	if(p_scene)
	{
        /* when there is no callback notify from levelman
        if(p_scene->ThreadActive())
        {
            Sleep(0);
            return 0L;
        }
        */
		DWORD ct = GetTickCount();
        if(p_scene->HasLevel())
        {
		    if(ct - pt > 20)
		    {
			    p_scene ? p_scene->Spin() : 0;
			    pt = ct;
		    }
        }
        else
        {
		    if(ct - pt > 40)
		    {
			    p_scene ? p_scene->Spin() : 0;
			    pt = ct;
		    }
        }
	}
    Sleep(0);    
	return 0L;
}

LRESULT Cgbtview::OnButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(p_scene)
    {
		p_scene->Focus();
    }
	return 0;
}

STDMETHODIMP Cgbtview::LoadLevel(BSTR bsURL)
{
	TRACEX("X::LoadLevel()<-\n");
	if(p_scene)
	{
		USES_CONVERSION;
		_tcscpy(_level, W2CT(bsURL));
		p_scene->ThreadLoad(_level);
		TRACEX("LoadLevel()->\n");
	    return S_OK;
	}
	TRACEX("X::LoadLevel()->\n");
	return E_FAIL;
}

STDMETHODIMP Cgbtview::Leave(void)
{
	return S_OK;
}



STDMETHODIMP Cgbtview::SetProxy(BSTR bsURL, ULONG port)
{
    USES_CONVERSION;
    if(p_scene)
    {
        p_scene->SetProxy(W2CT(bsURL), port);
    }
    return S_OK;
}


void Cgbtview::TrysetProxy()
{
   HKEY  hK = 0;
   if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),0,KEY_QUERY_VALUE, &hK)) 
   {
        TCHAR  sRegStr[128] = {0};
        DWORD  nLen = 128;
        DWORD  t    = 0;

        long ret = RegQueryValueEx(hK, _T("ProxyEnable"), NULL, &t, (LPBYTE)sRegStr, &nLen);
        if(ret == ERROR_SUCCESS) 
        {
            //if(*sRegStr != 0) 
            {
                TCHAR  szPrx[256] = {0};
                DWORD  nPrx = 256;

                if(ERROR_SUCCESS == RegQueryValueEx(hK, _T("ProxyServer"), NULL, &t, (BYTE*)szPrx, (DWORD*)&nPrx)) 
                {
                    szPrx[nPrx] = 0;
                    char* pAfterEq = strchr(szPrx,'=');
                    if(pAfterEq)++pAfterEq;
                    {
                        char* port = strchr(pAfterEq,':');
                        if(port)
                        {
                            *port++ = 0;
                            this->SetProxy(CComBSTR(pAfterEq), ::atol(port));
                        }
                    }
                }
            }
        }
        RegCloseKey( hK );
   }
}

#endif //#ifdef _NIS_

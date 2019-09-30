// gbtview.h : Declaration of the Cgbtview
#pragma once
#ifndef _NIS_

#include "resource.h"       // main symbols
#include <atlctl.h>
#include "Xscene.h"
#include "ShlGuid.h"        // for SID_IBrowser2


extern GUID IID_Igbtview;


//--------------------------------------------------------------------------------------
class LicXMoge
{
protected:
   static BOOL VerifyLicenseKey(BSTR bstr)
   {
      USES_CONVERSION;
      return TRUE;//!lstrcmp(OLE2T(bstr), _T("XMOGE 10 FAKE LICENSE RUNTIME KEY"));
   }

   static BOOL GetLicenseKey(DWORD dwReserved, BSTR* pBstr) 
   {
      USES_CONVERSION;
      *pBstr = SysAllocString( T2OLE(_T("XMOGE 10 FAKE LICENSE RUNTIME KEY"))); 
      return TRUE;
   }

   static BOOL IsLicenseValid() {  return TRUE; }
};

//--------------------------------------------------------------------------------------
// Igbtview
[
	object,
	uuid(BDD9FCFF-2DE0-47C2-8E1E-5A8640E2096E),
	dual,
	helpstring("Igbtview Interface"),
	pointer_default(unique)
]
__interface Igbtview : public IDispatch
{
    [id(1), helpstring("method LoadLevel")] HRESULT LoadLevel([in] BSTR bsURL);
	[id(2), helpstring("method Leave")] HRESULT Leave(void);
    [id(3), helpstring("method SetProxy")] HRESULT SetProxy([in] BSTR bsURL,[in] ULONG port);
};

//--------------------------------------------------------------------------------------
[
	coclass,
	//threading("apartment"),
	vi_progid("xmoge.gbtview"),
	progid("xmoge.gbtview.1"),
	version(1.0),
	uuid("5D32D59F-942D-40AB-8D8C-F78EA015B2E7"),
	helpstring("gbtview Class"),
	support_error_info(Igbtview),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE Cgbtview : 
	public Igbtview,
	public IPersistStreamInitImpl<Cgbtview>,
	public IOleControlImpl<Cgbtview>,
	public IOleObjectImpl<Cgbtview>,
	public IOleInPlaceActiveObjectImpl<Cgbtview>,
	public IViewObjectExImpl<Cgbtview>,
	public IOleInPlaceObjectWindowlessImpl<Cgbtview>,
	public IPersistStorageImpl<Cgbtview>,
	public ISpecifyPropertyPagesImpl<Cgbtview>,
	public IQuickActivateImpl<Cgbtview>,
	public IDataObjectImpl<Cgbtview>,
	public IProvideClassInfo2Impl<&__uuidof(Cgbtview), NULL>,
	public CComControl<Cgbtview>,
    public IObjectSafetyImpl<Cgbtview, INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public CComClassFactory2<LicXMoge>
{
public:

	Cgbtview()
	{
		m_bWindowOnly = TRUE;
        p_scene = 0;
	}

DECLARE_CLASSFACTORY2(LicXMoge)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)


BEGIN_PROP_MAP(Cgbtview)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()
// IClassFactory2
BEGIN_COM_MAP(Cgbtview)
	COM_INTERFACE_ENTRY(Igbtview)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IQuickActivate)
	COM_INTERFACE_ENTRY_IMPL(IPersistStorage)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY_IMPL(IDataObject)
	COM_INTERFACE_ENTRY(IObjectSafety)
	//COM_INTERFACE_ENTRY(ISupportErrorInfo)
	//COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_MSG_MAP(Cgbtview)
    
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonDown)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	CHAIN_MSG_MAP(CComControl<Cgbtview>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// Igbtview
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	STDMETHOD(TranslateAccelerator)(LPMSG pMsg)
	{
		CComPtr<IOleInPlaceActiveObject> spIOleInPlaceActiveObject;
		if(m_spBrowser.p)
		{
			HRESULT hr = m_spBrowser->QueryInterface(&spIOleInPlaceActiveObject);
			if (SUCCEEDED(hr))
				return spIOleInPlaceActiveObject->TranslateAccelerator(pMsg);
		}
		if((pMsg->message == WM_KEYDOWN || 
			pMsg->message == WM_SYSKEYDOWN || 
			pMsg->message == WM_CHAR) &&
			pMsg->wParam == VK_ESCAPE)
		{
			if(p_scene)
				p_scene->Escape();
		}
		return IOleInPlaceActiveObjectImpl<Cgbtview>::TranslateAccelerator(pMsg);;
	}
	CComPtr<IWebBrowser2> m_spBrowser;

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
        ATLASSERT(p_scene == 0);
		return S_OK;
	}

	LRESULT OnPaint(UINT /* uMsg */, WPARAM wParam,	LPARAM /* lParam */, BOOL& /* lResult */)
	{
		BOOL b;
		OnTimer(0,0,0,b);
		return 0;
	};

	virtual HRESULT OnDraw(ATL_DRAWINFO& di)
	{
        //if(p_scene){
        //    p_scene->Spin();
		::SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("XMOGE 0.001");
		::TextOut(di.hdcDraw, di.prcBounds->left + (di.prcBounds->right - di.prcBounds->left) / 2, di.prcBounds->top + (di.prcBounds->bottom - di.prcBounds->top) / 2, pszText, lstrlen(pszText));
        
		return S_OK;
	}
	
	void FinalRelease() 
	{
        if(p_scene){
            p_scene->Destroy();
            delete p_scene;
        }
        p_scene =  0;
	}

    STDMETHOD(IOleInPlaceObject_InPlaceDeactivate)(void)
    {
        return CComControlBase::IOleInPlaceObject_InPlaceDeactivate();
    }

	STDMETHOD(SetClientSite)(IOleClientSite *pClientSite)
	{
		
		HRESULT hr = S_OK;
		CComPtr<IServiceProvider> isp;
		CComPtr<IServiceProvider> isp2;
		CComPtr<IOleControlSite>  site; 

		if(pClientSite)
		{
			hr = pClientSite->QueryInterface(IID_IServiceProvider, reinterpret_cast<void **>(&isp));
			if(SUCCEEDED(hr))
			{
				hr = isp->QueryService(SID_STopLevelBrowser, IID_IServiceProvider, reinterpret_cast<void **>(&isp2));
				if(SUCCEEDED(hr))
				{
					hr = isp2->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void **>(&m_spBrowser));
				}
			}
		}
		return IOleObjectImpl<Cgbtview>::SetClientSite(pClientSite);
	}


    XScene* p_scene;
	UINT	u_timer;//
	string	s_lastlevel;
	string	t_proxy;
	UINT	n_port;
    mutex   _mxx;
    TCHAR   _level[255];
    BOOL    _destroing;
    
    STDMETHOD(LoadLevel)(BSTR bsURL);
	STDMETHOD(Leave)(void);
//---------------------------------------------------------------------------------
//NIS
    static const GUID& GetIID() {static const GUID iid = IID_Igbtview; return iid;}
    STDMETHOD(SetProxy)(BSTR bsURL, ULONG port);

    void TrysetProxy();
};

    
//--------------------------------------------------------------------------------------


#endif //#ifdef _NIS_

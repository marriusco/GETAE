
#include "baseutils.h"
#include "nsxmogePeer.h"
#include "pluginbase.h"
#include <windows.h>
#include "xscene.h"
#include "nsISupportsUtils.h" 
#include "nsIServiceManager.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"
//#include <npruntime.h>


TCHAR* sq_includes[256] = {
                            {_T("./")},
                            {_T("scripts/")},
                            {_T("../")},
                            {_T("../scripts/")},
                            0,
};



static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static NS_DEFINE_IID(kIScriptableIID, NS_IXMOGEPLUGIN_IID);
static NS_DEFINE_IID(kIClassInfoIID, NS_ICLASSINFO_IID);
static NS_DEFINE_IID(kISupportsIID, NS_ISUPPORTS_IID);

HINSTANCE   G_dllInstance;



CNS_Moge::CNS_Moge( NPP instance)
{
    p_scene = 0;
    mRefCnt = 0;

    u_timer = 0;
    mhWnd = NULL;
    mString[0] = '\0';
    _destroing = 0;
    _proxyaddr[0] =0  ;
    _proxyport = 0;

    ResolveProxy(instance);

    TRACEX("CNS_Moge()\n");
}

CNS_Moge::~CNS_Moge()
{
    TRACEX("~CNS_Moge()\n");
    p_scene = 0;
    assert(p_scene==0);
    assert(0==mRefCnt);
}

// AddRef, Release and QueryInterface are common methods and must 
// be implemented for any interface
NS_IMETHODIMP_(nsrefcnt) CNS_Moge::AddRef() 
{ 
    ++mRefCnt; 

    //TRACEX("AddRef is  %d \n",mRefCnt);
    return mRefCnt; 
} 

NS_IMETHODIMP_(nsrefcnt) CNS_Moge::Release() 
{ 
    //TRACEX("releasing %d \n",mRefCnt);
    
    --mRefCnt; 
    if (mRefCnt == 0) 
    { 
        assert(mhWnd == 0);
        delete this;
        return 0; 
    } 
    return mRefCnt; 
} 

// here CNS_Moge should return three interfaces it can be asked for by their iid's
// static casts are necessary to ensure that correct pointer is returned
NS_IMETHODIMP CNS_Moge::QueryInterface(const nsIID& aIID, void** aInstancePtr) 
{ 
    if(!aInstancePtr) 
        return NS_ERROR_NULL_POINTER; 

    if(aIID.Equals(kIScriptableIID)) {
        *aInstancePtr = static_cast<nsIXmogePlugin*>(this); 
        AddRef();
        return NS_OK;
    }

    if(aIID.Equals(kIClassInfoIID)) {
        *aInstancePtr = static_cast<nsIClassInfo*>(this); 
        AddRef();
        return NS_OK;
    }

    if(aIID.Equals(kISupportsIID)) {
        *aInstancePtr = static_cast<nsISupports*>(static_cast<nsIXmogePlugin*>(this)); 
        AddRef();
        return NS_OK;
    }

    return NS_NOINTERFACE; 
}


NS_IMETHODIMP CNS_Moge::LoadLevel(const char *bsURL, PRInt32 *retval)
{
    TRACEX("LoadLevel()\n");
    *retval = 0;
    return LoadLevel(bsURL);
}

/* long Leave (); */
NS_IMETHODIMP CNS_Moge::Leave(PRInt32* retval)
{
    TRACEX("Leave()\n");
    Leave();
    *retval = 0;
    return NS_OK;
}

NS_IMETHODIMP CNS_Moge::SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval)
{
    *_retval = 0;
    this->SetProxy(bsURL, port);
    return NS_OK;
}



static int   _scenes = 0;
NPBool CNS_Moge::CreateScene()
{
    TRACEX("NS::CreateScene()<-\n");
    
    _destroing = 0;
    if(XScene::PXEngine)
    {
        p_scene = XScene::PXEngine;
        p_scene->Destroy();
        delete p_scene;
    }
    p_scene = new XScene();
    _scenes++;
    if(!p_scene->Create(0, G_dllInstance, mhWnd))
    {
        delete p_scene;
        p_scene = 0;
    }
    if(p_scene)
    {
        _lpOldProc = ((WNDPROC)SetWindowLongPtr((mhWnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(PluginWinProc)));

        u_timer   = ::SetTimer(mhWnd,0,10,0);
        AddRef();
        SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);

        if(_proxyport)//
        {
            p_scene->SetProxy(_proxyaddr, _proxyport);
        }
    }

    TRACEX("NS::CreateScene()->\n");
    return (p_scene != 0);
}

void CNS_Moge::shut()
{
    TRACEX("NS::shut()<-\n");
    ::KillTimer(mhWnd, u_timer);
    u_timer = 0;

    _destroing = 1;
    // subclass it back

    SetWindowLong(mhWnd, GWL_USERDATA, (LONG)0);
    this->Release();

    if(--_scenes==0)
    {
        if(p_scene)
        {
            _scenes--;
            p_scene->Destroy();
            delete p_scene;
        }
        p_scene = 0;
    }
    
    _lpOldProc =     ((WNDPROC)SetWindowLongPtr((mhWnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(_lpOldProc)));
    mhWnd = NULL;
    TRACEX("NS::shut()->\n");
    Release();
    BOOL isw = ::IsWindow(mhWnd);
    if(!isw)
    {
        mhWnd = 0;
    }
}

NPBool CNS_Moge::isInitialized()
{
    return (p_scene != 0);
}


NPError	CNS_Moge::GetValue(NPPVariable aVariable, void *aValue)
{
    NPError rv = NPERR_NO_ERROR;

    if (aVariable == NPPVpluginScriptableInstance) 
    {
        // addref happens in getter, so we don't addref here
        AddRef();
        nsIXmogePlugin* pThis = this;

        if (pThis) 
        {
            *(nsISupports **)aValue = pThis;
        } 
        else
        {
            return NPERR_OUT_OF_MEMORY_ERROR;
        }
    }
    else if (aVariable == NPPVpluginScriptableIID) 
    {
        static nsIID scriptableIID = NS_IXMOGEPLUGIN_IID;
        nsIID* ptr = (nsIID *)NPN_MemAlloc(sizeof(nsIID));
        
        if (ptr) 
        {
            *ptr = scriptableIID;
            *(nsIID **)aValue = ptr;
        } 
        else
            return NPERR_OUT_OF_MEMORY_ERROR;
    }
    return 0;
}


void CNS_Moge::Render()
{
    static DWORD pt= 0;
    if(p_scene)
    {
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
}


static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CNS_Moge *plugin = (CNS_Moge *)GetWindowLong(hWnd, GWL_USERDATA);
    switch (msg) 
    {
    case WM_PAINT:
    case WM_TIMER:
        plugin = (CNS_Moge *)GetWindowLong(hWnd, GWL_USERDATA);
        if(plugin) 
        {
            if(plugin->_destroing)
                break;
            plugin->Render();
        }
        break;
    case WM_DESTROY:
        TRACEX("WM_DESTROY<-\n");
        break;
    default:
        break;
    }
    if(plugin)
        return CallWindowProc(plugin->_lpOldProc, hWnd, msg, wParam, lParam);
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


long CNS_Moge::LoadLevel(const char* level)
{
    if(p_scene)
    {
        p_scene->ThreadLoad(level);
        return NS_OK;
    }
    return NS_ERROR_NOT_INITIALIZED;
}

long CNS_Moge::Leave()
{
    if(p_scene)
    {
        _destroing = 1;
        p_scene->Destroy();
        delete p_scene;
        p_scene = 0;
        SetWindowLong(mhWnd, GWL_USERDATA, (LONG)0);
    }
    return NS_OK;
}

void CNS_Moge::SetProxy(const char *bsURL, PRInt32 port)
{
    if(p_scene)
    {
        p_scene->SetProxy(bsURL, port);
    }
    strcpy(_proxyaddr, bsURL);
    _proxyport = port;
}

NPBool CNS_Moge::init(NPWindow* aWindow)
{
    if(0 == SetWindow(aWindow))
        return true;
    return false;
}

NPError CNS_Moge::SetWindow(NPWindow* pNPWindow)
{
    if(p_scene)
    {
        if(pNPWindow && pNPWindow->window && IsWindow((HWND)pNPWindow->window))
        {
            return 0; //no error
        }
        else
        {
            Leave();
            mhWnd = 0;
        }
    }
    else
    {
        //
        // window created we did not create our renderer
        //
        if(pNPWindow && pNPWindow->window && IsWindow((HWND)pNPWindow->window))
        {
            mhWnd = (HWND)pNPWindow->window;
            this->CreateScene();
            return (p_scene == 0) ? NPERR_MODULE_LOAD_FAILED_ERROR : 0;
        }
        else
        {
            mhWnd = 0;
        }
    }
    return 0;
}

void      CNS_Moge::ResolveProxy(NPP instance)
{
    nsISupports*    piIunk  = 0;
    nsIPrefService* piPrefs = 0;
    nsIPrefBranch*  piBranch = 0;
    nsIServiceManager* piSman = 0;
    // Jump through some hoops to get the proxy info
    NPN_GetValue(instance, NPNVserviceManager, &piIunk);
    if (0 == piIunk)  return;

    if(NS_OK != piIunk->QueryInterface(NS_GET_IID(nsIServiceManager), (void **)&piSman))
        goto REL_IFACES;
    
    if(NS_OK != piSman->GetServiceByContractID("@mozilla.org/preferences-service;1",NS_GET_IID(nsIPrefService), (void **)&piPrefs))
        goto REL_IFACES;

    if(NS_OK != piPrefs->ReadUserPrefs(0))
        goto REL_IFACES;

    if(NS_OK != piPrefs->GetBranch("network.proxy.", &piBranch))
        goto REL_IFACES;
    
    char*   proxy;
    int     nPort;
    int     nSet;

    piBranch->GetCharPref("http", &proxy);
    piBranch->GetIntPref("http_port", &nPort);
    piBranch->GetIntPref("type", &nSet);
    if(nSet && nPort)
        this->SetProxy(proxy, nPort);

REL_IFACES:
    if(piIunk) NS_RELEASE(piIunk); // noo need
    if(piPrefs) NS_RELEASE(piPrefs);
    if(piBranch) NS_RELEASE(piBranch);
}


// gbtview.cpp : Implementation of Cgbtview
#include "stdafx.h"
#include "gbtview.h"
#include ".\gbtview.h"


STDMETHODIMP Cgbtview::NLoadLevel(CHAR* bsURL, ULONG* retval)
{
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
#define PR_BEGIN_MACRO  do {
#define PR_END_MACRO    } while (0)

#define NS_NEWXPCOM(_result,_type)                                            \
  PR_BEGIN_MACRO                                                              \
    _result = new _type();                                                    \
  PR_END_MACRO

#define NS_ADDREF(_ptr) \
  (_ptr)->AddRef()

#define NS_RELEASE(_ptr)                                                      \
  PR_BEGIN_MACRO                                                              \
    (_ptr)->Release();                                                        \
    (_ptr) = 0;                                                               \
  PR_END_MACRO


//======================================================================================

#define NS_GENERIC_FACTORY_CONSTRUCTOR(_InstanceClass)                        \
static HRESULT                                                                \
_InstanceClass##Constructor(IUnknown *aOuter, const IID& aIID,                \
                            void **aResult)                                   \
{                                                                             \
    unsigned long rv;                                                         \
                                                                              \
    _InstanceClass * inst;                                                    \
                                                                              \
    *aResult = NULL;                                                          \
    if (NULL != aOuter) {                                                     \
        rv = CLASS_E_NOAGGREGATION;                                           \
        return rv;                                                            \
    }                                                                         \
                                                                              \
    NS_NEWXPCOM(inst, _InstanceClass);                                        \
    if (NULL == inst) {                                                       \
        rv = ERROR_NOT_ENOUGH_MEMORY;                                          \
        return rv;                                                            \
    }                                                                         \
    NS_ADDREF(inst);                                                          \
    rv = inst->QueryInterface(aIID, aResult);                                 \
    NS_RELEASE(inst);                                                         \
                                                                              \
    return rv;                                                                \
}                                                                             

NS_GENERIC_FACTORY_CONSTRUCTOR(Cgbtview)


struct nsModuleComponentInfo {
    const char*                                 mDescription;
    IID                                         mCID;
    const char*                                 mContractID;
    NSConstructorProcPtr                        mConstructor;
    NSRegisterSelfProcPtr                       mRegisterSelfProc;
    NSUnregisterSelfProcPtr                     mUnregisterSelfProc;
    NSFactoryDestructorProcPtr                  mFactoryDestructor;
    NSGetInterfacesProcPtr                      mGetInterfacesProc;
    NSGetLanguageHelperProcPtr                  mGetLanguageHelperProc;
    nsIClassInfo **                             mClassInfoGlobal;
    PRUint32                                    mFlags;
};

static nsModuleComponentInfo components[] =
{
    {
       NIS_GBTVIEW_CLASSNAME, 
       NIS_GBTVIEW_CID,
       NIS_GBTVIEW_CONTRACTID,
       CgbtviewConstructor,
    }
};



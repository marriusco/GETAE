

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Apr 13 13:45:03 2010
 */
/* Compiler settings for _xmoge.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef ___xmoge_h__
#define ___xmoge_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Igbtview_FWD_DEFINED__
#define __Igbtview_FWD_DEFINED__
typedef interface Igbtview Igbtview;
#endif 	/* __Igbtview_FWD_DEFINED__ */


#ifndef __Cgbtview_FWD_DEFINED__
#define __Cgbtview_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cgbtview Cgbtview;
#else
typedef struct Cgbtview Cgbtview;
#endif /* __cplusplus */

#endif 	/* __Cgbtview_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"
#include "shldisp.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __Igbtview_INTERFACE_DEFINED__
#define __Igbtview_INTERFACE_DEFINED__

/* interface Igbtview */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_Igbtview;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BDD9FCFF-2DE0-47C2-8E1E-5A8640E2096E")
    Igbtview : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadLevel( 
            /* [in] */ BSTR bsURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Leave( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProxy( 
            /* [in] */ BSTR bsURL,
            /* [in] */ ULONG port) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IgbtviewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Igbtview * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Igbtview * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Igbtview * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Igbtview * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Igbtview * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Igbtview * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Igbtview * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadLevel )( 
            Igbtview * This,
            /* [in] */ BSTR bsURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Leave )( 
            Igbtview * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProxy )( 
            Igbtview * This,
            /* [in] */ BSTR bsURL,
            /* [in] */ ULONG port);
        
        END_INTERFACE
    } IgbtviewVtbl;

    interface Igbtview
    {
        CONST_VTBL struct IgbtviewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Igbtview_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Igbtview_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Igbtview_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Igbtview_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Igbtview_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Igbtview_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Igbtview_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define Igbtview_LoadLevel(This,bsURL)	\
    ( (This)->lpVtbl -> LoadLevel(This,bsURL) ) 

#define Igbtview_Leave(This)	\
    ( (This)->lpVtbl -> Leave(This) ) 

#define Igbtview_SetProxy(This,bsURL,port)	\
    ( (This)->lpVtbl -> SetProxy(This,bsURL,port) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __Igbtview_INTERFACE_DEFINED__ */



#ifndef __xmoge_LIBRARY_DEFINED__
#define __xmoge_LIBRARY_DEFINED__

/* library xmoge */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_xmoge;

EXTERN_C const CLSID CLSID_Cgbtview;

#ifdef __cplusplus

class DECLSPEC_UUID("5D32D59F-942D-40AB-8D8C-F78EA015B2E7")
Cgbtview;
#endif
#endif /* __xmoge_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



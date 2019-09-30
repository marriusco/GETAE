/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM d:\mco_code_collection\_prjs\Getae\np_xmoge\nsIxmogePlugin.idl
 */

#ifndef __gen_nsIxmogePlugin_h__
#define __gen_nsIxmogePlugin_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIXmogePlugin */
#define NS_IXMOGEPLUGIN_IID_STR "86345456-7a84-427e-b3c1-f122fa3e8f00"

#define NS_IXMOGEPLUGIN_IID \
  {0x86345456, 0x7a84, 0x427e, \
    { 0xb3, 0xc1, 0xf1, 0x22, 0xfa, 0x3e, 0x8f, 0x00 }}

class NS_NO_VTABLE nsIXmogePlugin : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IXMOGEPLUGIN_IID)

  /* long LoadLevel (in string bsURL); */
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) = 0;

  /* long Leave (); */
  NS_IMETHOD Leave(PRInt32 *_retval) = 0;

  /* long SetProxy (in string bsURL, in PRInt32 port); */
  NS_IMETHOD SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIXMOGEPLUGIN \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval); \
  NS_IMETHOD Leave(PRInt32 *_retval); \
  NS_IMETHOD SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIXMOGEPLUGIN(_to) \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return _to LoadLevel(bsURL, _retval); } \
  NS_IMETHOD Leave(PRInt32 *_retval) { return _to Leave(_retval); } \
  NS_IMETHOD SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval) { return _to SetProxy(bsURL, port, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIXMOGEPLUGIN(_to) \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->LoadLevel(bsURL, _retval); } \
  NS_IMETHOD Leave(PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Leave(_retval); } \
  NS_IMETHOD SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetProxy(bsURL, port, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsXmogePlugin : public nsIXmogePlugin
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIXMOGEPLUGIN

  nsXmogePlugin();
  virtual ~nsXmogePlugin();
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsXmogePlugin, nsIXmogePlugin)

nsXmogePlugin::nsXmogePlugin()
{
  /* member initializers and constructor code */
}

nsXmogePlugin::~nsXmogePlugin()
{
  /* destructor code */
}

/* long LoadLevel (in string bsURL); */
NS_IMETHODIMP nsXmogePlugin::LoadLevel(const char *bsURL, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long Leave (); */
NS_IMETHODIMP nsXmogePlugin::Leave(PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long SetProxy (in string bsURL, in PRInt32 port); */
NS_IMETHODIMP nsXmogePlugin::SetProxy(const char *bsURL, PRInt32 port, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIxmogePlugin_h__ */

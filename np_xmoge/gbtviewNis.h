/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM gbtviewNis.idl
 */

#ifndef __gen_gbtviewNis_h__
#define __gen_gbtviewNis_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIWebBrowser_h__
#include "nsIWebBrowser.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    IgbtviewNIS */
#define IGBTVIEWNIS_IID_STR "05ad1389-2436-40cd-b416-4335a95d7bd5"

#define IGBTVIEWNIS_IID \
  {0x05ad1389, 0x2436, 0x40cd, \
    { 0xb4, 0x16, 0x43, 0x35, 0xa9, 0x5d, 0x7b, 0xd5 }}

class NS_NO_VTABLE IgbtviewNIS : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(IGBTVIEWNIS_IID)

  /* long Init (in nsIWebBrowser browser); */
  NS_IMETHOD Init(nsIWebBrowser *browser, PRInt32 *_retval) = 0;

  /* long LoadLevel (in string bsURL); */
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_IGBTVIEWNIS \
  NS_IMETHOD Init(nsIWebBrowser *browser, PRInt32 *_retval); \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_IGBTVIEWNIS(_to) \
  NS_IMETHOD Init(nsIWebBrowser *browser, PRInt32 *_retval) { return _to Init(browser, _retval); } \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return _to LoadLevel(bsURL, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_IGBTVIEWNIS(_to) \
  NS_IMETHOD Init(nsIWebBrowser *browser, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(browser, _retval); } \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->LoadLevel(bsURL, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public IgbtviewNIS
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IGBTVIEWNIS

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, IgbtviewNIS)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* long Init (in nsIWebBrowser browser); */
NS_IMETHODIMP _MYCLASS_::Init(nsIWebBrowser *browser, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long LoadLevel (in string bsURL); */
NS_IMETHODIMP _MYCLASS_::LoadLevel(const char *bsURL, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_gbtviewNis_h__ */

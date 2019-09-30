/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM xpmoge.idl
 */

#ifndef __gen_xpmoge_h__
#define __gen_xpmoge_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    IXpmoge */
#define IXPMOGE_IID_STR "d6edbddf-99bd-4126-8dae-cb542f9d907c"

#define IXPMOGE_IID \
  {0xd6edbddf, 0x99bd, 0x4126, \
    { 0x8d, 0xae, 0xcb, 0x54, 0x2f, 0x9d, 0x90, 0x7c }}

class NS_NO_VTABLE IXpmoge : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(IXPMOGE_IID)

  /* long LoadLevel (in string bsURL); */
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_IXPMOGE \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_IXPMOGE(_to) \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return _to LoadLevel(bsURL, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_IXPMOGE(_to) \
  NS_IMETHOD LoadLevel(const char *bsURL, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->LoadLevel(bsURL, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public IXpmoge
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_IXPMOGE

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, IXpmoge)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* long LoadLevel (in string bsURL); */
NS_IMETHODIMP _MYCLASS_::LoadLevel(const char *bsURL, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_xpmoge_h__ */

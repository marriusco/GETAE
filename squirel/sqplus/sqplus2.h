#ifndef __SQPLUS_2__
#define __SQPLUS_2__
// SqPlus.h
// Created by John Schultz 9/05/05, major update 10/05/05.
// Template function call design from LuaPlusCD by Joshua C. Jensen, 
// inspired by luabind which was inspired by boost::python.
// Const argument, const member functions, and Mac OS-X changes by Simon Michelmore.
// DECLARE_INSTANCE_TYPE_NAME changes by Ben (Project5) from http://www.squirrel-lang.org/forums/.
// Free for any use.

#pragma warning (disable: 4996)
#include <stdlib.h>

#ifdef __APPLE__
  #include <malloc/malloc.h>
#else
  #include <malloc.h>
#endif
#include <memory.h>
#if defined(_MSC_VER) || defined(__BORLANDC__) 
  #include <tchar.h>
  #ifndef UNICODE
    #define SCSNPRINTF _snprintf
    #define SCPUTS puts
  #else
    #define SCSNPRINTF _snwprintf
    #define SCPUTS _putws
  #endif
#else
  #define _T(n) n
  #define SCSNPRINTF snprintf
  #include <stdio.h> // for snprintf
  #define SCPUTS puts
#endif

#ifndef _WINDEF_
  typedef int BOOL;
  typedef int INT;
  typedef float FLOAT;
  #define TRUE 1
  #define FALSE 0
#endif

#if 1
#define SQ_CALL_RAISE_ERROR SQTrue
#else
#define SQ_CALL_RAISE_ERROR SQFalse
#endif

#include "squirrel.h"

#include "SquirrelObject.h"
#include "SquirrelVM.h"
#include "SquirrelBindingsUtils.h"


namespace SqPlus {



template<typename T>
struct ReleaseClassPtrPtr2 {
  static int release(SQUserPointer up,SQInteger size) {
    if (up) { 
      T ** self = (T **)up; 
      delete[] *self;
    } // if
    return 0;
  } // release
};


template<typename T>
inline int TArrayPostConstruct(HSQUIRRELVM v,T * newClass,SQRELEASEHOOK hook) {
  sq_setinstanceup(v,1,newClass);
  sq_setreleasehook(v,1,hook);
  return 1;
} // PostConstruct


template<typename T>
struct ConstructReleaseClass2 {

    static int construct(HSQUIRRELVM v) 
    {
        unsigned char* pt = new unsigned char[sizeof(T)];
        return TArrayPostConstruct<T>(v,(T*)pt,release);
    } // construct
    static int release(SQUserPointer up,SQInteger size) 
    { 
        if (up) 
        { 
            T * self = (T *)up; 
            try{
                delete[] self;
            }
            catch(...)
            {
            }
        } 
        return 0;
    }
  
};


inline BOOL CreateTArray(HSQUIRRELVM v,SquirrelObject & newClass,SQUserPointer classType,const SQChar * name, int szType) 
{
    int n = 0;
    int oldtop = sq_gettop(v);
    sq_pushroottable(v);
    sq_pushstring(v,name,-1);

    //sq_newuserdata(v, szType);
    sq_newarray(v, szType);

    newClass.AttachToStackObject(-1);
    sq_settypetag(v,-1,classType);
    sq_createslot(v,-3);
    sq_pop(v,1);
    return TRUE;
} 


template<typename T> inline SquirrelObject 
RegisterTArray(HSQUIRRELVM v, const SQChar* scriptClassName) 
{
  int top = sq_gettop(v);
  SquirrelObject newClass;

  if (CreateTArray(v,newClass,(SQUserPointer)ClassTypeStorage<T>::type(),scriptClassName, sizeof(T))) 
  {
      SquirrelVM::CreateArray(sizeof(T));
  } // if

  sq_settop(v,top);
  return newClass;
} 

template <class T>struct SQTarrayDef
{
    SQTarrayDef(const SQChar * scriptClassName)
    {
        HSQUIRRELVM v;
        v = SquirrelVM::GetVMPtr();
        RegisterTArray<T>(v, scriptClassName);
    }
};


#define SQ_REGISTER_CLASSA(CLASSNAME)                                 \
  RegisterTArray(SquirrelVM::GetVMPtr(),_T(#CLASSNAME),_##CLASSNAME##_constructor)


};//namespace


#endif //

#ifndef __ENGINE_POOL_H__
#define __ENGINE_POOL_H__

#include "basecont.h"
class SceneImpl;
struct Active
{
    static void     SetScene(SceneImpl* p){_pscene=p;}
    static SceneImpl*  _pscene;
};
_declspec (selectany)SceneImpl* Active::_pscene=0;


template <class T, size_t SZ> 
class NO_VT Activated : public T, public Active
{
public:
	typedef PtrArray<T*>  TypeList;

    Activated(EITEM e):T(e){
		_pList = &_pscene->GetActiveList(SZ);
        _pList->Push((T*)this);
	}
    ~Activated(){
        _pList->Remove((T*)this);
    }
	static int Count(){
        return _pList->Count();
    }
	INLN static T* At(int index){
        return _pList->At();
    }
	INLN static void Push(T* obj){
        _pList->Push((T*)this);
    }
	INLN static void Remove(T* obj){
        _pList->Remove((T*)this);
    }
	INLN static void RemoveIdx(int idx){
        _pList->RemoveIdx(idx);
	}
    INLN static TypeList* List(){return _pList;};

protected:
	static TypeList*	_pList;
};

// statics of the template class----------------------------------------------------
template <class T,size_t SZ> Activated<T,SZ>::TypeList*	Activated<T,SZ>::_pList;

#endif //__ENGINE_POOL_H__
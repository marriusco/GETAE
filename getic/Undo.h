#ifndef __UNDO_H__
#define __UNDO_H__

#include "basecont.h"
#include "BaseEntity.h"

class CZ_ed2Doc;
class Brush;
class SceItem;
class CUndo  
{
public:
    typedef enum _CLS_TYPE{
                    CLS_BRUSH = 0,
                    CLS_ITEM,
                 }CLS_TYPE;
    struct Record
    {
        BaseEntity       *p_Obj;
        BaseEntity       *p_Obj0; //inital 0
        CLS_TYPE        e_type;
        DWORD           e_action;  // action taken to taht changed the p_ObjOriginal
    };
    CUndo():_cursor(0){_fromHere=0;};
    virtual ~CUndo(){Clear();};
    void  Clear();
    void  Add(Brush* pB, DWORD action);
    void  Add(SceItem* pB, DWORD action);
    void  Undo(CZ_ed2Doc* pDoc, DWORD action);
    UINT  Size(){return _records.size();}

private:
    void  _UpdatePtrs(BaseEntity* pB, BaseEntity* pBnew);
    void  _DelPtrs(BaseEntity* pB);
    void  _Resize();
    void  _ClearRecord(Record& record);
    void  _UndoItem(CZ_ed2Doc* pDoc, const Record& rec);
    void  _UndoBrush(CZ_ed2Doc* pDoc, const Record& rec);

    int                 _cursor;
    vvector<Record>     _records;
    BOOL                _fromHere;
};

#endif // __UNDO_H__

// Undo.cpp: implementation of the CUndo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Brush.h"
#include "SceItem.h"
#include "Mmove.h"
#include "Undo.h"
#include "z-edmap.h"
#include "z_ed2Doc.h"

void  CUndo::Add(Brush* pB, DWORD action)
{
    return;
    if(_fromHere)
        return;
    GNewUID = 0;
    Record r;
    r.e_action = action;
    r.e_type   = CLS_BRUSH;
    r.p_Obj    = new Brush(*pB);
    r.p_Obj0   = M_DELETED==action ? 0 : pB;
    _records << r;
    if(M_DELETED==action)
        _DelPtrs(pB);
    _Resize();
    GNewUID = 1;
}

void  CUndo::Add(SceItem* pB, DWORD action)
{
    return;
/*
    if(_fromHere)
        return;
    Record r;
    r.e_action = action;
    r.e_type   = CLS_ITEM
    //dispatch scene item
    r.p_Obj    = new SceItem(*pB);
    r.p_Obj0   = ITM_DELETED==action ? 0 : pB;
    _records << r;
    if(ITM_DELETED==action)
        _DelPtrs(pB);
    _Resize();
*/
}

void  CUndo::_UpdatePtrs(BaseEntity* pB, BaseEntity* pBnew)
{
    return;
    FOREACH(vvector<Record>, _records, record)
    {
        if((*record).p_Obj0 == pB)
        {
            (*record).p_Obj0 = pBnew;
        }
    }
}

void  CUndo::_DelPtrs(BaseEntity* pB)
{
    return;
AGAIN:
    FOREACH(vvector<Record>, _records, record)
    {
        if((*record).p_Obj0 == pB)
        {
            _ClearRecord(*record);
            _records.erase(record);
            goto AGAIN;
        }
    }
}

void  CUndo::_ClearRecord(Record& record)
{
    return;
    delete record.p_Obj;
}


void  CUndo::_Resize()
{
    return;
    if(_records.size() > 8)
    {
        Record& r = _records[0];
        delete r.p_Obj;
        _records.erase(_records.begin());
    }
}

void  CUndo::Clear()
{
    return;
    FOREACH(vvector<Record>, _records, record)
    {
        _ClearRecord(*record);
    }
    _records.clear();
}

void  CUndo::Undo(CZ_ed2Doc* pDoc,  DWORD action)
{
    return;
    _fromHere = 1;
    if(_records.size())
    {
        Record &rec = _records.back();
        
        if(rec.e_type == CLS_ITEM)
        {
            _UndoItem(pDoc, rec);
        }
        else
        {
            _UndoBrush(pDoc, rec);
        }//vector
        _records.pop_back();
    }
    _fromHere = 0;
}

void  CUndo::_UndoBrush(CZ_ed2Doc* pDoc, const Record& rec)
{
    return;
    if(rec.p_Obj0)
    {
        pDoc->DeleteBrush((Brush*)rec.p_Obj0);
        _UpdatePtrs(rec.p_Obj0, rec.p_Obj);
        
    }
    ((Brush*)rec.p_Obj)->Dirty(1);
    pDoc->AddBrush((Brush*)rec.p_Obj);
    pDoc->SelectBrush((Brush*)rec.p_Obj);
}

void  CUndo::_UndoItem(CZ_ed2Doc* pDoc, const Record& rec)
{
    /*
    if(rec.p_Obj0)
    {
        _UpdatePtrs(rec.p_Obj0, rec.p_Obj);
        pDoc->DelItem((SceItem*)rec.p_Obj0);
    }
    pDoc->AddItem(rec.p_Obj);
    */
}


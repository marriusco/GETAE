#pragma once

#include "baselib.h"


//---------------------------------------------------------------------------------------
class XScene;
class DnlThread : public OsThread
{
public:
    DnlThread(XScene* ps);
    virtual ~DnlThread();
    BOOL    Start(const TCHAR*, const TCHAR* tp, const TCHAR* proxy, UINT port);
    virtual void ThreadFoo();
    BOOL    Success(){return _success;};
    BOOL    Status(UINT& percentage);
public:
    XScene* _pscene;
    BOOL    _success;
    TCHAR   _level[512];
    TCHAR   _tp[255];
    TCHAR   _proxy[255]; 
    UINT    _port ;
    mutex   _cs;  
};

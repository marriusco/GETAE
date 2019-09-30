// SysEx.h: interface for the SysEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSEX_H__A0C10C73_6E6D_4434_8C25_C9BFE8F4F237__INCLUDED_)
#define AFX_SYSEX_H__A0C10C73_6E6D_4434_8C25_C9BFE8F4F237__INCLUDED_


class SysEx  
{
public:
    SysEx(const TCHAR* err){_tcscpy(_ex,err);}
    SysEx(const UINT err){_err=err; _stprintf(_ex,"Exception: %d", err);}
    virtual ~SysEx(){};
    const TCHAR* Desc(){return _ex;}
private:
    TCHAR _ex[128];
    int   _err;
};

#endif // !defined(AFX_SYSEX_H__A0C10C73_6E6D_4434_8C25_C9BFE8F4F237__INCLUDED_)

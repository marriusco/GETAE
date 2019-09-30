//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#if !defined(AFX_INIREADER_H__AFD1014B_45A2_4642_9865_096ED095E7DC__INCLUDED_)
#define AFX_INIREADER_H__AFD1014B_45A2_4642_9865_096ED095E7DC__INCLUDED_

#include <IO.H>
#include "baselib.h"
#include "sysex.h"

class CIniReader  
{
public:
    CIniReader(const TCHAR* initfile,BOOL bthrow=1){
        _bthrow = bthrow;
        if(!_tcschr(initfile,'/') && !_tcschr(initfile,'\\'))
        {
            TCHAR cd[260];
            getcwd(cd,260);
            _stprintf(_file, _T("%s\\%s"),cd,initfile);
        }
        else 
            _tcscpy(_file, initfile);

        struct _finddata_t fileFndData;
        long hFile    = _findfirst( _file, &fileFndData );
        if(hFile!=-1)_findclose( hFile );
        _exist  = (hFile != -1);        

    };
    virtual ~CIniReader(){};
    REAL Readf(const TCHAR* sec, const TCHAR* entry,REAL def=0);
    int  Readi(const TCHAR* sec, const TCHAR* entry, int def=0);
    const TCHAR* Reads(const TCHAR* sec, const TCHAR* entry, const TCHAR* def="");
    BOOL  Exist(){return _exist;};
private:
    TCHAR _file[256];
    TCHAR _loco[256];
    BOOL  _bthrow;  
    BOOL  _exist;
};

#endif // !defined(AFX_INIREADER_H__AFD1014B_45A2_4642_9865_096ED095E7DC__INCLUDED_)

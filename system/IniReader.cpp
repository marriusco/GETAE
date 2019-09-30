//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "baseutils.h"
#include "inireader.h"


REAL CIniReader::Readf(const TCHAR* sec, const TCHAR* entry, REAL def)
{
    TCHAR rets[32]={0};
    TCHAR sdef[32]={0};
    sprintf(sdef,"%f",def);
    ::GetPrivateProfileString(sec,entry,sdef,rets,32,_file);
    REAL rv;
    sscanf(rets,"%f",&rv);
    return rv;
}

int  CIniReader::Readi(const TCHAR* sec, const TCHAR* entry, int def)
{
    int rv = ::GetPrivateProfileInt(sec,entry,def,_file);
    return rv;
}

const TCHAR* CIniReader::Reads(const TCHAR* sec, const TCHAR* entry, const TCHAR* def)
{
    ::GetPrivateProfileString(sec,entry,def,_loco,254,_file);
    return _loco;
}


//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "_isound.h"
#include "system.h"
#include "sndsys.h"

//--------------------------------------------------------------------------------------------
BOOL     SndSys::_enabled = FALSE;
TCHAR    SndSys::_sndPath[_MAX_PATH]="res";

//--------------------------------------------------------------------------------------------
int     SndSys::Create(Isound* psound)
{
    SndSys::_enabled = 1;
    _sndobj.SetIface(psound);
    return 0;
}

//--------------------------------------------------------------------------------------------
BOOL SndSys::Create(const TCHAR* szDll, HWND hwnd)
{
	TRACEX("SndSys::Create\r\n");
	if(SndSys::_enabled==FALSE)
	{
		if(_sndobj.Load(szDll))
		    SndSys::_enabled = _sndobj->Create(hwnd,SndSys::_sndPath,0);
	}
    if(!SndSys::_enabled)
    {
        PSystem->Printf("Sound plugin not found.! (Requires DX8 and up)\n");
        PSystem->Printf("                      ...running without sound\n");
        _LOG(LOG_WARNING,"Cannoit load sound plugin %s.! (Requires DX8 and up)\n", szDll);
    }
	return SndSys::_enabled;
};

//--------------------------------------------------------------------------------------------
Itape* SndSys::AddSoundFile(const TCHAR* pFile)
{
    if(! _enabled ){
		return &_defTape;
	}
    iterator it = find(pFile);
    if(it == end()){
		Itape* pTape=_sndobj->CreateTape((TCHAR*)pFile, TRUE);
		if(pTape){
            map<string,Itape*>::operator[](pFile) = pTape;
			pTape->AddRef();
            return pTape;
		}
		return 0;
    }
	((Itape*)(*it).second)->AddRef();
	return (*it).second;
}

//--------------------------------------------------------------------------------------------
void SndSys::Destroy(){
	TRACEX("SndSys::Destroy\r\n");
    iterator it = begin();
    for(;it!=end();it++){
		_sndobj->DestroyTape((*it).second);
    }
    clear();
    if(_sndobj.Interface()){
        _sndobj->Destroy();
        Sleep(256);
        _sndobj.Free();
    }
};

//--------------------------------------------------------------------------------------------
void SndSys::RemoveSoundFile(const TCHAR* pFile){
    iterator it = find(pFile);
    if(it != end()){
        if(((Itape*)(*it).second)->Release()==0){
			erase(it);
		}
    }
}

void SndSys::Clear()
{
    iterator it = begin();
    for(;it!=end();it++)
	{
        while( ((Itape*)(*it).second)->Release()>0);
		erase(it);
        
    }

}

//--------------------------------------------------------------------------------------------
void SndSys::RemoveSound(Itape* pt)
{
    if(&_defTape == pt) return;
    iterator it = begin();
    for(;it!=end();it++){
        if((*it).second == pt){
            if(((Itape*)(*it).second)->Release()==0){
				erase(it);
				break;
			}
        }
    }
}

//--------------------------------------------------------------------------------------------
Itape* SndSys::GetTape(const TCHAR* fname)
{
    if(! _enabled )return 0;
    iterator it = find(fname);
    if(it != end()){
        return (*it).second;
	}
    return &_defTape;
}


//--------------------------------------------------------------------------------------------
Itape* SndSys::PlayPrimaryFile(const TCHAR* fname, DWORD mode)
{
    if(! _enabled ){
		return 0;
	}

    Itape* pt = GetTape(fname);
    if(pt){
       _sndobj->PlayTape(pt, mode);
	   return pt;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
void SndSys::Stop(const TCHAR* fname)
{
    if(! _enabled )return;
    Itape* pt = GetTape(fname);
    if(pt)
       _sndobj->StopTape(pt);
}

//--------------------------------------------------------------------------------------------
void SndSys::Precache(Itape** pPtr,  const TCHAR* pFile)
{
    if(! _enabled )return;
    if(pFile){
		string sf(pFile);
		const pair<const string, Itape**> p(sf, pPtr)		;
	    _precached.insert(p);
	}
}

//--------------------------------------------------------------------------------------------
void SndSys::AddPrecashed()
{
	multimap<string, Itape**>::iterator b = _precached.begin();
	multimap<string, Itape**>::iterator e = _precached.end();
	for(;b!=e;b++){
		Itape** pptr = (*b).second;
		*pptr = AddSoundFile((*b).first.c_str());
		if(*pptr){
			(*pptr)->AddRef();
		}
	}
    _precached.empty();
}

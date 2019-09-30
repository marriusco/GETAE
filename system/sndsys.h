//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __SNDSYS_H__
#define __SNDSYS_H__

#pragma warning (disable:4786)
#include <map>
#include "_isound.h"
using namespace std;

class DefTape : public Itape
{
public:
    BOOL D3Sound(){return 0;};
    void D3Sound(BOOL b){;};
    void D3Params(REAL minDistance, REAL maxDistance){};
    void D3Pos(REAL x, REAL y, REAL z){};
    void SetVolume(REAL x){};
    void AddRef(){};
    int  Release(){return 0;};

};

//---------------------------------------------------------------------------------------
class NO_VT SndSys : public map<tstring, Itape*>
{

public:
    SndSys(){}
	virtual ~SndSys(){}
    int     Create(Isound* psound);
    BOOL    Create(const TCHAR* dllname, HWND);
	void	Clear();
    void    Destroy();
    Isound* GetSound(){return _sndobj.Interface();}
    Itape*  AddSoundFile(const TCHAR* pFile);
	void    RemoveSoundFile(const TCHAR* pFile);
	void    RemoveSound(Itape* pt);
	Itape*  GetTape(const TCHAR* fname);
    void    PlayPrimary(const Itape* pt, DWORD mode=1){
        if(&_defTape == pt) return;
        if(pt)    _sndobj->PlayTape(pt, mode);
    }
   	void    Stop(const Itape* pt ){_sndobj->StopTape(pt);}
	void    Precache(Itape** pPtr,  const TCHAR* pFile);
	void    AddPrecashed();
    Itape*  PlayPrimaryFile(const TCHAR* fname, DWORD mode=1);
	void    Stop(const TCHAR* fname);
    static TCHAR    _sndPath[_MAX_PATH];
private:

	multimap<string, Itape**>	_precached;
    PlugInDll<Isound>	        _sndobj;
    static BOOL					_enabled;
    DefTape                     _defTape;
};


#endif // !defined(AFX_ZSOUND_H__07A55878_9AAA_4B1E_905C_A2711883BB9D__INCLUDED_)

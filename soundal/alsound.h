//============================================================================
// Author: Octavian Marius Chincisan 2003 - 2006
// Zalsoft Inc 1999-2006
//============================================================================

#ifndef __DXSOUND_H__
#define __DXSOUND_H__
//---------------------------------------------------------------------------------------
#ifndef _USRDLL
    #define _USRDLL
#endif //_USRDLL

#include "_isound.h"
#include "al.h"
#include "alut.h"
#include <windows.h>

//---------------------------------------------------------------------------------------

class Tape : public Itape
{
public:
   Tape(){
	   _refCount  = 0;
    }
	~Tape() 
	{
	}
public:
	void AddRef(){_refCount++;}
	int  Release(){
		--_refCount; 
		if(_refCount==0) 
		{
			delete this;
			return 0;
		}
		return _refCount;
	}
	BOOL D3Sound() { return 0; }
	void D3Sound(BOOL b) {  }
    void D3Params(REAL minDistance, REAL maxDistance);
    void D3Pos(REAL x, REAL y, REAL z);

private:
	int	    _refCount;
    int     n_buffer;
};

//---------------------------------------------------------------------------------------
class AlSound : public Isound  
{
public:
	AlSound();
	virtual ~AlSound();
public:
	BOOL Create(HWND hwnd, const TCHAR* srcpath, int maxSounds);
	void Destroy();

	Itape* CreateTape(char *filename, BOOL is3DSound);
	void DestroyTape(Itape* p){delete p;};
	void PlayTape(const Itape *audio, DWORD numRepeats);
	void StopTape(const Itape *audio);
	void SetListenerPos(REAL cameraX, REAL cameraY, REAL cameraZ);
	void SetListenerRolloff(REAL rolloff);
	void SetListenerOrientation(REAL forwardX, REAL forwardY, REAL forwardZ,
						        REAL topX, REAL topY, REAL topZ);
private:
    ALCdevice*      p_device;
    ALCcontext*     p_context;
    ALuint          n_bufers;
    ALuint*         p_buffers;
    ALuint*         p_source;
    ALuint          n_bufIdx;
};

#endif // !__DXSOUND_H__


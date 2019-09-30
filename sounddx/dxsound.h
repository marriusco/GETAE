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
#include <windows.h>

#ifdef WIN32
	#include <dmusicc.h>               
	#include <dmusici.h>	 // install direct X SDK and add the lib and h patch in project
	#include <d3d8types.h>    // install direct X SDK and add the lib and h patch in project      
	#include <D3DTYPES.H>
    #include <cguid.h>                 
    #include <oleauto.h>
#else
    #include <wtypes.h>
    #include <mmsystem.h>
    #include <dsound.h>
    #include <oleauto.h>
    #include <dmusici.h>
#endif //
//---------------------------------------------------------------------------------------

class Tape : public Itape
{
public:
   Tape(){
	   _refCount  = 0;
        p_segment = NULL; 
        p_3dBuffer = NULL; 
        b_3dsound = FALSE; 
    }
	~Tape() 
	{
		if (p_segment != NULL)
		{
			p_segment->Release(); 
			p_segment = NULL;
		}

		if (p_3dBuffer != NULL)
		{
			p_3dBuffer->Release(); 
			p_3dBuffer = NULL;
		}
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
	BOOL D3Sound() { return b_3dsound; }
	void D3Sound(BOOL b) { b_3dsound = b; }
    void D3Params(REAL minDistance, REAL maxDistance){
        	DS3DBUFFER dsBufferParams;
         // set minimum and maximum distances
            dsBufferParams.flMinDistance = minDistance;
            dsBufferParams.flMaxDistance = maxDistance;
            if (p_3dBuffer)
                  p_3dBuffer->SetAllParameters(&dsBufferParams, DS3D_IMMEDIATE);
    }
    void D3Pos(REAL x, REAL y, REAL z)
    {
        p_3dBuffer->SetPosition(x, y, -z, DS3D_IMMEDIATE);
    };

	void                    Segment(IDirectMusicSegment8* seg) { p_segment = seg; }
	IDirectMusicSegment8*   Segment() { return p_segment; }
    void                    Buffer(IDirectSound3DBuffer* dsBuff){p_3dBuffer = dsBuff;};
	IDirectSound3DBuffer*   Buffer() { return p_3dBuffer; }
	void	                SetVolume(REAL r){}
private:
	IDirectMusicSegment8*   p_segment;      
	IDirectSound3DBuffer*   p_3dBuffer;
	BOOL                    b_3dsound;	
	int	                    _refCount;
};

//---------------------------------------------------------------------------------------
class DxSound : public Isound  
{
public:
	DxSound();
	virtual ~DxSound();
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
						   REAL topX, REAL topY, REAL topZ)
	{
		p_3dlistener->SetOrientation(forwardX, forwardY, -forwardZ, topX, topY, topZ, DS3D_IMMEDIATE);
	}
private:
	IDirectSound3DBuffer8*  Create3DBuffer();
	IDirectMusicSegment8*   CreateSegment(char *filename, BOOL is3DSound);
	void PlaySegment(IDirectMusicSegment8* dmSeg, BOOL is3DSound, DWORD numRepeats);
	void StopSegment(IDirectMusicSegment8* dmSeg);
	IDirectMusicPerformance8 *GetPerformance() { return p_musicperformance; }

private:
	IDirectMusicLoader8*		p_musicloader;
	IDirectMusicPerformance8*	p_musicperformance;    
	IDirectMusicAudioPath8*		p_musicpath;       
	IDirectSound3DListener8*	p_3dlistener;              
	DS3DLISTENER				m_dsparams;                      
};

#endif // !__DXSOUND_H__


//============================================================================
// Author: Octavian Marius Chincisan 2003 - 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "dxsound.h"

#ifndef WIN32
    const GUID GUID_NULL = {0};
    const GUID IID_IDirectMusicLoader8={ 0x19e7c08c, 0xa44, 0x4e6a, 0xa1, 0x16, 0x59, 0x5a, 0x7c, 0xd5, 0xde, 0x8c};
    const GUID IID_IDirectMusicPerformance8= {0x679c4137, 0xc62e, 0x4147, 0xb2, 0xb4, 0x9d, 0x56, 0x9a, 0xcb, 0x25, 0x4c};
    const GUID IID_IDirectMusicSegment8 ={0xc6784488, 0x41a3, 0x418f, 0xaa, 0x15, 0xb3, 0x50, 0x93, 0xba, 0x42, 0xd4};
#endif //    
//--------------------------------------------------------------------------------------------
//    MEMBER:	 DxSound::DxSound
//    description: 
//    TO DO
DxSound::DxSound()
{
    p_musicpath		= 0;
    p_musicloader	= 0;
    p_musicperformance = 0;
    p_3dlistener	= 0;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	 DxSound::~DxSound
//    description: 
DxSound::~DxSound()
{
    if (p_musicpath)
		p_musicpath->Release();
    if (p_musicperformance)
		p_musicperformance->Release();
    if (p_musicloader)
		p_musicloader->Release();
    p_musicpath=0;
    p_musicloader=0;
    p_musicperformance=0;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::Create
//    description: 
BOOL DxSound::Create(HWND hwnd, const TCHAR* srccDir, int maxSounds)
{
	HRESULT hr=0;

    char pathStr[MAX_PATH];  // path for tape file
    WCHAR wcharStr[MAX_PATH];
    
    // create the loader object
	hr = CoCreateInstance(CLSID_DirectMusicLoader, 0, CLSCTX_INPROC,
                               IID_IDirectMusicLoader8, (void**)&p_musicloader);
    if (FAILED(hr))
    {
		TRACEX("Error: Cannot create Direct Muzic Loader %X",hr);
        return FALSE;
    }
    
    // create the performance object
	hr = CoCreateInstance(CLSID_DirectMusicPerformance, 0, CLSCTX_INPROC,
       IID_IDirectMusicPerformance8, (void**)&p_musicperformance);
    if (FAILED(hr))
    {
		TRACEX("Error: Cannot create Direct Muzic Performance %X",hr);
        return FALSE;
    }
    
    // initialize the performance with the standard tape path
    hr = p_musicperformance->InitAudio(0, 0, hwnd, DMUS_APATH_SHARED_STEREOPLUSREVERB, 64,
										DMUS_AUDIOF_ALL, 0);
	if(FAILED(hr))
	{
		TRACEX("Warning: Cannot init audio full efects %X",hr);
		hr = p_musicperformance->InitAudio(0, 0, hwnd, DMUS_APATH_DYNAMIC_STEREO, 32,
											DMUS_AUDIOF_ALL, 0);
		if(FAILED(hr))
		{
			TRACEX("Warning: Cannot init audio stereo efects %X",hr);
			hr = p_musicperformance->InitAudio(0, 0, hwnd, DMUS_APATH_DYNAMIC_MONO, 16,
											DMUS_AUDIOF_ALL, 0);
		}
	}
    
	if(FAILED(hr))
	{
        p_musicperformance->Release();
        p_musicperformance = 0;
		TRACEX("Error: Cannot Init audio at all %X",hr);
		return FALSE;
	}
    // create a 3D audiopath
	hr = p_musicperformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D,
														   64, TRUE, &p_musicpath);
    if (FAILED(hr))
    {
		TRACEX("Warning: Cannot create audio path dynamic 3d %X",hr);
		hr = p_musicperformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_STEREO,
														   32, TRUE, &p_musicpath);
		if(FAILED(hr))
		{
			TRACEX("Warning: Cannot create audio path stereo 3d %X",hr);
			hr = p_musicperformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_STEREO,
																16, TRUE, &p_musicpath);
	
		}
    }
    
	if(FAILED(hr))
	{
		TRACEX("Error: Cannot create audio path at all %X",hr);
		return FALSE;
	}

	
	hr = p_musicpath->GetObjectInPath(0, DMUS_PATH_PRIMARY_BUFFER, 0, GUID_NULL, 0,
                                           IID_IDirectSound3DListener8,
                                           (void**)&p_3dlistener);    
    if (FAILED(hr))
    {
        TRACEX("Error: Cannot get Object in audio path  %X",hr);
        return FALSE;
    }
    
    // get the listener parameters
    m_dsparams.dwSize = sizeof(DS3DLISTENER);
    p_3dlistener->GetAllParameters(&m_dsparams);
    
    // set position of listener
    m_dsparams.vPosition.x = 0.0f;
    m_dsparams.vPosition.y = 0.0f;
    m_dsparams.vPosition.z = 0.0f;
    p_3dlistener->SetAllParameters(&m_dsparams, DS3D_IMMEDIATE);
    
    // retrieve the current directory
    GetCurrentDirectory(MAX_PATH, pathStr);

    _tcscat(pathStr, "\\");
	_tcscat(pathStr, srccDir);
    MultiByteToWideChar(CP_ACP, 0, pathStr, -1, wcharStr, MAX_PATH);
    hr = p_musicloader->SetSearchDirectory(GUID_DirectMusicAllTypes, wcharStr, FALSE);
    return hr==S_OK;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::Destroy
//    description: 
void DxSound::Destroy()
{
    // stop the music
    if(p_musicperformance)
    {
        p_musicperformance->Stop(0, 0, 0, 0);
		Sleep(512);
        p_musicperformance->CloseDown();
    }
    if (p_musicpath)
	       p_musicpath->Release();
    
    if (p_musicperformance)
		   p_musicperformance->Release();
    
    if (p_musicloader)
			p_musicloader->Release();
    p_musicpath=0;
    p_musicperformance=0;
    p_musicloader=0;
    Sleep(256);
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::PlaySegment
//    description: 
void DxSound::PlaySegment(IDirectMusicSegment8 *dmSeg, BOOL is3DDxSound, DWORD numRepeats)
{
    // set the number of repeats
    dmSeg->SetRepeats(numRepeats);//DMUS_SEG_REPEAT_INFINITE);
    if (!is3DDxSound)
	    dmSeg->Download(p_musicperformance);
    else
    {
        dmSeg->Download(p_musicpath);
        p_musicperformance->PlaySegmentEx(dmSeg, 0, 0, DMUS_SEGF_DEFAULT, 0,
	                                       0, 0, p_musicpath);
    }
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::Play
//    description: 
void DxSound::PlayTape(const Itape* tape, DWORD numRepeats)
{
    // set number of repeats
	if(0==tape) return;
	if(((Tape*)tape)->Segment() == 0)return;

	((Tape*)tape)->Segment()->SetRepeats(numRepeats);
    if (((Tape*)tape)->D3Sound())
    {
        ((Tape*)tape)->Segment()->Download(p_musicpath);
        p_musicperformance->PlaySegmentEx(((Tape*)tape)->Segment(), 0, 0, DMUS_SEGF_SECONDARY, 0,
                                         0, 0, p_musicpath);
    }
    else
    {
        ((Tape*)tape)->Segment()->Download(p_musicperformance);
        p_musicperformance->PlaySegmentEx(((Tape*)tape)->Segment(), 0, 0, DMUS_SEGF_DEFAULT, 0,
                                         0, 0, 0);
    }
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::Stop
//    description: 
void DxSound::StopTape(const Itape *tape)
{
    if(tape && p_musicperformance)
        p_musicperformance->StopEx(((Tape*)tape)->Segment(), 0, 0);
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::StopSegment
//    description: 
void DxSound::StopSegment(IDirectMusicSegment8 *dmSeg)
{
    if(p_musicperformance && dmSeg)
        p_musicperformance->StopEx(dmSeg, 0, 0);
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::SetListenerPos
//    description: 
void DxSound::SetListenerPos(float cameraX, float cameraY, float cameraZ)
{
    // set the listener position
    p_3dlistener->SetPosition(cameraX, cameraY, -cameraZ, DS3D_IMMEDIATE);
    /*
    // get the listener parameters
    m_dsparams.dwSize = sizeof(DS3DLISTENER);
    p_3dlistener->GetAllParameters(&m_dsparams);
    
    // set position of listener
    m_dsparams.vPosition.x = cameraX;
    m_dsparams.vPosition.y = cameraY;
    m_dsparams.vPosition.z = -cameraZ;
    p_3dlistener->SetAllParameters(&m_dsparams, DS3D_IMMEDIATE);
    */
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::SetListenerRolloff
//    description: 
void DxSound::SetListenerRolloff(float rolloff)
{
    if (p_3dlistener)
		p_3dlistener->SetRolloffFactor(rolloff, DS3D_IMMEDIATE);
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  *DxSound::Create3DBuffer
//    description: 
IDirectSound3DBuffer *DxSound::Create3DBuffer()
{
    IDirectSound3DBuffer *buff;
    DS3DBUFFER dsBufferParams;          // 3d buffer properties
    
    if (FAILED(p_musicpath->GetObjectInPath(DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, GUID_NULL, 0,
											IID_IDirectSound3DBuffer,  (void**)&buff)))
    {
  		TRACEX("Error: Cannot create Direct Muzic Audiopath Unable to retrieve 3D buffer from audiopath");
        return 0;
    }
    
    // get the 3D buffer parameters
    dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    buff->GetAllParameters(&dsBufferParams);
    
    // set the new 3D buffer parameters
    dsBufferParams.dwMode = DS3DMODE_HEADRELATIVE;    // relative to the listener
    buff->SetAllParameters(&dsBufferParams, DS3D_IMMEDIATE);
    return buff;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  *DxSound::CreateSegment
//    description: 
IDirectMusicSegment8 *DxSound::CreateSegment(char *file, BOOL is3DDxSound)
{
    IDirectMusicSegment8 *seg;
    
    char filename[_MAX_PATH]={0};
    _tcscat(filename, file);
    WCHAR wcharStr[MAX_PATH];
    // convert filename to unicode string
    MultiByteToWideChar(CP_ACP, 0, filename, -1, wcharStr, MAX_PATH);
    // load the segment from file
    if (FAILED(p_musicloader->LoadObjectFromFile(CLSID_DirectMusicSegment,
												IID_IDirectMusicSegment8,
												wcharStr,
												(void**)&seg)))
    {
		TRACEX("Warning: music file '%s' not found", file);
        return 0;
    }
    return seg;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  DxSound::Create
//    description: 
Itape* DxSound::CreateTape(char *file, BOOL is3DDxSound)
{
    char filename[_MAX_PATH] ="g_sounds/";
    _tcscpy(filename, file);
    
    IDirectMusicSegment8*		dmSeg;
    IDirectSound3DBuffer8*	ds3D;
    
    Itape *tape = new Tape();
    
    dmSeg = CreateSegment(filename, is3DDxSound);
	if(dmSeg)
	{
		((Tape*)tape)->Segment(dmSeg);
    
		if (is3DDxSound)
		{
			ds3D = Create3DBuffer();
			((Tape*)tape)->Buffer(ds3D);
			((Tape*)tape)->D3Sound(TRUE);
		}
		else
		{
			((Tape*)tape)->Buffer(0);
			((Tape*)tape)->D3Sound(FALSE);
		}
		return tape;
	}
    delete tape;
	tape = 0;
	return 0;
}



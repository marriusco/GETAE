//============================================================================
// Author: Octavian Marius Chincisan 2003 - 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "stdafx.h"
#include "alsound.h"


//--------------------------------------------------------------------------------------------
//    MEMBER:	 AlSound::AlSound
//    description: 
//    TO DO
AlSound::AlSound():p_device(0),p_context(0)
{
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	 AlSound::~AlSound
//    description: 
AlSound::~AlSound()
{
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::Create
//    description: 
BOOL AlSound::Create(HWND hwnd, const TCHAR* srccDir, int maxSounds)
{
    p_device = alcOpenDevice(0);
    if (p_device)
    {
        p_context = alcCreateContext(p_device,NULL);
        alcMakeContextCurrent(p_context);
    }

    n_bufers = maxSounds;
    alGetError(); // clear error code
    alGenBuffers(n_bufers, p_buffers);
    if (alGetError() != AL_NO_ERROR)
    {
        alcMakeContextCurrent(0);
        alcDestroyContext(p_context);
        alcCloseDevice(p_device);
        p_context = 0;
        p_device  = 0;
        n_bufers  = 0;
        return FALSE;
    }

    alGenSources(n_bufers, p_source);
    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteBuffers(n_bufers, p_buffers);
        alcMakeContextCurrent(0);
        alcDestroyContext(p_context);
        alcCloseDevice(p_device);
        p_context = 0;
        p_device  = 0;
        n_bufers  = 0;
        return FALSE;
    }
    return 0;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::Destroy
//    description: 
void AlSound::Destroy()
{
    alDeleteSources(n_bufers, p_sources );
    alDeleteBuffers(n_bufers, p_buffers);
    alcMakeContextCurrent(0);
    alcDestroyContext(p_context);
    alcCloseDevice(p_device);
    p_context = 0;
    p_device  = 0;
    n_bufers  = 0;
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::Play
//    description: 
void AlSound::PlayTape(const Itape* tape, DWORD numRepeats)
{
    Tape* pt = dynamic_cast<Tape*>(tape);

    alutLoadWAVFile("a.wav",&format,&data,&size,&freq);
    alBufferData(buffer[0],format,data,size,freq);
    alutUnloadWAV(format,data,size,freq);
/*
    alSourcef(p_source[pt->Index()], AL_PITCH, 1.0f);
    alSourcef(p_source[pt->Index()], AL_GAIN, 1.0f);
    alSourcefv(p_source[pt->Index()], AL_POSITION, source0Pos);
    alSourcefv(p_source[pt->Index()], AL_VELOCITY, source0Vel);
    alSourcei(p_source[pt->Index()], AL_BUFFER,buffer[pt->Index()]);
    alSourcei(p_source[pt->Index()], AL_LOOPING, AL_TRUE);
*/

}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::Stop
//    description: 
void AlSound::StopTape(const Itape *tape)
{
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::SetListenerPos
//    description: 
void AlSound::SetListenerPos(float cameraX, float cameraY, float cameraZ)
{
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::SetListenerRolloff
//    description: 
void AlSound::SetListenerRolloff(float rolloff)
{
}

//--------------------------------------------------------------------------------------------
//    MEMBER:	  AlSound::Create
//    description: 
Itape* AlSound::CreateTape(char *file, BOOL is3DDxSound)
{
    if(0 == n_bufers)
    {
        return 0;
    }
    ALenum      format;
    void        *data;
    ALsizei     isize;
    ALsizei     ifreq;
    ALboolean   bloop;

    alutLoadWAVFile (file, &format, &data, &isize, Ifreq, bloop);

    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteBuffers(8, &n_bufers);
        return 0;
    }
    alBufferData(g_Buffers[n_bufIdx],format,data,size,ifreq);
    if (alGetError() != AL_NO_ERROR)
    {
        alDeleteBuffers(8, &n_bufers);
        return 0;
    }
    alSourcei(source[n_bufIdx], AL_BUFFER, g_Buffers[n_bufIdx]);
    if (alGetError() != AL_NO_ERROR)
    {
        return 0;
    }
    n_bufIdx = ++n_bufIdx % n_bufers;

}




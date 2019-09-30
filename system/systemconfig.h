//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__


struct SystemConfig
{
    int     _tracelevel;
    TCHAR   _imgSubPath[_MAX_PATH];
    TCHAR   _sndPath[_MAX_PATH];
    BOOL    _exportscr;
    TCHAR   _renderSel[_MAX_PATH];
    TCHAR   _inputSel[_MAX_PATH];
    TCHAR   _audioSel[_MAX_PATH];
        
    REAL    _physycsTime;
    TCHAR   _proxyip[_MAX_PATH];
    int     _proxyport;

    BOOL    hideCurosr;
    BOOL    bFullScreen;
    BOOL    retainedDraw;
    REAL    fNear;
    REAL    fFar;
    REAL    hFov;
    int     xPolyMode;
    int     pfDepthBits;
    int     pfStencilBits;
    int     bgColor;
    BOOL    clipCursor;
    BOOL    bsetCapture;
    int     dmPelsWidth;
    int     dmPelsHeight;
    int     dmBitsPerPel;
};



#endif //__SYSTEM_CONFIG_H__
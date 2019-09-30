//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#ifndef __TEXRUNTIME_H__
#define __TEXRUNTIME_H__


//---------------------------------------------------------------------------------------
class System;
class RTexture
{
public:
    RTexture(){
        pIdxBuffer = 0;
        pRGBBuffer = 0;
    }
    virtual ~RTexture(){
        delete[] pIdxBuffer;
		pIdxBuffer=0;
        delete[] pRGBBuffer;
		pRGBBuffer=0;
    }
    long    curFrame;
    Htex    texID;
    int     xDim;
    int     yDim;
    BYTE   *pIdxBuffer;             // indexes of RGB's
    BYTE   *pRGBBuffer;
    virtual void    Spin(DWORD frame, REAL delTime)=0;
};

//---------------------------------------------------------------------------------------
// fire runtime texture
class RFireTexture : public RTexture
{
public:
    void Spin(DWORD frame, REAL delTime);
    int  intensity;
    int  heat;
};

//---------------------------------------------------------------------------------------
class RBlobTexture : public RTexture
{
public:
    void Spin(DWORD frame, REAL delTime);
    int  radius;
    int  nBlobs;
    SIZE szBlob[32];
    BYTE byBlob[16][16];
};

//---------------------------------------------------------------------------------------
// runtime texture container
class TextuerRt  
{
public:
    friend class RFireTexture;
    friend class RBlobTexture;
    typedef enum _RT_TYPE{RT_NONE,RT_FIRE, RT_BLOB, RT_LAST}RT_TYPE;

  	TextuerRt();
    ~TextuerRt();

    void         Initialize(System* ps){_pSystem=ps;};
    BOOL         Update(RT_TYPE type, DWORD dynaFrame);
    BOOL         Update(DWORD dynaFrame);
    Htex         CreateFire(int x, int y, int intens, int heat);
    INLN BYTE*   GetFireTex(){return _texts[RT_FIRE]->pRGBBuffer;}
    INLN int     GetFireTexX(){return _texts[RT_FIRE]->xDim;}
    INLN int     GetFireTexY(){return _texts[RT_FIRE]->yDim;}

    Htex         CreateBlob(int x, int y, int blbRad, int nBlobs);
    INLN BYTE*   GetBlobTex(){return _texts[RT_BLOB]->pRGBBuffer;}
    INLN int     GetBlobTexX(){return _texts[RT_BLOB]->xDim;}
    INLN int     GetBlobTexY(){return _texts[RT_BLOB]->yDim;}
	void	     Clear();

private:
    static TextuerRt*   _pRt;
    static CLR          _firePalette[256];
    static CLR          _blobPalette[256];
    RTexture*           _texts[RT_LAST+1];
    static int          _refs;
    System*             _pSystem;
};

#endif // !__TEXRUNTIME_H__

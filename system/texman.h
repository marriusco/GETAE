    //============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#ifndef __ZTEXSYS_H__
#define __ZTEXSYS_H__

#pragma warning (disable:4786)
#include <map>
#include "_irender.h"
using namespace std;
#ifndef __HTEX
    typedef UINT HTEX;
    #define __HTEX
#endif //

//add search path for texture manager


//--------------[READS JPEG, TGA IMAGES FILE]--------------------------------------------
class   NO_VT TexHandler
{
public:
	TexHandler();
	virtual ~TexHandler();
    BOOL    LoadThisFile(const TCHAR *sFileName, DWORD flags);
	BOOL    LoadFile(const TCHAR *sFileName, DWORD flags);
	int     SaveTgaFile(const TCHAR *sFileName);
    BOOL    MakeBW();
    BOOL    MakeDot3(REAL);
	BYTE*   Buffer(){
		return p_pBuff[0];
	}
    void Reset()
    {
        Deallocate();
        n_x      = 0;
	    n_y      = 0;
	    n_bpp    = 0;
	    n_size   = 0;
	    b_Filter = FALSE;
	    b_MipMap = TRUE;
	    b_Ilum   = FALSE;
    }
	void MaskBW();
    static void Resize(int nBytes, BYTE* pBufIn, int wIn, 
		          int hIn, BYTE* pBufOut, int wOut, int hOut) ;
    static void  SetSearchPath(const TCHAR* exreaPath){
        if(exreaPath)
            _tcscpy(_extrapath, exreaPath);
        else 
            _extrapath[0]=0;
    }

    static BYTE* AlphaIt(BYTE* pTemp, int x, int y, int& bpp);
    static void MaskBW(BYTE* pb, int sz, int bpp);
    void  AlphaIt(BYTE alpha);
    void  SwapRB();
    void  Invert();
    void  SaveBMP(char *filename);
protected:	
    BOOL   _GetPixel(BYTE* pOld,  int x, int y);
    
    int    LoadFile2(const TCHAR *sFileName, DWORD flags=0);
	
    BOOL   LoadJpegFile(const TCHAR *sFileName, BOOL search, DWORD flags=0);
	BOOL   LoadTgaFile(const TCHAR *sFileName, BOOL search, DWORD flags=0);
	BOOL   LoadBmpFile(const TCHAR *sFileName, BOOL search, DWORD flags=0);

	BOOL   CreateBuffer(int width,int height, int deep);
	BOOL   CreateMap(REAL mapScale);
	void   Deallocate();
	BYTE** Allocate(int lines, int block);
    BOOL LoadBMP1bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);
	BOOL LoadBMP8bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);
	BOOL LoadBMP4bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);

public:
	int     n_x;
	int     n_y;
	int     n_bpp;
	int     n_size;
	BYTE**  p_pBuff;	
	BOOL    b_MipMap;
	BOOL    b_Filter; 
	BOOL    b_Ilum;
    static HTEX  GDefaultTexture;
    static Htex  GDefaultHtexture;
    static TCHAR _extrapath[_MAX_PATH];
};

//-------------[TEXTURE MAPP BETWEEN FILE AND ID]----------------------------------------
struct Texture
{
	Texture():genMode(TEX_NORMAL),cx(0),cy(0),bpp(0),pBuff(0),refcount(0){}
	Htex        hTex;
    DWORD       genMode;
    int         cx;
    int         cy;
    int         bpp;
    BYTE*       pBuff;
	int         refcount;
    TCHAR       filename[32];
};

//-------------[]-----------------------------------------------------------------------
typedef Htex  (*STCB)(int dx, int dy, int bpp, BYTE* pb, DWORD mips);
typedef void  (*RTCB)(UINT* tex, int ncount);
//-------------------------[manages textures in system]---------------------------------
class TexMan
{
public:
    TexMan(){
        _stx = 0;
        _rtx = 0;
    }
    TexMan(STCB scb, RTCB rtc){
        _stx = scb;
        _rtx = rtc;
    }
    ~TexMan(){assert(_textures.size()==0);}
    
    Htex& GenTexture(const TCHAR* ptName, int x, int y, int bpp, BYTE* buff, DWORD flags);
	Htex& AddTextureFile(const TCHAR* ptFile, DWORD flags = TEX_NORMAL);
    Htex& AddTextureFileGetImage(const TCHAR* filename, DWORD mips, BYTE** pb, int* dx, int* dy, int* bpp);
	Htex& operator[](const TCHAR* psz);
	const TCHAR* GetTexName(Htex& id);
    int RemoveTexture(Htex& iTex, BOOL allrefs=FALSE);
    int RemoveTextures(Htex* iextures, int count, BOOL allrefs=FALSE);
	int RemoveTextureFile(const TCHAR* psz, BOOL allrefs=FALSE);
    void Clear();
    Texture* GetTempTexture(const TCHAR* ptName);
    Texture* GetTempTexture(Htex& tex);
    
private:
    STCB    _stx;
    RTCB    _rtx;
    map<tstring, Texture>   _textures;
    
};

//-------------------------[for jpeg lbb]------------------------------------------------
typedef struct _jpegdata 
{
    BYTE *ptr;
    int    width;
    int    height;
    FILE  *output_file;
    FILE  *input_file;
    int    aritcoding;
    int    CCIR601sampling;
    int    smoothingfactor;
    int    quality;
    HWND   hWnd;
    int    ProgressMsg;
    int    status;
    int    components;
} JPEGDATA;

void JpegWrite( JPEGDATA *jpgStruct );
void JpegInfo(  JPEGDATA *jpgStruct );
void JpegRead(  JPEGDATA *jpgStruct );


#endif // !defined(AFX_ZTEXTURE_H__078F6B43_840A_4286_8A95_CB2DB6C66FE5__INCLUDED_)

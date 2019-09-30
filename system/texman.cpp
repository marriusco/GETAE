//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include <io.h>
#include <stdio.h>
#include "texman.h"


//--------[constructor]-------------------------------------------------------------------

HTEX  TexHandler::GDefaultTexture;
Htex  TexHandler::GDefaultHtexture;
TCHAR TexHandler::_extrapath[_MAX_PATH]={0};
extern TCHAR*   tex_includes[256];

FILE* texture_fopen(const TCHAR *filename ,const TCHAR *mode)
{
    TCHAR   loco[256]; // mco
    struct  _finddata_t c_file;
    long    hFile    = _tfindfirst( filename, &c_file );
    if(hFile ==-1)
    {
        bool found =false;
        if(*TexHandler::_extrapath)
        {
            _stprintf(loco,_T("%s%s"),TexHandler::_extrapath,filename);
            hFile    = _tfindfirst( loco, &c_file );
            if(hFile!=-1)
            {
                _findclose( hFile );
                found =true;
            }
        }
        if(!found)
        {
            for(int i=0; tex_includes[i]; i++)
            {
                if(tex_includes[i][0])
                {
                    _stprintf(loco,_T("%s%s"),tex_includes[i],filename);

                    hFile    = _tfindfirst( loco, &c_file );
                    if(hFile!=-1)
                    {
                        _findclose( hFile );
                        break;
                    }
                }
            }
        }
    }
    else
    {
        _tcscpy(loco, filename);
        _findclose( hFile );
    }
	return _tfopen(loco,mode);
}

//--------[constructor]-------------------------------------------------------------------
#define VALID_FILENAME(fstr_)   (fstr_ && fstr_[0] && fstr_[0]!=_T('*') )

//--------[constructor]-------------------------------------------------------------------
TexHandler::TexHandler(): n_x(0),n_y(0),n_bpp(0),p_pBuff(0),b_Filter(FALSE),b_MipMap(TRUE),
					      b_Ilum(FALSE)
{
}

//--------[destructor]--------------------------------------------------------------
TexHandler::~TexHandler()
{
    Deallocate();
};

//--------[heap alocation fr buffer to store the image]-----------------------------------
BOOL TexHandler::CreateBuffer(int width, int height, int deep)
{
    Deallocate();
    if(deep == 32)
    {
        n_x = width;
        n_y = height;
        n_bpp = 4;
        n_size=n_x * n_y * n_bpp;
        p_pBuff = Allocate(n_y, n_size);
        return Buffer()!=0;
    }
    else if(deep == 24)
    {
        n_x = width;
        n_y = height;
        n_bpp = 3;
        n_size = n_x * n_y * n_bpp;
        p_pBuff = Allocate(n_y, n_size);
        return Buffer()!=0;
    }
    return FALSE;
}
//--------------[resizes a texture]-------------------------------------------------
void TexHandler::Resize(int nBytes, BYTE* pBufIn, int wIn, 
			               int hIn, BYTE* pBufOut, int wOut, int hOut) 
{
	REAL sx = (REAL) wIn/wOut;
	REAL sy = (REAL) hIn/hOut;
	BYTE* pTmp = pBufIn;

	for (int y = 0; y < hOut; y++) 
	{
		int yy = (int) (y*sy)*wIn;
		for (int x = 0; x < wOut; x++) 
		{
			int xx = (int) (x*sx);
			pTmp = pBufIn + (yy+xx)*nBytes;
			for (int c = 0; c < nBytes; c++)
			{
				*pBufOut++ = *pTmp++;
			}
		}
	}
}

//----------------------------------------------------------------------------------
// places every color under RGB=16 (black) to 9, and everithing else to WHITE. 
// Creates a black white mask
void TexHandler::MaskBW()
{
	BYTE* pw = p_pBuff[0];	

    for(int i=0;i<n_size;i+=n_bpp)
    {
		if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
		{
			pw[i]   = 255;
			pw[i+1] = 255;
			pw[i+2] = 255;
		}
		else
		{
			pw[i]   = 0;
			pw[i+1] = 0;
			pw[i+2] = 0;
		}
    }
}

//----------------------------------------------------------------------------------------
void  TexHandler::SwapRB()
{
	BYTE* pw = p_pBuff[0];	

    BYTE   tmp;
    for(int i=0;i<n_size;i+=n_bpp)
    {
		tmp = pw[i];
        pw[i] = pw[i+2];
        pw[i+2] = tmp;
    }
}

BOOL TexHandler::LoadThisFile(const TCHAR *sFileName, DWORD flags)
{
    TCHAR sExt[16] = {0};
    TCHAR sDummy[_MAX_PATH];

    _tsplitpath(sFileName,sDummy,sDummy,sDummy,sExt);
    if (!stricmp(sExt,_T(".tga")))
        return LoadTgaFile(sFileName,1, flags);
    else  if (!stricmp(sExt,_T(".jpg")))
        return LoadJpegFile(sFileName, 1,flags);
    else if (!stricmp(sExt,_T(".bmp")))
        return LoadBmpFile(sFileName, 1, flags);
    return FALSE;
}

//--------[dispatches the load to the private implementations]----------------------------
BOOL TexHandler::LoadFile(const TCHAR *sFileName, DWORD flags)
{
	if(!VALID_FILENAME(sFileName))
		return 0;

	HTEX  iret;
    TCHAR newname[128]={0};
    if(!_tcschr(sFileName,_T('.')))
    {
        _tcscpy(newname, sFileName);
        _tcscat(newname,_T(".jpg")); 
        iret = LoadFile2(newname,flags);
		if(iret==0)
		{
			_tcscpy(newname, sFileName);
			_tcscat(newname,_T(".tga"));
			iret =LoadFile2(newname,flags);
			if(iret==0)
			{
				_tcscpy(newname, sFileName);
				_tcscat(newname,_T(".bmp"));
				iret =LoadFile2(newname,flags);
			}
		}
		if(iret)
			return iret;
    }
	iret = LoadFile2(sFileName,flags);
	if(iret==0)
	{
		iret = TexHandler::GDefaultTexture;
	}
	return iret;
}


//----------------------------------------------------------------------------------------
int TexHandler::LoadFile2(const TCHAR *sFileName, DWORD flags)
{
	const   TCHAR* phassl  = _tcschr(sFileName,_T('\\'));
	const   TCHAR* phassl2 = _tcschr(sFileName,_T('/'));
    BOOL    searchFile = !(phassl ||phassl2 );

    if(_tcsstr(sFileName,"tga"))
        return LoadTgaFile(sFileName, searchFile, flags);
    else if(_tcsstr(sFileName,"jpg"))
        return LoadJpegFile(sFileName, searchFile, flags);
    else if(_tcsstr(sFileName,"bmp"))
        return LoadBmpFile(sFileName, searchFile, flags);
    
    return FALSE;
}

//--------------------------------------------------------------------------------------
BOOL TexHandler::LoadJpegFile(const TCHAR *sFileName, BOOL bsearch, DWORD flags)
{
    FILE* f = 0;
    Deallocate();

    if(bsearch)
        f = texture_fopen(sFileName,_T("rb"));
    else
        f = _tfopen(sFileName,_T("rb"));

    if(0 != f)
    {
        JPEGDATA jpgStruct = {0};
        jpgStruct.input_file=f;
        JpegInfo(&jpgStruct);
        fclose(f);
        if (3 == jpgStruct.components)
        {
            // open the file again to read the pData by jpeg library
            CreateBuffer(jpgStruct.width, jpgStruct.height, 24);
            f = texture_fopen(sFileName,_T("rb"));
            
            jpgStruct.ptr        = Buffer();
            jpgStruct.input_file = f;
            jpgStruct.hWnd       = 0;
            jpgStruct.ProgressMsg= 0;
            JpegRead(&jpgStruct);
            fclose(f);

            if (jpgStruct.status==0 )
            {
				//Invert();
                return TRUE;
            }
        }
    }
    return FALSE;
}

//--------------------------------------------------------------------------------------
BOOL TexHandler::LoadTgaFile(const TCHAR *sFileName, BOOL bsearch, DWORD flags)
{
    FILE* fp = 0;
    Deallocate();

    if(bsearch)
        fp = texture_fopen(sFileName,_T("rb"));
    else
        fp = _tfopen(sFileName,_T("rb"));
    
    if(fp)
    {	
        BYTE tgaStruct[18] = {0};
        fread((TCHAR *)tgaStruct, 18,1,fp);
        if(!((tgaStruct[16]==24 || tgaStruct[16]==32) && (tgaStruct[2]==2 || tgaStruct[2]==10)))
        {
            fclose(fp);
            return FALSE;
        }
        
        int x= (int)(*((USHORT *)&tgaStruct[12]));
        int y= (int)(*((USHORT *)&tgaStruct[14]));
        if(tgaStruct[16]==24)
        {
            CreateBuffer(x, y, 24);
        }
        else
        {
            CreateBuffer(x, y, 32);
        }
        int pixOrder = tgaStruct[17] & 0x30;
        if(tgaStruct[2]==2)
        {
            fread(Buffer(), n_size,1,fp);
        }
        else
        {
            BYTE *p = p_pBuff[0];
            int idx = 0;
            while(idx < n_x * n_y)
            {
                BYTE by;
                fread(&by,1,1,fp);
                if(0 == (by & 0x80))
                {
                    by++;
                    fread(p, by * n_bpp,1,fp);
                    p += by * n_bpp;
                }
                else
                {
                    by = (by & 0x7f) + 1;
                    fread(p, n_bpp,1,fp);
                    BYTE* q = p;
                    for(int i=1; i < by; i++)
                    {
                        q += n_bpp;
                        q[0] = p[0];
                        q[1] = p[1];
                        q[2] = p[2];
                    }
                    p = q;
                    p += n_bpp;
                }
                idx += by;
            }
        }
        
        if (pixOrder==0x00 || pixOrder==0x10)
        {
            (void)0;
        }
		else
		{
			Invert();
		}
        
        for(y=0; y<n_y; y++ )
        {
            for(int p=0; p<n_x; p++)
            {
                int xPos1 = p * n_bpp;
                BYTE bFlag = p_pBuff[y][xPos1 + 2];
                p_pBuff[y][xPos1+2]=p_pBuff[y][xPos1];
                p_pBuff[y][xPos1]=bFlag;
            }
            if(pixOrder==0x10 || pixOrder==0x30)
            {
                for(int p=0; p<n_x/2; p++)
                {
                    int xPos1=p*n_bpp;
                    int xPos2=(n_x-1-p)*n_bpp;
                    for(int i=0; i<n_bpp; i++)
                    {
                        BYTE bFlag=p_pBuff[y][xPos2+i];
                        p_pBuff[y][xPos2+i]=p_pBuff[y][xPos1+i];
                        p_pBuff[y][xPos1+i]=bFlag;
                    }
                }
            }
        }
        fclose(fp);
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------------------
int TexHandler::SaveTgaFile(const TCHAR *sFileName)
{
    FILE* fp=_tfopen(sFileName,_T("wb"));
    
    if (0 != fp)
    {
        BYTE* ingLine = new BYTE[n_x*n_bpp];
        BYTE tgaStruct[18] = {0};
        
        tgaStruct[12]=(BYTE)(n_x%256);
        tgaStruct[13]=(BYTE)(n_x/256);
        tgaStruct[14]=(BYTE)(n_y%256);
        tgaStruct[15]=(BYTE)(n_y/256);
        tgaStruct[2]=2;
        tgaStruct[16]=8*n_bpp;
        fwrite((TCHAR *)&tgaStruct[0],18,1,fp);
        
        for(int ii=n_y-1;ii>=0;ii-- )
        {
            for(int jj=0;jj<n_x;jj++ )
            {
                ingLine[jj*n_bpp] = p_pBuff[ii][(jj*n_bpp+2)];
                ingLine[jj*n_bpp+1] = p_pBuff[ii][(jj*n_bpp+1)];
                ingLine[jj*n_bpp+2] = p_pBuff[ii][(jj*n_bpp)];
                if (n_bpp==4)
                   ingLine[jj*n_bpp+3] = p_pBuff[ii][(jj*n_bpp+3)];
            }
            n_bpp;
            int bytes = fwrite((TCHAR*)ingLine, n_x, n_bpp, fp);
            if(n_bpp != bytes)
            {
                delete[]ingLine;
                fclose(fp);
                return FALSE;
            }
        }
        fclose(fp);
        delete[]ingLine;
    }
    
    return TRUE;
}

//--------------[maby i'll useit for dynamic lighting]-----------------------------------
BOOL TexHandler::CreateMap(REAL mapScale)
{
    if (n_bpp == 4)
    {
        REAL fraport=1.0f/255.0f;
        BYTE *uc = p_pBuff[0];
        for(int ii=0;ii<n_x;ii++ )
        {
            for(int jj=0;jj<n_y;jj++ )
            {
                REAL c = uc[3] * fraport;
                REAL cx = p_pBuff[ii][((jj+1)%n_x)*4+3] * fraport;
                REAL cy = p_pBuff[(ii+1)%n_y][jj*4+3] * fraport;
                REAL dcx = mapScale * (c - cx);
                REAL dcy = mapScale * (c - cy);
                
                REAL len = dcx*dcx + dcy*dcy + 1.0f;
                REAL nz = (REAL)(1.0/sqrt(len));
                REAL nx = dcy*nz;
                REAL ny = -dcx*nz;
                
                uc[0] = (BYTE)(128 + 127*nx);
                uc[1] = (BYTE)(128 + 127*ny);
                uc[2] = (BYTE)(128 + 127*nz);
                uc+=4;
            }
        }
        return TRUE;
    }
    return FALSE;
}

//--------------[maby i'll useit for dynamic lighting]-----------------------------------
BOOL TexHandler::MakeBW()
{
    int    sz  = n_x*n_y*3;
    BYTE** pNew = Allocate(n_y, sz);
    BYTE* pNeww = pNew[0];
    BYTE* pOld = p_pBuff[0];
    
    int j=0;
    
    for(int i=0;i<n_size;i+=n_bpp,j+=3)
    {
        pNeww[j] = (pOld[i]+pOld[i+1]+pOld[i+2])/3;
        pNeww[j+1] = pNeww[j];
        pNeww[j+2] = pNeww[j];
    }
    Deallocate();
    p_pBuff = pNew;
    n_bpp   = 3;
    return (b_Ilum = TRUE);
}

//-----------------------------------------------------------------------------------------
BOOL TexHandler::_GetPixel(BYTE* pOld,  int x, int y)
{
	x = (x+n_x)%n_x;
	y = (y+n_y)%n_y;
	return pOld[(n_x*y+x)*n_bpp+0];
}

//-----------------------------------------------------------------------------------------
BOOL TexHandler::MakeDot3(REAL intens)
{
    int  sz= n_x*n_y*4;
    BYTE** pNew  = Allocate(n_y, sz);
    BYTE*  pNeww = pNew[0];
    BYTE*  pOld  = p_pBuff[0];
	REAL   dx,dy;		

	for(int x=0; x<n_x; x++)
	{
		for(int y=0; y<n_y; y++)
		{
			dx  = _GetPixel( pOld, x-1, y-1);					//  1  0 -1
			dx += 2.f*(REAL)_GetPixel( pOld, x-1, y);			//  2  0 -2
			dx += _GetPixel( pOld, x-1, y+1);					//  1  0 -1
			dx -= _GetPixel( pOld, x+1, y-1);
			dx -= 2.f*(REAL)_GetPixel( pOld, x+1, y);
			dx -= _GetPixel( pOld, x+1, y+1);
			dx /= 255.f;
			

			dy  = _GetPixel( pOld, x-1, y-1);					//  1  2  1
			dy += 2.f*(REAL)_GetPixel( pOld, x  , y-1);		//  0  0  0
			dy += _GetPixel( pOld, x+1, y-1);					// -1 -2 -1
			dy -= _GetPixel( pOld, x-1, y+1);
			dy -= 2.f*(REAL)_GetPixel( pOld, x  , y+1);
			dy -= _GetPixel( pOld, x+1, y+1);
			dy /= 255.f;

			V3   vclr(intens*dx, intens*dy, 1);
            vclr.norm();
            vclr = vclr*.05f + 0.5f;

			pNeww[(n_x*y+x)*n_bpp+0] = (unsigned char)(255.f*vclr.x);
			pNeww[(n_x*y+x)*n_bpp+1] = (unsigned char)(255.f*vclr.y);
			pNeww[(n_x*y+x)*n_bpp+2] = (unsigned char)(255.f*vclr.z);
		}
	}
    Deallocate();
    p_pBuff = pNew;
	return 1;
}

//--------------[frees the row vector and whole array]--------------------------------------
void TexHandler::Deallocate()
{
	if(p_pBuff)
	{
		delete[] p_pBuff[0];
		p_pBuff[0] = 0;
		delete[] p_pBuff;
		p_pBuff = 0;
	}
}

//--------------[new's the row vector and rows]--------------------------------------
BYTE** TexHandler::Allocate(int lines, int rgbasz)
{
	b_Filter = FALSE;
	b_MipMap = TRUE;
	b_Ilum = FALSE;
	BYTE** p;
	p = new BYTE* [lines];
	p[0] = new BYTE[rgbasz];
	for(int i=1;i<lines;i++)
	{
		p[i] = &p[0][i*n_x*n_bpp];
	}
	return p;
}

//--------------[:)]--------------------------------------------------------------
BOOL TexHandler::LoadBmpFile(const TCHAR *sFileName, BOOL bsearch, DWORD flags)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int				length;
	BYTE*			bits;
	BYTE*			ptr;
	BYTE			temp;
	int				x,y;
	BOOL			br = FALSE;      

    FILE* f = 0;
    Deallocate();

    if(bsearch)
        f = texture_fopen(sFileName,_T("rb"));
    else
        f = _tfopen(sFileName,_T("rb"));

	if (0==f) 
	{
		return FALSE;
	}

	if (!fread(&bmfh,sizeof(bmfh),1,f)) 
	  goto tERR;
	if (bmfh.bfType!=0x4D42) 
	  goto tERR;

	if (!fread(&bmih,sizeof(bmih),1,f)) 
	  goto tERR;

    if(bmih.biCompression!=BI_RGB)
	  goto tERR;

    if (!IsPowOf2(bmih.biWidth)) 
	  goto tERR;
	if (!IsPowOf2(bmih.biHeight)) 
	  goto tERR;

	switch(bmih.biBitCount)
    {
        case 1:
            br =LoadBMP1bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 4:
            br =LoadBMP4bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 8:
            br =LoadBMP8bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 24:
        case 32:
            break;
        default:
            fclose(f);
            return FALSE;
    }

	CreateBuffer(bmih.biWidth, bmih.biHeight, bmih.biBitCount);
	fseek(f, bmfh.bfOffBits, SEEK_SET);
	for ( y = 0; y < bmih.biHeight; y++)
	{
		fread(p_pBuff[y],1, n_bpp * n_x ,f);
	}
	fclose(f);
	length = (bmih.biWidth * 3 + 3) & ~3;
	bits   = Buffer();
	for (y = 0; y < bmih.biHeight; y ++)
	{
	      for (ptr = bits + y * length, x = bmih.biWidth; x > 0; x--, ptr += 3)
	      {
			    temp   = ptr[0];
			    ptr[0] = ptr[2];
			    ptr[2] = temp;
	      }
	}
	
    Invert();
	return TRUE;		
tERR:
  fclose(f);
  return FALSE;

}

BOOL TexHandler::LoadBMP1bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
	RGBQUAD pal[2];

	if(fread( pal, sizeof(RGBQUAD), infoheader.biClrUsed, f)!=infoheader.biClrUsed)
	{
		return FALSE;
	}

	BYTE* pData = 0;
	int lineSz = infoheader.biWidth/8;
	if(infoheader.biWidth%8)
        lineSz++;
	if(lineSz%4)
        lineSz += 4 - lineSz % 4;
	pData = new BYTE[lineSz  * infoheader.biHeight];
	if(pData==0)
    {
        return FALSE; 
    }
	
    if(!fread( pData, lineSz*infoheader.biHeight, 1, f))
	{	
		delete [] pData;
		return FALSE; 
	}

    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);

	int planes= 3;
	BYTE* pw = p_pBuff[0];	
	for(int y=0; y<infoheader.biHeight; y++)
	{
		for(int x=0; x<infoheader.biWidth; x++)
		{
			BYTE index=pData[y*lineSz+x/8];
			index >>= (7-x%8);
			index &= 0x01;
			pw[(y*infoheader.biWidth+x)*planes+0] = pal[index].rgbRed;
			pw[(y*infoheader.biWidth+x)*planes+1] = pal[index].rgbGreen;
			pw[(y*infoheader.biWidth+x)*planes+2] = pal[index].rgbBlue;
		}
	}
	return TRUE;
}


//----------------------------------------------------------------------------------------
BOOL TexHandler::LoadBMP4bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
	RGBQUAD pal[16];

	if(fread( pal, sizeof(RGBQUAD), infoheader.biClrUsed, f)!=infoheader.biClrUsed)
	{
		return FALSE;
	}

	BYTE*       pData = 0;
	int         lineSz = infoheader.biWidth/2+infoheader.biWidth%2;
	if(lineSz%4)lineSz += 4 - lineSz%4;
	pData = new BYTE[lineSz*infoheader.biHeight];
	if(pData==NULL)
    {	
        return FALSE;
    }
	if(!fread( pData, lineSz*infoheader.biHeight, 1, f))
	{	
		delete [] pData;
		return FALSE; 
	}

    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);
	int planes=3;
	BYTE* pw = p_pBuff[0];	
	for(int y=0; y<infoheader.biHeight; y++)
	{
		for(int x=0; x<infoheader.biWidth; x++)
		{
			int index;
			if(x%2) 
                index = pData[y*lineSz+x/2]&0x0F;
			else 
                index = (pData[y*lineSz+x/2]&0x0F0)>>4;

			pw[(y*infoheader.biWidth+x)*planes+0] = pal[index].rgbRed;
			pw[(y*infoheader.biWidth+x)*planes+1] = pal[index].rgbGreen;
			pw[(y*infoheader.biWidth+x)*planes+2] = pal[index].rgbBlue;
		}
	}
	return TRUE;
}

BOOL TexHandler::LoadBMP8bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
	RGBQUAD pal[256];

	if(fread( pal, sizeof(RGBQUAD), infoheader.biClrUsed, f)!=infoheader.biClrUsed)
	{
		return FALSE;
	}

	BYTE* pData = 0;
	int lineSz = infoheader.biWidth;
	if(lineSz%4)lineSz += 4 - lineSz%4;
	pData = new BYTE[lineSz*infoheader.biHeight];
	if(pData==NULL)
    {	
        return FALSE; 
    }
	if(!fread( pData, lineSz*infoheader.biHeight, 1, f))
	{	
		delete [] pData;
		return FALSE; 
	}
	int planes = 3;
    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);
	BYTE* pw = p_pBuff[0];	
	for(int y=0; y<infoheader.biHeight; y++)
	{
		for(int x=0; x<infoheader.biWidth; x++)
		{
			pw[(y*infoheader.biWidth+x)*planes+0] = pal[pData[y*lineSz+x]].rgbRed;
			pw[(y*infoheader.biWidth+x)*planes+1] = pal[pData[y*lineSz+x]].rgbGreen;
			pw[(y*infoheader.biWidth+x)*planes+2] = pal[pData[y*lineSz+x]].rgbBlue;
		}
	}
	return TRUE;
}

//--------------[]-----------------------------------------------------------------------
// Generates a texture from passed in RGB values
Htex& TexMan::GenTexture(const TCHAR* ptName, int x, int y, int bpp, BYTE* buff, DWORD flags)
{
    Texture&    tex = _textures[ptName];

    if(tex.refcount == 0)
    {
        tex.hTex = (_stx)(x, y, bpp, buff, flags);
        tex.cx  =   x;
        tex.cy  =   y;
        tex.bpp =   bpp;
        ::_tcscpy(tex.filename, ptName);
    }
    ++tex.refcount;
    return tex.hTex;
}


//--------------[adds a texture file and returns a copy of bitmap]-------------------
Htex&  TexMan::AddTextureFileGetImage(const TCHAR* szFileName, 
                             DWORD flags, BYTE** pb, int* dx, 
                             int* dy, int* bpp)
{
    TCHAR    locn[255];

    if(!VALID_FILENAME(szFileName))
		return TexHandler::GDefaultHtexture;
    _tcscpy(locn, szFileName);
    Texture&    tex = _textures[locn];
    if(tex.refcount == 0)
    {
    	TexHandler	t;
	    if(t.LoadFile(szFileName, flags))
        {
            tex.hTex = (_stx)(t.n_x, t.n_y, t.n_bpp, t.Buffer(), flags);
            ::_tcscpy(tex.filename, locn);
            *dx  = tex.cx  = t.n_x;
            *dy  = tex.cy  = t.n_y;
            *bpp = tex.bpp = t.n_bpp;
            tex.pBuff = new BYTE[t.n_x*t.n_y*t.n_bpp];
            ::memcpy(tex.pBuff, t.Buffer(), t.n_x*t.n_y*t.n_bpp);
            *pb    = tex.pBuff;
        }
    }
    ++tex.refcount;
    return tex.hTex;
}

//--------------[adds a texture file/ render wrapper]-------------------------------
Htex& TexMan::AddTextureFile(const TCHAR* szFileName, DWORD flags)
{
    TCHAR    locn[255];

    if(!VALID_FILENAME(szFileName))
		return TexHandler::GDefaultHtexture;
    _tcscpy(locn, szFileName);
    Texture&    tex = _textures[locn];
    if(tex.refcount == 0)
    {
    	TexHandler	t;
	    if(t.LoadFile(szFileName, flags))
        {
            tex.hTex = (_stx)(t.n_x, t.n_y, t.n_bpp, t.Buffer(), flags);
            ::_tcscpy(tex.filename, locn);
            tex.cx  = t.n_x;
            tex.cy  = t.n_y;
            tex.bpp = t.n_bpp;
            tex.hTex.genST   = TGET_GENST(flags);
            tex.genMode      = flags;
            tex.hTex.envMode = 0;

            ++tex.refcount;
            return tex.hTex;
        }
    }else if(tex.refcount)
    {
        ++tex.refcount;
        return tex.hTex;
    }
    return TexHandler::GDefaultHtexture;
}

//--------------[remove a texture file]-------------------------------------------
int TexMan::RemoveTextureFile(const TCHAR* szFileName, BOOL allrefs)
{
    Texture&    tex = _textures[szFileName];
    if(tex.refcount == 0)
        --tex.refcount;
    if(tex.refcount == 0 || allrefs)
	{
        if(_rtx) 
            (_rtx)(&tex.hTex.hTex,1);
        if(tex.pBuff) 
            delete[] tex.pBuff; 
        tex.pBuff=0;
		tex.refcount = 0;
        _textures.erase(szFileName);
        
	}
    return _textures.size();
}

//--------------[save opoerator against map]----------------------------------------
// note that map::operator[xx]= add's the x element
Htex& TexMan::operator[](const TCHAR* szFileName)
{
	return AddTextureFile(szFileName);
}

//--------------[removes texture by id]---------------------------------------------
int TexMan::RemoveTexture(Htex& itex, BOOL allrefs)
{
    map<tstring, Texture>::iterator b = _textures.begin();
	map<tstring, Texture>::iterator e = _textures.end();
	for(;b!=e;b++)
	{
		Texture& ip = (*b).second;
		if(ip.hTex == itex)
		{
			--ip.refcount;
			if(ip.refcount==0 || allrefs)
			{
                if(_rtx) (_rtx)(&itex.hTex,1);

                if(ip.pBuff) delete[] ip.pBuff; ip.pBuff=0;
				ip.refcount = 0;
                _textures.erase(b);
                
				break;
			}
		}
	}
    return _textures.size();
}

//--------------|removes an array of textures|--------------------------------------
int TexMan::RemoveTextures(Htex* itextures, int count, BOOL allrefs)
{
    int  k        = 0;
    UINT *uTexloco = new UINT[count];
    for(int i=0;i<count;i++)
    {
        Htex&  itex = *(itextures+i);

        map<tstring, Texture>::iterator b = _textures.begin();
	    map<tstring, Texture>::iterator e = _textures.end();
	    for(;b!=e;b++)
	    {
		    Texture& ip = (*b).second;
		    if(ip.hTex == itex)
		    {
			    --ip.refcount;
			    if(ip.refcount==0 || allrefs)
			    {
                    uTexloco[k++] = itex.hTex;
                    //if(_rtx) (_rtx)(&itex,1);
                    if(ip.pBuff) delete[] ip.pBuff; ip.pBuff=0;
				    ip.refcount = 0;
                    _textures.erase(b);
                    
				    break;
			    }
		    }
	    }
    }
    if(k)
        if(_rtx) (_rtx)(uTexloco,k);
    return _textures.size();
}

//----------------------------------------------------------------------------------
// cleas all textures
void TexMan::Clear()
{
	TRACEX("TexMan::Clear\r\n");

    map<tstring, Texture>::iterator bt = _textures.begin();
    map<tstring, Texture>::iterator et = _textures.end();
    for(;bt!=et;bt++)
    {
        if((*bt).second.pBuff) 
            delete[] (*bt).second.pBuff; 
        if(_rtx) 
            (_rtx)(&(*bt).second.hTex.hTex,1);
    }
	_textures.clear();
}

//----------------------------------------------------------------------------------
// temp pointer not to be stored
Texture* TexMan::GetTempTexture(const TCHAR* ptName)
{
	map<tstring, Texture>::iterator it = _textures.find(ptName);
	if(it != _textures.end())
	{
		return &(*it).second;
	}
    return 0;
}

//----------------------------------------------------------------------------------
Texture* TexMan::GetTempTexture(Htex& itex)
{
    map<tstring, Texture>::iterator b = _textures.begin();
	map<tstring, Texture>::iterator e = _textures.end();

	for(;b!=e;b++)
	{
		Texture& ip = (*b).second;
		if(ip.hTex == itex)
        {
            return &(*b).second;
        }
    }
    return 0;
}

const TCHAR* TexMan::GetTexName(Htex& itex)
{
   map<tstring, Texture>::iterator b = _textures.begin();
	map<tstring, Texture>::iterator e = _textures.end();

	for(;b!=e;b++)
	{
		Texture& ip = (*b).second;
		if(ip.hTex == itex)
        {
            return (*b).first.c_str();
        }
    }
    return "";
}
void  TexHandler::Invert()
{
    BYTE* line = new BYTE[32768];

	for(int i=0; i < n_y/2; i++ )
	{
		//flip vertical
		memcpy(&line[0], p_pBuff[i], n_bpp * n_x);
		memcpy(p_pBuff[i], p_pBuff[n_y-i-1], n_bpp * n_x);
		memcpy(p_pBuff[n_y-i-1], line, n_bpp * n_x);
	}

	delete[]	line;

}


void TexHandler::AlphaIt(BYTE alpha)
{
	BYTE* pw  = p_pBuff[0];	
    int   sz  = n_x*n_y*4;

    BYTE** pd = Allocate(n_y, sz);
    BYTE* px  = pd[0];	

    for(int i=0,j=0;i<n_size;i+=n_bpp,j+=4)
    {
        px[j]   = pw[i];
        px[j+1] = pw[i+1];
        px[j+2] = pw[i+2];
        if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
            px[j+3] = 0;
        else
            px[j+3] = 255;
        
    }
    n_size=sz;
    n_bpp=4;
    Deallocate();
    p_pBuff = pd;
}


//----------------------------------------------------------------------------------
BYTE* TexHandler::AlphaIt(BYTE* pTemp, int x, int y, int& bpp)
{
	BYTE* pw  = pTemp;	
    int   sz  = x*y*4;

    BYTE* pd  = new BYTE[sz];
    BYTE* px  = pd;	

    for(int i=0,j=0;i<x*y*bpp;i+=bpp,j+=4)
    {
        px[j]   = pw[i];
        px[j+1] = pw[i+1];
        px[j+2] = pw[i+2];
        if(pw[i] < 32 && pw[1+i] < 32 && pw[i+2] < 32)
            px[j+3] = 0;
        else
            px[j+3] = 255;
        
    }
    bpp=4;
    return pd;
}

//----------------------------------------------------------------------------------
// places every color under RGB=16 (black) to 9, and everithing else to WHITE. 
// Creates a black white mask
void TexHandler::MaskBW(BYTE* pb, int sz, int bpp)
{
  	BYTE* pw = pb;	

    for(int i=0;i<sz;i+=bpp)
    {
		if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
		{
			(BYTE)pw[i]   = 255;
			(BYTE)pw[i+1] = 255;
			(BYTE)pw[i+2] = 255;
		}
		else
		{
			(BYTE)pw[i]   = 0;
			(BYTE)pw[i+1] = 0;
			(BYTE)pw[i+2] = 0;
		}
    }
}

void TexHandler::SaveBMP(char *filename)
{
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;

	FILE *sub;
	
	int line_size = n_bpp*n_x;
	if(line_size%4)line_size += 4 - line_size%4;
	
	fileheader.bfType = 0x4D42; // Magic identifier   - "BM"	| identifikacia BMP suboru musi byt "BM"
	fileheader.bfSize = n_bpp*n_x*n_y+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// File size in bytes			| velkos suboru v byte
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// Offset to image data, bytes	| posun na zaciatok dat
	
	infoheader.biSize = sizeof(BITMAPINFOHEADER);	// Header size in bytes			| velkost hlavicky BITMAPINFOHEADER
	infoheader.biWidth = n_x;	// Width of image			| sirka obrazka - sizeX
	infoheader.biHeight = n_y;	// Height of image			| vyska obrazka - sizeY
	infoheader.biPlanes = 1;		// Number of colour planes	| pocet farebnych rovin musi bit 1
	infoheader.biBitCount = 24;		// Bits per pixel			| bitov na pixel moze bit 1,4,8,24
	infoheader.biCompression = 0;	// Compression type			| typ compresie , 0 - bez kompresie
	infoheader.biSizeImage = line_size*infoheader.biHeight ;	// Image size in bytes		| velkost obrazka v byte
	infoheader.biXPelsPerMeter = 0;	// Pixels per meter X		| pixelov na meter v smere x
	infoheader.biYPelsPerMeter = 0;	// Pixels per meter Y		| pixelov na meter v smere y
	infoheader.biClrUsed = 0;		// Number of colours		| pocet  farieb v palete, ak 0 vsetky su pouzivane
	infoheader.biClrImportant = 0;	// Important colours		| dolezite farby v palete, ak 0 vsetky su dolezite
	
	sub = fopen(filename,"wb");
	if(sub==NULL)return;
	fwrite( &fileheader, sizeof(BITMAPFILEHEADER), 1, sub);
	fwrite( &infoheader, sizeof(BITMAPINFOHEADER), 1, sub);

    BYTE* pw = p_pBuff[0];	
	BYTE* p  = new BYTE[line_size*n_x];

	for(int y=0; y<n_y; y++)
	{
		for(int x=0; x<n_x; x++)
		{
            /*
			if(BRG)
			{
				p[y*line_size+x*3+0] = pw[(y*n_x+x)*3+0];
				p[y*line_size+x*3+1] = pw[(y*n_x+x)*3+1];
				p[y*line_size+x*3+2] = pw[(y*n_x+x)*3+2];
			}
			else
			{
            */
				p[y*line_size+x*3+0] = pw[(y*n_x+x)*3+2];
				p[y*line_size+x*3+1] = pw[(y*n_x+x)*3+1];
				p[y*line_size+x*3+2] = pw[(y*n_x+x)*3+0];
			//}
		}
	}
	fwrite( p, line_size*n_y, 1, sub);
	delete[] p;
	fclose(sub);
}

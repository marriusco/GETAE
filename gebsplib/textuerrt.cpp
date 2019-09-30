//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include "system.h"
#include "textuerrt.h"

//---------------------------------------------------------------------------------------
CLR          TextuerRt::_blobPalette[256];
CLR          TextuerRt::_firePalette[256];
TextuerRt*   TextuerRt::_pRt;
int          TextuerRt::_refs = 0;

//---------------------------------------------------------------------------------------
TextuerRt::TextuerRt()
{
     int i; //ISO scoping;//
     TRACEX("TextuerRt\r\n");
     memset(_texts,0,sizeof(_texts));

     memset(_firePalette, 0, sizeof(_firePalette));
     for (i = 0; i < 32; ++i)
     {
          _firePalette[i].r = 0;// << 1;
		  _firePalette[i].g = 0;// << 1;
		  _firePalette[i].b = 0;// << 1;

          _firePalette[i + 32].r = i << 3;
          _firePalette[i + 32].b = 64 - (i << 1);

          _firePalette[i + 64].r = 255;
          _firePalette[i + 64].g = i << 3;

          _firePalette[i + 96].r = 255;
          _firePalette[i + 96].g = 255;
          _firePalette[i + 96].b = i << 2;

          _firePalette[i + 128].r = 255;
          _firePalette[i + 128].g = 255;
          _firePalette[i + 128].b = 64 + (i << 2);

          _firePalette[i + 160].r = 255;
          _firePalette[i + 160].g = 255;
          _firePalette[i + 160].b = 128 + (i << 2);

          _firePalette[i + 192].r = 255;
          _firePalette[i + 192].g = 255;
          _firePalette[i + 192].b = 192 + i;

          _firePalette[i + 224].r = 255;
          _firePalette[i + 224].g = 255;
          _firePalette[i + 224].b = 224 + i;
    }

     for (i = 0; i < 256; i++)
     {
         _blobPalette[i].r = i;
         _blobPalette[i].g = i;
         _blobPalette[i].b = i;
     }
    ++TextuerRt::_refs;
}

//---------------------------------------------------------------------------------------
TextuerRt::~TextuerRt(){}

//---------------------------------------------------------------------------------------
void	   TextuerRt::Clear()
{

    for(int i=0; i< RT_LAST;i++)
	{
        delete _texts[i];
		_texts[i]=0;
	}
    TextuerRt::_refs=0;
}

//---------------------------------------------------------------------------------------
// spin 'type' texture
BOOL    TextuerRt::Update(RT_TYPE type, DWORD dynaFrame)
{
    if((dynaFrame & 0x3) ==0x3 )
    {
        if(_texts[type] && _texts[type]->curFrame != dynaFrame)
        {
            _texts[type]->Spin(dynaFrame,0);
            _texts[type]->curFrame = dynaFrame;
            return 1;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------
// spin all textures
BOOL   TextuerRt::Update(DWORD dynaFrame)
{
    for(int i=0; i< RT_LAST;i++)
    {
        if(_texts[i] && _texts[i]->curFrame != dynaFrame)
        {
            _texts[i]->Spin(dynaFrame,0);
            _texts[i]->curFrame = dynaFrame;
            return 1;
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------
Htex   TextuerRt::CreateBlob(int x, int y, int blbRad, int nBlobs)
{
    if(_texts[RT_BLOB])
        return _texts[RT_BLOB]->texID;// already created. All use the same fire
    RBlobTexture* pFt = new RBlobTexture;

    nBlobs          = min(nBlobs,32);   // max blobs 32
    blbRad          = min(8,blbRad);    // max radius 8

    pFt->xDim       = x;
    pFt->yDim       = y;
    pFt->radius     = blbRad;
    pFt->nBlobs     = nBlobs;
    pFt->pIdxBuffer = new BYTE[x*(y+8)];
    pFt->pRGBBuffer = new BYTE[x*(y+8)*3];

    int bsRad = blbRad * blbRad;
    int bdRad = blbRad * 2,i,j; //ij ISO scoping

    for ( i = -blbRad; i < blbRad; ++i)
    {
        for ( j = -blbRad; j < blbRad; ++j)
        {
            int distance_squared = i * i + j * j;
            if (distance_squared <= bsRad)
            {
                float fraction = (float)distance_squared / (float)bsRad;
                pFt->byBlob[i + blbRad][j + blbRad] = LOBYTE(pow((1.0 - (fraction * fraction)),4.0) * 255.0);
            }
            else
                pFt->byBlob[i + blbRad][j + blbRad] = 0;
        }
    }

    for (i = 0; i < nBlobs; i++)
    {
        pFt->szBlob[i].cx = x/2;
        pFt->szBlob[i].cy = y/2;
    }


    ::memset(pFt->pIdxBuffer, 0, x*y);
    ::memset(pFt->pRGBBuffer, 0, x*y);
    _texts[RT_BLOB] = (RTexture*)pFt;
    pFt->texID = *_pSystem->GenTex("blob-gt", x,y,3,(const char*)pFt->pRGBBuffer,TEX_NORMAL);
    return pFt->texID;
}

//---------------------------------------------------------------------------------------
Htex   TextuerRt::CreateFire(int x, int y, int intens, int heat)
{
    if(_texts[RT_FIRE])
        return _texts[RT_FIRE]->texID;// already created. All use the same fire
    RFireTexture* pFt = new RFireTexture;
    pFt->xDim       = x;
    pFt->yDim       = y;
    pFt->intensity  = intens;
    pFt->heat       = heat;
    pFt->pIdxBuffer =  new BYTE[x*(y+8)];
    pFt->pRGBBuffer =  new BYTE[x*(y+8)*3];
    ::memset(pFt->pIdxBuffer, 0, x*y);
    ::memset(pFt->pRGBBuffer, 0, x*y*3);
    _texts[RT_FIRE] = (RTexture*)pFt;
    pFt->texID = *_pSystem->GenTex("fire-gt", x,y,3,(const char*)pFt->pRGBBuffer,TEX_NORMAL);
    return pFt->texID;
}

//---------------------------------------------------------------------------------------
void    RFireTexture::Spin(DWORD frame, REAL delTime)
{
#ifndef _USRDLL
    assert(curFrame != frame);
#endif //
    int temp,i,j,yWalk,random;

    int MM = 0;


    j = xDim * (yDim- 1);
    for (i = 1; i < xDim-1 ; i++)
    {
        random = 1 + (int)(16.0 * (rand()/(RAND_MAX+1.0)));
        if (random > intensity)
           pIdxBuffer[j + i] = heat;
        else
           pIdxBuffer[j + i] = 0;

        MM = max(MM,j+i);
    }

    for (yWalk = 1; yWalk < yDim-1 ; yWalk++)
    {
        for (i = 1; i < xDim -1 ; ++i)
        {
            if (i <6 || i > xDim - 6)
            {
                temp = pIdxBuffer[j];
                temp += pIdxBuffer[j + 1];
                temp += pIdxBuffer[j - yDim];
                temp /=8;
            }
            else
            {
                temp =  pIdxBuffer[j + i];
                temp += pIdxBuffer[j + i + 1];
                temp += pIdxBuffer[j + i - 1];
                temp += pIdxBuffer[j - yDim + i];
                temp >>= 2;
            }
            if (temp > 1)
                temp -= 1;

			MM = max(MM,j - yDim + i);

            pIdxBuffer[j - yDim + i] = temp;
        }
        j -= yDim;
    }

    BYTE* pByRgb = pRGBBuffer;
    int  tidx;
    for (i = yDim-1;  i >0 ; --i)
    {
        for (j = 0; j < xDim; ++j)
        {
			if(i>yDim-3)
			{
				*pByRgb++=0;
				*pByRgb++=0;
				*pByRgb++=0;
				continue;
			}
            tidx = i * yDim + j;

            *pByRgb++ = TextuerRt::_firePalette[pIdxBuffer[tidx]].r;
            *pByRgb++ = TextuerRt::_firePalette[pIdxBuffer[tidx]].g;
            *pByRgb++ = TextuerRt::_firePalette[pIdxBuffer[tidx]].b;

			MM = max(MM,tidx);
        }
    }

//	TRACEX("%d \r\n", MM);
}

//---------------------------------------------------------------------------------------
void RBlobTexture::Spin(DWORD frame, REAL delTime)
{
#ifndef _USRDLL
    assert(curFrame != frame);
#endif
    int start,i,k;
    int dRadius = radius*2;

    ::memset(pIdxBuffer, 0, xDim*yDim);

    for ( i = 0; i < nBlobs; i++)
    {
        szBlob[i].cx += -2 + (int)(5.0 * (rand()/(RAND_MAX+1.0)));
        szBlob[i].cy += -2 + (int)(5.0 * (rand()/(RAND_MAX+1.0)));
    }

    for (k = 0; k < nBlobs; ++k)
    {
        if (szBlob[k].cx > 0 && szBlob[k].cx < xDim - dRadius &&
            szBlob[k].cy > 0 && szBlob[k].cy < yDim - dRadius)
        {
            start = szBlob[k].cx + szBlob[k].cy * yDim;

            for (i = 0; i < dRadius; ++i)
            {
                for (int j = 0; j < dRadius; ++j)
                {
                    if(pIdxBuffer[start + j] < 255)
                    {
                        if ((int)pIdxBuffer[start + j] + (int)byBlob[i][j] > 255)
                        {
                            pIdxBuffer[start + j] = 255;
                        }
                        else
                            pIdxBuffer[start + j] += byBlob[i][j];
                    }

                }
                start += xDim;
            }
        }
        else
        {
            szBlob[k].cx = xDim/2-radius;
            szBlob[k].cy = xDim/2-radius;
        }
    }

    BYTE* pByWlk  = pIdxBuffer;
    BYTE* pRGBWlk = pRGBBuffer;
    for(int c=0;c<xDim;c++)
    {
        for(int l=0;l<yDim;l++)
        {
            *pRGBWlk++ = TextuerRt::_blobPalette[LOBYTE(*pByWlk)].r;
            *pRGBWlk++ = TextuerRt::_blobPalette[LOBYTE(*pByWlk)].g;
            *pRGBWlk++ = TextuerRt::_blobPalette[LOBYTE(*pByWlk)].b;
            pByWlk++;
        }
    }
}


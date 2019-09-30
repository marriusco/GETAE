//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#include <tchar.h>
#include "system.h"
#include "basecont.h"
#include "beamtree.h"
#include "beamtreesi.h" //script interfaces
#include "inireader.h"


RndStruct       DrawProps;
System          *PS;
//---------------------------------------------------------------------------------------
enum {EX_BADVERSION, EX_BAD_FILE,EX_OUTOFMEMORY, EX_IMAGETOBIG, EX_CANTCREATETEX, EX_TEXARANGEMENT};
#define TEST_PTR(ptr_)  if(0 == ptr_){_lasterror = "Out of memory "; throw EX_OUTOFMEMORY;}
int      BeamTree::_configured = 0;

static void s_destructor(void* pd, int sz)
{
    delete (BeamTree*)pd;
}


BeamTree::BeamTree()
{
    TRACEX("BeamTree::BeamTree()\r\n");
    _bspSysProc  = 0;
    _cbBspProc   = 0;
    _pGamma      = 0;
    _pterrain    = 0;
    _curLeaf     = -2;
    _frmDynamic  = -1;
    _frmStatic   = -1;
    _pLmapRGB    = 0;
    _pPVS        = 0;
    _loaded      = 0;
    _useHwLighting = 0;
    ::memset(_levFilesDir,0,sizeof(_levFilesDir));
    assert(PS);
    _pSystem = (System*)PS;

    _pSystem->RegisterDestructor(s_destructor, this);

}



//---------------------------------------------------------------------------------------
BeamTree::BeamTree(BspProc cbBspProc, void* userData, const TCHAR* pszScriptProc)
{
    TRACEX("BeamTree::BeamTree(...)\r\n");
    //_pSystem->RegisterDestructor(s_destructor, this);
    _bspSysProc  = 0;
    _pGamma      = 0;
    _pterrain    = 0;
    _curLeaf     = -2;
    _frmDynamic  = -1;
    _frmStatic   = -1;
    _pLmapRGB    = 0;
    _pPVS        = 0;
    _loaded      = 0;
    _wantMsgs    = 0xFFFFFFFF;
    _useHwLighting = 0;
    ::memset(_levFilesDir,0,sizeof(_levFilesDir));
    if(pszScriptProc)
    {
        try{
            _bspSysProc = new SquirrelFunction<int>(pszScriptProc);
        }catch(SquirrelError& e)
        {
            _bspSysProc = 0;
            _LOG(LOG_ERROR, "Script: %s \r\n", e.desc);
        }
    }
    _cbBspProc = cbBspProc;
    _pSystem = (System*)PS;
    _Dispatch(this, BM_CREATED,userData,0);
}



int BeamTree::LoadLevel(const char* specificData)
{
    _LOG(LOG_INFO,"BeamTree::LoadLevel(%s)\r\n", specificData);

    int iret = 1;
    
    _pSystem->GetInputSys()->Unaquire();
	if(!_pSystem) return 0;
    if(-1 == _pSystem->SendMessage(SYS_PROGRESS,-1,(long)MKSTR("-Please Wait... Loading %s", specificData)))
		return 0;

    Clear();
    if(0==_pSystem )return -1;
    _pCamera = _pSystem ->GetCamera();
    _prims  = _pSystem ->Primitives();

    memcpy(&DrawProps, _pSystem->DrawProps(), sizeof(DrawProps));
    FlStream fl;
    if(specificData && *specificData)
    {
        TCHAR local[_MAX_PATH];
        _tcscpy(local, specificData);
        PathHandler::FixSlashes(local);

        PathHandler ph(local);

        if(!_tcscmp(ph.Ext(),".ml") || !_tcscmp(ph.Ext(),".gbt"))
        {
            if(!_tcscmp(ph.Ext(),".ml"))
            {
                CDirChange  cd(ph.Path());
                _stprintf(local, "%s%s%s.gbt", ph.JustFile(), BSS, ph.JustFile());
           
                if(fl.Open(local,"rb"))
                {
                    iret =  ReadStream(fl) ? NO_ERROR : -1;
                }
                else
                {
                    _LOG(LOG_WARNING,"Cannot open file (%s)/%d\r\n", local, GetLastError());
                }
            }
            else if(fl.Open(local,"rb"))
            {
                iret =  ReadStream(fl) ? NO_ERROR : -1;
            }
            else
            {
                _LOG(LOG_INFO,"Cannot open file (%s)/%d\r\n", local, GetLastError());
            }
        }
    }
	if(!_pSystem) return 0;
	if(-1==_pSystem ->SendMessage(SYS_PROGRESS,-1,0))
			iret=0;
    return iret;

}

BOOL	BeamTree::Export2DLL(HINSTANCE hi, System* pSystem)
{
    //if(BeamTree::_configured) return 0;
    PS = pSystem;
    return 1;
}

//---------------------------------------------------------------------------------------
BOOL	BeamTree::ReadStream(FlStream& fw)
{
    BOOL        hasImages   = FALSE;
    BOOL        brv         = TRUE;
    BOOL        hasSounds   = FALSE;
    long        fileLength  = fw.Getlength();
    

    _LOG(LOG_INFO,"ReadStream()\r\n" );
    _curfile     = fw.Name();
    _lasterror = "";
    try{

        vvector<BTF_LmapInfo>   lmInfos(1024);
        BTF_Section             fs = {0};
        RawBuff<V3>             ves;

        ReadHeader(fw);   // read header
        int progress = 0;

        while(fs._type != SECT_EOF)
        {
            if(!fw.Read(fs))
            {
                TRACEX("Unespected EOF");
                throw 1;            
            }
			if(!_pSystem)return 0;
			if(-1==_pSystem->SendMessage(SYS_PROGRESS,fw.GetPos(),fileLength))
				return 0;
            
            if((fs._type > SECT_DETAILS) || (fs._type<0 && fs._type!=-1))
                throw 1;

			if(!_pSystem) return 0;
            if(1 == _pSystem->SendMessage(SYS_BSP_READ, (long)&fs, (long)&fw) /*handeled by system*/)
            {
                 continue;
            }
		    // read section by section
            switch(fs._type)
            {
                default:    // not known skip section
                    {
                        BYTE bu;
                        for(size_t i=0;i<fs._bytes;i++)
                            fw.Read(bu);
                    }
                    break;
                case SECT_TERRAIN:
                    _LOG(LOG_INFO,"SECT_TERRAIN\r\n" );
                    assert(_pterrain==0);
                    _pterrain = new Tr_TerTree();
                    _pterrain->p_BspTree=this;
                case SECT_TERRAINGLIGHT:
                case SECT_TERRAINNODES:
                case SECT_TERRAINLEAFS:
                    _LOG(LOG_INFO,"SECT_TERR*\r\n" );
                    assert(_pterrain);
                    _pterrain->Read(fw, fs, _texAssoc,_lmAssoc, lmInfos);
                    break;
                case SECT_EOF:
                    _LOG(LOG_INFO,"SECT_EOF\r\n" );
                    break;
                case SECT_INITGAME:
                    _LOG(LOG_INFO,"SECT_INITGAME\r\n" );
                    break;
                case SECT_SCENE:
                    _LOG(LOG_INFO,"SECT_SCENE\r\n" );
                    fw.Read(_si);
					_si.gravAcc*=100;
                    CreateDefLmaps();
                    //_si.defLumin = ZZGREY;
                    break;
                case SECT_SKYDOM:
                    _LOG(LOG_INFO,"SECT_SKYDOM\r\n" );
                    ReadSkyDomInfo(fw, fs._elems);
                    break;
                case SECT_TEXRGB:
                    _LOG(LOG_INFO,"SECT_TEXRGB\r\n" );
                    hasImages = ReadImages(fw, fs._elems);
                    break;
                case SECT_TEXINFO:
                    _LOG(LOG_INFO,"SECT_TEXINFO\r\n" );
                    if(hasImages)
                    {
                        assert(_texAssoc.size());
                        if(0 == _texAssoc.size())
                        {
                            _lasterror = "Invalid FIle Format. Textures assocications corrupted";
                            throw EX_TEXARANGEMENT;
                        }
                    }
                    do{
                        CDirChange cdPreserve(0);
                        ReadTexInfo(fw, fs._elems);
                    }while(0);
                    break;
                case SECT_LMINFO:
                    _LOG(LOG_INFO,"SECT_LMINFO\r\n" );
                    ReadLightMapInfo(fw, fs._elems, lmInfos);
                    break;
                case SECT_LMRGB:
                    _LOG(LOG_INFO,"SECT_LMRGB\r\n" );
                    ReadLightMapsRGB(fw, fs._elems);
                    BuildLightMaps(lmInfos);
                    TRACEX("There are %d lmaps\r\n",_lmAssoc.size());
                    TRACEX("There are %d linfos\r\n",lmInfos.size());
                    break;
                case SECT_SNDFILE:
                    _LOG(LOG_INFO,"SECT_SNDFILE\r\n" );
                    hasSounds = ReadSoundsData(fw, fs._elems);
                    break;
                case SECT_SNDINFO:
                    _LOG(LOG_INFO,"SECT_SNDINFO\r\n" );
                    ReadSoundsInfo(fw, fs._elems);
                    break;
                case SECT_ITEMSINFO:
                    _LOG(LOG_INFO,"SECT_ITEMSINFO\r\n" );
                    ReadItemsInfo(fw, fs._elems);
                    break;
                case SECT_ITEMS:
                    _LOG(LOG_INFO,"SECT_ITEMS\r\n" );
                    ReadItemSubSection(fw, fs._elems);
                    break;
                case SECT_V3:
                    _LOG(LOG_INFO,"SECT_V3\r\n" );
                    Read3DPoints(fw, fs._elems, ves);
                    break;
                case SECT_PLANES:
                    _LOG(LOG_INFO,"SECT_PLANES\r\n" );
                    ReadPlanes(fw, fs._elems);
                    break;
                case SECT_POLYS:
                    _LOG(LOG_INFO,"SECT_POLYS\r\n" );
                    ReadPolys(fw, fs._elems, ves, lmInfos);
                    ves.Destroy();
                    break;
                case SECT_NODES:
                    _LOG(LOG_INFO,"SECT_NODES\r\n" );
                    ReadNodes(fw, fs._elems);
                    break;
                case SECT_LEAFS:
                    _LOG(LOG_INFO,"SECT_LEAFS\r\n" );
                    ReadLeafs(fw, fs._elems);
                    break;
                case SECT_MODELS:
                    _LOG(LOG_INFO,"SECT_MODELS\r\n" );
                    ReadModels(fw, fs._elems);
                    break;
                case SECT_PVS:
                    _LOG(LOG_INFO,"SECT_PVS\r\n" );
                    ReadPVS(fw, fs._elems);
                    break;
                case SECT_PORTALS:
                    _LOG(LOG_INFO,"SECT_PORTALS\r\n" );
                    ReadPortals(fw, fs._elems);
                    break;
				case SECT_MOTION:
                    _LOG(LOG_INFO,"SECT_MOTION\r\n" );
                    ReadMotions(fw, fs._elems);
                    break;
                case SECT_ZONES:
                    {
                        _LOG(LOG_INFO,"SECT_ZONES\r\n" );
                        _zones.ObjReserve(fs._elems);
                        fw.Readv(_zones._ptr, _zones.ByteSize());
                    }
                    break;
				case SECT_SCRINFO:
					{//skip it
                        _LOG(LOG_INFO,"SECT_SCRINFO\r\n" );
						for(size_t i=0;i<fs._elems;i++)
						{
							BTF_ScrInfo bsi;
							fw.Read(bsi);
                            HandleReadScriptInfo(&bsi);
						}
					}
					break;
				case SECT_SCRDATA:
					{//skip it
                        _LOG(LOG_INFO,"SECT_SCRDATA\r\n" );
						for(size_t i=0;i<fs._elems;i++)
						{
							int textLen;

							fw.Read(textLen);
							BYTE* by = new BYTE[max(4096,textLen)];
							fw.Readv(by,textLen);
                            HandleReadScriptText(by);
							delete[] by;
						}

					}
					break;
				case SECT_ITEMCAT:
					{//not used here
                        _LOG(LOG_INFO,"SECT_ITEMCAT\r\n" );
						BTF_ItemCat	bic  ;
						for(size_t i=0;i<fs._elems;i++)
						{
							fw.Read(bic);
                            HandleReadItemCategory(&bic);
						}
                    }
                    break;
            }
        }
		Post_Prepare();

    }
    catch(SquirrelError& e)
    {
        brv = FALSE;
        _tcscpy(_pSystem->_lastError, e.desc);
        _LOG(LOG_ERROR, "BSP Load: Script: %s \r\n", e.desc);
    }
    catch(int& ex)
    {
        ex;
        brv = FALSE;
        TRACEX("Error In Load BSP File. Invalid Format\r\n");
        _LOG(LOG_ERROR,"Error In Load BSP File: Invalid Format \r\n" );
    }
    catch(...)
    {
        brv = FALSE;
        TRACEX("Exception In Load BSP FIle. Invalid Format\r\n");
        _LOG(LOG_ERROR,"Error In Load BSP File: Unknown\r\n" );
    }

    fw.Close();
	_loaded = brv;
    _LOG(LOG_INFO,"ReadStream()->\r\n" );
    return brv;
}

//---------------------------------------------------------------------------------------
// crete directory to place extracted files from this leve_modelsl. sounds, meshes, textures
void BeamTree::EnsureDirectory(const TCHAR* directory)
{
}


//---------------------------------------------------------------------------------------
BeamTree::~BeamTree()
{
	if(_loaded) 
		Clear();
    _pSystem->RegisterDestructor(0, this);
	_pSystem = 0;
    delete _bspSysProc;
    _bspSysProc = 0;
	TRACEX("BeamTree::~BeamTree()\r\n");    
}

//---------------------------------------------------------------------------------------
Face::~Face()
{
}

//---------------------------------------------------------------------------------------
Tr_TerLeaf::~Tr_TerLeaf()
{
}

//---------------------------------------------------------------------------------------
Tr_TerTree::~Tr_TerTree()
{
    delete []v_heights;
    delete[] g_light;
    g_light = 0;
    v_heights = 0;
    n_glights = 0;
    p_leafs.clear();
    p_nodes.clear();
    v_strips.clear();
}

//---------------------------------------------------------------------------------------
void BeamTree::Clear()
{
    if(_loaded==0)       
        return;
    if(0==_pSystem)
        return;
    _LOG(LOG_INFO,"BeamTree::Clear()\r\n" );

    map<int, Htex>::iterator b = _texAssoc.begin();
    map<int, Htex>::iterator e = _texAssoc.end();
    for(;b!=e;b++)
    {
        _pSystem->DeleteTex(&b->second, 1);
    }
    b = _lmAssoc.begin();
    e = _lmAssoc.end();
    for(;b!=e;b++)
    {
        _pSystem->DeleteTex(&b->second, 1);
    }
    delete _pterrain; 
    _pterrain = 0;
    _portals.Destroy();
	_vertexes.Destroy();
	_normals.Destroy();
	_planes.Destroy();
	_nodes.Destroy();
	_leafs.Destroy();
	_items.Destroy();
	_models.Destroy();
    _texAssoc.clear();
    _lmAssoc.clear();
    _nleafs = 0;
    _ipPoints.clear();
    _lInfo.Clear();

#ifdef HL_
    _detailsRendering = 0;
    _hardLights.clear();
    _hardMats.clear();
    _hlights.clear();
#endif //

    _fogs.clear();
    _debugFaces.clear();
    _trFaces.clear();
    _trMirrors.clear();
	_trBlackMsk.clear();
	_trBothFaces.clear();
    _fogs.clear();
    _walkPoints.clear();
    _motions.clear();
    _skyModels.clear();
    _lasterror="";

    _curLeaf     = -2;
    _frmDynamic  = -1;
    _frmStatic   = -1;
    //_pLmapRGB    = 0;
    //_pPVS        = 0;
    _loaded      = 0;
    _useHwLighting = 0;
    ::memset(_levFilesDir,0,sizeof(_levFilesDir));

	delete[] _pLmapRGB;     _pLmapRGB    = 0;
	delete[] _pPVS;         _pPVS        = 0;
    _rTex.Clear();
    _loaded = 0;
}

//---------------------------------------------------------------------------------------
BTF_Item*   BeamTree::GetItems(size_t& count)
{
    count = _items._size;
    return _items._ptr;
}

//---------------------------------------------------------------------------------------
void	BeamTree::ReadHeader(FlStream& fw)
{
    fw.Read(_fh);

    if(strcmp(_fh.sig, "GGBT"))
    {
        _lasterror = "Not a GBT file";
        throw EX_BAD_FILE;
    }
    if(_fh.version < 200) //
    {
        _lasterror = "Old Version. Please recompile with getic 2.00";
        throw EX_BADVERSION;
    }
}

//---------------------------------------------------------------------------------------
BOOL   BeamTree::ReadImages(FlStream& fw, int count)
{
    BTF_TextureRGB     srgb;
    BYTE*              pBuff = new BYTE[MAX_IMG_SZ];

    TEST_PTR(pBuff);

    _texAssoc[-1] = _defTexID;

    for(int i=0 ; i < count; i++)
    {
        fw.Read(srgb);
        DWORD texFlag		 = srgb.flags & 0xFFFF;
        DWORD compresionFlag = (srgb.flags & 0xFFFF0000) >> 16;
        //assert(srgb.bpp*srgb.cx*srgb.cy - srgb.sz <= (srgb.cy*3));
        assert(srgb.sz < MAX_IMG_SZ);
        if(srgb.sz > MAX_IMG_SZ)
        {
            _lasterror = "Texture files are too big";
            throw EX_IMAGETOBIG;
        }
        if(srgb.sz == 0) //light map is not saved only it's index for now
        {
            _texAssoc[srgb.index] = _defTexID;
            continue;
        }

        fw.Readv(pBuff, srgb.sz);
        Htex* itex = _pSystem->GenTex(MKSTR(_T("%d"),i), srgb.cx,srgb.cy,srgb.bpp,(const char*)pBuff,srgb.flags);
        if((*itex) <= 0)
            _texAssoc[srgb.index] = _defTexID;
        else
            _texAssoc[srgb.index] = *itex;

    }
    delete[] pBuff;
    return count;
}

//---------------------------------------------------------------------------------------
// we have to read this block (!!!) to pass over it
// This gives us texture gfile name and how the render should create the texture
// this happen if the BSP does not stores the texture
void    BeamTree::ReadTexInfo(FlStream& fw, int count)
{
    BOOL            found=0;
    BOOL            hasTex = (_texAssoc.size() > 1);
    BTF_TextureInfo ti;

    for(int i=0; i<count; i++)
    {
        fw.Read(ti);
        if(ti.texname[0]=='@')      
            continue; // light map

        if(!hasTex)
        {
            Htex* itex = _pSystem->GenTexFile(ti.texname, ti.creaFlags);
            if((*itex)<=0)
                _texAssoc[i] = _defTexID;
            else
                _texAssoc[i] = *itex;
        }
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadLightMapInfo(FlStream& fw, int count, vvector<BTF_LmapInfo>& lminfos)
{
    for(int i=0; i < count; i++)
    {
        BTF_LmapInfo    rLm;

        fw.Read(rLm);
        lminfos.push_back(rLm);
    }
}

//---------------------------------------------------------------------------------------
// reads the entire RGB stream
void BeamTree::ReadLightMapsRGB(FlStream& fw, int count)
{
    if(count)
    {
        _pLmapRGB = new BYTE[count];
        TEST_PTR(_pLmapRGB);
        fw.Readv(_pLmapRGB, count);
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::BuildLightMaps(vvector<BTF_LmapInfo>& lminfos)
{
    int     index    = 0;

    BYTE* pHead = _pLmapRGB;
    FOREACH(vvector<BTF_LmapInfo>, lminfos, pLm)
    {
        BTF_LmapInfo& rLm = *pLm;

        // for version
        if(rLm.index >= 0)      // get the proper buffer
        {
            BYTE*   pBuffStr = &_pLmapRGB[rLm.index];
            BYTE*   pBuffRGB = pBuffStr;
            //
            // this lobyte hibyte stuff was added on version BSP_VERSION=201
            //
            SIZE     realsz  = {(int)(rLm.xsz>>8), (int)(rLm.ysz>>8)};
            SIZE     sz = {(int)(rLm.xsz & 0xFF), (int)(rLm.ysz & 0xFF)};
            if(realsz.cx + realsz.cy)
            {
                int        srcOff = 0;
                int        dstOff = 0;
                pBuffRGB = new BYTE[sz.cx * sz.cy * rLm.bpp];
                //expand  bmpsz
                int i=0,j=0;
                for(int y=0; y < realsz.cy; y++)
                {
                    for(int x=0; x < realsz.cx; x++)
                    {
                        int    indexS  = y * (sz.cx*rLm.bpp)     + (x*rLm.bpp);
                        int    indexD  = y * (realsz.cx*rLm.bpp) + (x*rLm.bpp);
                        BYTE*  pCelSrc = &pBuffStr[indexS]; // from stream
                        BYTE*  pCelDst = &pBuffRGB[indexD]; // to dest

                        for(int rgb=0; rgb<rLm.bpp; rgb++)
                        {
                            pCelDst[rgb] = pCelSrc[rgb];
                        }
                    }
                }
            }

            Htex* itex = _pSystem->GenTex(MKSTR(_T("%d"),pBuffStr),
                                                    sz.cx,
                                                    sz.cy,
                                                    rLm.bpp,
                                                    (const char*)pBuffRGB,
                                                    TEX_NORMAL_LM);

            if(pBuffRGB != pBuffStr)
                delete[] pBuffRGB;
            
            if((*itex)<=0)
                _texAssoc[index++] = _defTexID;
            else
                _lmAssoc[index++] = *itex;

            
        }
    }
}


//---------------------------------------------------------------------------------------
void    BeamTree::ReadSoundsInfo(FlStream& fw, int count)
{
    for(int i=0;i<count;i++)
    {
        BTF_SndInfo  bsi;
        fw.Read(bsi);
    }
}

//---------------------------------------------------------------------------------------
BOOL    BeamTree::ReadSoundsData(FlStream& fw, int count)
{
    for(int i=0;i<count;i++)
    {
        BTF_SndFile  sw = {0};
        fw.Read(sw);

        BYTE* wholeFile = new BYTE[sw.length];
        TEST_PTR(wholeFile);
        fw.Readv(wholeFile,sw.length);

        assert(_levFilesDir[0]);
        FlStream fw2;
        if(fw2.Open(MKSTR(_T("%s\\%s"),_levFilesDir,sw.sndname),"wb"))
        {
            fw2.Writev(wholeFile,sw.length);
            fw2.Close();
        }
        delete[]wholeFile;
    }
    return count>0;
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadItemsInfo(FlStream& fw, int count)
{
    _items.ObjReserve(count);
    fw.Readv(_items._ptr, _items.ByteSize());

	for(int i=0;i<count;i++)
	{
        HandleReadItem(&_items[i]);
/*
#ifdef _BSPVIEW
        _pSystem->CreateObject(_items[i]);
#else
        Pscene->CreateObject(_items[i]);
#endif //
*/

        if(_items[i]._type==ITM_LIGTBULB)
        {
#ifdef HL_
            HLight      hl ;
            RenderLight rl;

            memset(&rl,0,sizeof(rl));
            memset(&hl,0,sizeof(hl));

            hl.pos      = _items[i]._pos;
            hl.radius   = _items[i]._ligtbulb._radius;

            rl._colorDiffuse  =_items[i]._ligtbulb._colorD ;
            rl._colorSpecular =_items[i]._ligtbulb._colorS;
            rl._flags         = LIGHT_POINT;
            if(find(_hardLights.begin(), _hardLights.end(), rl)==_hardLights.end())
            {
                hl.index = _hardLights.size();
                _hardLights << rl;
            }
            _hlights << hl;
#endif //
        }
	}
}

//---------------------------------------------------------------------------------------
void    BeamTree::Read3DPoints(FlStream& fw, int count, RawBuff<V3>& ves)
{
    ves.ObjReserve(count);
    fw.Readv(ves._ptr, ves.ByteSize());
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadPlanes(FlStream& fw, int count)
{
    BTF_Plane plane;
    _planes.ObjReserve(count);

    for(int i=0;i<count;i++)
    {
        fw.Read(plane);

        _planes[i]._n = plane.normal;
        _planes[i]._c  = plane.dist;
        _planes[i].TypeIt();
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadPolys(FlStream&   fw,             // file
                            int         count,          // polygons
                            RawBuff<V3>& ves,           // all vertexes
                            vvector<BTF_LmapInfo>& rLm) // lmapinformation
{
    BTF_Poly        bPoly;
    vvector<Vtx>    allvertexes(32768);

    allvertexes.reserve(count*4);       // approx 4 per poly minimizes realocation
    _faces.ObjReserve(count);           // reserve faces

    for(int i=0 ;i < count; i++)
    {
        Face& face = _faces[i];
        fw.Read(bPoly);                                     // read Btf_Poly
        ReadPoly(fw, face, bPoly, allvertexes, ves, rLm);   //
    }
    // strore all vertexes
    _vertexes.ObjReserve(allvertexes.size());
    ::memcpy(_vertexes._ptr, &allvertexes[0], _vertexes.ByteSize());
    allvertexes.clear();
}

//---------------------------------------------------------------------------------------
void BeamTree::ReadPoly(FlStream&       fw,
                        Face&           face,
                        BTF_Poly&       bPoly,
                        vvector<Vtx>&   allvertexes,
                        RawBuff<V3>&    ves,
                        vvector<BTF_LmapInfo>& rLm)
{
    BTF_Vertex      bvx;
    Vtx             vtinc;
    UV              mM0, mM1;
    BOOL            lmisTexture=FALSE;

    face._planeidx    = bPoly.planeIdx;
    face._texanim[0]  = bPoly.tanimA;
    face._brIdx       = bPoly.brIdx;
    face._flags       = bPoly.flags;
    face._flags2      = bPoly.flags2;
    face._texCombine  = bPoly._texp.combine; // ?300
    

    face._firstvx     = allvertexes.size();
    face._vertexcount = bPoly.vxCount;
    face._sliceidx[0] = 0;
    face._sliceidx[1] = 0;
    face._bump		  = bPoly.bump/100;        // bump factor
    face._friction	  = bPoly.friction/100;    // friction factor

    face._color       = bPoly.colorD;

    if(_pGamma)
    {
        face._color.r   = (BYTE)_pGamma[face._color.r*3];
        face._color.g   = (BYTE)_pGamma[face._color.g*3+1];
        face._color.b   = (BYTE)_pGamma[face._color.b*3+2];

    }

    face._box.Reset();
    face._lmapaxes[0] = bPoly.lmAxes[0];
    face._lmapaxes[1] = bPoly.lmAxes[1] - bPoly.lmAxes[0];
    face._lmapaxes[2] = bPoly.lmAxes[2] - bPoly.lmAxes[0];

    RenderMaterial rm;
    memset(&rm,0,sizeof(rm));
    rm._colorDiffuse   = bPoly.colorD;
    rm._colorEmmisive  = bPoly.colorE;
    rm._colorSpecular  = bPoly.colorS;
    rm._shiness        = bPoly.shiness;

#ifdef HL_
    if(find(_hardMats.begin(), _hardMats.end(), rm) == _hardMats.end())
    {
        face._dlights = _hardMats.size();
        _hardMats << rm;
    }
#endif //

    int ts = (face._flags & FACE_CUSTLM); // 300

    _rTex.Initialize(_pSystem);
    if(face._flags & FACE_TG_FIRE )
    {

        int sx = face._texanim[0].u;
        int sy = face._texanim[0].v;
        face._textures[0] = _rTex.CreateFire(32,32,10,192);
    }
    else if(face._flags & FACE_TG_BLOB )
    {
        int sx = face._texanim[0].u;
        int sy = face._texanim[0].v;
        face._textures[0] = _rTex.CreateBlob(32,32,8,8);
    }
    else if(face._flags & FACE_TG_PLASMA ||
            face._flags & FACE_TG_WATER ||
            face._flags & FACE_TG_LAVA )
    {
        face._textures[0] = _defTexID;
    }
    else
    {
        for(int k=0; k < 4; k++)
        {
            if(k==1)
            {
                if(ts==1) // overwritten Lmap (custom lmap)
                {
                    if(_texAssoc.find(bPoly._texp.textures[1]) != _texAssoc.end()) //300
                    {
	                    face._textures[1] = _texAssoc[bPoly._texp.textures[1]]; //300
                        lmisTexture = TRUE;
                    }
                }
            }
            else if(_texAssoc.find(bPoly._texp.textures[k]) != _texAssoc.end()) //300
            {
	            face._textures[k] = _texAssoc[bPoly._texp.textures[k]];         //300  
            }
            face._textures[k].genST = bPoly._texp.texApply[k];
        }


        if(face._flags & FACE_TA_FRAMING)
        {

	        if(face._texanim[ts].u == 0)
                face._texanim[ts].u = 2;

	        if(face._texanim[ts].v == 0)
                face._texanim[ts].v = 2;
        }

    }

    if(!lmisTexture)
    {
        if(face._flags & FACE_FULLBRIGHT || face._flags & FACE_NOLIGTMAP)
        {
            face._textures[1] = _deftexFB;  // full bright lmap created here
        }
        else if(_lmAssoc.find(bPoly._texp.textures[1]) != _lmAssoc.end()) // 300 default light map (buil dby global ilum)
        {
            face._textures[1]       = _lmAssoc[bPoly._texp.textures[1]]; //300
            face._lmapsize.cx       = rLm[bPoly._texp.textures[1]].xsz & 0xFF;  //300
            face._lmapsize.cy       = rLm[bPoly._texp.textures[1]].ysz & 0xFF;  //300
            face._pdynalights[0]    = &_pLmapRGB[rLm[bPoly._texp.textures[1]].index]; // hold a pointer to initial lmap data
            assert(face._lmapsize.cx <= 128 && face._lmapsize.cy <= 128);
        }
        else
        {
            face._textures[1] = _deftexLM; //default lmap create here based on scene global ilum
        }
    }
    
    // get the lmap texture min max coordinates
    mM0.setval(INFINIT);
    mM1.setval(-INFINIT);

    for(int j=0;j<bPoly.vxCount;j++)
    {
        fw.Read(bvx);

        vtinc._xyz    = ves[bvx.vxIdx];

        vtinc._uv[0] = bvx.tc0;
        vtinc._uv[1] = bvx.tc1;
        vtinc._uv[2] = bvx.tc2;
        vtinc._uv[3] = bvx.tc3;

        if(lmisTexture) 
        {
            vtinc._uv[1]=vtinc._uv[0]; // use first stahe coord tex 
        }

        vtinc._nrm    = V0;

        if(face._flags & FACE_TAMASK)
        {
            //vtinc._uv[1].u = vtinc._uv[0].u;
            //vtinc._uv[1].v = vtinc._uv[0].v;
            face._initalUV << vtinc._uv[0]; // store intial tex coord
        }

        mM0.domin(bvx.tc1);     //lmap minmax
        mM1.domax(bvx.tc1);

	    // face vertex color
	    if(face._color.r || face._color.g ||face._color.b)
	    {
		    vtinc._rgb[0] = face._color.r;
		    vtinc._rgb[1] = face._color.g;
		    vtinc._rgb[2] = face._color.b;
		    vtinc._rgb[3] = face._color.a;
	    }
	    else
	    {
		    vtinc._rgb[0] = bvx.clr.r;
		    vtinc._rgb[1] = bvx.clr.g;
		    vtinc._rgb[2] = bvx.clr.b;
		    vtinc._rgb[3] = bvx.clr.a;
	    }
        allvertexes.push_back(vtinc);
        face._box.AddPoint(vtinc._xyz);
    }

    //
    // fix normals close to 1
    //
    /*
    Plane tp(allvertexes[face._firstvx]._xyz,
             allvertexes[face._firstvx+1]._xyz,
             allvertexes[face._firstvx+2]._xyz);

    if(tp._n.x<1.0 &&  tp._n.x>.9)
    {
        face._box.Reset();

        REAL x = allvertexes[face._firstvx]._xyz.x;
        face._box.AddPoint(allvertexes[face._firstvx]._xyz);
        for(int v = face._firstvx+1; v<face._firstvx+face._vertexcount;v++)
        {
            allvertexes[v]._xyz.x=x;
            face._box.AddPoint(allvertexes[v]._xyz);
        }
    }
    else if(tp._n.y<1.0 &&  tp._n.y>.9)
    {
        face._box.Reset();
        REAL y = allvertexes[face._firstvx]._xyz.y;
        face._box.AddPoint(allvertexes[face._firstvx]._xyz);
        for(int v = face._firstvx+1; v<face._firstvx+face._vertexcount;v++)
        {
            allvertexes[v]._xyz.y=y;
            face._box.AddPoint(allvertexes[v]._xyz);
        }

    }
    else if(tp._n.z<1.0 &&  tp._n.z>.9)
    {
        face._box.Reset();
        REAL z = allvertexes[face._firstvx]._xyz.z;
        face._box.AddPoint(allvertexes[face._firstvx]._xyz);
        for(int v = face._firstvx+1; v<face._firstvx+face._vertexcount;v++)
        {
            allvertexes[v]._xyz.z=z;
            face._box.AddPoint(allvertexes[v]._xyz);
        }

    }
    tp.CalcNormal(  allvertexes[face._firstvx]._xyz,
                    allvertexes[face._firstvx+1]._xyz,
                    allvertexes[face._firstvx+2]._xyz);
    
*/


     

    face._box.Expand(16);

    //
    // Light map vertex coordinates and texture axes
    //
    if(!lmisTexture && bPoly._texp.textures[1] >=0 )
    {

        if(bPoly._texp.textures[1] < rLm.size())////300
        {
            BTF_LmapInfo* plMi = &rLm[bPoly._texp.textures[1]]; //300

            // cache lm step
            face._lmstep.u   = 1.0 / face._lmapsize.cx;
            face._lmstep.v   = 1.0 / face._lmapsize.cy;

            // see face mins maxes
            face._lmapbox[0].cx =  floor(mM0.u*face._lmapsize.cx);
            face._lmapbox[0].cy =  floor(mM0.v*face._lmapsize.cy);

            face._lmapbox[1].cx =  ceil(mM1.u*face._lmapsize.cx);
            face._lmapbox[1].cy =  ceil(mM1.v*face._lmapsize.cy);
        }
    }
}
//---------------------------------------------------------------------------------------
void    BeamTree::ReadNodes(FlStream& fw, int count)
{
    BTF_Node    bN;

    _nodes.ObjReserve(count);
    for(int i=0; i<count;i++)
    {
        fw.Read(bN);

        _nodes[i]._visFrame     = -1;
        _nodes[i]._bbox         = bN.bbox;
        _nodes[i]._leafIdx      = bN.leafIdx;
        _nodes[i]._nodesIdx[0]  = bN.nodeidx[0];
        _nodes[i]._nodesIdx[1]  = bN.nodeidx[1];
        _nodes[i]._parentIdx    = bN.idxParent;
        _nodes[i]._planeIdx     = bN.planeIdx;
        _nodes[i]._flags        = bN.flags;
        _nodes[i]._cluster      = bN.cluster;
        _nodes[i]._zone         = -1; 
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadLeafs(FlStream& fw, int count)
{
    int         i,j;
    BTF_Leaf    bL;

    _nleafs  =  count;
    _leafs.ObjReserve(count);
    // do this to have cluster pvs in one countinous block of memory as leafs PVS
    map<int, vector<int> >  clusterpvs;

    for(i=0; i<count;i++)
    {
        fw.Read(bL);

        _leafs[i]._visFrame   = -1;
        _leafs[i]._content   = bL.flags;
        _leafs[i]._pvs       = bL.pvxIdx;
        _leafs[i]._nodeIdx   = bL.nodeIdx;
        _leafs[i]._firstPoly = bL.firstPoly;
        _leafs[i]._polys     = bL.polys;
        _leafs[i]._firstItem = bL.firstItem;
        _leafs[i]._items     = bL.items;
        _leafs[i]._cluster   = bL.cluster;  // new code

        //obsolette {{
        if(bL.portals)
        {
            _leafs[i]._portals.ObjReserve(bL.portals);
            // portals
            for(j=0;j<bL.portals;j++)
            {
                fw.Read(_leafs[i]._portals[j]);
            }
        }

        if(bL.models)
        {
            // detail model (discarded)
            for(j=0;j<bL.models;j++)
            {
			    int k;
                fw.Read(k);
            }
        }
        //obsolette }}


		// build leaf sides for any leaf sides collision detection modes
		vvector<int>	uniques(1024);
		Face* pFace = &_faces[_leafs[i]._firstPoly];
		for(j=0;j<_leafs[i]._polys;j++,pFace++)
		{
			Plane plane = _planes[pFace->_planeidx];
			uniques.push_unique(pFace->_planeidx);
		}
		_leafs[i]._sides.ObjReserve(uniques.size());
		j=0;
		FOREACH(vvector<int>,uniques,ppidx)
        {
			_leafs[i]._sides[j++]=*ppidx;       //plane indexes
        }
    }
}


//---------------------------------------------------------------------------------------
void    BeamTree::ReadModels(FlStream& fw, int count)
{
    BTF_ModelInfo bM;
    _models.ObjReserve(count);
    for(int i=0; i < count; i++)
    {
        fw.Read(bM);
        strcpy(_models[i]._name,bM.name);
        _models[i]._onterrain   = 0;
        _models[i]._visFrm      = -1;
        _models[i]._rootNode    = bM.firstNode;
        _models[i]._firstLeaf   = bM.firstLeaf;
        _models[i]._leafs       = bM.leafs;
        _models[i]._flags       = bM.flags;
        _models[i]._props       = bM.props;
		_models[i]._motionIndex = bM.dynamicIdx;
        _models[i]._rotVx       = V3(1.0,1.0,1.0);
        _models[i]._index       = i;
        #pragma message("rotation skydom")
	
    }
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadPVS(FlStream& fw, int count)
{
    _pPVS   = new BYTE[count];
    TEST_PTR(_pPVS);

    fw.Readv(_pPVS,count);
}

void    BeamTree::SetGamma(BYTE* p768rgb)
{
    _pGamma = p768rgb;
}

//---------------------------------------------------------------------------------------
void    BeamTree::ReadPortals(FlStream& fw, int count)
{
    _portals.ObjReserve(count);

    BTF_Portal  notCare;
    for(int i=0; i < count; i++)
    {
        fw.Read(notCare);
        
        for(int j=0;j<notCare.vxes;j++)
        {
            V3 r;
            fw.Read(r);
            _portals[i]._center+=r;
        }
        _portals[i]._center /= (REAL)notCare.vxes;

        _portals[i]._leaf[0]=notCare.bLeaf;
        _portals[i]._leaf[1]=notCare.fLeaf;
        _portals[i]._visframe=-1;

    }
}

//---------------------------------------------------------------------------------------
// build the motions dada structure
void	BeamTree::ReadMotions(FlStream& fw, int count)
{
    BTF_Motion          motion;
    BTF_MotionNode      motionNode;

    for(int i=0; i< count; i++)
    {
        fw.Read(motion);

        MotionPath  mp(motion.flags);
        for(int j=0; j<motion.cntNodes; j++)
        {
            fw.Read(motionNode);
            mp.AddNode(motionNode.vPos, motionNode.vDir,
                       /*motionNode.pauseTime*/5.0, 5000.0);
        }
        _motions.push_back(mp);
    }
}

//---------------------------------------------------------------------------------------
void BeamTree::ReadSkyDomInfo(FlStream& fw, int count)
{
    BTF_SkyDom  sky;

    fw.Read(sky);

    if(!_Noval(sky.skyModelIdx))
    {
        _skyModels.push_back(sky.skyModelIdx);
    }

    
    for(int i=0;i<sky.subModels;i++)
    {
        int isubm;
        fw.Read(isubm);
        _skyModels.push_back(isubm);
    }
}

//---------------------------------------------------------------------------------------
void BeamTree::CreateDefLmaps()
{
    BYTE dBits[12] = {255,0,0, 0,255,0, 0,0,255, 255,0,255};
    _defTexID  = *_pSystem->GenTex("def1_tex", 2,2,3,(const char*)dBits,TEX_NORMAL);
    if(_defTexID <=0)
    {
        _lasterror = "Not enough resources to generate textures";
        throw EX_CANTCREATETEX;
    }

    BYTE byFullbright[8*8*3];
    ::memset(byFullbright,255, 8*8*3);

    _deftexFB  = *_pSystem->GenTex("fullb_tex", 8,8,3,(const char*)byFullbright,TEX_NORMAL);
    if(_defTexID <=0) {
        _lasterror = "Not enough resources to generate textures";
        throw EX_CANTCREATETEX;
    }

    BYTE* pWlk = byFullbright;
    for(int i=0;i<16*16;i++)
    {
        *pWlk = _si.defLumin.r;
        *pWlk = _si.defLumin.g;
        *pWlk = _si.defLumin.b;
    }

    _deftexLM  = *_pSystem->GenTex("def_lmtex", 8,8,3,(const char*)byFullbright,TEX_NORMAL);
    if(_deftexLM <=0) {
        _lasterror = "Not enough resources to generate textures";
        throw EX_CANTCREATETEX;
    }
}

//---------------------------------------------------------------------------------------
BspModel*   BeamTree::GetModel(char* name)
{
	BspModel* pModel =  &_models[0];
	for(int i=0; i< _models._size;i++,pModel++)
	{
		if(!strncmp(pModel->_name,name, strlen(name)))
			return pModel;
	}
	return 0;
}

//---------------------------------------------------------------------------------------
void	BeamTree::FinalizeClustersPVS()
{
}


//---------------------------------------------------------------------------------------
void	BeamTree::RegisterTrigger(int leaf, int trigger)
{
    if(_leafs[leaf]._trigs.Size()<32)
	    _leafs[leaf]._trigs.Push(trigger);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// 300

//---------------------------------------------------------------------------------------
void    Tr_TerTree::Read(FlStream& fw,  BTF_Section& fs, 
                           map<int, Htex>& texAssoc,    
                           map<int, Htex>&   lmAssoc,
                           vvector<BTF_LmapInfo>& lmInfo)
{
    switch(fs._type)
    {
        default: 
            assert(0);
            break;
        case SECT_TERRAIN:
            _ReadTerrInfo(fw,fs,texAssoc,lmAssoc, lmInfo);
            break;
        case SECT_TERRAINGLIGHT:
            _ReadGLight(fw,fs);
            break;
        case SECT_TERRAINNODES:
            _ReadNodes(fw,fs);
            break;
        case SECT_TERRAINLEAFS:
            _ReadLeafs(fw,fs,lmAssoc,lmInfo);
            break;
    }
}

//---------------------------------------------------------------------------------------
void    Tr_TerTree::_ReadTerrInfo(FlStream& fw,  BTF_Section& fs,  
                           map<int, Htex>& texAssoc,    
                           map<int, Htex>&   lmAssoc,
                           vvector<BTF_LmapInfo>& lmInfo)
{
    BTF_Terrain_Tree bt;

    fw.Read(bt);
    _texCombine  = bt._texp.combine;
    for(int k=0; k<4; k++)
    {
        if(texAssoc.find(bt._texp.textures[k]) != texAssoc.end())
        {
            _textures[k] = texAssoc[bt._texp.textures[k]];
            _texCombine|=(1<<k);
        }else if(k==1)
        {
            if(lmAssoc.find(bt._texp.textures[k]) != lmAssoc.end())
            {
                _textures[k] = lmAssoc[bt._texp.textures[k]];
                _texCombine|=(1<<k);
            }
            else  if(texAssoc.find(bt._texp.textures[k]) != texAssoc.end())
            {
                 _textures[k] = texAssoc[bt._texp.textures[k]];
                 _texCombine|=(1<<k);
            }
        }
        _textures[k].genST = bt._texp.texApply[k];
    }
    

    _flags   =  bt.flags;
    _colorS  = bt.colorS;      
    _colorD  = bt.colorD;     
    _colorE  = bt.colorE;       
    _shiness = bt.shiness;
    _tanim[0]= bt._tanim[0];
    _tanim[1]= bt._tanim[1];
    _bump    = (REAL)bt.bump/100.0f;
    _friction= (REAL)bt.friction/100.0f;
    b_box    = bt.box; 
    z_tiles.cx = bt.tiles[0];    
    z_tiles.cy = bt.tiles[1];     

    delete []    v_heights;
    v_heights = new REAL[bt.heightMap];
    BYTE byval;
    for(int i=0; i< bt.heightMap;i++)
    {
        fw.Read(byval);//
        v_heights[i] = (REAL)byval;
    }
}

//---------------------------------------------------------------------------------------
void    Tr_TerTree::_ReadGLight(FlStream& fw,  BTF_Section& fs)
{
    delete[] g_light;
    n_glights = 0;

    CLRNOA cr;
    g_light = new CLRNOA[fs._elems];
    n_glights = fs._elems;

    for(int i=0 ;i<fs._elems;i++)
    {
        if(!fw.Read(g_light[i]))
            throw -1;
    }
}

//---------------------------------------------------------------------------------------
void    Tr_TerTree::_ReadNodes(FlStream& fw,  BTF_Section& fs)
{
    BTF_Terrain_Node    btn;
    Tr_TerNode          tn;    
    for(int i=0 ;i<fs._elems;i++)
    {
        fw.Read(btn);
        tn.n_idx        = i;
        tn.l_idx        = btn.lidx;
        tn.b_box        = btn.bbox;
        tn.i_nodes[0]   = btn.inodes[0];   
        tn.i_nodes[1]   = btn.inodes[1];   
        tn.i_parentNode = btn.iparentNode;
        tn.x_flags      = 0; 
        tn.d_split      = btn.dsplit;    // direction split 0 on x 1 on (z/y)
        tn._visframe    = -1;
	    tn._visframe1   = -1;
        p_nodes << tn;
    }
}

//---------------------------------------------------------------------------------------
void    Tr_TerTree::_ReadLeafs(FlStream& fw,  BTF_Section& fs, 
                                map<int, Htex>&   lmAssoc,
                                vvector<BTF_LmapInfo>& lmInfo)
{
    BTF_Terrain_Leaf    blt;
    Tr_TerLeaf          tl;
    for(int i=0 ;i<fs._elems;i++)
    {
        fw.Read(blt);

        tl.s_tiles[0].cx   = blt.tiles[0];
        tl.s_tiles[0].cy   = blt.tiles[1];
        tl.s_tiles[1].cx   = blt.tiles[2];
        tl.s_tiles[1].cy   = blt.tiles[3];
        tl.n_idx           = blt.nidx;
        tl.t_idx           = i;
        tl.x_flags         = blt.xflags;
        tl._dlights        = 0;

        memset(tl._stripStart,0,sizeof(tl._stripStart));
        memset(tl.n_strips,0,sizeof(tl.n_strips));
        if(lmAssoc.find(blt.lightMapInfo) != lmAssoc.end())
        {
            tl.l_text = lmAssoc[blt.lightMapInfo];
            BTF_LmapInfo& pli = lmInfo[blt.lightMapInfo];

	        tl._lmapsize.cx    = pli.xsz & 0xFF;
            for(int k=0; k<16; k++)
                tl._stripStart[k]     = 0;
            tl._lmapsize.cy    = pli.ysz;
/*
            tl._lmapaxes[0]    = ;
            tl._lmapaxes[1]    = ;
            tl._lmapaxes[2]    = ;
            tl._lmapbox[0].cx  =0 ;
            tl._lmapbox[0].cx  =0 ;    
            tl._lmapbox[1].cx  =0 ;
            tl._lmapbox[1].cy  =0 ;
            tl._lmstep         =0 ;
*/
        }
        tl._visframe       =-1;
	    tl._visframe1      =-1;
        p_leafs << tl;
    }
}

//---------------------------------------------------------------------------------------
int	    Tr_TerTree::GetCurrentLeaf(const V3& pov, int nodeIdx)
{
    FOREACH(vvector<Tr_TerLeaf>, p_leafs, pleaf)
    {
        if(p_nodes[pleaf->n_idx].b_box.ContainPoint(pov))
            return pleaf->t_idx;
    }
    return -1;
}

//---------------------------------------------------------------------------------------
// just packes the triangle strips/leaf. if you received uncompiled 
// handle it as you wish.
void    Tr_TerTree::PackVxes()
{
    /*
    // If you did not save nodes leafs in getic the you may do youyr compiolation or preparatio
    // of terain here. We are using a strip in eac leaf
    // v_strips
    const SIZE  t   = {z_tiles.cx,z_tiles.cy};
    const V3&   c   = b_box._min;
    const V3&   ex  = b_box.GetExtends();
    const REAL  xS  = (ex.x) / (REAL)t.cx;
    const REAL  zS  = (ex.z) / (REAL)t.cy;
    const BOOL  bG  = g_light.size()!=0;
    int   x,z;
    Vtx   v0,v1;  
    UV    lMsz(z_tiles.cx*xS, z_tiles.cy*zS);
    REAL  fx,fx1,fz;
    int   k = 0;

    FOREACH(vvector<Tr_TerLeaf>, p_leafs, pLeaf)
    {
        const SIZE& s   = pLeaf->s_tiles[0];
        SIZE  e   = pLeaf->s_tiles[1];

        k  = 0;
        for(x=s.cx; x < e.cx; x++)
        {
            assert(x-s.cx<32);

            pLeaf->_stripStart[k] = this->v_strips.size();
            pLeaf->n_strips[k]    = 0;

            fx   = x * xS;
            fx1  = (x+1) * xS;
            v0._xyz.x = c.x + (fx);
            v1._xyz.x = c.x + (fx1);

            for(z=s.cy; z <= e.cy; z++)
            {
                fz = z * zS;
                v0._xyz.z = c.z + (fz);
                v0._xyz.y = *(element(&v_heights[0], x, z, t.cx));
                v0._xyz.y /= 255.0;
                v0._xyz.y *= ex.y;
                v0._xyz.y += c.y;

                v1._xyz.z = v0._xyz.z;
                v1._xyz.y = *(element(&v_heights[0], (x+1), z, t.cx));
                v1._xyz.y /= 255.0;
                v1._xyz.y *= ex.y;
                v1._xyz.y += c.y;

                if(!bG)
                {
                    if(g_light.size())
                        v0._rgb = *(element(&g_light[0], x, z, t.cx));
                    else
                        v0._rgb = p_BspTree->GetSceneInfo()->defLumin;
                }
                else
                {
                    v0._uv[1] = UV(fx/lMsz.u , 1-fz/lMsz.v);
                }
                if(_flags & TERR_UNTILEDTEX0)
                {
                    v0._uv[0] = UV((v0._xyz.x-c.x)/ex.x , 1-(v0._xyz.z-c.z)/ex.z);
                }
                else
                {
                    if((z&1)==0)
                        v0._uv[0] = UV(0,0);
                    else
                        v0._uv[0] = UV(0,1);
                }

                if(bG)
                {
                    if(g_light.size())
                        v1._rgb = *(element(&g_light[0], x+1, z, t.cx));
                    else
                        v1._rgb = p_BspTree->GetSceneInfo()->defLumin;

                }
                else
                {
                    v1._uv[1] = UV(fx1/lMsz.u , 1-fz/lMsz.v);
                }
                if(_flags & TERR_UNTILEDTEX0)
                {
                    v1._uv[0] = UV((v1._xyz.x-c.x)/ex.x , 1-(v1._xyz.z-c.z)/ex.z);
                }
                else
                {
                    if((z&1)==0)
                        v1._uv[0] = UV(1,0);
                    else
                        v1._uv[0] = UV(1,1);
                }

                if(x==s.cx){
                    v0._rgb = ZRED;
                    v1._rgb = ZGREEN;
                }
                else 
                {
                    v0._rgb = ZWHITE;
                    v1._rgb = ZWHITE;
                }

                this->v_strips << v0;
                this->v_strips << v1;
                pLeaf->n_strips[k]+=2;
            }
            k++;
        }
        
        TRACEX("leaf");
    }
    */

}

int   BeamTree::_FindZone(const V3& pos)
{
    return 0;
}

//---------------------------------------------------------------------------------------
void	BeamTree::Post_Prepare()
{

    FinalizeClustersPVS();

    //
    //  link models / leafs for visibility / leaf
    //
    map<int,vvector<int> >    modsperleaf;
    map<int,vvector<int> >    modsperleafTer;

	if(_models._size > 1)
	{
		BspModel* pModel =  &_models[1];
		for(int i=1; i< _models._size;i++,pModel++)
		{
            // 300 (a) and (b)
            pModel->_bx      = _nodes[pModel->_rootNode]._bbox;
            pModel->_trpos   = pModel->_bx.GetCenter();
            pModel->_origin  = pModel->_trpos;

            //
            // if touches terrain box patition this on terrain leaf
            //
            if(_pterrain && _pterrain->b_box.ContainBox(_models[i]._bx))
            {
                _models[i]._onterrain = 1;
                pModel->_leaf    = _pterrain->GetCurrentLeaf(pModel->_trpos);
                pModel->_bspNode = 0;
                if(pModel->_leaf >= 0)
                {
                    pModel->_dirty = 1;
                    modsperleafTer[pModel->_leaf] << i;
                }
            }
            else
            {
                _models[i]._onterrain = 0;
                             
                pModel->_leaf     = GetCurrentLeaf(pModel->_trpos);
                pModel->_bspNode  = GetCurentNode(0, pModel->_bx);
                pModel->_nCluster = _nodes[pModel->_bspNode]._cluster;
                if(pModel->_leaf>=0)
                {
                    modsperleaf[pModel->_leaf] << i;
                    pModel->_dirty = 1;
                }
            }
		}
	}

    for(map<int,vvector<int> >::iterator b = modsperleaf.begin();
                                         b!= modsperleaf.end();
                                         ++b)
     {
           const int&       nleaf   = (*b).first;
           vvector<int>&    models  = (*b).second;
           BtLeaf&          leaf    =_leafs[nleaf];

           leaf._models.ObjReserve(models.size());
           int j=0;
           FOREACH(vvector<int>, models, m)
           {
               leaf._models[j++]=(*m);
           }
     }

     for(map<int,vvector<int> >::iterator b = modsperleafTer.begin(); b!= modsperleafTer.end(); ++b)
     {
           const int&       nleaf   = (*b).first;
           vvector<int>&    models  = (*b).second;
           Tr_TerLeaf&      leaf    = _pterrain->p_leafs[nleaf];

           leaf._models.ObjReserve(models.size());
           int j=0;
           FOREACH(vvector<int>, models, m)
           {
               leaf._models[j++]=(*m);
           }
     }

     
    if(_pterrain)
    {
        _pterrain->_landingLeaf = GetCurrentLeaf(_pterrain->b_box.GetCenter());
        _pterrain->PackVxes();
    }
    if(_zones._size)
        AssignZones();

    HandleSetSceneInfo(&_si);

}


void BeamTree::AssignZones()
{
	BspModel* pModel =  &_models[0];
	for(int i=0; i< _models._size;i++,pModel++)
	{
        if(i==0)
        {
            BtLeaf* pLeaf = &_leafs[0];
            for(int i=0;i <_leafs._size; i++,pLeaf++)
            {
                pLeaf->_nZone = _FindZone(_nodes[pLeaf->_nodeIdx]._bbox.GetCenter());
            }
        }
        else
        {
            //zone it at model level
            pModel->_nZone = _FindZone(pModel->_trpos);
        }
    }

    if(_pterrain)
    {
        FOREACH(vvector<Tr_TerLeaf>, _pterrain->p_leafs, pleaf)
        {
            Tr_TerLeaf& tl = *pleaf;
        }
    }
}

long    BeamTree::HandleReadItem( BTF_Item* pbu)
{
    // remfix textures as for the polygons
    for(int i=0;i<5;i++)
    {
        if(!_Noval(pbu->_texp.textures[i]))
        {
            if(_texAssoc.find(pbu->_texp.textures[i]) != _texAssoc.end())
            {
                pbu->_texp.textures[i] = _texAssoc[pbu->_texp.textures[i]];
            }
        }
    }
    if(_wantMsgs &WANT_BSP_LOAD)
        return this->_Dispatch(this, BM_READITEM, pbu, 0);
    return 0;
}

long    BeamTree::HandleReadItemCategory( BTF_ItemCat* ic)
{
    if(_wantMsgs &WANT_BSP_LOAD)
        return this->_Dispatch(this, BM_READCATEGORY, ic,0);
    return 0;
}

long    BeamTree::HandleModelCollision(BspModel* pm, SystemData* psd)
{
    if(_wantMsgs & WANT_BSP_MODELMOVE)
        return this->_Dispatch(this, BM_COLLIDEMODEL, pm, psd);
    return 0;
}

long    BeamTree::HandleMovingModel(const MotionPath* pp, BspModel* pm, SystemData* )
{
    if(_wantMsgs & WANT_BSP_MODELMOVE)
        return this->_Dispatch(this, BM_MOVEMODEL, pp, pm) ;
    return 0;
}

long    BeamTree::HandleReadScriptInfo(const BTF_ScrInfo* bsi)
{
    if(_wantMsgs & WANT_BSP_LOAD)
        return this->_Dispatch(this, BM_SCRIPTINFO, bsi, 0);
    return 0;
}

long    BeamTree::HandleReadScriptText(const BYTE* pd)
{
    if(_wantMsgs & WANT_BSP_LOAD)
        return this->_Dispatch(this, BM_SCRIPT, (const TCHAR*)pd, 0);
    return 0;
}

long    BeamTree::HandleSetSceneInfo(const BST_SceneInfo* psi)
{
    if(_wantMsgs & WANT_BSP_LOAD)
        return this->_Dispatch(this, BM_SCENEINFO, psi, 0);
    return 0;
}

//---------------------------------------------------------------------------------------
void BeamTree::TriggerMove(BspModel* pModel)
{
    if(pModel->_motionIndex>=0)
    {
        MotionPath& mp = _motions[pModel->_motionIndex];        // get the assoc motion path
        if(mp._curState==MotionPath::MP_PAUSE)
        {
            MotionNode* pMNode = &mp._moveNodes[mp._brPos];
            pMNode->_pauseTTL = .7;
        }
    }
}


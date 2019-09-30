//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================

#ifndef __BEAMTREE_H__
#define __BEAMTREE_H__

#include "baselib.h"
#include "basecont.h"
#include "system.h"
#include "flstream.h"
#include "textuerrt.h"
#include "motion.h"
#include "bspfilestr.h"

    #include "sqplus.h"
    using namespace SqPlus;
#define LOC_EPSILON	    1.0/16.0
#define MIN_DIST        1.0/8.0

#define COL_WANT_FACES  0x8000000
#define COL_WANT_PLANES 0x1
#define COL_BASIC       0x0
#define INVALID_PLANE   (1<<30)

//---------------------------------------------------------------------------------------
#define WANT_BSP_LOAD        0x1
#define WANT_BSP_MODELMOVE   0x2
#define WANT_BSP_LEAFCHANGE  0x4

//---------------------------------------------------------------------------------------
class BeamTree;

typedef long  (*BspProc)(BeamTree*, long, long, long);

//---------------------------------------------------------------------------------------
struct Performance
{
	void Reset(){
        TRACEX("Performance()\r\n");
        memset(this,0,sizeof(Performance));
    }
	int curLeaf;
	int	visPolys;
	int	visLights;
	int	dynaLights;
	int	visObjects;
	int	visModels;
    DWORD content;
};

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Terrain tree // 300
struct Tr_TerNode
{
    Tr_TerNode(){}
    int     n_idx;
    int     l_idx;
    Box     b_box;
    int     i_nodes[2];   // if index is - is an index in leafs struct
    int     i_parentNode;
    DWORD   x_flags;    // x_ for dword types
    int     d_split;    // direction split 0 on x 1 on (z/y)
    int     _visframe;
	int     _visframe1;
};

//---------------------------------------------------------------------------------------
class BeamTree;
struct Tr_TerLeaf
{
    Tr_TerLeaf(){};
    ~Tr_TerLeaf();
    SIZE            s_tiles[2];     // start tile, end tile
    int             n_idx;          // node index
    int             t_idx;          // leaf index
    DWORD           x_flags;        //
    Htex            l_text;         // light texture

    DWORD           _dlights;
	SIZE            _lmapsize;
    V3              _lmapaxes[3];
    SIZE            _lmapbox[2];
    UV              _lmstep;
    int             _visframe;
	int             _visframe1;
    int             _stripStart[32];
    int             n_strips[32];
    RawBuff<int>	_models;     // indexes in models array
};

//---------------------------------------------------------------------------------------
class Camera;
class Tr_TerTree  
{
public:
    Tr_TerTree(){z_tiles.cx=0;_landingLeaf=-1; v_strips.reserve(128*128);g_light=0;n_glights=0;v_heights=0;};
	~Tr_TerTree();
    void    Read(FlStream& fw,  BTF_Section& fs, 
                           map<int, Htex>& texAssoc,    
                           map<int, Htex>&   lmAssoc,
                           vvector<BTF_LmapInfo>& lmInfo);
    BOOL    IsPrecompiled(){return p_leafs.size()!=0;}
    BOOL    IsGouroud(){return n_glights!=0;}
    void    PackVxes();
    void    Render(SystemData* sd, DWORD dwf);
    int	    GetCurrentLeaf(const V3& pov, int nodeIdx=-1);
    void    FrameTerrain(const Camera* pCam);
private:
    void    R_FrameTerrain(int inode, const Camera* pCam);
    void    AnimateTerrainTextures(Vtx& v0, Vtx& v1);
    void    _ReadTerrInfo(FlStream& fw,  BTF_Section& fs, 
                           map<int, Htex>& texAssoc,    
                           map<int, Htex>&   lmAssoc,
                           vvector<BTF_LmapInfo>& lmInfo);
    void    _ReadGLight(FlStream& fw,  BTF_Section& fs);
    void    _ReadNodes(FlStream& fw,  BTF_Section& fs);
    void    _ReadLeafs(FlStream& fw,  BTF_Section& fs,  map<int, Htex>&  lmAssoc,  vvector<BTF_LmapInfo>& lmInfo);
    void    RenderLeaf(SystemData* pData, int nleaf, DWORD how);
    void    R_Render(SystemData* pData, int node, DWORD how);

public:
    SIZE                z_tiles;
    Box                 b_box;
    REAL                *v_heights;
    vvector<Tr_TerLeaf> p_leafs;
    vvector<Tr_TerNode> p_nodes;
    CLRNOA               *g_light;        // gouraoud light
    int                 n_glights;
    Htex                _textures[4];
    DWORD               _texCombine;
	REAL	            _bump;
	REAL	            _friction;
    CLR	                _colorS;
    CLR	                _colorD;
    CLR	                _colorE;
    REAL                _shiness;
    DWORD               _flags;
    int                 _landingLeaf;
    vvector<Vtx>        v_strips;
    UV                  _tanim[2]; 
    BeamTree*           p_BspTree;
};



//---------------------------------------------------------------------------------------
// dummy structures
struct BtVertex
{
    int vxIdx;      //  index of vertex in vertex array
    int vnIdx;      //  index of the normal vertx in normal arrays
    UV  uv[4];      //  texCoord;
    CLR color;      //  color 
};


//---------------------------------------------------------------------------------------
struct BtNode
{
    BtNode():_visFrame(-1),_hitCnt(0){}
    BOOL    IsLeaf(){return  0!=(_flags & NODE_LEAF);}
    BOOL    IsSolid(){return 0!=(_flags & NODE_SOLID);};
    int     _visFrame;
    DWORD   _hitCnt;
    int     _parentIdx;     // index of parent nodeIdx
    int     _nodesIdx[2];   // index or back and front nodes
    int     _planeIdx;      // splitter plane index
    int     _cluster;
    int     _zone;          // zone 
    Box     _bbox;          // box of all under geometry
    int     _leafIdx;       // -1 no leaf
    DWORD   _flags;
    
};

//---------------------------------------------------------------------------------------
struct BtLeaf
{
    BtLeaf(){
        _visFrame = -1;
    }
    ~BtLeaf(){
        _portals.Destroy();
		_sides.Destroy();
		_trigs.Reset();
    }
    int               _nodeIdx;
    DWORD             _content;         // leaf content if not detail (gas) otherwise model content
    int               _firstPoly;
    int               _polys;
    int               _firstItem;
    int               _items;
    int               _cluster;
    int               _pvs;
    int               _nZone;
	RawBuff<int>      _sides;	   // indexes in plane array
    RawBuff<int>	  _portals;    // indexes in portals array
    RawBuff<int>	  _models;     // indexes in models array
	RawArray<int,32>  _trigs;      // indexes in trigers array
    int               _visFrame;   // visited frame
};

//---------------------------------------------------------------------------------------
struct BtPortal // used to make the walking path
{
    DWORD _leaf[2];
    V3    _center;
    int   _visframe;
};

//---------------------------------------------------------------------------------------
struct BtItem
{
    DWORD   Item;
    void*   pItemData;
};

//---------------------------------------------------------------------------------------
struct DbgFace
{
    Vtx  _vxes[4];
};

//---------------------------------------------------------------------------------------
struct Fog
{
    CLR     color;
    REAL    radius;
    REAL    intens;
    V3      center;
    int     nmodelNode;
};

#ifdef HL_
//---------------------------------------------------------------------------------------
struct HLight
{
    int     leaf;
    V3      pos;
    int     index;
    REAL    radius;
};
#endif

//---------------------------------------------------------------------------------------
class Trigger
{
public:
	Trigger(){_delayDummy=0;_frame=-2;};
	Box		    _box;
	int			_leafIdx;
	int			_modelIdx;
	int			_delayDummy;
    DWORD       _frame;
};

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
enum {LM_0=0, LM_DYNA,LM_2,LM_3};
//point light
struct PointLight
{
    INLN     void Set(const V3& pos,
                       REAL radius,
                       REAL intens,
                       const CLR& color,
                       DWORD dw = LIGHT_POINT|LM_DYNA)
    {
        _flags  = dw;
        _pos    = pos;
        _radius = radius;
        _color  = color;
        _intens = intens;
    }
    INLN void Clear(){::memset(this,0,sizeof(*this));}
    DWORD   _flags;
    V3      _pos;
    REAL    _radius;
    CLR     _color;                 //  ambient
    REAL    _intens;
};
#define MAX_DL  32
struct LightInfo
{
	LightInfo(){
        TRACEX("LightInfo()\r\n");
        Clear();
    }
	void Clear(){memset(this,0,sizeof(*this));}
    PointLight   _dynalights[32];
    int          _count;
};

//face
struct Face
{
public:
    Face(){memset(this,0,sizeof(*this)-sizeof(vvector<UV>));}
    ~Face();
    
public:
    CLR	    _color;                 // overwritten by vx color
    DWORD   _flags;                 // see bspfilestr for flags
    DWORD   _flags2;
    DWORD   _frameno;
    int     _modelindex;            // model is part of
    int     _planeidx;              // plan eindex in BSP plane array
    int     _firstvx;               // first vertex
    int     _vertexcount;
    int     _visframe;
	int     _visframe1;
    int     _brIdx;                 // brush index from editor
    Box     _box;
    UV      _texanim[2];            // texture animation
    REAL    _sliceidx[2];           // animated
    BYTE*   _pdynalights[4];
    long    _user;        
	SIZE    _lmapsize;
    V3      _lmapaxes[3];
    SIZE    _lmapbox[2];
    UV      _lmstep;
    DWORD    _dlights;
	REAL	 _bump;
	REAL	 _friction;
    Htex     _textures[4];
    DWORD    _texCombine;
    int      _nmodel;
    vvector<UV> _initalUV;
};


struct Impact
{
    friend class BeamTree;
    Impact(){Reset();}
    void    Reset(){
        _hit     = 0;
        _env     = 0;
        _model   = 0;
        _node    = 0;
        _leaf    = -1;
        _end     = 0;
        _dist    = INFINIT;
        _solid   = FALSE;
        _nplane  = -1;
    }
    BOOL        _solid;
    BOOL        _hit;
    int         _model;
    int         _node;
    int         _leaf;
    DWORD       _hitCnt;
    DWORD       _env;
    V3          _ip;

    Box         _mbox;
    V3          _extends;
    DWORD       _what2coll;
    V3          _fwd;

    REAL        _dist;
    BOOL        _end;
    Plane       _plane;
    int         _nplane;
};

struct BspModel 
{
    int          _rootNode;  // first root nodeIdx in nodes array
    int          _bspNode;   // main bsp first splitting node
    Box          _bx;        // bounding box translated in world pos
    M4           _trmat;     // tr matrix from it's original position
	M4           _trmatStep; // tr matrix
	V3           _origin;    // allways is original point
    V3           _from;
    V3           _trpos;     // moves by trmatrix
    V3           _speed;
    V3           _depl;
    int          _firstLeaf;
    int          _leafs;
    int          _visFrm;
	int			 _polys;
    DWORD        _flags;     // if is detail is the content
    DWORD        _props;
	int		     _motionIndex;
    int          _nZone;
    char	     _name[32];
    V3           _rotVx;
    int          _leaf;     
    int          _nCluster;
    BOOL         _dirty;
    DWORD        _type;
    BOOL         _onterrain;
    int          _index;
    

    INLN BOOL IsColidable(int frame, int nLeaf=-1){
        if(_flags & MODEL_NOCOLIDE)return FALSE;
        if(frame != _visFrm)return FALSE;
        if(nLeaf>=0 && nLeaf != _leaf)return FALSE;
        return TRUE;
    }

    INLN void  ToModelSpace(Box& b){
        if(_origin==_trpos) return;//V3
        TransformVertex(b._min);
        TransformVertex(b._max);
    }

    INLN V3  ToModelSpace(const V3& pt){
        V3 pout(pt);
        pout-=_trpos;
        _trmat.v3transposeTransform(pout);
        pout+=_trpos;
        return pout;
	}

    INLN V3  FromModelSpace(const V3& pt){
        V3 pout(pt);
		pout-=_trpos;
        _trmat.v3transform(pout);
        pout+=_trpos;
        return pout;
	}
    INLN void TransformVertex( V3& pt){
        pt-=_trpos;
        _trmat.v3transposeTransform(pt);
        pt+=_trpos;
	}
    INLN  void  UnTransformVertex(V3& pt){
        pt-=_trpos;
        _trmat.v3transform(pt);
        pt+=_trpos;
	}
    INLN void   UnTransformPlane(Plane& plane, const V3& point){
        _trmat.v3rotate(plane._n);
		plane._c = Vdp(plane._n, point);
    }


};

    
//---------------------------------------------------------------------------------------

class BeamTree // : public IGc
{
public:
    friend class Tr_TerTree;
    BeamTree();
    BeamTree(BspProc cbBspProc, void* userData, const TCHAR* pszScriptProc=0);
    virtual         ~BeamTree();
    string          _lasterror;
public:
    static DWORD S_ClassifyBox(const Plane& plan, const Box& box);
    static BOOL	Export2Script(System* ps, HINSTANCE hi);
    static BOOL	Export2DLL(HINSTANCE hi, System* pSystem);

    void    WantMessages(DWORD msgMask){_wantMsgs = msgMask;};
    long    HandleReadItem( BTF_Item*);
    long    HandleReadItemCategory(BTF_ItemCat* );
    long    HandleModelCollision(BspModel* , SystemData*);
    long    HandleMovingModel(const MotionPath* , BspModel* , SystemData* );
    long    HandleReadScriptInfo(const BTF_ScrInfo* bsi);
    long    HandleReadScriptText(const BYTE* pd);
    long    HandleSetSceneInfo(const BST_SceneInfo*);

    void        TriggerMove(BspModel*);
    int         Animate(Pos* pov, SystemData*);
    void        UpdateValues(V3& pos, int& leaf);
    int         Render( SystemData* sd, DWORD);
    int         LoadLevel(const char* specificData);
    void        RunMotionPaths(BspModel* pModel, SystemData* sd); 
    BOOL		IsLoaded(){return _loaded;}
	void		Clear();
    BOOL		ReadStream(FlStream& fstream);
    int         GetCurentNode(int nRootNode, const Box& bbox);
    BspModel*   GetModel(int idx){return &_models[idx];};
    size_t      GetModels(){return _models._size;};
	BspModel*   GetModel(char* name);
    void        GetNodeExtend(int n, V3& ex);
	int			GetCurrentLeaf(const V3& pov, int nodeIdx=-1);
    HTEX        GetTextureHandle(int idx){ if(_texAssoc.find(idx) != _texAssoc.end()) return _texAssoc[idx]; return 0;}
    HTEX        GetLmapHandle(int idx){ if(_texAssoc.find(idx) != _texAssoc.end()) return _lmAssoc[idx]; return 0;}
    DWORD	    GetLeafContent(int leaf);
    
    DWORD	    R_GetContent(const V3& pt, const V3& ex, DWORD& content, int node=0);
    DWORD	    GetContent(const Box& box, int node=0);
    DWORD	    GetContent(const V3& pt, int node=0);
	BtLeaf&		GetLeaf(int leaf)  {return _leafs[leaf];}     // all leafs
    int         GetLeafsCount(){return _nleafs ;}
    V3          GetLeafCenter(int leaf){return _nodes[_leafs[leaf]._nodeIdx]._bbox.GetCenter();}
    REAL        GetLeafMinExtend(int leaf){return _nodes[_leafs[leaf]._nodeIdx]._bbox.GetMinExtend();}
    int         GetPovCamLeaf(){return _curLeaf;}
    Htex&       GetHTex(int index){return _texAssoc[index];}
    BOOL		NotCanSee(V3& a, V3& b);
	void		RegisterTrigger(int leaf, int trigger);
	BOOL		IsLeafInPVS(int leaf);
	BOOL		IsLeafVisFromLeaf(int l1, int l2);
    BOOL		R_IsBbInVisibleLeaf(int nodeIdx, Box& box);
    void		PreprocessBspData();
    BTF_Item*   GetItems(size_t& count);
    Vtx*		GetVxPtr(int& length);
    void        SetGamma(BYTE* p768rgb);
    // dynamic light management
	
    PointLight*	GetDynaLight();
	void		ReleaseDynaLight(PointLight* pl);
	void		SetDynaLightAttrib(PointLight* pl,V3& pos, REAL radius, int intens, CLR& color);
	BST_SceneInfo* GetSceneInfo(){return  &_si;}

    const int*        GetPortalsIndexes(int leaf, int count)const ;
    const BtPortal&   GetPortal(int index)const;

    //collision
    BOOL        SegmentIntersect(Impact &pi, const V3& a,const  V3& b, int node=0, DWORD w2coll=0xFFFFFFFF);
    BOOL        SegmentIntersectModel(Impact &pi, int model, const V3& a,const  V3& b, int node=0, DWORD w2coll=0xFFFFFFFF);
    BOOL        ObjectIntersect (Impact &pi, const V3& a, const V3& b, const REAL ra, const REAL rb, const Box& box, DWORD w2coll=0xFFFFFFFF);

    

    // collision 2 
    BOOL        GetPotentialyColidingFaces(Impact& im, const V3& dir, const Box& box, DWORD w2coll);
    BOOL	    FaceContainPoint(const Face* pFace, const V3& point);
    BOOL        TestPolygonEdge(const Face* pFace, const V3&   point, REAL  radius, V3& ip);
    
    // whatever
    Performance&    GetPerformance(){return  _Perf	;};
    Plane&          GetPlane(int idx){return _planes[idx];}    // all planes
    const Vtx*      GetVertexes(const Face* pFace, int& ncount);
    const Plane&    GetPlane(const Face* pFace){return _planes[pFace->_planeidx];};



private:
    BOOL        _R_ModelIntersect(Impact& im, int rootnode, const V3& nstart, const V3& nend, const REAL ra, const REAL rb);

    void 	    R_GetContent(const Box& box, int model, int node, DWORD& content);
    int         _CuulTree(Pos* pov, SystemData* pSd);
    //collision
    BOOL        R_SegmentIntersect(Impact& im,int model, int nodeIdx, const V3& a, const V3& b);
    
    

    void        _R_GetPotentialyColidingFaces(Impact& im, int , const Box& , DWORD);
    void        _GetFacesOrPlanesFromLeaf(Impact& im, int nleaf, const Box& box, DWORD w2c);
    int         _ParentNode(int node){
                if(node == 0) 
                    return 0;
                    if(_nodes[node]._parentIdx==-1)
                        return 0;
                    return _nodes[node]._parentIdx;
                }
    BOOL        ClassifyBox(Plane& plan, Box& box);
    void        FrameMainModel(int nodeidx);
    void        FrameMovingModels(int leafIdx);
    void        FrameNodeParents(int nodeIdx);
    void        RenderBlendFaces(DWORD how);
    void        RenderStatic_R(int nodeIdx,SystemData* pData, DWORD how);
    void        RenderStatic_RB2F(V3& cp, int nodeIdx,SystemData* pData,  DWORD how);	// go with the walls
    void	    RenderDetails(V3& cp, SystemData* pData,  DWORD how);
	void	    R_RenderDetails(int nodeIdx, SystemData* pData,  DWORD how);
    void        AssignZones();
    BOOL        ZonesAreVisible(int z1, int z2){
        if(_zones._size==0)return 1;
        return (_zones[z1].visData & (1<<z2));
    }
private:

    template <class T, class U> long  _Dispatch(BeamTree* ps, long msg, T u, U v)
    {
        long rv = 0;
        if(this->_bspSysProc)
        {
            rv =(*_bspSysProc)(this, msg, u, v);
        }
        if(0==rv && this->_cbBspProc)
            return  this->_cbBspProc(this, msg, (long)u, (long)v);
        return rv;
    }
    void    _ModelHitLeaf(BtNode* pNode, Impact& best, const V3& vStart, 
                          const V3& vEnd, const REAL fStart, const REAL fEnd);
    BOOL    _TestLeaf(BtNode* pNode, Impact& pImpact, const V3& vStart, const V3& vEnd) ;
    int     _FindZone(const V3& pos);
    void    EnsureDirectory(const TCHAR* directory);
    // loading file private methods
    void    RenderLeaf(V3& cp, BtLeaf&  pLeaf, DWORD dwf );
    void	ReadHeader(FlStream&);
    BOOL    ReadImages(FlStream& fw, int count);
    void    ReadTexInfo(FlStream& fw, int count);
    void    ReadLightMapInfo(FlStream& fw, int count, vvector<BTF_LmapInfo>& lminfos);
    void    ReadLightMapsRGB(FlStream& fw, int count);
    void    BuildLightMaps(vvector<BTF_LmapInfo>& lminfos);
    void    ReadSoundsInfo(FlStream& fw, int count);
    BOOL    ReadSoundsData(FlStream& fw, int count);
    void    ReadItemsInfo(FlStream& fw, int count);
    void    ReadItemSubSection(FlStream& fw, int count){};
    void    Read3DPoints(FlStream& fw, int count, RawBuff<V3>&);
    void    ReadPlanes(FlStream& fw, int count);
    void    ReadPolys(FlStream& fw, int count, RawBuff<V3>&, vvector<BTF_LmapInfo>& lmi);
    void    ReadPoly(FlStream& fw, Face& face, BTF_Poly& bpoly, vvector<Vtx>& allvertexes, RawBuff<V3>&, vvector<BTF_LmapInfo>& lmi);
    void    ReadNodes(FlStream& fw, int count);
    void    ReadLeafs(FlStream& fw, int count);
    void    ReadModels(FlStream& fw, int count);
    void    ReadPVS(FlStream& fw, int count);
    void    ReadPortals(FlStream& fw, int count);
	void	ReadMotions(FlStream& fw, int count);// NEW
    void    ReadSkyDomInfo(FlStream& fw, int count);
    void    RenderSurface(Face* pFace, DWORD how);
    void    RenderSkyDom(SystemData* pData,  DWORD how);
    BOOL    SurfaceTest(Face* pFace,  BOOL bFrustCull, BOOL bBackCull);
    void    ApplyDynaLights();
    void	PrepareHardwareLights(const Box& theBox, int nLeaf);
    void    R_IlumMainModel(int n, PointLight* , int);
	void    R_IlumDetailModel(BspModel* pModel, PointLight* , int);
    void    CastDynaLightInLeaf(BtLeaf*, PointLight* pl, int lIdx);
    void    BuildDynaLmaps(Face* pFace);
    BOOL    BuildFogLmap(Face* pFace);
    void    ApplyToFace(PointLight* pl,  Face* pFace, BYTE* byDyna,  int& blit);
    void    AnimateSurfaceTexture(Face* pFace, Vtx*  pStart, int count);
    void    FinalizeClustersPVS();
	void	CreateDefLmaps();
    void    RenderMirrors(DWORD);
    void    AddDbg_Polygon(V3&, V3&, V3&);
    void    AddDbg_Polygon(Plane* p);
	void	Post_Prepare();
private:
    Performance       _Perf;
    Tr_TerTree*       _pterrain;
    map<int, Htex>    _texAssoc;
    map<int, Htex>    _lmAssoc;
    RawBuff<Vtx>      _vertexes;
    RawBuff<Plane>    _planes;    // all planes
    RawBuff<Face>     _faces;
    RawBuff<BtNode>   _nodes;     // all nodes
    RawBuff<BtLeaf>   _leafs;     // all leafs
    RawBuff<BTF_Item> _items;    // all items
    RawBuff<BspModel> _models;    
    RawBuff<BtPortal> _portals;
    RawBuff<V3>       _normals;
    RawBuff<BTF_Zone> _zones;

    TCHAR             _levFilesDir[_MAX_PATH];

    vector<DbgFace>   _debugFaces;

    vvector<Face*>   _trFaces;
    vvector<Face*>   _trMirrors;
	vvector<Face*>   _trBlackMsk;
	vvector<Face*>   _trBothFaces;
    vvector<Fog>     _fogs;
    vector<V3>       _walkPoints;

    SquirrelFunction<int>*  _bspSysProc;
    BspProc                 _cbBspProc;

#ifdef HL_
    vvector<RenderLight>        _hardLights;
    vvector<RenderMaterial>     _hardMats;
    vvector<HLight>             _hlights;
#endif //

    vvector<MotionPath> _motions;
    vvector<int>    _skyModels;
    BYTE*           _pLmapRGB;
    BYTE*           _pPVS;
    LightInfo       _lInfo;
    TextuerRt       _rTex;

    // additional information about BSP file
    string          _curfile;
    string          _comments;
    string          _author;
    DWORD           _levType;
    int             _maxActors;
    int             _maxFrags;
    int             _maxTime;
    Htex            _deftexLM;
    Htex            _deftexFB;
    Htex            _defTexID;

    BTF_Header      _fh;
    BST_SceneInfo   _si;
    BTF_SkyDom      _skd;
    int             _curLeaf;

    int             _frmStatic;   
    int             _frmDynamic;  

	BOOL			_hitTest;
	BOOL			_loaded;
    BOOL            _useHwLighting;
    int             _nleafs;    
    System*         _pSystem;     
    Camera*         _pCamera;

    vvector<V3>     _ipPoints;
    static int      _configured;
    const UINT      *_prims;
    BYTE*           _pGamma;        
    int             _povZone;
    BOOL            _testVisBox;
    DWORD           _wantMsgs;
};

extern  System  *PS;

DECLARE_INSTANCE_TYPE(BeamTree);
static int BspCtor(const TCHAR* scrProc,  HSQUIRRELVM v)
{
    return PostConstruct<BeamTree>(v, new BeamTree(0,0,scrProc), ReleaseClassPtr<BeamTree>::release);
}

#include "beamtreeexport.h"

#endif // !defined(AFX_BEAMTREE_H__CEEA3F71_6E52_4EB9_8747_20BD4A3AE7D6__INCLUDED_)


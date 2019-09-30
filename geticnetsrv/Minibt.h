#ifndef __MINIBT_H__
#define __MINIBT_H__

#include "../_include/bspfilestr.h"
#include "../baselib/baselib.h"
#include "../baselib/basecont.h"
#include "../baselib/baseutils.h"




struct BtNode
{
    BtNode():_visFrame(-1){}
    int     _visFrame;
    int     _parentIdx;     // index of parent nodeIdx
    int     _nodesIdx[2];   // index or back and front nodes
    Box     _bbox;          // box of all under geometry
    int     _leafIdx;       // -1 no leaf
    int     _planeIdx;
};

struct BtLeaf
{
    BtLeaf(){
        _visFrame = -1;
    }
    ~BtLeaf(){
    }
    int               _nodeIdx;
    int               _cluster;
    int               _zone;
    int               _pvs;
    int               _visFrame;
};


struct BtZone
{
    int     _visFrame;
    DWORD   _pvs;
    Box     _box;
};


struct BtModel
{
    int     _rootNode;
    Box     _box;
    V3      _pos;
    V3      _dir;
    M4      _trmat; // form pos
    int     _visFrame;
    int     _firstLeaf;
    int     _leafs;
    int     _leaf;
    int     _zone;
    DWORD   _flags;
};



//- minimal beam tree
class NO_VT MiniBt
{
public:
    MiniBt(){_pPVS=0;};
    ~MiniBt(){Clean();}
    void     LoadNewLevel(const string& levName);
    void     Clean();
    Pos      GetNewPlayerPos(BYTE characterID);
    int      GetCurrentleaf(V3& leaf, int* zonetoo=0);
    BOOL     LeafPVSLeaf(int l1,int l2);
private:
    void     _PrepareData();

    RawBuff<Plane>    _planes;
    RawBuff<BtNode>   _nodes;
    RawBuff<BtLeaf>   _leafs;
    RawBuff<BtModel>  _models;
    RawBuff<BtZone>   _zones;
    vvector<Pos>      _playerstart;
    BYTE*             _pPVS;
    BST_SceneInfo     _si;

};

enum {EX_NOFILE, EX_BADVERSION, EX_BAD_FILE,EX_OUTOFMEMORY, EX_IMAGETOBIG, EX_CANTCREATETEX, EX_TEXARANGEMENT};

#endif //

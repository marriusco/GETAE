// MiniBt.cpp: implementation of the MiniBt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniBt.h"


void     MiniBt::LoadNewLevel(const string& levName)
{
    BTF_Header  fh;
    FileWrap    fw;
    BTF_Section fs = {0,0,0};

    Clean();

    if(fw.Open(levName.c_str(),"rb") == 0)
    {
        DWORD dw = GetLastError();
        throw (int)dw;
    }

    fw.Read(fh);

    if(strcmp(fh.sig, "GGBT"))
        throw (int)EX_BAD_FILE;

    if(fh.version < BSP_VERSION1)
        throw (int)EX_BADVERSION;

    while(fs._type != SECT_EOF)
    {
        /*
        RawBuff<Plane>    _planes;
        RawBuff<BtNode>   _nodes;
        RawBuff<BtLeaf>   _leafs;
        RawBuff<BtModel>  _models;
        RawBuff<BTF_Zone> _zones;
        */

        fw.Read(fs);
        switch(fs._type)
        {
            case SECT_EOF:
                break;
            case SECT_INITGAME:
                break;
            case SECT_SCENE:
                fw.Read(_si);
                break;
            case SECT_PLANES:
                {
                    BTF_Plane plane;

                    _planes.ObjReserve(fs._elems);
                    for(UINT i=0;i<fs._elems;i++)
                    {
                        fw.Read(plane);

                        _planes[i]._n = plane.normal;
                        _planes[i]._c  = plane.dist;
                        _planes[i].TypeIt();
                    }
                }

                break;
            case SECT_NODES:
                {
                    BTF_Node    bN;
                    _nodes.ObjReserve(fs._elems);
                    for(UINT i=0; i<fs._elems;i++)
                    {
                        fw.Read(bN);

                        _nodes[i]._visFrame     = -1;
                        _nodes[i]._bbox         = bN.bbox;
                        _nodes[i]._leafIdx      = bN.leafIdx;
                        _nodes[i]._nodesIdx[0]  = bN.nodeidx[0];
                        _nodes[i]._nodesIdx[1]  = bN.nodeidx[1];
                        _nodes[i]._parentIdx    = bN.idxParent;
                        _nodes[i]._planeIdx     = bN.planeIdx;
                    }


                }
                break;
            case SECT_LEAFS:
                {
                    UINT        i; 
                    BTF_Leaf    bL;
                    _leafs.ObjReserve(fs._elems);
                    for(i=0; i<fs._elems;i++)
                    {
                        fw.Read(bL);

                        _leafs[i]._visFrame   = -1;
                        _leafs[i]._pvs       = bL.pvxIdx;
                        _leafs[i]._nodeIdx   = bL.nodeIdx;
                        _leafs[i]._cluster   = bL.cluster;
                        _leafs[i]._zone      = bL.areavis;
                        
                        //obsolette allways 0
                        /*
                        for(j=0; j < bL.portals;j++)
                            fw.Read(dummy);

                        for(j=0;j<bL.models;j++)
                            fw.Read(dummy);
                        */
                    }

                }
                break;
            case SECT_MODELS:
                {
                    BTF_ModelInfo bM;
                    _models.ObjReserve(fs._elems);
                    for(UINT i=0; i < fs._elems; i++)
                    {
                        fw.Read(bM);
                        _models[i]._visFrame    = -1;
                        _models[i]._rootNode    = bM.firstNode;
                        _models[i]._firstLeaf   = bM.firstLeaf;
                        _models[i]._leafs       = bM.leafs;
                        _models[i]._flags       = bM.props;
                        _models[i]._dir         = V3(0.0,0.0,1.0);
                    }
                }
                break;
            case SECT_PVS:
                _pPVS   = new BYTE[fs._elems];
                if(_pPVS)
                    fw.Read(_pPVS,fs._elems);
                break;
            case SECT_ZONES:
                {
                    BTF_Zone bsi;
				    for(UINT i=0;i<fs._elems;i++)
				    {
					    fw.Read(bsi);
                        _zones[i]._visFrame = -1;
                        _zones[i]._pvs      = bsi.visData;
                        _zones[i]._box      = bsi.box;
                    }
                }
                break;
            default:
                // Skip this much bytes
                if(fs._bytes)
                {
                    BYTE* by = new BYTE[fs._bytes];
                    fw.Read(by, fs._bytes);
                    delete[] by;
                }

                break;
        }
    }
    fw.Close();

    _PrepareData();
}

void     MiniBt::_PrepareData()
{
	if(_models._size > 1)
	{
		BtModel* pModel =  &_models[1];
		for(int i=1; i< _models._size;i++,pModel++)
		{
            // 300 (a) and (b)
            pModel->_box  = _nodes[pModel->_rootNode]._bbox;
            pModel->_pos  = pModel->_box.GetCenter();
            pModel->_leaf = this->GetCurrentleaf(pModel->_pos);
            if(pModel->_leaf>=0 && pModel->_leaf < _leafs._size)
                pModel->_zone = _leafs[pModel->_leaf]._zone;
            else
                pModel->_zone = -1;
		}
	}
}


void     MiniBt::Clean()
{
    delete[] _pPVS;
    _nodes.Destroy();
    _leafs.Destroy();
    _models.Destroy();
    _zones.Destroy();
    ::memset(&_si, 0, sizeof(_si));

}

Pos     MiniBt::GetNewPlayerPos(BYTE characterID)
{
    if(_playerstart.size())
    {
        return _playerstart[0];
    }

    Pos o;

    for(int i = 0; i<_leafs._size; i++)
    {
        BtNode &n = _nodes[_leafs[i]._nodeIdx];
        if(n._bbox.GetExtends().x > 200 &&
           n._bbox.GetExtends().y > 200 &&
           n._bbox.GetExtends().z > 200)
        {
            o._pos = n._bbox.GetCenter();
            o._euler= V0;
        }
    }
    return o;
}


int      MiniBt::GetCurrentleaf(V3& pov, int* zonetoo)
{
    if(_nodes._size==0)
        return -1;
    
    REAL  rdist;
    int   nodeIdx   = 0;

    BtNode* pNode = &_nodes[nodeIdx];
    while(pNode->_leafIdx==-1)
    {
        rdist = _planes[pNode->_planeIdx].DistTo(pov);
        if (rdist <0 ) 
            nodeIdx = pNode->_nodesIdx[0];
		else
            nodeIdx = pNode->_nodesIdx[1];

        pNode = &_nodes[nodeIdx];
    }
    int leafidx = _nodes[nodeIdx]._leafIdx;
    if(zonetoo && leafidx>=0)
    {
        *zonetoo = _leafs[leafidx]._zone;
    }
    return leafidx;
}

BOOL  MiniBt::LeafPVSLeaf(int l1, int l2)
{
    if(0 == _pPVS || l1==-1 || l2==-1 || l1==l2)     
        return 1; // allways see each other

    if(l1 < _leafs._size && l2<_leafs._size)
    {
        /*
        DWORD bz1 = 1 << _leafs[l1]._zone;
        DWORD bz2 = 1 << _leafs[l2]._zone;
        DWORD vz1 = _zones[nz1]._pvs;
        DWORD vz2 = _zones[nz2]._pvs;
        if((vz1 & bz1) & (vz2 & bz2)) // zones can see text leafs
        {
        */
            BYTE* pPvs1 = &_pPVS[_leafs[l1]._pvs];
            return (pPvs1[l2 >> 3] & (1<<(l2 & 7)) )!=0;
        //}
    }
    return 0;

}



//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __LEV_MAN__
#define __LEV_MAN__

#include "_isystem.h"
#include "sock.h"
#include "WIN_WIN/InetFtpHttp.h"
#include "basecont.h"
//#include "GameServer.h"
#ifndef GESERVER
    #include "_isystem.h"
#endif //
#include "basecont.h"
#include "../geticnetsrv/pkbuffer.h"


typedef int    (*CB_Proc)(UINT msg, long wp, long lp, void* pUser);

//---------------------------------------------------------------------------------------
class GServer
{
public:
    GServer(){}
    ~GServer(){}

    TCHAR*  GetIp(){return (TCHAR*)_ip.c_str();}
    int     GetPort(){return  _port;}

    tstring _ip;
    int     _port;
};


//---------------------------------------------------------------------------------------
class System;
class LevelMan
{
public:
    struct  Resource
    {
        tstring name;
        tstring loconame;
        size_t  length;
        int     remote;
        int     locally;
        vvector<Resource*>  _resources;
    };

public:
    LevelMan(){
        TRACEX("LevelMan()\r\n");
        _loadProgress=0;
        _proxyip = "";
        _proxyport = 0;
        _recbytes =0;
        _expectedbytes=0;
        _recursive = 0;
        _guard[0] = 0;
        _szloco="";
		_pcb = 0;
		_pu = 0;
        _break = 0;
        _umsg    = SYS_PROGRESSPAINT;
    }
    LevelMan(const TCHAR* url, const TCHAR* locopath,const TCHAR* proxy=0, UINT port=0);
    ~LevelMan();
    void    SetupAddrs(const TCHAR* url, const TCHAR* locopath,const TCHAR* proxy=0, UINT port=0);
    int     CheckLocally();
    int     Download(BOOL recursive=FALSE);
    int     GetResourceCount(const TCHAR* resource);
    const   TCHAR* GetLocoLevel(){return _szloco.c_str();};
	void	FillLocoFile();
    void    Clean();
    const TCHAR*   GetResourceAt(const TCHAR* resource, int res);
    int      QueryMaster(const TCHAR* masterIP, int port, const TCHAR* proxy=0, int prxPort=0);
    GServer*  GetServerAt(int index);
    void    Break();
	void	Notify( CB_Proc c, void* v){
		_pcb = c;
		_pu = v;
	}
private:
    long    _OnDataAvailable(BYTE* pBuff, long length);
    static   long Nf(BYTE* pdata, long dwlength, void* pUser);
    int     _Download(InetFtpHttp& inet, Resource* resource);
    int     _DownloadRess(InetFtpHttp& inet, vvector<Resource*>& resources);
    void    _Explode(tstring& indexxml, vvector<Resource*>&  _resources);
    int     _CheckResources(tstring& locoFile);
    int     _ManageDownloadedFile(tstring& locoFile, Resource*);
    BOOL    _EnsureDir(tstring& localFile);
    int     _UnZip(const TCHAR* filename, const TCHAR* destDir, Resource* res);
    BOOL    _FileExist(tstring& fname, long length=0);
    BOOL    _ResExist(tstring& fname, long length=0);
    int     _ParseServerList(const tstring& allofthem);
	long	_Notify(UINT msg, long w, long p);
    void    _DeleteResources(vvector<Resource*>* pres, const TCHAR* baseFile);
private:
    tstring             _szloco;
    tstring             _path;
    Ip                  _ip;
    int                 _loadProgress;
    tstring             _proxyip;
    UINT                _proxyport;
    vvector<Resource*>  _resources;
    FileWrap            _hFile;
    long                _recbytes;
    long                _expectedbytes;
    BOOL                _recursive;
    char                _guard[256];
    vvector<GServer>    _servers;
    BOOL                _break;
	CB_Proc				_pcb;
	void*				_pu;
    UINT                _umsg;
};

#ifdef GESERVER
    DECLARE_INSTANCE_TYPE(LevelMan);
#endif //

#endif //__LEV_MAN__
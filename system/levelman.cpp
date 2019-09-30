//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================


#include <io.h>
#include "_isystem.h"
#ifndef GESERVER
    #include "system.h"
#else
    #include "../geticnetsrv/gameserver.h"
#endif //
#include "levelman.h"
#include "BzipCompressor.h"


LevelMan::LevelMan(const TCHAR* url, const TCHAR* locopath,
                   const TCHAR* proxy, UINT port)
{
    _umsg    = SYS_PROGRESSPAINT;
    _break   = 0;
    SetupAddrs(url, locopath, proxy, port);
}


void   LevelMan::SetupAddrs(const TCHAR* url, const TCHAR* locopath,const TCHAR* proxy, UINT port)
{
    _proxyport = 0;
    if(proxy )
    {
        if(proxy[0] && port==0)
        {
            Ip  ip;
            ip.ParseUrl(proxy);
            _proxyip    = ip.s_url;
            _proxyport  = ip.n_port;

        }
        else if(port)
        {
            _proxyip    = proxy;
            _proxyport  = port;
        }
    }
   
    _path = locopath;
    string slevel = url;
    _ip.ParseUrl(slevel);
    _loadProgress = 0;
}

void	LevelMan::FillLocoFile()
{
    tstring   locoFile;
    size_t    laststalh = _ip.s_target.find("/", 1);
    tstring   remPth    = _ip.s_path;
    tstring   tarFile   = _ip.s_file;
    size_t    p0;

    FOREACH(vvector<Resource*>, _resources , pres)
        (*pres)->_resources.deleteelements();
    _resources.deleteelements();

    if(tarFile.find("getic_index.ml")!=-1)
    {
        locoFile = _path + "/index_";
        if((p0 = _ip.s_ip.find("http://")) != tstring::npos)
        {
            locoFile += _ip.s_ip.substr(p0+7);
        }
        size_t pd = locoFile.find_last_of('.');
        string nlf;
        if(pd>=0)
        {
            nlf = locoFile.substr(0,pd);
            locoFile = nlf;
        }
        locoFile += ".ml";
    }
    else
    {
        locoFile =  _path + tarFile;
    }
    int pos;
    while((pos = locoFile.find('/'))!=-1)
    {
        locoFile.replace(pos,1,1,'\\');
    }
	_szloco = locoFile;
}

int  LevelMan::CheckLocally()
{

    tstring   locoFile;
    size_t    laststalh = _ip.s_target.find("/", 1);
    tstring   remPth    = _ip.s_path;
    tstring   tarFile   = _ip.s_file;
    size_t   p0;

    FOREACH(vvector<Resource*>, _resources , pres)
        (*pres)->_resources.deleteelements();
    _resources.deleteelements();

    if(tarFile.find("getic_index.ml")!=-1)
    {
        locoFile = _path + "/index_";
        if((p0 = _ip.s_ip.find("http://")) != tstring::npos)
        {
            locoFile += _ip.s_ip.substr(p0+7);
        }
        size_t pd = locoFile.find_last_of('.');
        string nlf;
        if(pd>=0)
        {
            nlf = locoFile.substr(0,pd);
            locoFile = nlf;
        }
        locoFile += ".ml";
    }
    else
    {
        locoFile =  _path + tarFile;
    }
    int pos;
    while((pos = locoFile.find('/'))!=-1)
    {
        locoFile.replace(pos,1,1,'\\');
    }

    _hFile.Close();
    _unlink(locoFile.c_str());
    bool    error = 0;
    InetFtpHttp inet(LevelMan::Nf, this);
    if(_proxyport)inet.SetProxy(_proxyip, _proxyport);

    if(!_EnsureDir(locoFile))
    {
        return -1;
    }
    if(_hFile.Open(locoFile.c_str(),"wb"))
    {
        _recbytes = 0;
        _expectedbytes = 0;

        if(-1 == _Notify(_umsg, -1, (long)locoFile.c_str()))
			return -1;
        try{
            inet.Connect(_ip.s_user, _ip.s_passwd, _ip.s_ip, _ip.n_port);
            inet.GetFile(_ip.s_target);
            if(_expectedbytes == 0)
                _expectedbytes=_recbytes;
            if(_expectedbytes)
                error = (_recbytes != _expectedbytes);

        }catch(...)
        {
            error = 1;
        }
        inet.Disconnect();
        _hFile.Close();
        if(-1 == _Notify(_umsg, error ? -2 : -1, 0))
			return -1;
    }
    if(!error)
    {
        _Explode(locoFile, _resources);
        return _CheckResources(locoFile);
    }
    _unlink(locoFile.c_str());
    return -1;
}

int LevelMan::_CheckResources(tstring& locoFile)
{
    int     error = 0;
    BOOL    isindex = locoFile.find("index_")!=-1;
    tstring loclfile;
    // check every ppres from xml if exist locally
    // and the length matches;
    FOREACH(vvector<Resource*>, _resources, pres)
    {
        loclfile = _path + BS; 
        loclfile += (*pres)->name;
        if(!_ResExist(loclfile, (*pres)->length))
            ++error;
        else
        {
            if(loclfile.find("gbz") != -1 || loclfile.find("gbt") != -1)
            {
                size_t pos = loclfile.find_last_of('.');
                _szloco = loclfile.substr(0,pos);
                _szloco+=".gbt";
            }
            (*pres)->loconame = (*pres)->name;
        }
    }
    return error;
}

void  LevelMan::_Explode(tstring& indexxml, vvector<Resource*>& resources)
{
    tstring   content;
    int       read;
    TCHAR      loco[1024];
    FileWrap  fw;

    fw.Open(indexxml.c_str(),"rb");

    while(!fw.IsEof())
    {
        memset(loco,0,1024);
        read = fw.Read(loco, 1024);
        if(read > 0)
        {
            content.append(loco);
        }
    }
    TCHAR tag1[32] = "<file url='";

    if(content.find(tag1, 0) == -1)
        strcpy(tag1,"<file name='");
    
    TCHAR tofix[128];
    TCHAR ln[]   = "length='";
    TCHAR tag2[] = "'";
    int  startpos = 0, endpos;
    Resource *res;
    while(1)
    {
        startpos = content.find(tag1, startpos);
        if(startpos == tstring::npos) break;
        startpos += ::strlen(tag1);
        endpos = content.find(tag2, startpos);
        if(endpos == tstring::npos) break;

        res = new Resource;
        
        strcpy(tofix, content.substr(startpos, endpos-startpos).c_str());
        for(int i=0; i< strlen(tofix);i++)
        {
            if(tofix[i]=='/')
                tofix[i]=BS;
        }

        res->name = tofix;//content.substr(startpos, endpos-startpos);

        startpos = content.find(ln, startpos);
        if(startpos == tstring::npos) break;

        startpos += ::strlen(ln);
        endpos = content.find(tag2, startpos);
        if(endpos == tstring::npos) break;
        
        res->length = ::atoi(content.substr(startpos, endpos-startpos).c_str());
        res->remote  = 1;
        res->locally = 0;
        startpos = endpos;
        if(res->name=="getic_index.ml")
        {
            delete res;
            continue;
        } 
        resources << res;
    }
}

int  LevelMan::Download(BOOL recursive)
{
    int error = -1;

    if(_resources.size()==0) return error;

    /*
    if(PSystem->GetMode() != DOWNLOADING_MODE)
    {
        PSystem->Log(100, "Level man cannot start. PSystem->GetMode() != DOWNLOADING_MODE\n");
        return -1;
    }
    */

    
    _recursive = recursive;
    
    InetFtpHttp inet(LevelMan::Nf, this);
    if(_proxyport)inet.SetProxy(_proxyip, _proxyport);
    
    FOREACH(vvector<Resource*>,  _resources, ppres) // get each level
    {
        if((*ppres)->locally==0)
        {
            try{
                if(0==_Download(inet, *ppres))
                {
                    (*ppres)->locally = 1;
                    error = 0;
                    if((*ppres)->name.find(".ml")!=-1)
                    {
                        (*ppres)->loconame = (*ppres)->name;
                    }
                }
                else 
                    break;
            }catch(...)
            {
                TRACEX("cannot find %s  \n", (*ppres)->name.c_str());
            }
        }
    }
    inet.Disconnect();


    return error;
}


int  LevelMan::_DownloadRess(InetFtpHttp& inet, vvector<Resource*>& resources)
{
    int error = -1;
    if(resources.size()==0) return error;
    
    FOREACH(vvector<Resource*>,  resources, ppres) // get each level
    {
        if((*ppres)->locally==0)
        {
            try{
                if(0==_Download(inet, *ppres))
                {
                    (*ppres)->locally = 1;
                    error=0;
                }
            }catch(...)
            {
                TRACEX("cannot find %s  \n", (*ppres)->name.c_str());
            }
        }
    }
    return error;
}

int     LevelMan::_Download(InetFtpHttp& inet, Resource* ppres)
{
    tstring   locoFile = _path + BS;  locoFile += ppres->name;
    
    ppres->_resources.deleteelements();
    ppres->locally = 0;

    _recbytes = 0;
    _hFile.Close();
    /*
     if(_ResExist(locoFile, ppres->length))
     {
        ppres->loconame = ppres->name;
        return 0;
     }
     */

    _unlink(locoFile.c_str());

    tstring resname ;
    if(ppres->name.find("www")!=-1)
        resname = ppres->name;
    else
        //resname = _ip.s_ip + _ip.s_path + '/';  resname+=ppres->name;
        resname =  _ip.s_path + '/';  resname+=ppres->name;

    
    int pos;
    while((pos = resname.find('\\'))!=-1)
    {
        resname.replace(pos,1,1,'/');
    }

    
    _EnsureDir(locoFile);
    if(_hFile.Open(locoFile.c_str(),"wb"))
    {
        _recbytes = 0;
        _expectedbytes = 0;
        if(-1 == _Notify(_umsg, -1, (long)locoFile.c_str()))
        {
            _hFile.Close();
            _DeleteResources(&_resources, locoFile.c_str());
			return -1;
        }
        
        try{
            inet.Connect(_ip.s_user, _ip.s_passwd, _ip.s_ip, _ip.n_port);
            inet.GetFile(resname);
            ppres->locally = (_recbytes == _expectedbytes);
        }catch(...)
        {
            ppres->locally = 0;
        }
        
        inet.Disconnect();
        _hFile.Close();
       

        if(-1 == _Notify(_umsg, ppres->locally ? -1 : -2, 0))
        {
            _hFile.Close();
            _DeleteResources(&_resources, locoFile.c_str());
            return -1;
        }
    }
    if(ppres->locally)
    {
        if(locoFile.find(".ml")!=-1 && _recursive)
        {
            _Explode(locoFile, ppres->_resources);
            return _DownloadRess(inet, ppres->_resources);
        }
        return _ManageDownloadedFile(locoFile, ppres);
    }
    _hFile.Close();
    _DeleteResources(&_resources, locoFile.c_str());
    return -1;
}

int     LevelMan::_ManageDownloadedFile(tstring& locoFile, Resource* res)
{
    TCHAR   destLoc[256];
    int     error = 0;
    struct _finddata_t fileFndData;
    long hFile    = _findfirst( locoFile.c_str(), &fileFndData );
    if(hFile != -1)
    {
        _findclose( hFile );
       if(fileFndData.size == res->length && 
          fileFndData.size == _recbytes)
       {
           
         if(locoFile.find(".gbz")!=-1 || locoFile.find(".gtz")!=-1||
            locoFile.find(".gsz")!=-1 || locoFile.find(".gmz")!=-1)
         {
            ::_tcscpy(destLoc, locoFile.c_str());
            size_t pos = locoFile.find_last_of('/');
            if(pos==-1)pos = locoFile.find_last_of('\\');
            if(pos!=-1) destLoc[pos]=0;
            try{
                error = _UnZip(locoFile.c_str(), destLoc, res);
            }
            catch(...)
            {
                error = -1;
            }
            if(!error)
            {
                ////unlink(locoFile.c_str());
            }
         }
       }
    }
    return error;
}


int    LevelMan::_UnZip(const TCHAR* filename, const TCHAR* destDir, Resource* res)
{
    TRACEX("Uncommpressing : %s %s \r\n",filename, destDir);
    try{
        CBzipCompressor decz(filename, destDir);
        if(decz.Process(0,0))
        {
            if(strstr(filename,".gbz") )
            {
                res->loconame = decz.GetDestFileName();
                TRACEX("levelname = %s \r\n",res->loconame.c_str());
                _szloco = res->loconame;
            }
        }
    }
    catch(...)
    {
        if(strstr(filename,".gbz") )
        {
            return 1;
        }
    }
    return 0;
}

BOOL LevelMan::_EnsureDir(tstring& localFile)
{
    TCHAR     cd[_MAX_PATH]={0};
    TCHAR     tp[_MAX_PATH]={0};
    _tcscpy(tp, localFile.c_str());

    _getcwd(cd,_MAX_PATH);
    for(int i=0; i < localFile.length(); i++)
    {
        if(tp[i]=='\\' || tp[i]=='/')
        {
            if(tp[i+1] && (tp[i+1]=='/' || tp[i+1]=='\\'))
            {
                ++i; continue;
            }
            
            TCHAR s = tp[i];
            tp[i] = 0;
            if(!strchr(tp,'.'))
            {
                _mkdir(tp);
            }
            tp[i] = s;
        }
    }
    return TRUE;
}

long LevelMan::Nf(BYTE* pdata, long dwlength, void* pUser)
{
    return ((LevelMan*)(pUser))->_OnDataAvailable(pdata, dwlength);
}

//---------------------------------------------------------------------------------------
void    LevelMan::Break()
{
    _break = 1;
}

//---------------------------------------------------------------------------------------
long   LevelMan::_OnDataAvailable(BYTE* pBuff, long length)
{
#ifdef _DEBUG
    //ge_sleep(64);
#endif //

    if(_break)    
    {
        return -1; //broken process
    }

    if(length < 0)
    { 
        // header is here
        const TCHAR* ps = strstr((const TCHAR*)pBuff,"Content-Length: ");//293
        if(ps)
        {
            if(ps)ps+=16;
            _expectedbytes = ::atoi(ps);
            if(-1 == _Notify(_umsg, 0, _expectedbytes))
			    return -1;
            return _expectedbytes;
        }
        return 0;
    }
    else
    {
        _recbytes  +=length;
        
        if(-1==_Notify(_umsg, _recbytes, _expectedbytes))
        {
            return -1;//interrupted
        }

        if(_hFile._store)
        {
            if(length)
                return _hFile.Write(pBuff, length);
            return 1;
        }
        return _hFile.Read(pBuff, length);
    }
}

int     LevelMan::GetResourceCount(const TCHAR* ppres)
{
    if(ppres==0 || *ppres==0)
    {
        return _resources.size();
    }
    FOREACH(vvector<Resource*>, _resources , pres)
    {
        if((*pres)->loconame == ppres)
        {
            return (*pres)->_resources.size();
        }
    }
    return 0;
}   

const TCHAR*   LevelMan::GetResourceAt(const TCHAR* ppres, int res)
{
    static TCHAR Sret[256] = "";

    if(ppres==0 || *ppres==0)
    {
        if(res < _resources.size() )
        {
            Resource* pr = _resources[res];
            _tcscpy(Sret, pr->loconame.c_str());
            return Sret;
        }
    }

    FOREACH(vvector<Resource*>, _resources , pres)
    {
        if((*pres)->loconame == ppres)
        {
            if(res < (*pres)->_resources.size() )
            {
                return _tcscpy(Sret, (*pres)->_resources[res]->loconame.c_str());
                break;
            }
        }
    }
    return Sret;

}

BOOL    LevelMan::_ResExist(tstring& fname, long length)
{
    BOOL    rv = 0;
    vvector<Resource*>  resources;
    if(_FileExist(fname, length))
    {
        rv = 1;
        if(fname.find(".ml")!=-1)
        {
            _Explode(fname, resources);
            FOREACH(vvector<Resource*>,  resources, res)
            {
                tstring   locoFile = _path + BS;  
                locoFile += (*res)->name;

                if(!_FileExist((*res)->name, (*res)->length))
                {
                    rv = 0;
                    break;
                }
            }
        }
    }
    resources.deleteelements();

    return rv;
}

BOOL    LevelMan::_FileExist(tstring& fname, long length)
{
    struct _finddata_t fileFndData = {0};
    long    hFile    = _findfirst( fname.c_str(), &fileFndData );
    if(hFile != -1)
    {
        _findclose( hFile );
        return (abs((long)(fileFndData.size - length)) < 2);
    }
    return FALSE;
}

LevelMan::~LevelMan()
{
    Clean();
    TRACEX("~LevelMan()\r\n");
}

void LevelMan::Clean()
{
    FOREACH(vvector<Resource*>, _resources , pres)
    {
        (*pres)->_resources.deleteelements();
    }
    _resources.deleteelements();
}

int LevelMan::QueryMaster(const TCHAR* srvip, int port, 
                          const TCHAR* proxy, int prxPort)
{
    tstring locoip;
    int     locop;
    tcp_cli_sock s;
    TCHAR        addrloco[32];
    sock::GetLocalIP(addrloco);

    if(!_tcscmp(srvip,"localhost") || 
        !_tcsncmp(srvip,"127",3) || 
        !_tcsncmp(srvip,"192",3) ||
        !_tcsncmp(srvip,addrloco,3)){
            locoip = srvip;
            locop   = port;
    }
    else
    {
        if(prxPort){
            locoip = proxy;
            locop   = prxPort;
        }
    }
    if(s.connect(locoip.c_str(), locop) >= 0)
    {
        char    request[256];
        tstring  allofthem;

        sprintf(request, "GET /getservers.php HTTP/1.1\r\n"
                         "Host: %s:%d\r\n"
                         "User-Agent: GeticSrv/1.0\r\n\r\n",
                         srvip, port);
        s.send((BYTE*)request, strlen(request)+1);
        char loco[256];
        int  bytes;
        while(bytes = s.receive((BYTE*)loco,256))
        {
            allofthem+=loco;
            loco[bytes]=0;
        }
        s.destroy();
        //parse allofthem
        _ParseServerList(allofthem);
    }
    return  _servers.size();
}


int LevelMan::_ParseServerList(const tstring& allofthem)
{
    _servers.clear();

    // 1.2.3.4-2334<br>...
    int sindex=0,eindex;
    while(1)
    {
        eindex = allofthem.find("<br>", sindex);
        if(eindex != -1)
        {
            tstring    srvstring = allofthem.substr(sindex, eindex-sindex);
            GServer gs;
            int        dash = srvstring.find(srvstring, 0);

            gs._ip   = srvstring.substr(0, dash);
            gs._port = ::atoi(srvstring.substr(0, srvstring.length()-dash-1).c_str());
            _servers.push_back(gs);
        }
        sindex = eindex+4;
    }

}


GServer*  LevelMan::GetServerAt(int index)
{
    if(index < _servers.size()){
        return &_servers[index];
    }
    return 0;
}

long	LevelMan::_Notify(UINT msg, long w, long l)
{
	if(_pcb) // take over system
	{
		return _pcb(msg, w, l,_pu);
	}
    if(PSystem)
	{
		return PSystem->SendMessage(msg, w, l);
	}
	return DEFAULT;
}


//delete incomplette downloads
void LevelMan::_DeleteResources(vvector<Resource*>* pres, const TCHAR* baseFile)
{
    int locally = pres->size();

   
    FOREACH(vvector<Resource*>,  (*pres), ppres) // get each level
    {
        if((*ppres)->_resources.size())
        {
            _DeleteResources(&(*ppres)->_resources, (*ppres)->name.c_str());
        }
        if((*ppres)->locally)
        {
            --locally;
        }
    }

    if(0 != locally)
    {
        if(baseFile)
        {
             tstring loclfile = _path + BS; 
             loclfile += baseFile;
             _unlink(loclfile.c_str());
        }

        FOREACH(vvector<Resource*>,  (*pres), ppres) 
        {
            if(!(*ppres)->loconame.empty())
            {
                _unlink((*ppres)->loconame.c_str());
                (*ppres)->locally = 0;
            }
            else
            {
                tstring loclfile = _path + BS; 
                loclfile += (*ppres)->name;
                _unlink(loclfile.c_str());
                (*ppres)->locally = 0;
            }
        }
    }
}





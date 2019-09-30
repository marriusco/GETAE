// XmlLevParser.cpp: implementation of the XmlLevParser class.
//
//////////////////////////////////////////////////////////////////////

#include <io.h>
#include "system.h"
#include "basecont.h"
#include "levparser.h"
#include "BzipCompressor.h"

//---------------------------------------------------------------------------------------
static void _MakeDestLocation(tstring& localfile, LPTSTR destLoc)
{
    ::_tcscpy(destLoc, localfile.c_str());
    size_t pos = localfile.find_last_of('/');
    if(pos!=-1)
        destLoc[pos]=0;
}

//---------------------------------------------------------------------------------------
BOOL     XmlLevParser::ExistLocally()
{
    _inetCheck = 0;
    return _ResolveLevel();
}

//---------------------------------------------------------------------------------------
BOOL     XmlLevParser::Download()
{
    _inetCheck = 1;
    return _ResolveLevel();
}

//---------------------------------------------------------------------------------------
BOOL     XmlLevParser::_ResolveLevel()
{
    _locopath = "dnl_levels";
    size_t    laststalh = _ip.s_target.find("/", 1);
    tstring   remPth    = _ip.s_target.substr(0,laststalh+1);
    tstring   target    = _ip.s_target.substr(laststalh);
    tstring   localfile =  _locopath + target;

    l_gatheredbytes = 0;

    try{
        BOOL gf = 0;
        InetFtpHttp inet(XmlLevParser::_NotiFyFromInet, this);
        if(_proxiport)
        {
            inet.SetProxy(_proxyaddr, _proxiport);
        }

        inet.Connect(_ip.s_user, _ip.s_passwd, _ip.s_ip, _ip.n_port);
        // this session dnls
        if(_xmllevels.findelement(_ip.s_target) == _xmllevels.end())
        {
            h_file.Close();
            _unlink(localfile.c_str());
            if(h_file.Open(localfile.c_str(),"wb"))
            {
                gf = inet.GetFile(_ip.s_target); 
                h_file.Close();
            } 
            if(gf)
            {
                ;
            }
            else
            {
                throw 404L;
            }
        }
        
        tstring content;
        if(_ReadFileContent(localfile, content))
        {
            if(!_EnumerateLevelResources(inet,
                                          content,
                                          remPth,
                                          &XmlLevParser::_CbRemoteLevelResource))
            {
                throw 1;
            }
        }
        else
            throw 1;
    }
    catch(long &i)
    {
        PSystem->Printf("EXEPTION: %d IT MAY BE HTTP: 400 FILE ! FOUND", i);
        i;
        return 0;
    }
    catch(...)
    {
        PSystem->Printf("*** UNKNOWN EXCEPTION ***");
        return 0;
    }
    return TRUE;
}

//---------------------------------------------------------------------------------------
DWORD   XmlLevParser::_NotiFyFromInet(BYTE* pBuff, DWORD length, void* pUser)
{
    return ((XmlLevParser*)(pUser))->_OnDataAvailable(pBuff, length);
}

//---------------------------------------------------------------------------------------
DWORD   XmlLevParser::_OnDataAvailable(BYTE* pBuff, DWORD& length)
{
    l_gatheredbytes += length;
    if(0==l_filesize)
        l_filesize=1000;
    PSystem->Wait((l_gatheredbytes * 100) /l_filesize);

    int rv = PSystem->PumpMessage();
    if(OS_ESCAPE==rv)
        throw 2;
    else
    	PSystem->Refresh();

    if(h_file._store)
    {
        return h_file.Write(pBuff, length);
    }
    else
    {
        return h_file.Read(pBuff, length);
    }

}

//---------------------------------------------------------------------------------------
BOOL XmlLevParser::_ReadFileContent(const tstring &index, tstring& content)
{
    FileWrap    fw;
    if(fw.Open(index.c_str(),"rb"))
    {
        long  len  = fw.Getlength();
        char* buff = new char[len+1];
        fw.Read(buff,len);
        buff[len]=0;
        content = buff;
        fw.Close();
        delete[] buff;
        return 1;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
BOOL    XmlLevParser::_EnumerateLevelResources(InetFtpHttp& inet,
                                               const tstring& content,
                                               const string& remotepath,
                                               CB_REML pfnCBRemRes)
{
    int   k ;
    TCHAR url[_MAX_PATH];
    TCHAR urlR[_MAX_PATH];
    TCHAR fsz[_MAX_PATH];

    int   sp = 0, fp;
    while(1) //ugly parsing
    {
        fp = content.find("url=\"", sp);
        if(fp==-1)
            break;
        fp += 5;
        k  = 0;
        while(content[fp]!='\"')
        {
            urlR[k]=content[fp];
            if(content[fp]=='/')
                url[k]='\\';
            url[k]=content[fp];
            ++fp;++k;
        }
        url[k]=0;    urlR[k]=0;
        fp = content.find("length=\"", sp);
        if(fp==-1)
            break;
        fp += 8;      k  = 0;
        while(content[fp]!='\"')
        {
            fsz[k]=content[fp];
            ++fp;++k;
        }
        fsz[k]=0;
        long remotesize = ::atoi(fsz);
        tstring remoteFile = remotepath + urlR;
        tstring localfilez;
        
        _BuildLocalFileLoc(urlR, localfilez);
        
 
        if(!(this->*pfnCBRemRes)(inet, remoteFile.c_str(), localfilez.c_str(), remotesize))
        {
            PSystem->Printf("DOWNLOADING ERROR FOR: %s ", localfilez.c_str());
            return 0;
        }
        sp=fp;
    }
    return 1;
}


void XmlLevParser::_BuildLocalFileLoc(const tstring& s, tstring& localFile)
{
    TCHAR     tp[_MAX_PATH]={0};

    sprintf(tp, "%s/%s",_locopath.c_str(), s.c_str());
    localFile = tp;
    // ensure directories exist
    for(int i=0; i < localFile.length(); i++)
    {
        if(tp[i]=='\\' || tp[i]=='/')
        {
            char s = tp[i];
            tp[i] = 0;
            _mkdir(tp);
            tp[i] = s;
        }
    }
}

//---------------------------------------------------------------------------------------
BOOL    XmlLevParser::_CbRemoteLevelResource(InetFtpHttp& inet,
                                             const TCHAR* remoteFile,
                                             const TCHAR* localfile,
                                             long remotesize)
{
    struct _finddata_t c_file;
    long    hFile    = _findfirst( localfile, &c_file );
    long    filesize = 0;
    TCHAR   destLoc[_MAX_PATH] = {0};

    l_filesize      = remotesize;
    l_gatheredbytes = 0;

    if(hFile!=-1)
    {
         filesize = c_file.size;
        _findclose( hFile );
    }
    if(_inetCheck)
    {
        BOOL gf = 0;
        if(filesize != remotesize || _forceDnl)
        {
            h_file.Close();
            _unlink(localfile);
            if(h_file.Open(localfile,"wb"))
            {
                string s(remoteFile);

                PSystem->Printf("DOWNLOADING: %s",remoteFile);
                gf = inet.GetFile(s);
                h_file.Close();
            }
            else
            {
                return 0;
            }

            if(gf)
            {
                hFile    = _findfirst( localfile, &c_file );
                if(-1!=hFile)
                {
                    if(c_file.size != remotesize)
                    {
                        SetLastError(404);
                        return 0;
                    }
                }
            }
            else
                return 0;
        }
    }
    else
    {
        if(filesize == remotesize)
        {
            if(localfile[::_tcslen(localfile)-1]=='z')
            {
                tstring lf(localfile);
                _MakeDestLocation(lf, destLoc);
                _UnZip(localfile, destLoc);
            }
            else if(strstr(localfile,".gbt")  )
            {
                _levelname = localfile;
            }
            return TRUE;
        }
        return 0;
    }

    if(localfile[::_tcslen(localfile)-1]=='z') // is a zip
    {
        tstring lf(localfile);
        _MakeDestLocation(lf, destLoc);
        _UnZip(localfile, destLoc);
    }
    else
    {
        if(strstr(localfile,".gbt")  )
        {
            _levelname = localfile;
        }
    }
    return TRUE;
}

//---------------------------------------------------------------------------------------
BOOL    XmlLevParser::_UnZip(const char* filename, const char* destDir)
{
    PSystem->Printf("UNCOMPRESSING: %s->%s \r\n",filename, destDir);
    try{
        CBzipCompressor decz(filename, destDir);
        if(decz.Process(0,0))
        {
            if(strstr(filename,".gbz") )
            {
                _levelname = decz.GetDestFileName();
            }
        }
    }
    catch(...)
    {
        if(strstr(filename,".gbz") )
        {
            return 0;
        }
    }
    return TRUE;
}
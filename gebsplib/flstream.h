//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __FLSTREAM_H__
#define __FLSTREAM_H__

#include "baselib.h"
#include "basecont.h"

//-------------------------------------------------------------------------------------------------
class ByteStream
{
public:
    ByteStream(){
        _offset    = 0;
    }
    virtual ~ByteStream(){
    }

    void Reserve(DWORD sz)
    {
        _pBuffer.reserve(sz);
    }


    bool   appendBuffer(BYTE* bufeer, DWORD length)
    {
        for(DWORD j=0;j<length;j++)
            _pBuffer.push_back(bufeer[j++]) ;
        return true;
    }

    size_t   read( BYTE *buffer, size_t sizeT, size_t count)
    {
        size_t i,j;
        for( i = _offset,j=0; i < _offset+count*sizeT; i++,j++)
        {
            buffer[j] = _pBuffer[i];
        }
        _offset += j;
        return j/sizeT;
    }

    size_t write( const BYTE *buffer, size_t size, size_t count){return 0;}
    char*  gets( char *string, int n)
    {
        size_t i,j;
        for( i = _offset,j=0; i < _offset+n; i++,j++)
        {
            string[j] = _pBuffer[i];
            if(string[j]==0)
            {
                break;
            }
        }
        _offset+=j;
        return string;
    }
    size_t    seek (long offset, long origin ){return 0;}
    size_t    tell (){return _offset;}
    size_t    close(){_pBuffer.clear(); return 1;}
    size_t    open(const wchar_t *filename, const wchar_t *mode){}
    size_t    eof(){ return  _offset>=_pBuffer.size()-1;}

    vvector<BYTE>   _pBuffer;
    size_t          _offset;
};

//-------------------------------------------------------------------------------------------------
class FlStream
{
public:
    FlStream(){_pf=0;_pByteStream=0;_ok=0;_store=0; _name[0]=0;};

    virtual ~FlStream(){
            if(_pf)
                ::fclose(_pf);
            if(_pByteStream)
                _pByteStream->close();
    }

    void SetStream(ByteStream* pByteStream){
        _pByteStream = pByteStream;
    }

    BOOL    Open(const TCHAR* file, const TCHAR* how)
    {
        if(!_pByteStream)
        {
            _pf = ::_tfopen(file, how);
            _ok = _pf != 0;
            _tcscpy(_name, file);
        }
        _store = (how[0]==_T('w'));
        return _ok;
    }

    void Close(){
        if(_pByteStream)
        {
            if(_ok)
                _pByteStream->close();
        }
        else
        {
            if(_pf)
                ::fclose(_pf);
            _pf=0;
        }
    }

    void Pos(int pos){
        if(_pByteStream)
        {
            _pByteStream->seek(pos,SEEK_SET);
        }
        else
        {
            ::fseek(_pf,pos,SEEK_SET);
        }
    }

    void ReStart(){
        if(_pByteStream)
        {
            _pByteStream->seek(0,SEEK_SET);
        }
        else
        {
            ::fseek(_pf,0,SEEK_SET);
        }
    }

    size_t GetPos(){
        if(_pByteStream)
        {
            return _pByteStream->tell();
        }
        return ::ftell(_pf);
    }

    DWORD Getlength(){
        if(_pByteStream)
        {
            _pByteStream->seek(0,SEEK_END);
            DWORD flength = ftell(_pf);
            _pByteStream->seek(0,SEEK_SET);
            return flength;
        }
        ::fseek(_pf,0,SEEK_END);
        DWORD flength = ftell(_pf);
        ::fseek(_pf,0,SEEK_SET);
        return flength;
    }

    BOOL ReadLine(char* line, int len){
        if(_pByteStream)
        {
            return _pByteStream->gets(line,len)!=0;
        }
        return ::fgets(line,len,_pf)!=0;
    }

    template <class T> BOOL Serialize(T& t){
        if(_store)
            return Write(t);
        return Read(t);
    }

    size_t    IsEof(){
        if(_pByteStream)
        {
            return _pByteStream->eof();
        }
        return feof(_pf);
    }

    template <class T> BOOL Write(T& t, const char* sinfo=0){
        if(_pByteStream)
        {
            int lenr = _pByteStream->write(&t,1,sizeof(T));
            return (lenr == sizeof(T));
        }
        int lenr = ::fwrite(&t,1,sizeof(T),_pf);
        return (lenr == sizeof(T));
    }

    template <class T> BOOL Read(T& t) {
        if(_pByteStream)
        {
            int lenr = _pByteStream->read((BYTE*)&t,1,sizeof(T));
            return (lenr == sizeof(T));
        }
        int lenr = ::fread(&t,1,sizeof(T),_pf);
        return (lenr == sizeof(T));
    }

    BOOL Readv(void* pv, size_t len) {
        if(_pByteStream)
        {
            size_t lenr = _pByteStream->read((BYTE*)pv,1,len);
            return (lenr == lenr);
        }
        size_t lenr = ::fread(pv,1,len,_pf);
        return (lenr == lenr);
    }

    BOOL Writev(void* pb, size_t len) {
        if(_pByteStream)
        {
            return (len == _pByteStream->write((BYTE*)pb,1,len));
        }
        return (len == ::fwrite(pb,1,len,_pf));
    }

    BOOL Serialize(void* pv, int len){
        if(_store) return Writev(pv,len);
        return Readv(pv,len);
    }

    size_t WriteTextAsIs(const TCHAR* t){
        if(_pByteStream)
        {
            size_t l = _tcslen(t);
            return _pByteStream->write((BYTE*)t, 1, _tcslen(t));
        }
        size_t l = _tcslen(t);
        return fwrite(t, 1, _tcslen(t),  _pf);
    }

    size_t WriteStr(const TCHAR* t){
        if(_pByteStream)
        {
            size_t l = _tcslen(t);
            _pByteStream->write((BYTE*)&l, 1, sizeof(short));
            return _pByteStream->write((BYTE*)t, 1, _tcslen(t));
        }
        size_t l = _tcslen(t);
        ::fwrite(&l, 1, sizeof(short), _pf);
        return fwrite(t, 1, _tcslen(t),  _pf);
    }

    size_t ReadStr(const TCHAR* t){
        if(_pByteStream)
        {
            size_t len;
            _pByteStream->read((BYTE*)&len, 1, sizeof(short));
            size_t rd = _pByteStream->read((BYTE*)t, 1, len);
            return rd=len;
        }
        size_t len;
        ::fread(&len, 1, sizeof(short), _pf);
        size_t rd = ::fread((void*)t, 1, len,  _pf);
        return rd=len;
    }
    const TCHAR* Name()const{return _name;}

private:
    FILE*        _pf;
    ByteStream*  _pByteStream;
    BOOL         _ok;
    BOOL         _store;
    TCHAR        _name[260];
};


#endif //__FLSTREAM_H__


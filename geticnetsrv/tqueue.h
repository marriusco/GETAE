#ifndef _TQUEUE_H_
#define _TQUEUE_H_

#include <deque>
#include "pkbuffer.h"
using namespace std;


#pragma warning (disable: 4786)
class SyncQ : public deque<counted_ptr<Buffer> >
{
public:
    SyncQ(const char *pn){
        ::strcpy(_name,pn);
        _maxsz = 256;
        _actl  = 0;
    };
    virtual ~SyncQ(){
        {
            clear();
        }
    };

    void SetMaxSize(long ms){
        _maxsz = ms;
    }
    long    Count(){
         return size();
    }

    void Put(counted_ptr<Buffer> row){
        _cs.Lock();
        push_back(row);
        _actl = size();
        _cs.Unlock();
        _hmq.Notify();
        ////TRACEX("->Q %s=%d\r\n",_name, _actl);
    }

    int TryGet(counted_ptr<Buffer>* prow)
    {
        if(_cs.TryLock())
        {
            _actl = size();
		    if(_actl)
		    {
                *prow = this->at(0);
			    pop_front();
		    }
            _cs.Unlock();
        }
        if(_actl)
        {
            _hmq.Notify();
            ////TRACEX("<-Q %s=%d\r\n",_name, _actl);
        }
        return _actl;
    }
    int Get(counted_ptr<Buffer>* prow)
    {
        do{
            _cs.Lock();
            _actl = size();
		    if(_actl)
		    {
                *prow = this->at(0);
			    pop_front();
		    }
            _cs.Unlock();
        }while(0);
        if(_actl)
        {
            ////TRACEX("<-Q %s=%d\r\n",_name, _actl);
            _hmq.Notify();
        }
        return _actl;
    }

    void Lock(){_cs.Lock();}
    void Unlock(){_cs.Unlock();}
    int Pick(counted_ptr<Buffer>* prow)
    {
        _actl = size();
		if(_actl)
		{
            *prow = at(0);
			pop_front();
            ////TRACEX("<-Q %s=%d\r\n",_name, _actl);
		}
        return _actl;
    }
    void Clean()
    {
        this->clear();
    }

    mutex            _cs;
    semaphore        _hmq;
    int              _maxsz;
    int              _actl;
    char             _name[32];
};

#endif //_TQUEUE_H_


#ifndef _ProcUnit_H_
#define _ProcUnit_H_

#define _USE_BASEG

#include "pkbuffer.h"
#include "tqueue.h"

//-----------------------------------------------------------------------------
class Statistics;
class Processor : public OsThread
{
public:
    Processor(int io):_ord(io),_buzzy(0){};
    ~Processor(){};
    void    ThreadFoo();
    const BOOL  IsBuzzy()const {return _buzzy;}
public:
    int     _error;
    int     _ord;
    long    _buzzy;
};


//-----------------------------------------------------------------------------
typedef vector<Processor*> ProcessUnit;

class ProcUnit
{
public:
    friend class Statistics;
     ProcUnit(){}
    ~ProcUnit(){}

    BOOL    Start(int nthreads);
    void    Stop(const SyncQ& inq);
    BOOL    Increase();
    void    Decrease();
    int     Size(){return _procesors.size();}
private:
    ProcessUnit  _procesors;
    mutex        _mutex;
};


#endif//__RECTHREAD_H_

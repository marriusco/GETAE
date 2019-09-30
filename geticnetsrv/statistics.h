// statistics.h: interface for the statistics class.

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include "~preconfig.h"

class server;
class Statistics  
{
public:
    Statistics(){memset(this,0,sizeof(*this));};
    ~Statistics(){};
    void     Update(const server* ps, DWORD delta);

    int     nThreads;
    int     percinQ;
    int     percinQUdp;
    int     percinQTcp;
    int     percBuffs;
    int     sendsa[MAX_CLIENTS];
    int     receivesa[MAX_CLIENTS];
    int     sendsUa[MAX_CLIENTS];
    int     receivesUa[MAX_CLIENTS];
    long    sends;
    long    receives;
    long    sendsU;
    long    receivesU;
    DWORD   fps;
    DWORD   _prevtic;
    int     _loops;
};

#endif 

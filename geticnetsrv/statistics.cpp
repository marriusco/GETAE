// statistics.cpp: implementation of the statistics class.
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "pkbuffer.h"
#include "statistics.h"
#ifdef _MULTI_UDPS
    #include "MULTIUDP/handler.h"
    #include "MULTIUDP/server.h"
#else
    #include "SINGLEUDP/handler.h"
    #include "SINGLEUDP/server.h"
#endif //


//-----------------------------------------------------------------------------
void     Statistics::Update(const server* ps, DWORD delta)
{
    int ctick = ge_gettick();
    if(ctick-_prevtic > 1000) // per second
    {
        if (delta==0) delta=1;

        sends               = 0;
        receives            = 0;
        sendsU              = 0;
        receivesU           = 0;
        fps                 = 1000/delta;
        nThreads            = ((ProcUnit&)ps->_procUnit).Size();
        percinQ             = ps->_inQueue.size();
        percinQUdp          = ps->_outQueueTCP.size();
        percinQTcp          = ps->_outQueueUDP.size();
        percBuffs           = (DPool<Buffer>::Size()*100)/Pool<Buffer>::Capacity();
        Handler*    pH = PgServer->GetHandlers();
        while(pH){

            if(pH->IsConnected())
            {
                int iid = pH->Uid();

                sendsa[iid]     = pH->_st_bytesOutTcp;
                receivesa[iid]  = pH->_st_bytesInTcp;
                receivesUa[iid] = pH->_st_bytesInUdp;
                sendsUa[iid]    = pH->_st_bytesOutUdp;
                
                sends           += sendsa[iid];
                receives        += receivesa[iid];
                sendsU          += sendsUa[iid];
                receivesU       += receivesUa[iid];

                pH->_st_bytesInTcp   = 0; 
                pH->_st_bytesOutTcp  = 0;
                pH->_st_bytesInUdp   = 0;
                pH->_st_bytesOutUdp  = 0;
            }
            pH = pH->Next();
        }
        _prevtic = ctick;
    }
}


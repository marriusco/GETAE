#include "StdAfx.h"
#include "sock.h"
#ifdef _MULTI_UDPS
    #include "MULTIUDP/server.h"
    #include "MULTIUDP/netio.h"
#else
    #include "SINGLEUDP/server.h"
    #include "SINGLEUDP/netio.h"
#endif //
#include "procunit.h"
//-----------------------------------------------------------------------------

extern BOOL GlobalExit;

//-----------------------------------------------------------------------------
void Processor::ThreadFoo()
{
    MyPrintf(DL, "->Processor::ThreadFoo(%d)\r\n",_ord);

    SyncQ&      iQ   = PgServer->_inQueue;
    semaphore&  inEv = iQ._hmq;

    while(!BStop())
    {
        if(PgServer->IsBlocked())
        {
            PgServer->Count();
            ge_sleep(256);
            continue;
        }
        int sleepFact = 8;
        if(inEv.Wait())
        {
            ++_buzzy;
            int nCount = 0; 
            counted_ptr<Buffer> pB;
            nCount = iQ.Get(&pB);
            if(nCount != 0)
            {
                Handler*  pH = PgServer->GetHandler(pB->GetClientUid());
                if(pH)
                {
                    if(pH->ToClose()!=0)
                    {
                        pH->PreClose(); // already is 1 that means it was here
                    }
                    if(pH->AcceptMessage(pB))
                    {
                        PgServer->MT_UpdateSimulation(this, pH, pB);
                    }
                }
                PgServer->Count();
            }
            ge_sleep(0);
            /*
            sleepFact = 16-(nCount);
            if(sleepFact<0)
                sleepFact=0;
            ge_sleep(sleepFact);
            --_buzzy;
            */
        }
    }

    if(!BStop())
    {
        MyPrintf(EL, "<-Processor::ThreadFoo() EXCEPTION\r\n");
        GlobalExit=1;
    }


    MyPrintf(DL, "<-Processor::ThreadFoo(%d)\r\n",_ord);
}

//-----------------------------------------------------------------------------
BOOL    ProcUnit::Start(int nthreads)
{
    while(!(--nthreads < 0))
    {
        Processor* pT = new Processor(nthreads);
        pT->Start();
        _procesors.push_back(pT);
    }
    return 1;
}

//-----------------------------------------------------------------------------
void    ProcUnit::Stop(const SyncQ& inq)
{
    ProcessUnit::iterator b = _procesors.begin();
    ProcessUnit::iterator e = _procesors.end();
    for(;b!=e;b++)
    {
        inq._hmq.Notify();
        (*b)->IssueStop();
    }
    ge_sleep(1000);
    b = _procesors.begin();
    for(;b!=e;b++)
    {
        (*b)->Stop();
        delete (*b);
    }
}

//-----------------------------------------------------------------------------
BOOL    ProcUnit::Increase()
{
    Processor* pT = new Processor(Size()+1);
    pT->Start();
    _procesors.push_back(pT);
    return 1;
}

//-----------------------------------------------------------------------------
void    ProcUnit::Decrease()
{
    if(Size())
    {
        Processor* pT = _procesors.front();
        pT->IssueStop();
        while(pT->IsBuzzy()) 
            ge_sleep(10);
        pT->Stop();
        _procesors.erase(_procesors.begin());
        delete pT;
    }
}



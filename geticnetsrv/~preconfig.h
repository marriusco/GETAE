
#ifndef __PRECONFIG_H__
#define __PRECONFIG_H__

//#define _MULTI_UDPS             // define it for one udp socket per client

/*
    receive buffer size
*/
#define REC_BF_SIZE            16384    

/*
    send buffer size. used on _SEND_ALL_ONFLUSH
*/

#define SND_BF_SIZE            16384    

/*
    max clients cannot exceed 32
*/
#define MAX_CLIENTS            8


/*
    server configurations flags.
*/
//---------------------------------------------------------------------
//---------------------------------------------------------------------
#define    CFG_NOSENDQUEUE              0x0
/*
-----------------------------------------------------------------------
-server has just input queue
-each thread in the pool send the data trough udp or tcp when 
 available. same thread S/R doea receiving and sending when FD_SETS
 are set acordingly

 for single udp one udp-socket is used to send to any connected  client
_SINGLE_UDP        ->R(T1)========>(P(*)/shared_udp)->

for multi udp each client has a 'connected' udp socket.
_MULTI_UDPS        ->R(T1)========>(P(*)/client-udp)->
-----------------------------------------------------------------------
*/


//---------------------------------------------------------------------
//---------------------------------------------------------------------
#define    CFG_SENDQUEUE                0x1
/*
-----------------------------------------------------------------------
-same as previous but the processor is sepatated by outgoing
 queue. same thread is used for receiving/sending

_SINGLE_UDP
        ->R(T1)========>(P(*))=======>(S(T1)/shared_udp)
_MULTI_UDPS
        ->R(T1)========>(P(*))=======>(S(T1)/client-udp)->
-----------------------------------------------------------------------
*/

//---------------------------------------------------------------------
//---------------------------------------------------------------------
#define    CFG_SEPARATETHREADS          0x3
/*
-----------------------------------------------------------------------
-receiving and sending are 2 sepatae threads. all above apply

_SINGLE_UDP
        ->R(T1)========>(P(*))=======>(S(T2)/shared_udp)
_MULTI_UDPS
        ->R(T1)========>(P(*))=======>(S(T2)/client-udp)->
-------------------------------------------------------------
*/



/*
    accept 8 errors per client of invalid or hacked buffer then ban it's IP.
*/
#define ERR_TRESHHOLD                   8   


#define IL  0x1   // info level
#define WL  0x3   // warning level
#define EL  0x7   // error level
#define DL  0xF   // debug level

extern     long     Srv_Cfg;
extern     long     Print_Level;


#endif //__PRECONFIG_H__

#ifndef _LISTENTHREAD_H_
#define _LISTENTHREAD_H_

#include "sock.h"


class ListenThread : public OsThread
{
public:
    ListenThread(){};
    ~ListenThread(){};
    void    ThreadFoo();
    int     Start(int port);
    int     PoolConnections(int);
    void    ServerIsFull();

    tcp_srv_sock    _sock;
    int             _port;

};



#endif//__RECTHREAD_H_

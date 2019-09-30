#ifndef _REMADMIN_H_
#define _REMADMIN_H_

#include "sock.h"
#include "pkbuffer.h"
#include "tqueue.h"

class RemAdmin: public OsThread
{
public:
    RemAdmin(void);
    ~RemAdmin(void);
    void    ThreadFoo();
    void    IssueStop();

private:
    tcp_srv_sock    _sock;
};

#endif //_REMADMIN_H_
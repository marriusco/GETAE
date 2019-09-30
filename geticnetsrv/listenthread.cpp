

#include "sock.h"
#include "../baselib/baseutils.h"
#ifdef _MULTI_UDPS
    #include "MULTIUDP/handler.h"
    #include "MULTIUDP/server.h"
    #include "MULTIUDP/netio.h"
#else
    #include "SINGLEUDP/handler.h"
    #include "SINGLEUDP/server.h"
    #include "SINGLEUDP/netio.h"
#endif //
#include "listenthread.h"


extern BOOL GlobalExit;

void    ListenThread::ThreadFoo()
{
    printf("->ListenThread::ThreadFoo()\r\n");

    if(_sock.create(_port) != 0)
    {
         printf("cannot create socket on port %d. error:%d", _port, _sock.error());
         return;
     }
    _sock.listen(_port);

    while(!BStop())
    {
        PoolConnections(32);
        ge_sleep(64);
    }

    if(!BStop())
    {
        printf("<-ListenThread::ThreadFoo() EXCEPTION\r\n");
        GlobalExit=1;
    }


    TRACEX("<-ListenThread::ThreadFoo()\r\n");
}

int ListenThread::Start(int port)
{
    _port = port;
    return OsThread::Start();
}


void ListenThread::ServerIsFull()
{
    tcp_cli_sock  s;

    if(_sock.accept(s) > 0)
    {
        Buffer b;
        b.SetOwner(SERVER_ID);
        b.SetMsg(MSG_MESSAGE);
        b.SetString("Server is Full. Please try later on...<br>\r\n");
        s.send((BYTE*)b.GetBuffer(), b.Length());
        ge_sleep(128);
        s.destroy();
    }
}

int ListenThread::PoolConnections(int toutms)
{
    timeval  tv     = {0, toutms};

    fd_set		rdSet;
    fd_set		wrSet;
    fd_set		exSet;

    ge_error(0);
    FD_ZERO(&rdSet);
    FD_ZERO(&exSet);
    FD_ZERO(&wrSet);
    FD_SET(_sock.socket(), &rdSet);
    FD_SET(_sock.socket(), &exSet);
    int nfds = _sock.socket() + 1;
    int sel  = ::select(nfds, &rdSet, NULL, &exSet, &tv);
    if(sel > 0)
    {
        if( FD_ISSET( _sock.socket(), &rdSet ) )
        {
            Handler* pH = PgServer->OpenHandler();

            if(pH)
            {
                if(_sock.accept(pH->_sock) > 0)
                {
                    AutoLock    al(&PgServer->_mutex);

                    if(PgServer->IsBlocked())
                    {
                        Buffer b;

                        b.SetOwner(SERVER_ID);
                        b.SetMsg(MSG_RETRY);
                        pH->_sock.send(b.Storage(), b.Length());
                        ge_sleep(128);
                        PgServer->CloseHandler(pH);
                        TRACEX("%d[%d] New Connection retry\n", pH->Counter(), pH->Index());
                    }
                    else
                    {
                        char sa[64];
                        pH->_sock.getsocketaddr(sa);
                        TRACEX("%d[%d] New Connection from %s\n", pH->Counter(), pH->Index(), sa);
                        pH->OnCreate();
                        if(FALSE == pH->OnConnect(0))          /// rejected by handler
                        {
                            PgServer->CloseHandler(pH);      // just dstroy it. has not been added to list
                        }
                    }
                }
                else
                {
                    PgServer->CloseHandler(pH);
                }
            }
            else
            {
                ServerIsFull();
            }
        }
    }
    else if(sel < 0)
    {
        int errror = ge_error();
        TRACEX("tcp_server select() error:%d\r\n",errror);
        return  errror;
    }
    return 0;
}

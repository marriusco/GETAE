#include "remadmin.h"
#include "gameserver.h"

RemAdmin::RemAdmin(void)
{
}

RemAdmin::~RemAdmin(void)
{
}

void    RemAdmin::IssueStop()
{
    _sock.destroy();
    OsThread::IssueStop();
}

//---------------------------------------------------------------------------------------
void    RemAdmin::ThreadFoo()
{
    char    request[4096];
    if(_sock.create(8080) != 0)
    {
         printf("CANNOT CREATE SOCKET ON PORT %d. ERROR:%d", 8080, _sock.error());
         return;
     }
    _sock.setblocking(1);
    _sock.listen(1);

    tcp_cli_sock  s;
    while(_sock.accept(s) > 0)
    {
        int recbytes=s.receive((BYTE*)request,4096);
        if(recbytes>0)
        {
            request[recbytes]=0;
        }
        s.destroy();
    }
}

/*  info retreived from server
    UPTIME;
    STARTTIME;
    LOCALIP,
    MAPLIST,
    FILESERVER,
    MASTERSERVER,
    LEVELS PLAYED (NO OF EACH)
    CUR MAP
    UPTIME CURMAP
    CLIENTLIST (IP, VERSION, PING UDP, PING TCP, SCORE, ERRORS, HACKS)
    USED BUFF POOL
    THREADS
    QUEUES IN OUT
    FPS
    UDP/SEC IN-OUT
    TCP/SEC IN-OUT

*/

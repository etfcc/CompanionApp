#include "LibVoipEvents.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

LibVoipEvents::Event::Event()
{
    memset(buf,0,sizeof(buf));
} 

LibVoipEvents::LibVoipEvents()
{
    char name[100];
    snprintf(name, sizeof(name),"%s/%s",getenv("TMPDIR"),"libvoip.events");
    struct sockaddr_un listener_addr;
    memset(&listener_addr, 0, sizeof(listener_addr));
    listener_addr.sun_family = AF_UNIX;
    strncpy(listener_addr.sun_path, name, sizeof(name));

    unlink(name);
    
    m_iSockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(m_iSockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    
    int ret=bind(m_iSockfd, (struct sockaddr *) &listener_addr, sizeof(listener_addr));
    if (ret==-1) throw "bind failed";
}

LibVoipEvents::~LibVoipEvents()
{
    close(m_iSockfd);
}

LibVoipEvents::Event LibVoipEvents::read()
{
    LibVoipEvents::Event evt;
    int bytes = (int)::read(m_iSockfd, evt.buf, sizeof(evt.buf));
    if (bytes==-1)
    {
        if (errno==EAGAIN || errno==EWOULDBLOCK)
            ;
        else
            throw "read failed";
    }
    return evt;
}

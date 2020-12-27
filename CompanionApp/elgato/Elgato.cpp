#include <arpa/inet.h>
#include "zeroconf/zeroconf.hpp"
#include "Elgato.hpp"

//------------------------------------------------------------------------------------------
void* get_in_addr(sockaddr_storage* sa)
{
    if (sa->ss_family == AF_INET)
        return &reinterpret_cast<sockaddr_in*>(sa)->sin_addr;

    if (sa->ss_family == AF_INET6)
        return &reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr;

    return nullptr;
}

//------------------------------------------------------------------------------------------
Elgato::Light::Light(std::string host)
{
    address=host;
    status=-1;
    status_change_ts=0;
    temperature=0;
    brightness=0;
}

//------------------------------------------------------------------------------------------
Elgato::Elgato(): m_xLight("")
{
}

//------------------------------------------------------------------------------------------
const Elgato::Light & Elgato::light() const
{
    return m_xLight;
} 

//------------------------------------------------------------------------------------------
const Elgato::Light & Elgato::refresh(int timeout)
{
    m_xLight=Light("");
    std::vector<Zeroconf::mdns_responce> result;
    bool st = Zeroconf::Resolve("_elg._tcp.local.", timeout, &result);

    if (!st)
        throw "MDNS query failed";
    else
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            auto& item = result[i];

            char buffer[INET6_ADDRSTRLEN + 1] = {0};
            inet_ntop(item.peer.ss_family, get_in_addr(&item.peer), buffer, INET6_ADDRSTRLEN);
            m_xLight=Light(buffer);
        }
    }
    return m_xLight;
}

//------------------------------------------------------------------------------------------
void Elgato::parse_status(const char * buffer) 
{
    const char * ss[3];
    ss[0]="\"lights\":[{\"on\":";
    ss[1]="\"brightness\":";
    ss[2]="\"temperature\":";
    int iOldStatus=m_xLight.status;
    for (int i=0;i<3;i++)
    {
        const char * pos=strstr(buffer,ss[i]);
        if (pos==NULL) throw "unexpected answer from the server";
        pos+=strlen(ss[i]);
        if (i==0) m_xLight.status=atoi(pos);
        if (i==1) m_xLight.brightness=atoi(pos);
        if (i==2) m_xLight.temperature=atoi(pos);
    }
    if (iOldStatus!=m_xLight.status)
        m_xLight.status_change_ts=(unsigned long)time(NULL);
}

//------------------------------------------------------------------------------------------
void Elgato::send(const char * address, const char * buffer)
{
    int s = socket(AF_INET , SOCK_STREAM , 0);
    
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_family = AF_INET;
    server.sin_port = htons( 9123 );
    
    
    if (connect(s , (struct sockaddr *)&server , sizeof(server))==-1)
        throw "can't connect to the server";
    
    if (::send(s, buffer, strlen(buffer) , 0)==-1)
        throw "can't send command";
    
    char rbuf[1024];
    int ret=(int)::recv(s, (void*)rbuf, (int)sizeof(rbuf) , 0);
    if (ret==-1)
        throw "can't send command";
    rbuf[ret]=0;
    parse_status(rbuf);
    close(s);
}

//------------------------------------------------------------------------------------------
void Elgato::refresh_status()
{
    const char m[]=
    "GET /elgato/lights HTTP/1.1\r\n"
    "Host: %s:9123\r\n"
    "Accept: */*\r\n"
    "Accept-Language: en-us\r\n"
    "User-Agent: fcc-companion-app\r\n\r\n";
    
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), m, light().address.c_str());
    send(light().address.c_str(),buffer);
}

//------------------------------------------------------------------------------------------
void Elgato::switch_light(bool bOn)
{
    const char m[]=
    "PUT /elgato/lights HTTP/1.1\r\n"
    "Host: %s:9123\r\n"
    "User-Agent: fcc-companion-app\r\n"
    "Accept: */*\r\n"
    "Content-Length: 40\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
    "{\"numberOfLights\":1,\"lights\":[{\"on\":%i}]}";
    
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), m, light().address.c_str(), bOn);
    send(light().address.c_str(),buffer);
}

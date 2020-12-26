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
Elgato::Elgato(): m_sMdnsQuery("_elg._tcp.local.")
{
    m_uOnTS=0;
}

//------------------------------------------------------------------------------------------
const std::vector<std::string> & Elgato::devices() const
{
    return m_xDevices;
} 

//------------------------------------------------------------------------------------------
unsigned long Elgato::getOnTS() const
{
    return m_uOnTS;
}

//------------------------------------------------------------------------------------------
const std::vector<std::string> & Elgato::refresh(int timeout)
{
    m_xDevices.clear();
    std::vector<Zeroconf::mdns_responce> result;
    bool st = Zeroconf::Resolve(m_sMdnsQuery, timeout, &result);

    if (!st)
        throw "MDNS query failed";
    else
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            auto& item = result[i];

            char buffer[INET6_ADDRSTRLEN + 1] = {0};
            inet_ntop(item.peer.ss_family, get_in_addr(&item.peer), buffer, INET6_ADDRSTRLEN);
            m_xDevices.push_back(buffer);
        }
    }
    return m_xDevices;
}

//------------------------------------------------------------------------------------------
void Elgato::lights(bool bOn)
{
    const char m[]=
    "PUT /elgato/lights HTTP/1.1\r\n"
    "Host: %s:9123\r\n"
    "User-Agent: curl/7.64.1\r\n"
    "Accept: */*\r\n"
    "Content-Length: 40\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
    "{\"numberOfLights\":1,\"lights\":[{\"on\":%i}]}";
    
    
    for (int i=0;i<m_xDevices.size();i++)
    {
        int s = socket(AF_INET , SOCK_STREAM , 0);
        
        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(m_xDevices[i].c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons( 9123 );
        
        char buffer[1024];
        if (connect(s , (struct sockaddr *)&server , sizeof(server))==-1)
            throw "can't connect to the server";
        
        
        snprintf(buffer, sizeof(buffer), m, devices()[0].c_str(), bOn);
        if (send(s, buffer, strlen(buffer) , 0)==-1)
            throw "can't send command";
        close(s);
    }
    if (bOn) m_uOnTS=(unsigned long)time(NULL);
    else m_uOnTS=0;
    
    
}

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "elgato/Elgato.hpp"
#include "fcc/LibVoipEvents.hpp"

int main(int argc, char** argv)
{
#ifdef WIN32
    WSADATA wsa = {0};
    if (WSAStartup(0x202, &wsa) != 0)
    {
        std::cout << "Unable to initialize WinSock" << std::endl;
        return 1;
    }
#endif

    while(1)
    {
        try
        {
            Elgato el;
            while (el.devices().size()==0)
            {
                std::cout<<"refreshing Elgato devices ..."<<std::endl;
                el.refresh();
            }
            std::cout<<"found "<<el.devices().size()<<" device(s)"<<std::endl;
            for (int i=0;i<el.devices().size();i++)
                std::cout<<"\t"<<el.devices()[i]<<std::endl;
            LibVoipEvents lib;
            std::cout<<"waiting for FCC events ..."<<std::endl;
            while(1)
            {
                LibVoipEvents::Event event=lib.read();
                if (event.type()==LibVoipEvents::Event::TIMEOUT)
                {
                    if (el.getOnTS()!=0 && (unsigned long)time(NULL)-el.getOnTS()>7200) //2h
                    {
                        std::cout<<"Turning light on the timeout"<<std::endl;
                        el.lights(false);
                    } 
                }
                else if (event.type()==LibVoipEvents::Event::CAMERA && event.payload_size()==1)
                {
                    bool bOn=*event.payload()==1;
                    std::cout<<"Camera is "<<(bOn?"ON":"OFF")<<std::endl;
                    el.lights(bOn);
                }
            }
        }
        catch (const char * err)
        {
            std::cout<<"Something went wrong: "<<std::endl;
            perror(err);
            sleep(5);
        }
    }
    
#ifdef WIN32
    WSACleanup();
#endif

    return 0;
}

#ifndef Elgato_h
#define Elgato_h

#include <string>
#include <vector>

class Elgato
{
public:
    
    struct Light
    {
        Light(std::string host);
        std::string address;
        int status;
        unsigned long status_change_ts;
        int temperature;
        int brightness;
    };

private:
    
    Light m_xLight;  //intentionally work with one device only
    void send(const char * address, const char * buffer);
    void parse_status(const char * buffer);

public:
    Elgato();
    const Light & light() const;
    const Light & refresh(int timeout=5);
    
    void switch_light(bool bOn);
    void refresh_status();
};

#endif 

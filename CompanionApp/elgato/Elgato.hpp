#ifndef Elgato_h
#define Elgato_h

#include <string>
#include <vector>

class Elgato
{
private:
    const std::string m_sMdnsQuery;
    std::vector<std::string> m_xDevices;
    unsigned long m_uOnTS;
    
public:
    Elgato();
    const std::vector<std::string> & devices() const;
    const std::vector<std::string> & refresh(int timeout=5);
    void lights(bool bOn);
    unsigned long getOnTS() const;
    
};

#endif 

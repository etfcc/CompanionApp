
#ifndef LibVoipEvents_hpp
#define LibVoipEvents_hpp


class LibVoipEvents
{
public:  
    struct Event
    {
        Event();
        static const int            TIMEOUT=0;
        static const int            CAMERA=1;
        unsigned char               buf[128];
        unsigned                    size;
        int version()               {return buf[0];}
        int type()                  {return buf[1];}
        int payload_size()          {return (buf[3]<<8 | buf[2]);}
        unsigned char * payload()   {return buf+4;}
    };
    
public:
   
    LibVoipEvents();
    ~LibVoipEvents();
    Event read();
    
private:
    
    int m_iSockfd;
};

#endif 

#ifndef BBRAUN_H
#define BBRAUN_H

#include "device.h"
#include <vector>

#define SOHCHAR 0x01
#define STXCHAR  0x02
#define ETXCHAR  0x03
#define ETBCHAR  0x17
#define EOTCHAR  0x04
#define RSCHAR  0x1E
#define DCHAR  0x44
#define ECHAR  0x45
#define dCHAR  0x64
#define eCHAR  0x65
#define XCHAR  0x58
#define xCHAR  0x78
#define ACKCHAR 0x06
#define NAKCHAR 0x15

class Bbraun: public Device
{
public:
    Bbraun();
    void request_initialize_connection();
private:
    bool m_fstart = false;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    std::vector<unsigned char>b_list;

    void create_frame_list_from_byte(unsigned char b);
    ~Bbraun(){};
};

#endif // BBRAUN_H

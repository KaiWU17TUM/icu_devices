#ifndef EVITA4_VENT_H
#define EVITA4_VENT_H

#include "device.h"
#include "draeger.h"
#include <vector>
#include "stdio.h"
#include <chrono>
#include <ctime>
#include <QFile>

class Evita4_vent: public Device
{  
    Q_OBJECT
public:
    Evita4_vent();
    ~Evita4_vent(){};
    void write_buffer(std::vector<byte> cmd);
    void create_frame_list_from_byte(byte b);
    void read_packet_from_frame();

private:
    QString pathcsv = ("/home/wei/Documents/DHM/ICU_devices/DrgVentDataExport.csv");
    bool m_storestart2 = true;
    bool m_storestart1 = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    std::vector<byte>b_list;
    std::vector<std::vector<byte>>frame_buffer;
    std::string pkt_timestamp;
    std::vector<NumVal> numval_list;
    std::vector<std::string> header_list;
    Q_DISABLE_COPY(Evita4_vent);
    void add_checksum(std::vector<byte>& payload);
    //void create_frame_list_from_byte(byte b);
    //void read_packet_from_frame();
    void command_echo_response(std::vector<byte>& cmd);
    void parse_data_response_measured(std::vector<byte> &packetbuffer, byte type);
    void save_value_list_rows();

public slots:
    void process_buffer();
};



#endif // EVITA4_VENT_H

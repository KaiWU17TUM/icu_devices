#ifndef EVITA4_VENT_H
#define EVITA4_VENT_H

#include "device.h"
#include "draeger.h"
#include <vector>
#include "stdio.h"
#include <chrono>
#include <ctime>
#include <QFile>
#include <QTimer>
#include <iostream>

class Evita4_vent: public Device
{  
    Q_OBJECT
public:
    Evita4_vent();
    void start();
    ~Evita4_vent(){};
    void write_buffer(std::vector<byte> cmd);
    void create_frame_list_from_byte(byte b);
    void read_packet_from_frame();


private:
    QTimer *timer_cp1;
    std::string pathcsv = ("/home/wei/Documents/DHM/ICU_devices/DrgVentDataExport.csv");
    bool m_storestart2 = true;
    bool m_storestart1 = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    std::vector<byte>b_list;
    std::vector<std::vector<byte>>frame_buffer;
    std::string pkt_timestamp;
    std::vector<NumVal> numval_list;
    std::vector<std::string> header_list;
    bool m_transmissionstart = true;
    bool m_transmissionstart2 = true;
    bool m_transmissionstart3 = true;
    bool m_transmissionstart4 = true;

    Q_DISABLE_COPY(Evita4_vent);
    void add_checksum(std::vector<byte>& payload);
    //void create_frame_list_from_byte(byte b);
    //void read_packet_from_frame();
    void command_echo_response(std::vector<byte>& cmd);
    void parse_data_response_measured(std::vector<byte> &packetbuffer, byte type);
    void parse_data_device_settings(std::vector<byte> &packetbuffer);
    void parse_data_text_settings(std::vector<byte> &packetbuffer);
    bool write_header_for_data_type(std::string datatype);
    void write_num_header_list(std::string datatype);
    void save_num_val_list_rows(std::string datatype);

public slots:
    void process_buffer();
    void request_icc(){this->write_buffer({poll_request_icc_msg});}
    void request_dev_id(){this->write_buffer({poll_request_deviceid});}
    void request_measurement_cp1(){this->write_buffer({poll_request_config_measured_data_codepage1});qDebug()<<"require measuremnt 1";}
    void request_measurement_cp2(){this->write_buffer({poll_request_config_measured_data_codepage2});}
    void request_device_settings(){this->write_buffer({poll_request_device_settings});}
    void request_text_messages(){this->write_buffer({poll_request_text_messages});}
    void request_stop_communication(){this->write_buffer({poll_request_stop_com});}
};



#endif // EVITA4_VENT_H

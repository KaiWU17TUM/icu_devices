#ifndef EVITA4_VENT_H
#define EVITA4_VENT_H

#include "device.h"
#include "draeger.h"
#include <vector>
#include "stdio.h"
#include <chrono>
#include <ctime>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <iostream>
#include <math.h>
#include "QThread"
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
    QTimer *alarm_low_limit_timer;
    QTimer *alarm_high_limit_timer;
    QTimer *alarm_codebook1_timer;
    QTimer *alarm_codebook2_timer;
    QTimer *measurement_timer;
    QTimer *logger_timer;
//    unsigned long int timelapse;

    QString pathcsv = QDir::currentPath() + "/../icu_devices/data/draeger_evita4/";
    bool free_flag = true;
    bool sync_data = false;
    bool new_data = false;
    bool m_storestart2 = true;
    bool m_storestart1 = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    std::vector<byte>b_list;
    std::vector<short>realtime_data_list;
    std::vector<unsigned char> sync_cmd;
    std::vector<unsigned char> realtime_transmission_request;

    std::vector<std::vector<byte>>frame_buffer;
    std::string pkt_timestamp;
    std::vector<NumVal> numval_list;
    std::vector<AlarmInfo> alarm_list;
    std::vector<std::string> header_list;
    std::vector<RealtimeCfg> cfg_list;
    bool m_transmissionstart = true;
    bool m_transmissionstart2 = true;
    bool m_transmissionstart3 = true;
    bool m_transmissionstart4 = true;
    bool m_transmissionstart5 = true;


    Q_DISABLE_COPY(Evita4_vent);
    void add_checksum(std::vector<byte>& payload);

    void command_echo_response(std::vector<byte>& cmd);
    void parse_data_response_measured(std::vector<byte> &packetbuffer, byte type);
    void parse_data_device_settings(std::vector<byte> &packetbuffer);
    void parse_data_text_settings(std::vector<byte> &packetbuffer);
    void parse_realtime_data_configs(std::vector<byte> &packetbuffer);
    void parse_realtime_data(std::vector<byte> &packetbuffer);
    void parse_alarm(std::vector<byte> &packetbuffer);
    bool write_header_for_data_type(std::string datatype);
    void write_num_header_list(std::string datatype, QString filename);
    void save_num_val_list_rows(std::string datatype);
    void save_alarm_list_rows();

public slots:
    void save_data();
    void process_buffer();
    void request_icc(){if(free_flag){free_flag=false;this->write_buffer({poll_request_icc_msg});}}
    void request_dev_id(){if(free_flag){free_flag=false;this->write_buffer({poll_request_deviceid});}}
    void request_measurement_cp1(){if(free_flag){free_flag=false;this->write_buffer({poll_request_config_measured_data_codepage1});}}
    void request_alarm_low_limit(){if(free_flag){free_flag=false;this->write_buffer({poll_request_low_alarm_limits});qDebug()<<"require low alarm limit";}}
    void request_alarm_high_limit(){if(free_flag){free_flag=false;this->write_buffer({poll_request_high_alarm_limits});qDebug()<<"require high alarm limit";}}
    void request_alarmCP1(){if(free_flag){free_flag=false;this->write_buffer({poll_request_alarmCP1});qDebug()<<"require alarmCP1";}}
    void request_alarmCP2(){if(free_flag){free_flag=false;this->write_buffer({poll_request_alarmCP2});qDebug()<<"require alarmCP2";}}
    void request_realtime_config(){{qDebug()<<("RTCFG");free_flag=false;this->write_buffer({request_realtime_configuration});sync_data=false;}}
    void request_realtime_data(){this->write_buffer({realtime_transmission_request});}
    void request_sync(){this->write_buffer({sync_cmd});}
    void request_device_settings(){if(free_flag){free_flag=false;this->write_buffer({poll_request_device_settings});}}
    void request_text_messages(){if(free_flag){free_flag=false;this->write_buffer({poll_request_text_messages});}}
    void request_stop_communication(){if(free_flag){free_flag=false;this->write_buffer({poll_request_stop_com});}}

};




#endif // EVITA4_VENT_H

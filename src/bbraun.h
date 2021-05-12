#ifndef BBRAUN_H
#define BBRAUN_H

#include "device.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <QFile>
#include <QTimer>
#include <map>
#include <QDir>
#include "bbraun_const.h"


class Bbraun: public Device
{
       Q_OBJECT
public:
    Bbraun();
    void start();
    ~Bbraun(){};
private:
    std::map<std::string, std::string> GeneralParameters;
    std::map<std::string, std::string> InfusionPumpParameters;
    std::map<std::string, std::string> AdditionalParameters;
    QTimer *timer_cp1;
    std::vector<byte> m_bedid;
    QString pathcsv = QDir::currentPath()+"/../icu_devices/data/bbraun_perfusor/";
    bool ack_flag = false;
    bool m_fstart = false;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    bool m_transmissionstart = true;
    bool m_transmissionstart2 = true;
    bool m_transmissionstart3 = true;
    bool m_transmissionstart4 = true;

    std::vector<unsigned char>b_list;
    std::vector<std::vector<byte>>frame_buffer;
    std::string pkt_timestamp;
    std::vector<NumValB> numval_list;
    std::vector<std::string> header_list;

    void write_buffer(std::vector<byte> bedid, std::vector<byte> txbuf);

    byte compute_checksum(std::vector<byte> bytes);
    void create_frame_list_from_byte(unsigned char b);
    void int_save_to_buffer(int integer, std::vector<byte> &bytes);
    void send_ack();
    void request_initialize_connection();

    void read_packet_from_frame();
    std::vector<std::string> split_string(std::string s, byte delimiter);
    void save_num_values_by_type();
    void save_num_value_list_row(std::string datatype);
    bool write_header_for_data_type(std::string datatype);
    void write_num_header_list(std::string datatype, QString filename);

private slots:
    void process_buffer();
    void send_get_mem_request();

};

#endif // BBRAUN_H

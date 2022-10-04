#ifndef DATEX_OHMEDA_H
#define DATEX_OHMEDA_H
#include "protocol.h"
#include "datex.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <ctime>
#include <chrono>

class Datex_ohmeda : public Protocol
{
public:
    Datex_ohmeda(std::string config_file, Device *device);
    void from_literal_to_packet(byte b);
    void from_packet_to_structures();
    void save_data();
    void send_request();
    void write_buffer(byte *payload, int length);
    ~Datex_ohmeda() = default;

private:
    std::vector<struct NumericValueDatex> m_NumericValueList;
    std::vector<struct WaveValueDatex> m_WaveValueList;
    std::vector<struct AlarmDatex> m_AlarmList;
    std::vector<std::string> m_NumValHeaders;

    std::string machine_datetime;
    unsigned long int machine_timestamp;

    bool m_fstart = true;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    bool m_transmissionstart = true;

    int phdb_data_interval;
    std::string filename_phdb;
    std::string filename_alarm;
    std::map<std::string, std::string> filenames_wave;
    std::vector<byte> wave_ids;

    void load_protocol_config(std::string config_file);
    void save_basic_sub_record(datex::dri_phdb driSR);
    void save_ext1_and_ext2_and_ext3_record(datex::dri_phdb driSR);
    void validate_add_data(std::string physio_id, short value,
                           double decimalshift, bool rounddata);

    void save_alarm_to_csv();
    void save_wave_to_csv();
    void save_numeric_to_csv();
    void write_header(std::string filename);
};

#endif // DATEX_OHMEDA_H

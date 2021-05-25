#ifndef GE_MONITOR_H
#define GE_MONITOR_H
#include "device.h"
#include "datex.h"
#include <vector>
#include <QDebug>
#include <QFile>
#include <iostream>
#include <math.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <QDir>

class GE_Monitor: public Device
{
Q_OBJECT
public:
    GE_Monitor();
    void start();

    ~GE_Monitor(){};

private:
    bool m_fstart = true;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    std::string machine_timestamp;
    std::time_t pc_time;
    std::string m_DeviceID = "GE_Monitor";
    bool m_transmissionstart = true;
    QString pathcsv = QDir::currentPath()+"/../icu_devices/data/ge_monitor_b650/";
    std::vector<struct NumericValResult> m_NumericValList;
    std::vector<struct WaveValResult> m_WaveValList;
    std::vector<struct AlarmResult> m_AlarmList;
    std::vector<std::string> m_NumValHeaders;

    std::vector<std::vector<unsigned char>>frame_buffer;
    std::vector<unsigned char>b_list;

    void request_phdb_transfer();
    void request_wave_transfer();
    void tx_buffer(byte* payload, int length);

    void create_frame_list_from_byte(byte b);
    void read_packet_from_frame();

    void validate_add_data(std::string physio_id, short value, double decimalshift, bool rounddata);
    std::string validate_wave_data(short value, double decimalshift, bool rounddata);
    void save_basic_sub_record(datex::dri_phdb driSR);
    void write_to_rows();
    void write_to_file_header(QString filename);
    void save_ext1_and_ext2_and_ext3_record(datex::dri_phdb driSR);
    double get_wave_unit_shift(std::string physioId);
    void request_alarm_transfer();
    void save_wave_to_csv();
    void save_alarm_to_csv();
    void request_wave_stop();
    Q_DISABLE_COPY(GE_Monitor);


public slots:
    void process_buffer();

};

/*******************/
struct NumericValResult
{
    std::string Timestamp;
    std::string PhysioID;
    std::string Value;
    std::string DeviceID;
};

struct  WaveValResult
{
    std::string Timestamp;
    std::vector<unsigned long int> TimeList;
    std::string PhysioID;
    std::vector<short> Value;
    std::string DeviceID;
    double Unitshift;
};

#pragma pack(1)
struct  AlarmResult
{
    std::string Timestamp;
    std::string text;
    std::string color;
    std::string sound;
};
#endif // GE_MONITOR_H

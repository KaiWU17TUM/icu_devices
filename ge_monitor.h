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
    std::string m_strTimestamp;
    std::string m_DeviceID = "GE_Monitor";
    bool m_transmissionstart = true;
    std::string pathcsv = ("/home/wei/Documents/DHM/ICU_devices/");
    std::vector<struct NumericValResult> m_NumericValList;
    std::vector<struct WaveValResult> m_WaveValList;
    std::vector<std::string> m_NumValHeaders;

    std::vector<std::vector<unsigned char>>frame_buffer;
    std::vector<unsigned char>b_list;

    void request_phdb_transfer();
    void create_frame_list_from_byte(byte b);
    void read_packet_from_frame();
    void request_wave_transfer();
    void tx_buffer(byte* payload, int length);
    void validate_add_data(std::string physio_id, short value, double decimalshift, bool rounddata);
    std::string validate_wave_data(short value, double decimalshift, bool rounddata);
    void save_basic_sub_record(datex::dri_phdb driSR);
    void write_to_rows();
    void write_to_file_header();
    void save_ext1_and_ext2_record(datex::dri_phdb driSR);
    double get_wave_unit_shift(std::string physioId);
    void save_wave_to_csv();
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
    std::string PhysioID;
    std::vector<short> Value;
    std::string DeviceID;
    double Unitshift;
};

#endif // GE_MONITOR_H

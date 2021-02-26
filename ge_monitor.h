#ifndef GE_MONITOR_H
#define GE_MONITOR_H
#include "device.h"
#include "datex.h"
#include <vector>
#include <QDebug>
#include <QFile>


class GE_Monitor: public Device
{
Q_OBJECT
public:
    GE_Monitor();
    void prepare_phdb_request();
    void create_frame_list_from_byte(byte b);
    void create_record_list();
    ~GE_Monitor(){};

private:
    bool m_fstart = true;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    QString m_strTimestamp;
    QString m_DeviceID = "GE_Monitor";
    bool m_transmissionstart = true;
    QString pathcsv = ("/home/wei/Documents/DHM/ICU_devices/AS3DataExport.csv");
    std::vector<struct NumericValResult> m_NumericValList;
    std::vector<QString> m_NumValHeaders;

    std::vector<std::vector<unsigned char>>frame_buffer;
    std::vector<unsigned char>b_list;


    void tx_buffer(byte* payload, int length);
    void validate_add_data(QString physio_id, short value, double decimalshift, bool rounddata);
    void save_basic_sub_record(datex::dri_phdb driSR);
    void write_to_rows();
    void write_to_file_header();
    //void create_frame_list_from_byte(byte b);
    //void create_record_list();
    Q_DISABLE_COPY(GE_Monitor);


public slots:
    void process_buffer();

};

/*******************/
struct NumericValResult
{
    QString Timestamp;
    QString PhysioID;
    QString Value;
    QString DeviceID;
};

struct  WaveValResult
{
    QString Timestamp;
    QString PhysioID;
    short Value;
    QString DeviceID;
    double Unitshift;
};

#endif // GE_MONITOR_H

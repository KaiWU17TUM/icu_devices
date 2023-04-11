#ifndef MEDIBUS_H
#define MEDIBUS_H
#include "protocol.h"
#include "draeger.h"
#include <QThread>

class Medibus : public Protocol
{
    Q_OBJECT
public:
    Medibus(std::string config_file, Device *device);
    void send_request();
    void from_literal_to_packet(byte b);
    void from_packet_to_structures();
    void write_buffer(byte *payload, int length){};  // not used
    // void write_buffer(std::vector<byte> &bedid, std::vector<byte> &txbuf);
    void write_buffer(std::vector<unsigned char> cmd);
    void save_data();

    ~Medibus() = default;

private:
    unsigned long int create_files_timer_ms;
    
    std::string parse_datetime;
    unsigned long int parse_timestamp;

    bool free_flag = true;
    bool sync_data = false;
    bool new_data = false;
    bool m_storestart2 = true;
    bool m_storestart1 = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    bool m_transmissionstart = true;
    bool m_transmissionstart2 = true;
    bool m_transmissionstart3 = true;
    bool m_transmissionstart4 = true;
    bool m_transmissionstart5 = true;

    QTimer *measurement_timer;
    QTimer *alarm1_timer;
    QTimer *alarm2_timer;
    QTimer *alarm_low_limit_timer;
    QTimer *alarm_high_limit_timer;

    std::time_t measurement_time_interval;
    std::time_t alarm1_time_interval;
    std::time_t alarm2_time_interval;
    std::time_t alarm_low_limit_time_interval;
    std::time_t alarm_high_limit_time_interval;

    std::string filename_measurement;
    std::string filename_low_limit;
    std::string filename_high_limit;
    std::string filename_alarm;

    std::vector<short> realtime_data_list;
    std::vector<unsigned char> sync_cmd;
    std::vector<unsigned char> realtime_transmission_request;

    std::vector<NumericValueDraeger> m_NumericValueList;
    std::vector<AlarmInfo> m_AlarmInfoList;
    std::vector<RealtimeCfg> m_RealtimeCfgList;

    void load_protocol_config(std::string config_file);
    void create_files();
    void create_request_timers();
    void parse_data_text_settings(std::vector<byte> &packetbuffer);
    void parse_data_device_settings(std::vector<byte> &packetbuffer);
    void parse_data_response_measured(std::vector<byte> &packetbuffer, byte type);
    void parse_alarm(std::vector<byte> &packetbuffer);
    void command_echo_response(std::vector<byte> &cmd);
    void parse_realtime_data_configs(std::vector<byte> &packetbuffer);
    void parse_realtime_data(std::vector<byte> &packetbuffer);

    void save_numeric_value_list_to_row(std::string datatype);
    void save_m_AlarmInfoList_rows();
    void write_numeric_value_list_header(std::string datatype, std::string filename);
    bool numeric_value_list_header_selector(std::string datatype);

public slots:
    void request_icc()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_icc_msg});
        }
    }
    void request_dev_id()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_deviceid});
        }
    }
    void request_measurement_cp1()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_config_measured_data_codepage1});
        }
    }
    void request_alarm_low_limit()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_low_alarm_limits});
            qDebug() << "require low alarm limit";
        }
    }
    void request_alarm_high_limit()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_high_alarm_limits});
            qDebug() << "require high alarm limit";
        }
    }
    void request_alarmCP1()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_alarmCP1});
            qDebug() << "require alarmCP1";
        }
    }
    void request_alarmCP2()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_alarmCP2});
            qDebug() << "require alarmCP2";
        }
    }
    void request_realtime_config()
    {
        {
            qDebug() << ("RTCFG");
            free_flag = false;
            this->write_buffer({request_realtime_configuration});
            sync_data = false;
        }
    }
    void request_realtime_data() { this->write_buffer({realtime_transmission_request}); }
    void request_sync() { this->write_buffer({sync_cmd}); }
    void request_device_settings()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_device_settings});
        }
    }
    void request_text_messages()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_text_messages});
        }
    }
    void request_stop_communication()
    {
        if (free_flag)
        {
            free_flag = false;
            this->write_buffer({poll_request_stop_com});
        }
    }
};

#endif // MEDIBUS_H

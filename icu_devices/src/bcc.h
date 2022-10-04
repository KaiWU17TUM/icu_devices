#ifndef BCC_H
#define BCC_H
#include <chrono>
#include <QObject>
#include "protocol.h"
#include "bbraun_const.h"
#include "device.h"

class Bcc : public Protocol
{
    Q_OBJECT
public:
    Bcc(std::string config_file, Device *device);
    void from_literal_to_packet(byte b);
    void from_packet_to_structures();
    void save_data();
    void send_request();
    void write_buffer(byte *payload, int length){};
    void write_buffer(std::vector<byte> &bedid, std::vector<byte> &txbuf);

    //    virtual ~Bcc(){};

private:
    QTimer *request_timer;
    std::vector<byte> m_bedid;
    bool ack_flag = false;
    bool m_fstart = false;
    bool m_storestart = false;
    bool m_storeend = false;
    bool m_bitschiftnext = false;
    bool m_transmissionstart = true;
    bool m_transmissionstart2 = true;
    bool m_transmissionstart3 = true;
    bool m_transmissionstart4 = true;

    std::vector<NumericValueBbraun> m_NumericValueList;

    int interval;
    std::string filename_GeneralP;
    std::string filename_InfusionPumpP;
    std::string filename_UndefinedP;
    std::string filename_AdditionalP;

    void load_protocol_config(std::string config_file);
    void send_ack();
    void request_initialize_connection();
    void save_numeric_value_list_to_row(std::string filename, std::string datatype);
    bool numeric_value_list_header_selector(std::string datatype);
    void write_numeric_value_list_header(std::string datatype, std::string filename);
    Q_DISABLE_COPY(Bcc);

public slots:
    void send_get_mem_request();
};

#endif // BCC_H

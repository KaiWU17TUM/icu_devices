#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <string>
#include <math.h>
#include <QDebug>
#include <cstring>
#include <QTimer>
#include <QObject>

class Device;
enum protocol {datex_ohmeda, bbraun, draeger};
typedef unsigned char byte;

class Protocol: public QObject
{
    Q_OBJECT

public:
    Device* device;
    std::vector<std::vector<unsigned char>>frame_buffer;
    Protocol(std::string config_file, Device* device);
    //void bind_to_serial_port(MySerialPort* serial);
    //void bind_to_structures();
    virtual void write_buffer(byte* payload, int length)=0;
    virtual void from_literal_to_packet(byte b)=0;
    virtual void from_packet_to_structures()=0;
    virtual void save_data()=0;
    virtual void send_request()=0;
    virtual ~Protocol(){};

protected:
    std::vector<unsigned char>b_list;

private:
    Q_DISABLE_COPY(Protocol);
//private:
    //void load_protocol_config(std::string config_file){};
};

#endif // PROTOCOL_H

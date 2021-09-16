#ifndef DEVICE_H
#define DEVICE_H
#include "myserialport.h"
#include "logger.h"
#include "protocol.h"
#include "datex_ohmeda.h"
#include <string>
#include <fstream>
#include <QSerialPort>
#include <QByteArray>
#include <vector>
#include <QObject>
#include <QThread>
#include <QMetaMethod>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

class Device: public QObject
{
    Q_OBJECT

public:
     Device(const std::string config_file);
     void try_to_open_port();
     void send_request();
//     MySerialPort* get_serial(){
//         return local_serial_port;}
     Logger* get_logger(){return local_logger;}
     virtual void start()=0;
     void write_buffer(std::vector<byte>* temptxbuff, qint64 len);
     virtual ~Device(){};

protected:
     Logger* local_logger;
     MySerialPort* local_serial_port;
     Protocol* local_protocol;

private:
     std::string config_file;
     Q_DISABLE_COPY(Device);

public slots:
    void process_buffer();
    void save_data();
};

#endif // DEVICE_H

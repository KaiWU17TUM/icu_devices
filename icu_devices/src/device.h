#ifndef DEVICE_H
#define DEVICE_H
#include "myserialport.h"
#include "logger.h"
#include "protocol.h"
#include "datex_ohmeda.h"
#include "bcc.h"
#include "medibus.h"
#include <string>
#include <fstream>
#include <chrono>
#include <QSerialPort>
#include <QByteArray>
#include <vector>
#include <QObject>
#include <QMetaMethod>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

class Device : public QObject
{
    Q_OBJECT

public:
    Device(std::string device_name, std::string config_file, std::string general_config, std::string protocol);
    Logger *get_logger() { return local_logger; }
    void start();
    void write_buffer(const char *temptxbuff, qint64 len);

    Logger *local_logger;
    MySerialPort *local_serial_port;
    Protocol *local_protocol;

private:
    std::string device_name;
    std::string config_file;
    void try_to_open_port();
    void send_request();
    std::string get_measurement_folder_name(std::string config_file);
    Q_DISABLE_COPY(Device);

public slots:
    void process_buffer();
    void save_data();
};

#endif // DEVICE_H

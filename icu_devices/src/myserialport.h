#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H
#include <QSerialPort>
#include <QObject>
#include <QDebug>
#include <string>
#include <fstream>
#include <iostream>
#include <QMetaMethod>
#include <QThread>

typedef unsigned char byte;

class MySerialPort
{
public:
    MySerialPort(const std::string config_file);
    void open_serial_port();
    void write_data(const char *temptxbuff, qint64 len);
    void connect_serial_port(const QObject *receiver, const char *slot);
    QSerialPort *get_serial_port() { return serial; }

private:
    QSerialPort *serial;
    void load_serial_configs(const std::string config_file);
};

#endif // MYSERIALPORT_H

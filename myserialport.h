#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H
#include <QSerialPort>
#include <QObject>
#include <QDebug>

class MySerialPort
{
public:
    QSerialPort* serial;
    MySerialPort();
    void open_serial_port();
    void write_data(const QByteArray &data);


};

#endif // MYSERIALPORT_H

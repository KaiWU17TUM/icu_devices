#include "myserialport.h"
#include <QDebug>
#include <iostream>

MySerialPort::MySerialPort()
{
    serial = new QSerialPort();
    this->open_serial_port();
}

void MySerialPort::open_serial_port()
{
    if (serial->open(QIODevice::ReadWrite)) {
        std::cout<<"open port success\n"<<std::endl;

    } else {
        std::cout<<"open port failed\n"<<std::endl;
    }
}



void MySerialPort::write_data(const QByteArray &data)
{
    serial->write(data);
}


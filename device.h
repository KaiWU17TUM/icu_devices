#ifndef DEVICE_H
#define DEVICE_H
#include "myserialport.h"
#include <QSerialPort>
#include <QByteArray>
#include <vector>
#include <QObject>

class Device: public QObject
{
    Q_OBJECT

public:
     Device();
     MySerialPort* local_serial_port;



    void try_to_open_port();
    virtual ~Device(){};
private:
    Q_DISABLE_COPY(Device);
};

#endif // DEVICE_H

#ifndef DEVICE_H
#define DEVICE_H
#include "myserialport.h"
#include <QSerialPort>
#include <QByteArray>
#include <vector>
#include <QObject>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

class Device: public QObject
{
    Q_OBJECT

public:
     Device();
     MySerialPort* local_serial_port;

    virtual void start(){};
    void try_to_open_port();
    virtual ~Device(){};
private:
    Q_DISABLE_COPY(Device);
};

#endif // DEVICE_H

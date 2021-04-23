#include <QCoreApplication>
#include "ge_monitor.h"
#include "evita4_vent.h"
#include "bbraun.h"
#include <QDebug>
#include <vector>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    Bbraun my_perfusor;
    my_perfusor.start();


  /*
    GE_Monitor my_monitor;
    my_monitor.start();

    Evita4_vent my_evita4_vent;
    my_evita4_vent.start();
*/
    /*uint8_t example_input[] ={0x7E ,0X80 ,0X04 ,0X00 ,0X0B ,0X00 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X16 ,0X01 ,0X01 ,0X2C ,0X02 ,0X01 ,0X42 ,0X03 ,0X01 ,0X00 ,0X00 ,0XFF ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X4B ,0X00 ,0X00 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X03 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X02 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X03 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0B ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0C ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0D ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0E ,0X00 ,0X01 ,0X80 ,0X01 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X09 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0XAE ,0X1D ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X20 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X1F ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X02 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0D ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0E ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X4B ,0X00 ,0X01 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X4B ,0X00 ,0X00 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X14 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X15 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X01 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X40 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X09 ,0X03 ,0X0F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XCC ,0X14 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X02 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X03 ,0X52 ,0X7E};
    for (int i = 0; i < 1155; ++i) {
        my_monitor.create_frame_list_from_byte(example_input[i]);
    }*/
    /*
    std::vector<byte> data = {0x01, 0x24, 0x45, 0x42,0x20,0x39,0x38,0x20,0x45,0x31,0x20,0x37,0x30,0x20,0x37,0x41,0x0d};
    for (int i = 0; i < data.size(); ++i) {
        my_evita4_vent.create_frame_list_from_byte(data[i]);
    }*/
    //my_monitor.create_record_list();


    return a.exec();
}

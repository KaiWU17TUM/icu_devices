#include <QCoreApplication>
#include "device.h"
#include <QDebug>
#include <vector>
#include <ctime>
#include <filesystem>

int main(int argc, char *argv[])
{

//    if(argc<2)
//    {
//        printf("Please enter the path to the config folder...\n");
//        return 0;
//    }
//    if(argc>2)
//    {
//        printf("Too much arguments. Please enter only the path to the config folder...\n");
//        return 0;
//    }

    std::string base_config_path = argv[1];
//    std::string base_config_path = "/home/kai/workspace/check-version-icu-device/icu-suite_check/repositories/icu_devices/icu_devices/cfg";

    QCoreApplication a(argc, argv);
    // B,C,D => USB1,2,3

    // Uncomment the following lines to enable or disable the data retrieving from hardwares
    std::string general_config_file = base_config_path + "/general_config.txt";

    std::string ge_config_file = base_config_path + "/ge_config.txt";
    Device my_monitor("ge_monitor_b650", ge_config_file, general_config_file, "datex_ohmeda");
    my_monitor.start();

//    std::string bb_config_file = base_config_path + "/bb_config.txt";
//    Device my_perfusor("bbraun", bb_config_file, general_config_file, "bcc");
//    my_perfusor.start();

//    std::string ev_config_file = base_config_path + "/ev_config.txt";
//    Device my_evita4_vent("evta4", ev_config_file, general_config_file, "medibus");
//    my_evita4_vent.start();

    /****************************************************************************************************************************/
    // Codes below are for debugging purpose, please ignore them
    /*uint8_t example_input[] ={0x7E ,0X80 ,0X04 ,0X00 ,0X0B ,0X00 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X16 ,0X01 ,0X01 ,0X2C ,0X02 ,0X01 ,0X42 ,0X03 ,0X01 ,0X00 ,0X00 ,0XFF ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X4B ,0X00 ,0X00 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X03 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X02 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X03 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0B ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0C ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0D ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0E ,0X00 ,0X01 ,0X80 ,0X01 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X09 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0XAE ,0X1D ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X20 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X1F ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X02 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0D ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X0E ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X00 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X4B ,0X00 ,0X01 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X4B ,0X00 ,0X00 ,0X00 ,0X00 ,0X12 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X14 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X15 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X01 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X40 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X09 ,0X03 ,0X0F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XCC ,0X14 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X02 ,0XDC ,0X92 ,0XF0 ,0X5F ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X03 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X01 ,0X80 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0X00 ,0XB1 ,0X03 ,0X52 ,0X7E};
    for (int i = 0; i < 1155; ++i) {
        my_monitor.create_frame_list_from_byte(example_input[i]);
    }*/
    /*
    std::vector<byte> data = {0x01, 0x24, 0x45, 0x42,0x20,0x39,0x38,0x20,0x45,0x31,0x20,0x37,0x30,0x20,0x37,0x41,0x0d};
    for (int i = 0; i < data.size(); ++i) {
        my_evita4_vent.create_frame_list_from_byte(data[i]);
    }*/
    // my_monitor.create_record_list()
    /****************************************************************************************************************************/

    return a.exec();
}

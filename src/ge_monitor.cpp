#include "ge_monitor.h"

GE_Monitor::GE_Monitor(const std::string config_file):Device{config_file}{
    local_protocol = new Datex_ohmeda(config_file);
    local_protocol->device = this;
    //QObject::connect(local_serial_port->get_serial_port(), SIGNAL(readyRead()), this, SLOT(process_buffer()));
}

/**
 * @brief GE_Monitor::start : the main configuration part of GE_Monitor
 */
void GE_Monitor::start(){
    try {
        std::cout<<"Try to open the serial port for GE Monitor"<<std::endl;
        try_to_open_port();

        std::cout<<"Start sending requests";
        send_request();

        std::cout<<"Start the logger";
        local_logger->start_logging();

    } catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }
}


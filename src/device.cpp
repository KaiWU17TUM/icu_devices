#include "device.h"

Device::Device(std::string config_file)
{
    // open serial port
    local_serial_port = new MySerialPort(config_file);
    local_serial_port->connect_serial_port(this, SLOT(process_buffer()));
    local_logger = new Logger(config_file);
    local_logger->connect_logger(this, SLOT(save_data()));
}

void Device::send_request(){
    local_protocol->send_request();
}

void Device::try_to_open_port(){
    local_serial_port->open_serial_port();
}

void Device::process_buffer(){
    QByteArray data = local_serial_port->get_serial_port()->readAll();
    for (int i = 0; i < data.size(); ++i) {
        local_protocol->from_literal_to_packet(data[i]);
    }
    if(local_protocol->frame_buffer.size()>0){
        local_protocol->from_packet_to_structures();
        local_protocol->frame_buffer.clear();
    }
}

void Device::save_data(){
    local_protocol->save_data();
}

void Device::write_buffer(std::vector<byte>* temptxbuff, qint64 len){
    local_serial_port->write_data(temptxbuff, len);
}

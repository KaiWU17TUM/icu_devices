#include "device.h"

Device::Device(std::string device_name, std::string config_file, std::string general_config, std::string protocol)
{
    device_name = device_name;
    std::string foldername = get_measurement_folder_name(general_config);

    // set up serial port
    local_serial_port = new MySerialPort(config_file);
    local_serial_port->connect_serial_port(this, SLOT(process_buffer()));

    // set up logger timer
    local_logger = new Logger(config_file, foldername);
    local_logger->connect_logger(this, SLOT(save_data()));

    // set protocol
    if(protocol=="medibus"){
        local_protocol = new Medibus(config_file, this);
    }else if(protocol=="bcc"){
        local_protocol = new Bcc(config_file, this);
    }else if(protocol=="datex_ohmeda"){
        local_protocol = new Datex_ohmeda(config_file, this);
    }else{
        std::cout<<"Not implemented protocol"<<std::endl;
    }

    // copy configure files into folder
    QFile::copy( QString::fromStdString(config_file), QString::fromStdString(foldername+"/"+device_name+"_config.txt"));
    QFile::copy(QString::fromStdString(general_config), QString::fromStdString(foldername+"/general_config.txt") );
}

/**
 * @brief Device::start: start the device workflow
 */
void Device::start(){
    try {
        std::cout<<device_name<<" tries to open the serial port"<<std::endl;
        try_to_open_port();

        std::cout<<device_name<<" start sending requests"<<std::endl;
        send_request();

        std::cout<<device_name<<" start the logger"<<std::endl;
        local_logger->start_logging();

    } catch (const std::exception& e) {
        qDebug()<<" Error opening/writing to serial port "<<e.what();
    }
}

/**
 * @brief Device::send_request: send request to the device according to definition in protocols
 */
void Device::send_request(){
    local_protocol->send_request();
}

/**
 * @brief Device::try_to_open_port: open the serial port
 */
void Device::try_to_open_port(){
    local_serial_port->open_serial_port();
}

/**
 * @brief Device::process_buffer: process data in the UART buffer
 */
void Device::process_buffer(){
    QByteArray data = local_serial_port->get_serial_port()->readAll();
    // decode bytes, check checksum and put information into packets
    for (int i = 0; i < data.size(); ++i) {
        local_protocol->from_literal_to_packet(data[i]);
    }
    // decode packets and put information into structures
    if(local_protocol->frame_buffer.size()>0){
        local_protocol->from_packet_to_structures();
        local_protocol->frame_buffer.clear();
    }
}

/**
 * @brief Device::save_data: save data into files according to definition in protocol
 */
void Device::save_data(){
    local_protocol->save_data();
}

/**
 * @brief Device::write_buffer: put data in the UART buffer
 * @param temptxbuff
 * @param len
 */
void Device::write_buffer(const char* temptxbuff, qint64 len){
    local_serial_port->write_data(temptxbuff, len);
}

/**
 * @brief Device::get_measurement_folder_name : build folder according to the timestamp and content(patient id, bed id) in config_file
 * @param config_file
 * @return foldername
 */
std::string Device::get_measurement_folder_name(std::string config_file){
    std::time_t current_pc_time = std::time(nullptr);
    std::ifstream cfg_file(config_file);
    std::string room;
    std::string bed;
    std::string path;
    std::string folder_name;

    if (cfg_file.is_open()){
        std::string line;
        while (std::getline(cfg_file, line)){
            if(line[0] == '#' || line.empty())
                continue;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

           if(name == "room"){
                room = value;
            }
           else if(name == "bed"){
                bed = value;
           }
           else if(name == "path"){
                path = value;
           }
        }
        cfg_file.close();
    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    folder_name = path + std::to_string(current_pc_time)+"_"+room+"_"+bed;
    return folder_name;
}

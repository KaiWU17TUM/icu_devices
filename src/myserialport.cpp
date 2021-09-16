#include "myserialport.h"


MySerialPort::MySerialPort(const std::string config_file)
{
    serial = new QSerialPort();
    load_serial_configs(config_file);
}

void MySerialPort::open_serial_port()
{
    if (serial->open(QIODevice::ReadWrite)) {
        std::cout<<"open port success\n"<<std::endl;

    } else {
        std::cout<<"open port failed\n"<<std::endl;
    }
}


void MySerialPort::write_data(std::vector<byte>* temptxbuff, qint64 len)
{
    serial->write((const char*)&temptxbuff, len);
}



QSerialPort::FlowControl resolveFlowControl(std::string input){
    if(input=="no_flow_control") return QSerialPort::NoFlowControl;
    if(input=="hardware_control") return QSerialPort::HardwareControl;
    if(input=="software_control") return QSerialPort::SoftwareControl;
    return QSerialPort::UnknownFlowControl;
}

QSerialPort::StopBits resolveStopBits(std::string input){
    if(input=="one_stop") return QSerialPort::OneStop;
    if(input=="one_and_half_stop") return QSerialPort::OneAndHalfStop;
    if(input=="two_stop") return QSerialPort::TwoStop;
    return QSerialPort::UnknownStopBits;
}

QSerialPort::Parity resolveParity(std::string input){
    if(input=="no_parity") return QSerialPort::NoParity;
    if(input=="even_parity") return QSerialPort::EvenParity;
    if(input=="odd_parity") return QSerialPort::OddParity;
    if(input=="space_parity") return QSerialPort::SpaceParity;
    if(input=="mark_parity") return QSerialPort::MarkParity;
    return QSerialPort::UnknownParity;
}

QSerialPort::DataBits resolveDatabits(std::string input){
    if(input=="data8") return QSerialPort::Data8;
    if(input=="data5") return QSerialPort::Data5;
    if(input=="data6") return QSerialPort::Data6;
    if(input=="data7") return QSerialPort::Data7;
    return QSerialPort::UnknownDataBits;
}

QSerialPort::BaudRate resolveBaudRate(std::string input){
    if(input=="1200") return QSerialPort::Baud1200;
    if(input=="2400") return QSerialPort::Baud2400;
    if(input=="4800") return QSerialPort::Baud4800;
    if(input=="9600") return QSerialPort::Baud9600;
    if(input=="19200") return QSerialPort::Baud19200;
    if(input=="38400") return QSerialPort::Baud38400;
    if(input=="57600") return QSerialPort::Baud57600;
    if(input=="115200") return QSerialPort::Baud115200;
    return QSerialPort::UnknownBaud;
}

/**
 * @brief MySerialPort::load_serial_configs : load serial port configurations from config file
 * @param config_file
 */
void MySerialPort::load_serial_configs(const std::string config_file){
    std::ifstream cfg_file(config_file);
    if (cfg_file.is_open()){
        std::string line;
        while (std::getline(cfg_file, line)){
            if(line[0] == '#' || line.empty())
                continue;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            if(name == "usb_port"){
                serial->setPortName(QString::fromStdString(value));
            }
            else if(name == "baud_rate"){
                serial->setBaudRate(resolveBaudRate(value));
            }
            else if(name == "data_bits"){
                serial->setDataBits(resolveDatabits(value));
            }
            else if(name == "parity"){
                serial->setParity(resolveParity(value));
            }
            else if(name == "stop_bits"){
                serial->setStopBits(resolveStopBits(value));
            }
            else if(name == "flow_control"){
                serial->setFlowControl(resolveFlowControl(value));
            }
        }
        cfg_file.close();
    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
}

void MySerialPort::connect_serial_port(const QObject *receiver, const char* slot){
    //QObject::connect(serial, SIGNAL(readyRead()), receiver, slot);

}

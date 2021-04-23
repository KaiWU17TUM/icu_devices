#include "evita4_vent.h"

Evita4_vent::Evita4_vent(){
    local_serial_port = new MySerialPort();
    local_serial_port->serial->setPortName("/dev/ttyUSB4");
    local_serial_port->serial->setBaudRate(QSerialPort::Baud19200);
    local_serial_port->serial->setDataBits(QSerialPort::Data8);
    local_serial_port->serial->setParity(QSerialPort::EvenParity);
    local_serial_port->serial->setStopBits(QSerialPort::OneStop);
    local_serial_port->serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(local_serial_port->serial, SIGNAL(readyRead()), this, SLOT(process_buffer()));

    timer_cp1 = new QTimer();
    connect(timer_cp1, SIGNAL(timeout()), this, SLOT(request_device_settings()));
}

void Evita4_vent::start(){
    try {
        std::cout<<"Try to open the serial port for Evita 4"<<std::endl;
        try_to_open_port();

        std::cout<<"Initialize the connection with Evita 4"<<std::endl;
        request_icc();

        timer_cp1->start(5000);

    }  catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }
}



void Evita4_vent::process_buffer(){
    QByteArray data = local_serial_port->serial->readAll();
    try
    {
        for (int i = 0; i < data.size(); ++i) {
            create_frame_list_from_byte(data[i]);
        }

        if(frame_buffer.size()>0){
            read_packet_from_frame();
            frame_buffer.clear();
        }
    }catch (const std::exception& e) {
        qDebug()<<e.what();
    }
}
void Evita4_vent::create_frame_list_from_byte(byte b){
    switch (b)
    {
        case SOHCHAR:
            m_storestart1 = true;
            m_storeend = false;
            b_list.push_back(b);
            break;
        case ESCCHAR:
            if (m_storestart1 != true)
            {
                m_storestart2 = true;
                m_storeend = false;
            }
            b_list.push_back(b);
            break;
        case CRCHAR:
            m_storestart1 = false;
            m_storestart2 = false;
            m_storeend = true;
            break;
        default:
            if ((m_storestart1 == true && m_storeend == false) || (m_storestart2 == true && m_storeend == false))
                b_list.push_back(b);
            break;
    }

    if ((m_storestart1 == false && m_storeend == true) || (m_storestart2 == false && m_storeend == true))
    {
        int framelen = b_list.size();
        std::vector<byte> payload;
        if (framelen != 0)
        {
            for(unsigned long i=0;i<framelen-2;i++){
                payload.push_back(b_list[i]);
            }
            add_checksum(payload);
            if(payload[framelen-1]==b_list[framelen-1] and payload[framelen-2]==b_list[framelen-2]){
                 qDebug()<<"Checksum Correct!";
                frame_buffer.push_back(payload);
            }else{
                qDebug()<<"Checksum Error!";
            }
            b_list.clear();
            m_storeend = false;

        }

    }

}


void Evita4_vent::read_packet_from_frame(){
    std::time_t result = std::time(nullptr);
    pkt_timestamp =std::asctime(std::localtime(&result));
    pkt_timestamp.erase(pkt_timestamp.end()-1);

    for(unsigned long i=0;i<frame_buffer.size();i++){
        byte response_type = frame_buffer[i][0];
        byte command_type = frame_buffer[i][1];
        std::vector<byte> cmd;
        switch (response_type) {
        case 0x1b: // command received
            if(command_type=='Q'){ // ICC
                cmd = {0x51};
                qDebug()<<"get ICC command";
                command_echo_response(cmd);
                break;
            }
            else if(command_type=='R'){ // Device id
                cmd = {0x52};
                qDebug()<<"get Device id command";
                command_echo_response(cmd); //return empty device identification
                break;
            }


        case 0x01: // response received
            if(command_type=='Q')// ICC
                break;
            else if(command_type=='R'){ // Device id
                break;
            }
            else if(command_type=='$'){ // Response cp1
                parse_data_response_measured(frame_buffer[i], poll_request_config_measured_data_codepage1);
                qDebug()<<"get measuremnt cp 1";
                break;
            }
            else if(command_type=='+'){ // Response cp2
                parse_data_response_measured(frame_buffer[i], poll_request_config_measured_data_codepage2);
                break;
            }
            else if(command_type==')'){ // Device setting
                parse_data_device_settings(frame_buffer[i]);
                 qDebug()<<"get device setting 1";
                break;
            }
            else if(command_type=='*'){ // TextMessage
                parse_data_text_settings(frame_buffer[i]);
                break;
            }
            else if(command_type=='0'){ //Nop
                break;
            }

        default:

            break;

        }

    }
}

void Evita4_vent::parse_data_text_settings(std::vector<byte> &packetbuffer){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for(unsigned long i=2;i<framelen-2;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();
        int textlen=0;
        for(int i=0;i<responselen;i=i+textlen){
            std::vector<byte> DataCode(response.begin()+i,response.begin()+i+2);
            std::vector<byte> TextLen(response.begin()+i+2,response.begin()+i+3);
            textlen = int(TextLen[0]-0x30);
            std::string DataValue(response.begin()+3+i,response.begin()+3+textlen+i);
            byte datacode = 0x0;
            if(DataCode[0]<=0x39)
                datacode+=16*(DataCode[0]-0x30);
            else
                datacode+=16*(DataCode[0]-0x41+10);
            if(DataCode[1]<=0x39)
                datacode+=1*(DataCode[1]-0x30);
            else
                datacode+=1*(DataCode[1]-0x41+10);

            std::string physio_id;
            physio_id = TextMessages.find(datacode)->second;


            NumVal local_NumVal;
            local_NumVal.Timestamp = pkt_timestamp;
            local_NumVal.PhysioID = physio_id;
            local_NumVal.Value =DataValue;

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);

        }
        save_num_val_list_rows("TextMessages");
        numval_list.clear();
    }
}
void Evita4_vent::parse_data_device_settings(std::vector<byte> &packetbuffer){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for(unsigned long i=2;i<framelen-2;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for(int i=0;i<responselen;i=i+6){
            std::vector<byte> DataCode(response.begin()+i,response.begin()+2+i);
            std::string DataValue(response.begin()+2+i,response.begin()+2+5+i);
            byte datacode = 0x0;
            if(DataCode[0]<=0x39)
                datacode+=16*(DataCode[0]-0x30);
            else
                datacode+=16*(DataCode[0]-0x41+10);
            if(DataCode[1]<=0x39)
                datacode+=1*(DataCode[1]-0x30);
            else
                datacode+=1*(DataCode[1]-0x41+10);

            std::string physio_id;
            physio_id = DeviceSettings.find(datacode)->second;


            NumVal local_NumVal;
            local_NumVal.Timestamp = pkt_timestamp;
            local_NumVal.PhysioID = physio_id;
            local_NumVal.Value =DataValue;

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);
        }
         save_num_val_list_rows("DeviceSettings");
         numval_list.clear();
    }
}
void Evita4_vent::parse_data_response_measured(std::vector<byte> &packetbuffer, byte type){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for(unsigned long i=2;i<framelen-2;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for(int i=0;i<responselen;i=i+6){
            std::vector<byte> DataCode(response.begin()+i,response.begin()+2+i);
            std::string DataValue(response.begin()+2+i,response.begin()+6+i);
            byte datacode = 0x0;
            if(DataCode[0]<=0x39)
                datacode+=16*(DataCode[0]-0x30);
            else
                datacode+=16*(DataCode[0]-0x41+10);
            if(DataCode[1]<=0x39)
                datacode+=1*(DataCode[1]-0x30);
            else
                datacode+=1*(DataCode[1]-0x41+10);

            std::string physio_id;
            if(type == poll_request_config_measured_data_codepage1){
                physio_id = MeasurementCP1.find(datacode)->second;
            }
            else if(type==poll_request_config_measured_data_codepage2){
                physio_id = MeasurementCP2.find(datacode)->second;
            }

            NumVal local_NumVal;
            local_NumVal.Timestamp = pkt_timestamp;
            local_NumVal.PhysioID = physio_id;
            local_NumVal.Value =DataValue;

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);

        }
        if(type == poll_request_config_measured_data_codepage1){
            save_num_val_list_rows("MeasuredCP1");
        }
        else if(type==poll_request_config_measured_data_codepage2){
            save_num_val_list_rows("MeasuredCP2");
        }
        numval_list.clear();
    }
}



void Evita4_vent::write_buffer(std::vector<byte> cmd){
    std::vector<byte> temptxbufferlist;

    if(cmd.size()!=0){
        temptxbufferlist.push_back(ESCCHAR);
        for(unsigned long i=0;i<cmd.size();i++){
            temptxbufferlist.push_back(cmd[i]);
        }

        add_checksum(temptxbufferlist);

        temptxbufferlist.push_back(CRCHAR);
        const char* payload_data = (const char*)&temptxbufferlist[0];
        this->local_serial_port->serial->write(payload_data, temptxbufferlist.size());
    }
}


void Evita4_vent::command_echo_response(std::vector<byte>& cmd){
    std::vector<byte> temptxbufferlist;
    temptxbufferlist.push_back(SOHCHAR);
    for(unsigned long i=0;i<cmd.size();i++){
        temptxbufferlist.push_back(cmd[i]);
    }

    add_checksum(temptxbufferlist);
    temptxbufferlist.push_back(CRCHAR);
    const char* payload_data = (const char*)&temptxbufferlist[0];
    this->local_serial_port->serial->write(payload_data, temptxbufferlist.size());
}

void Evita4_vent::save_num_val_list_rows(std::string datatype){
    if(numval_list.size()!=0){
        write_num_header_list(datatype);
        std::string filename = pathcsv +datatype+".csv";
        std::string row;
        row+=(numval_list[0].Timestamp);
        row.append(",");

        for(int i=0;i<numval_list.size();i++){
            row+=(numval_list[i].Value);
            row.append(",");
        }
        row.append("\n");
        QFile myfile(QString::fromStdString(filename));
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&row[0], row.length());

        }
        qDebug()<<"write data to file";
    }
}

void Evita4_vent::write_num_header_list(std::string datatype){
    if(write_header_for_data_type(datatype)){
        std::string header;
        std::string filename = pathcsv +datatype+".csv";
        header.append("Time");
        header.append(",");

        for(unsigned long i=0;i<numval_list.size();i++){
                header+=numval_list[i].PhysioID;
                header.append(",");
        }
        header.append("\n");
        QFile myfile(QString::fromStdString(filename));
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&header[0], header.length());
        }
        header_list.clear();
    }
}

bool Evita4_vent::write_header_for_data_type(std::string datatype)
{
    bool writeheader = true;
    if(datatype == "MeasuredCP1"){
        if (m_transmissionstart)
        {
            m_transmissionstart = false;

        }
        else writeheader = false;
    }
    else if(datatype == "MeasuredCP2"){
        if (m_transmissionstart2)
        {
            m_transmissionstart2 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "DeviceSettings"){
        if (m_transmissionstart3)
        {
            m_transmissionstart3 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "TextMessages"){
        if (m_transmissionstart4)
        {
            m_transmissionstart4 = false;

        }
        else writeheader = false;
    }
    return writeheader;

}

void Evita4_vent::add_checksum(std::vector<byte>& payload){
    int sum = 0;

    for (unsigned long i = 0; i < payload.size(); ++i)
    {
        sum += payload[i];
    }

    //get 8-bit sum total checksum
    byte checksum = (byte)(sum & 0xFF);
    std::vector<byte> checksum_array;
    checksum_array.push_back(checksum/16);
    checksum_array.push_back(checksum&0x0F);
    if(checksum_array[0]<10)
        payload.push_back(checksum_array[0]+0x30);
    else
        payload.push_back(checksum_array[0]-10+0x41);

    if(checksum_array[1]<10)
        payload.push_back(checksum_array[1]+0x30);
    else
        payload.push_back(checksum_array[1]-10+0x41);
}

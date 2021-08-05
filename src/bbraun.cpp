#include "bbraun.h"

Bbraun::Bbraun()
{
    GeneralParameters = GeneralParameters2;
    InfusionPumpParameters = InfusionPumpParameters2;
    AdditionalParameters = AdditionalParameters2;

    local_serial_port = new MySerialPort();
    local_serial_port->serial->setPortName("/dev/ttyUSB2");
    local_serial_port->serial->setBaudRate(QSerialPort::Baud19200);
    local_serial_port->serial->setDataBits(QSerialPort::Data8);
    local_serial_port->serial->setParity(QSerialPort::NoParity);
    local_serial_port->serial->setStopBits(QSerialPort::OneStop);
    local_serial_port->serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(local_serial_port->serial, SIGNAL(readyRead()), this, SLOT(process_buffer()));

    // Bind the timer with GET_MEM request, which should be sent to the device periodically
    timer_cp1 = new QTimer();
    connect(timer_cp1, SIGNAL(timeout()), this, SLOT(send_get_mem_request()));
    timer_cp2 = new QTimer();
    connect(timer_cp2, SIGNAL(timeout()), this, SLOT(save_num_values_by_type()));
}

/**
 * @brief Bbraun::start : The main workflow
 */
void Bbraun::start(){
    try {
        std::cout<<"Try to open the serial port for Bbraun perfusor"<<std::endl;
        try_to_open_port();

        std::cout<<"Initialize the connection with "<<std::endl;
        request_initialize_connection();

        int period = 5000; //unit ms
        timer_cp1->start(period);
        period = 3000; //unit ms
        timer_cp2->start(period);

    }  catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }
}

/**
 * @brief Bbraun::process_buffer : The serial port callback when reveive data
 */
void Bbraun::process_buffer(){
    QByteArray data = local_serial_port->serial->readAll();
    try
    {
        for (int i = 0; i < data.size(); ++i) {
            create_frame_list_from_byte(data[i]);
        }
        //if there is content received
        if(frame_buffer.size() > 0){
            // parse the raw content
            read_packet_from_frame();
            frame_buffer.clear();
            // save parsed content
            //save_num_values_by_type();
        }
    }catch (const std::exception& e) {
        qDebug()<<e.what();
    }
}

/**
 * @brief Bbraun::create_frame_list_from_byte : call this function to get raw packet out from serial data
 */
void Bbraun::create_frame_list_from_byte(unsigned char b){
    // Detect the start/end character and get raw packet
    switch(b){
        case SOHCHAR:
            m_storestart = true;
            m_storeend = false;
            b_list.push_back(b);
            break;
        case EOTCHAR:
            m_storestart = false;
            m_storeend = true;
            break;
        case ACKCHAR:
            break;
        case NAKCHAR:
            break;
        default:
            if(m_storestart == true && m_storeend == false)
                b_list.push_back(b);
            break;
    }

    // Character debuffer
    if(m_storestart == false && m_storeend == true){
        for(uint i=0;i<b_list.size();i++){
            unsigned char bchar = b_list[i];
            switch(bchar){
                case ECHAR:
                    if(b_list[i+1] == XCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = DCHAR;

                    }
                    else if(b_list[i+1] == ECHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = ECHAR;
                    }
                    break;

                case eCHAR:
                    if(b_list[i+1] == xCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = dCHAR;
                    }
                    else if(b_list[i+1] == eCHAR){
                        b_list.erase(b_list.begin()+i+1);
                        b_list[i] = eCHAR;
                    }
                    break;
            }
        }

        // Checksum calculation
        int framelen = b_list.size();
        if(framelen!=0){
            std::vector<byte> frameBuffer2(b_list);
            char checksum_buf[5];
            int checksum_rece=0;

            //get away the checksum
            for(int n=0;n<5;n++){
                checksum_buf[n] = frameBuffer2[framelen-1-n];
                frameBuffer2.pop_back();
            }

            for(int n=0;n<5;n++){
                if(checksum_buf[n]==0)
                    break;
                else
                    checksum_rece += ((int)checksum_buf[n]-48)*(pow(10,n));
            }
            int checksum_computed = compute_checksum(frameBuffer2);

            if(checksum_rece == checksum_computed){
                ack_flag = true;
                frameBuffer2.erase(frameBuffer2.begin(),frameBuffer2.begin()+7);
                frame_buffer.push_back(frameBuffer2);
                qDebug()<<"Chechsum correct";
            }else{
                qDebug()<<"Chechsum wrong";
            }
            b_list.clear();
            m_storeend = false;
        }
    }
}

/**
 * @brief Bbraun::read_packet_from_frame : call this function to get parsed data from raw packet
 */
void Bbraun::read_packet_from_frame(){
    // the timestamp from pc [seconds since 01-Jan-1970]
    unsigned long int pc_time = std::time(nullptr);

    for(unsigned long i=0;i<frame_buffer.size();i++){
        // get time from PC
        std::time_t result = std::time(nullptr);
        pkt_timestamp = std::asctime(std::localtime(&result));
        pkt_timestamp.erase(pkt_timestamp.end()-1);

        //
        std::string str(frame_buffer[i].begin(), frame_buffer[i].end());
        std::size_t pos_1 = str.find(">");
        std::size_t pos_2 = str.find(ETBCHAR);
        std::size_t pos_3 = str.find(ETXCHAR);
        m_bedid.clear();
        for(unsigned long n=0;n<pos_1;n++){
            m_bedid.push_back(str[n]);
        }
        std::string text = str.substr(pos_1);
        if(pos_2 != std::string::npos){text.erase(text.begin()+pos_2-pos_1, text.end());}
        if(pos_3 != std::string::npos){text.erase(text.begin()+pos_3-pos_1, text.end());}

        //for each block
        std::vector<std::string> block_list = split_string(text, RSCHAR);
        for(unsigned int j=0;j<block_list.size();j++){
            //Get the content from each block
            std::vector<std::string> block_content;
            int n=0;
            int pos=0;
            while((pos = block_list[j].find(','))!=std::string::npos && n<3){
                block_content.push_back(block_list[j].substr(0, pos));
                block_list[j].erase(0, pos+1);
                n=n+1;
            }
            block_content.push_back(block_list[j]);
            std::string parameter = block_content[2];
            std::string parametername;
            std::string parametertype = "GeneralParameters";
            std::map<std::string, std::string>::iterator it;
            it = GeneralParameters.find(parameter);
            if(it!=GeneralParameters.end()){
                parametername = GeneralParameters.find(parameter)->second;
            }
            else{
                it = InfusionPumpParameters.find(parameter);
                if(it!=InfusionPumpParameters.end()){
                    parametername = InfusionPumpParameters.find(parameter)->second;
                    parametertype = "InfusionPumpParameters";
                }
                else{
                    it = AdditionalParameters.find(parameter);
                    if(it!=AdditionalParameters.end()){
                        parametername = AdditionalParameters.find(parameter)->second;
                        parametertype = "AdditionalParameters";
                    }
                    else{
                        parametername = "Undefined";
                        parametertype = "UndefinedParameters";
                    }
                }
            }


            //save the parsed data into list
            NumValB numval;
            numval.Timestamp = pkt_timestamp;
            numval.timestamp = pc_time;
            numval.Relativetime = block_content[0];
            numval.Address = block_content[1];
            numval.Value = block_content[3];
            numval.Parametertype =parametertype;
            numval.PhysioID = parametername;
            numval_list.push_back(numval);
            header_list.push_back(parametername);
        }
    }
    //each frame reply ACK
    send_ack();
}

/**
 * @brief Bbraun::write_buffer : embed the message into a packet and put it into the buffer of serial port
 */
void Bbraun::write_buffer(std::vector<byte> bedid, std::vector<byte> txbuf){
    std::vector<byte> temptxbuff;
    temptxbuff.push_back(SOHCHAR);
    //deal with length
    int totalframelen = 15 + bedid.size() + txbuf.size();

    int_save_to_buffer(totalframelen, temptxbuff);
    temptxbuff.push_back(STXCHAR);

    for(unsigned long i=0;i<bedid.size();i++){
        temptxbuff.push_back(bedid[i]);
    }

    temptxbuff.push_back('>');

    for(unsigned long i=0;i<txbuf.size();i++){
        temptxbuff.push_back(txbuf[i]);
    }
    temptxbuff.push_back(ETXCHAR);

    int checksum = compute_checksum(temptxbuff);
    int_save_to_buffer(checksum, temptxbuff);
    temptxbuff.push_back(EOTCHAR);

    // do character stuffing
    for (unsigned long i=0; i<temptxbuff.size(); i++)
    {
        char bchar = temptxbuff[i];
        switch(bchar)
        {
            case DCHAR: //D->EX
                temptxbuff[i] = ECHAR;
                temptxbuff.insert(temptxbuff.begin()+i+1, XCHAR);
                i=i+1;
                break;
            case ECHAR: //E->EE
                temptxbuff.insert(temptxbuff.begin()+i+1, ECHAR);
                i=i+1;
                break;
            case dCHAR: //d->ex
                temptxbuff[i] = eCHAR;
                temptxbuff.insert(temptxbuff.begin()+i+1, xCHAR);
                i=i+1;
                break;
            case eCHAR: //e->ee
                temptxbuff.insert(temptxbuff.begin()+i+1, eCHAR);
                i=i+1;
                break;
            default:
                break;
        }
    }

    const char* payload_data = (const char*)&temptxbuff[0];
    this->local_serial_port->serial->write(payload_data, temptxbuff.size());
}

/**
 * @brief Bbraun::request_initialize_connection : with this request the host system initialize the connection to ComDevice
 */
void Bbraun::request_initialize_connection(){
    std::vector<unsigned char> bedid = {'1','/','1','/','1'};
    std::vector<unsigned char> command = {'A','D','M','I','N',':','A','L','I','V','E'};
    qDebug()<<"sending init msg";
    write_buffer(bedid, command);
}

/**
 * @brief Bbraun::send_ack : Ack msgs end with ETBCHAR
 */
void Bbraun::send_ack(){
    std::vector<byte> temptxbuff;
    temptxbuff.push_back(ACKCHAR);
    const char* payload_data = (const char*)&temptxbuff[0];
    qDebug()<<"sending ACK";
    this->local_serial_port->serial->write(payload_data, temptxbuff.size());
}
/**
 * @brief Bbraun::send_get_mem_request : with this request the host asks for all actual parameter values of the ComDevice / pumps
 */
void Bbraun::send_get_mem_request(){
    std::vector<byte> bedid = m_bedid;
    std::vector<unsigned char> command = {'M','E','M',':','G','E','T'};
    qDebug()<<"sending get mem request";
    write_buffer(bedid, command);
}


/**
 * @brief Saving functions : call these function to save parsed data
 */
void Bbraun::save_num_values_by_type(){
    for (uint i = 0; i < numval_list.size(); i++)
    {
        if(numval_list[0].Parametertype == "GeneralParameters"){
            save_num_value_list_row("GeneralParameters");
        }else if(numval_list[0].Parametertype == "InfusionPumpParameters"){
            save_num_value_list_row("InfusionPumpParameters");
        }else if(numval_list[0].Parametertype == "AdditionalParameters"){
            save_num_value_list_row("AdditionalParameters");
        }else if(numval_list[0].Parametertype == "UndefinedParameters"){
            save_num_value_list_row("UndefinedParameters");
        }
    }
}

void Bbraun::save_num_value_list_row(std::string datatype){
    std::time_t current_pc_time = std::time(nullptr);
    std::string pkt_timestamp =std::asctime(std::localtime(&current_pc_time));
    pkt_timestamp.erase(8,11);

    QString filename = pathcsv + QString::fromStdString(pkt_timestamp) + QString::fromStdString(datatype)+".csv";

    std::string row;
    row.append(numval_list[0].Timestamp);
    row.append(",");
    row.append(numval_list[0].Relativetime);
    row.append(",");
    row.append(numval_list[0].Address);
    row.append(",");
    int elementcount=0;
    bool changed=false;


    for(unsigned long i=0;i<numval_list.size();i++){
        if(numval_list[i].timestamp==numval_list[0].timestamp && current_pc_time>(numval_list[i].timestamp+timelapse)){
            changed = true;
            write_num_header_list(datatype, filename);
            if(numval_list[i].Parametertype == datatype){
                elementcount++;
                int pos =0;
                if(numval_list[i].Value == "")
                    row.append("-");
                else if((pos=numval_list[i].Value.find(','))!=std::string::npos)
                {
                    numval_list[i].Value[pos]='.';
                    row.append(numval_list[i].Value);
                }
                else
                    row.append(numval_list[i].Value);
                row.append(",");
            }
        }
    }
    if(changed){
        row.append("\n");
        QFile myfile(filename);
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&row[0], row.length());
        }
        numval_list.erase(numval_list.begin(), numval_list.begin()+elementcount);
    }
}

void Bbraun::write_num_header_list(std::string datatype, QString filename){
    if(write_header_for_data_type(datatype)){
        std::string header;

        header.append("Time");
        header.append(",");
        header.append("RelativeTime");
        header.append(",");
        header.append("Address");
        header.append(",");
        for(unsigned long i=0;i<numval_list.size();i++){
            if(numval_list[i].Parametertype == datatype &&numval_list[i].timestamp==numval_list[0].timestamp){
                header+=numval_list[i].PhysioID;
                header.append(",");
            }
        }
        header.append("\n");
        QFile myfile(filename);
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&header[0], header.length());
        }

        header_list.clear();
    }
}

bool Bbraun::write_header_for_data_type(std::string datatype){
    bool writeheader = true;
    if(datatype == "GeneralParameters"){
        if (m_transmissionstart)
        {
            m_transmissionstart = false;

        }
        else writeheader = false;
    }
    else if(datatype == "InfusionPumpParameters"){
        if (m_transmissionstart2)
        {
            m_transmissionstart2 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "AdditionalParameters"){
        if (m_transmissionstart3)
        {
            m_transmissionstart3 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "UndefinedParameters"){
        if (m_transmissionstart4)
        {
            m_transmissionstart4 = false;

        }
        else writeheader = false;
    }
    return writeheader;
}

/*****************************************************/
/**
 * @brief Helper functions
 */
byte Bbraun::compute_checksum(std::vector<byte> bytes){
    int sum = 0;
    int crc = 0;
    for (uint i = 0; i < bytes.size(); i++)
    {
        sum += bytes[i];
    }
    crc = (unsigned char)(sum % 256);
    return crc;
}

void  Bbraun::int_save_to_buffer(int integer, std::vector<byte> &bytes){
    char length_buffer[5] = "0";
    sprintf(length_buffer,"%d",integer);
    int end_index = 0;
    for(end_index=0;length_buffer[end_index]!='\0';end_index++)
    {}
    for(int i=0;i<5-end_index;i++){
        bytes.push_back('0');
    }
    for(int i=5;i>5-end_index;i--){
        bytes.push_back(length_buffer[5-i]);
    }
}
std::vector<std::string> Bbraun::split_string(std::string s, byte delimiter){
    size_t pos = 0;
    std::string token;
    std::vector<std::string> output;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        output.push_back(token);
        s.erase(0, pos + 1);
    }
    output.push_back(s);
    return output;
}

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


    //prepare transmission request
    realtime_data_list.push_back(0x00);
    realtime_data_list.push_back(0x06);

    //realtime_transmission_request.push_back(0x1b);
    realtime_transmission_request.push_back(0x54);
    for(uint i=0;i<realtime_data_list.size();i++){
        realtime_transmission_request.push_back(0x30);
        realtime_transmission_request.push_back(realtime_data_list[i]+0x30);
        realtime_transmission_request.push_back(0x30);
        realtime_transmission_request.push_back(0x31);
    }

    //prepare enable datastream request
    sync_cmd.push_back(0xd0);
    sync_cmd.push_back(0xc1);
    sync_cmd.push_back(0xc3);
    sync_cmd.push_back(0xc0);
    sync_cmd.push_back(0xc0);


    timer_cp1 = new QTimer();
    connect(timer_cp1, SIGNAL(timeout()), this, SLOT(request_alarm_low_limit()));

    timer_cp2 = new QTimer();
    connect(timer_cp2, SIGNAL(timeout()), this, SLOT(request_alarm_high_limit()));

    timer_cp3 = new QTimer();
    connect(timer_cp3, SIGNAL(timeout()), this, SLOT(request_alarmCP1()));

    timer_cp4 = new QTimer();
    connect(timer_cp4, SIGNAL(timeout()), this, SLOT(request_alarmCP2()));

    timer_cp5 = new QTimer();
    connect(timer_cp5, SIGNAL(timeout()), this, SLOT(request_measurement_cp1()));

    timer_cp6 = new QTimer();
    connect(timer_cp6, SIGNAL(timeout()), this, SLOT(save_data()));

}

void Evita4_vent::start(){
    try {
        std::cout<<"Try to open the serial port for Evita 4"<<std::endl;
        try_to_open_port();

        std::cout<<"Initialize the connection with Evita 4"<<std::endl;
        request_icc();

        timer_cp1->start(3000);
        QThread::sleep(1);
        timer_cp2->start(3000);

        //timer_cp3->start(1000);
        //timer_cp4->start(1000);
        QThread::sleep(1);
        timer_cp5->start(3000);

        timer_cp6->start(1000);


    }  catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }
}

/**
 * @brief Evita4_vent::process_buffer: The serial port callback when reveive data
 */
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
/**
 * @brief Evita4_vent::create_frame_list_from_byte:call this function to get raw packet out from serial data
 * @param b
 */
void Evita4_vent::create_frame_list_from_byte(byte b){
    if(sync_data==true){
        if((b & 0xf0)==0xd0)
            new_data=true;
        else
            new_data=false;
        if(b_list.size()>0 && new_data==true){
            std::vector<byte> payload;
            for(unsigned long i=0;i<b_list.size();i++){
                    payload.push_back(b_list[i]);
            }
            frame_buffer.push_back(payload);
            b_list.clear();
        }
        b_list.push_back(b);
    }
    else{
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
                for(int i=0;i<framelen-2;i++){
                    payload.push_back(b_list[i]);
                }
                add_checksum(payload);
                if(payload[framelen-3]==b_list[framelen-3] and payload[framelen-2]==b_list[framelen-2]){
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
}

/**
 * @brief Evita4_vent::read_packet_from_frame: call this function to get parsed data from raw packet
 */
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
                    //request_realtime_config();
                    break;
                }
                else if(command_type=='R'){ // Device id
                    cmd = {0x52};
                    qDebug()<<"get Device id command";
                    command_echo_response(cmd); //return empty device identification
                    break;
                }
                else{break;}


            case 0x01: // response received
                if(command_type=='Q'){// ICC-Response
                    qDebug()<<"receive ICC-Response";
                    free_flag=true;
                    break;
                }
                else if(command_type=='R'){ // Device id
                    qDebug()<<"receive Device_ID-Response";
                    free_flag=true;
                    break;
                }
                else if(command_type=='0'){ //Nop
                    qDebug()<<"receive Nop-Response";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x24){ // Response cp1
                    parse_data_response_measured(frame_buffer[i], poll_request_config_measured_data_codepage1);
                    qDebug()<<"get measuremnt cp 1";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x25){ // Response low alarm limits
                    parse_data_response_measured(frame_buffer[i], poll_request_low_alarm_limits);
                    qDebug()<<"get low alarm limits";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x26){ // Response high alarm limits
                    parse_data_response_measured(frame_buffer[i], poll_request_high_alarm_limits);
                    qDebug()<<"get high alarm limits";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x27){ // Response alarm cp1
                    parse_alarm(frame_buffer[i]);
                    qDebug()<<"get  alarm CP1";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x23){ // Response alarm cp2
                    parse_alarm(frame_buffer[i]);
                    qDebug()<<"get alarm CP2";
                    free_flag=true;
                    break;
                }

                else if(command_type==')'){ // Device setting
                    parse_data_device_settings(frame_buffer[i]);
                     qDebug()<<"get device setting";
                     free_flag=true;
                    break;
                }

                else if(command_type=='*'){ // TextMessage
                    parse_data_text_settings(frame_buffer[i]);
                    qDebug()<<"get TextMessage";
                    free_flag=true;
                    break;
                }

                else if(command_type==0x53){ // Response realtime configuration
                    parse_realtime_data_configs(frame_buffer[i]);
                    qDebug()<<"get Response realtime configuration";
                    free_flag=true;
                    request_realtime_data();
                    break;
                }

                else if(command_type==0x54){ // Response realtime data
                    qDebug()<<"get realtime data command, enable datastream";
                    sync_data = true;
                    request_sync();
                    break;
                }
                else{break;}

        default:
            if((response_type & 0xf0) == 0xd0){
                qDebug()<<"get realtime data SYNC";
                parse_realtime_data(frame_buffer[i]);
            }
            break;
        }

    }
}

/**
 * @brief Evita4_vent::parse_data_text_settings
 * @param packetbuffer
 */
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
            local_NumVal.timestamp = std::time(nullptr);

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);

        }
        //save_num_val_list_rows("TextMessages");
        //numval_list.clear();
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

        for(int i=0;i<responselen;i=i+7){
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
            local_NumVal.timestamp = std::time(nullptr);

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);
        }
         //save_num_val_list_rows("DeviceSettings");
         //numval_list.clear();
    }
}
void Evita4_vent::parse_data_response_measured(std::vector<byte> &packetbuffer, byte type){
    unsigned long framelen = packetbuffer.size();
    unsigned long int grp_timestamp = std::time(nullptr);;
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
            else if(type==poll_request_low_alarm_limits){
                physio_id = LowLimits.find(datacode)->second;
            }
            else if(type==poll_request_high_alarm_limits){
                physio_id = HighLimits.find(datacode)->second;
            }

            NumVal local_NumVal;
            local_NumVal.type = type;
            local_NumVal.Timestamp = pkt_timestamp;
            local_NumVal.PhysioID = physio_id;
            local_NumVal.Value =DataValue;
            local_NumVal.timestamp = grp_timestamp;

            numval_list.push_back(local_NumVal);
            header_list.push_back(physio_id);
        }
        /*
        if(type == poll_request_config_measured_data_codepage1){
            save_num_val_list_rows("MeasuredCP1");
        }
        else if(type==poll_request_low_alarm_limits){
            save_num_val_list_rows("LowLimits");
        }
        else if(type==poll_request_high_alarm_limits){
            save_num_val_list_rows("HighLimits");
        }
        numval_list.clear();*/
    }
}
void Evita4_vent::parse_alarm(std::vector<byte> &packetbuffer){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for(unsigned long i=2;i<framelen-2;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for(int i=0;i<responselen;i=i+15){
            byte priority = response[i+1];
            std::string AlarmCode(response.begin()+i+1,response.begin()+3+i);
            std::string AlarmPhase(response.begin()+3+i,response.begin()+15+i);

            AlarmInfo local_alarm;
            local_alarm.Timestamp = pkt_timestamp;
            local_alarm.AlarmCode = AlarmCode;
            local_alarm.AlarmPhrase =AlarmPhase;
            local_alarm.Priority = std::to_string(priority);
            local_alarm.timestamp = std::time(nullptr);

            alarm_list.push_back(local_alarm);
        }
        //save_alarm_list_rows();
        //alarm_list.clear();
    }
}

void Evita4_vent::save_data(){
    save_alarm_list_rows();
    if(numval_list.size()!=0){
        if(numval_list[0].type==poll_request_config_measured_data_codepage1){
            save_num_val_list_rows("MeasuredCP1");
        }
        else if(numval_list[0].type==poll_request_low_alarm_limits){
            save_num_val_list_rows("AlarmLow");
        }
        else if(numval_list[0].type==poll_request_high_alarm_limits){
            save_num_val_list_rows("AlarmHigh");
        }
    }

}

void Evita4_vent::parse_realtime_data_configs(std::vector<byte> &packetbuffer){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for(unsigned long i=2;i<framelen-2;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for(int i=0;i<responselen;i=i+23){
            std::vector<byte> DataCode(response.begin()+i,response.begin()+2+i);
            std::string Interval(response.begin()+2+i,response.begin()+10+i);
            std::string MinimalV(response.begin()+10+i,response.begin()+15+i);
            std::string MaximalV(response.begin()+15+i,response.begin()+20+i);
            std::string MaxBin(response.begin()+20+i,response.begin()+23+i);
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
            physio_id = RealtimeConfigs.find(datacode)->second;
            RealtimeCfg local_cfg;
            local_cfg.id = physio_id;

            int interval = 0;
            for(uint n=0;n<Interval.length();n++){
                if(Interval[Interval.length()-1-n]==0x20)
                    break;
                else{
                    interval+= pow(10,n)*(Interval[Interval.length()-1-n]-0x30);
                }
            }
            local_cfg.interval = interval;

            int minimal_v = 0;
            for(uint n=0;n<MinimalV.length();n++){
                if(MinimalV[MinimalV.length()-1-n]==0x20 || MinimalV[MinimalV.length()-1-n]==0x2D)
                    break;
                else{
                    minimal_v+= pow(10,n)*(MinimalV[MinimalV.length()-1-n]-0x30);
                }
                if(MinimalV[0]==0x2D)
                    minimal_v=-1*minimal_v;
            }
            local_cfg.minimal_val = minimal_v;

            int maximal_v = 0;
            for(uint n=0;n<MaximalV.length();n++){
                if(MaximalV[MaximalV.length()-1-n]==0x20 || MaximalV[MaximalV.length()-1-n]==0x2D)
                    break;
                else{
                    maximal_v+= pow(10,n)*(MaximalV[MaximalV.length()-1-n]-0x30);
                }
                if(MaximalV[0]==0x2D)
                    maximal_v = -1*maximal_v;
            }
            local_cfg.maximal_val = maximal_v;

            int max_bin = 0;
            for(uint n=0;n<MaxBin.length();n++){
                if(MaxBin[MaxBin.length()-1-n]==' ')
                    break;
                else{
                    max_bin+= pow(16,n)*(MaxBin[MaxBin.length()-1-n]-0x30);
                }
            }
            local_cfg.max_bin = max_bin;
            cfg_list.push_back(local_cfg);
        }
    }
}

void Evita4_vent::parse_realtime_data(std::vector<byte> &packetbuffer){
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        byte sync_byte = packetbuffer[0];
        for(unsigned long i=1;i<framelen-1;i++){
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();
        int value_index = 0;
        std::string physio_id;
        std::string data_value;

        for(int i=0;i<responselen;i=i+2){
            std::vector<byte> DataValue(response.begin()+i,response.begin()+2+i);
            if((DataValue[0] & 0xc0) == 0x80){ // sync data
                while((sync_byte) &((0x01<<value_index)==1))
                {
                    byte datacode = realtime_data_list[value_index];
                    physio_id = RealtimeConfigs.find(datacode)->second;
                    byte front_num = (0x3f & DataValue[0]);
                    byte back_num = (0x3f & DataValue[1]);
                    int value = int(front_num)+int(back_num)*(64);
                    float value2=0;

                    for(uint j=0;j<cfg_list.size();j++){
                        if(cfg_list[j].id == physio_id){
                            value2 = cfg_list[j].minimal_val+value*(cfg_list[j].maximal_val-cfg_list[j].minimal_val)/float(cfg_list[j].max_bin);
                            break;
                        }
                    }

                    data_value = std::to_string(value2);

                    NumVal local_NumVal;
                    local_NumVal.Timestamp = pkt_timestamp;
                    local_NumVal.PhysioID = physio_id;
                    local_NumVal.Value =data_value;

                    numval_list.push_back(local_NumVal);
                    //header_list.push_back(physio_id);
                    value_index=value_index+1;}
            }
            else if((DataValue[0] & 0xc0) == 0xc0){// sync command
                qDebug()<<"cmd";
                //TODO::deal with sync command
            }
        }
        //save_num_val_list_rows("RealtimeData");
        //numval_list.clear();
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
        std::time_t current_pc_time = std::time(nullptr);
        std::string pkt_timestamp =std::asctime(std::localtime(&current_pc_time));
        pkt_timestamp.erase(8,11);

        QString filename = pathcsv + QString::fromStdString(pkt_timestamp) + QString::fromStdString(datatype)+".csv";
        write_num_header_list(datatype, filename);

        std::string row;
        bool changed=false;
        int elementcount=0;

        row+=(numval_list[0].Timestamp);
        row.append(",");

        for(uint i=0;i<numval_list.size();i++){
            if(current_pc_time>(numval_list[i].timestamp+timelapse)
                && numval_list[i].timestamp==numval_list[0].timestamp){
                elementcount+=1;
                changed=true;
                row+=(numval_list[i].Value);
                row.append(",");
            }
        }
        row.append("\n");
        if(changed){
            QFile myfile(filename);
            if (myfile.open(QIODevice::Append)) {
                myfile.write((char*)&row[0], row.length());
            }
            qDebug()<<"write data to file";
            numval_list.erase(numval_list.begin(), numval_list.begin()+elementcount);
        }

    }
}

void Evita4_vent::save_alarm_list_rows(){
    if(alarm_list.size()!=0){
        std::time_t current_pc_time = std::time(nullptr);
        std::string pkt_timestamp =std::asctime(std::localtime(&current_pc_time));
        pkt_timestamp.erase(8,11);

        QString filename = pathcsv + QString::fromStdString(pkt_timestamp) + QString::fromStdString("Alarm")+".csv";

        std::string row;
        bool changed=false;
        int elementcount=0;

        for(uint i=0;i<alarm_list.size();i++){
            if(current_pc_time>(alarm_list[i].timestamp+timelapse) &&alarm_list[i].timestamp==alarm_list[0].timestamp){
                elementcount+=1;
                changed=true;
                row+=(alarm_list[i].Timestamp);
                row.append(",");
                row+=(alarm_list[i].AlarmCode);
                row.append(",");
                row+=(alarm_list[i].Priority);
                row.append(",");
                row+=(alarm_list[i].AlarmPhrase);
                row.append(",\n");
            }
        }
        row.append("\n");
        if(changed){
            QFile myfile(filename);
            if (myfile.open(QIODevice::Append)) {
                myfile.write((char*)&row[0], row.length());
            }
            qDebug()<<"write alarm to file";
            alarm_list.erase(alarm_list.begin(), alarm_list.begin()+elementcount);
        }
    }
}

void Evita4_vent::write_num_header_list(std::string datatype, QString filename){
    if(write_header_for_data_type(datatype)){
        std::string header;

        header.append("Time");
        header.append(",");

        for(unsigned long i=0;i<numval_list.size();i++){
            if(numval_list[i].timestamp==numval_list[0].timestamp){
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
    else if(datatype == "DeviceSettings"){
        if (m_transmissionstart2)
        {
            m_transmissionstart2 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "TextMessages"){
        if (m_transmissionstart3)
        {
            m_transmissionstart3 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "AlarmLow"){
        if (m_transmissionstart4)
        {
            m_transmissionstart4 = false;

        }
        else writeheader = false;
    }
    else if(datatype == "AlarmHigh"){
        if (m_transmissionstart5)
        {
            m_transmissionstart5 = false;

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

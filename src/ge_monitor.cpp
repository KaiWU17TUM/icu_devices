#include "ge_monitor.h"
#include "QThread"


GE_Monitor::GE_Monitor()
{
    local_serial_port = new MySerialPort();
    local_serial_port->serial->setPortName("/dev/ttyUSB1");
    local_serial_port->serial->setBaudRate(QSerialPort::Baud19200);
    local_serial_port->serial->setDataBits(QSerialPort::Data8);
    local_serial_port->serial->setParity(QSerialPort::EvenParity);
    local_serial_port->serial->setStopBits(QSerialPort::OneStop);
    local_serial_port->serial->setFlowControl(QSerialPort::HardwareControl);
    QObject::connect(local_serial_port->serial, SIGNAL(readyRead()), this, SLOT(process_buffer()));

    logger_timer = new QTimer();
    connect(logger_timer, SIGNAL(timeout()), this, SLOT(save_data()));
}

/**
 * @brief GE_Monitor::start : the main configuration part of GE_Monitor
 */
void GE_Monitor::start(){
    try {
        std::cout<<"Try to open the serial port for GE Monitor"<<std::endl;
        try_to_open_port();

        // prepare files
        std::time_t current_pc_time = std::time(nullptr);
        filename_phdb = pathcsv + QString::fromStdString(std::to_string(current_pc_time)) + "_PHDB_data.csv";
        filename_alarm = pathcsv + QString::fromStdString(std::to_string(current_pc_time)) + "_Alarm.csv";

        std::cout<<"Reset wave transfer";
        request_wave_stop();

        // Set the period of phdb data, minimal interval is 5
        int phdb_interval = 5;
        request_phdb_transfer(phdb_interval); // send the phdb data transfer request

        // Set the wave that you want to retrieve, make sure the sum of samples/sec is smaller than 600
        std::vector<byte> wave_ids = {1, 8};
        for(int i=0;i<wave_ids.size();i++){
            std::string physioId = datex::WaveIdLabels.find(wave_ids[i])->second;
            QString filename = pathcsv + QString::fromStdString(std::to_string(current_pc_time)) + "_" + QString::fromStdString(physioId)+".csv";
            filenames_wave[physioId] = filename;
        }
        request_wave_transfer(wave_ids); // send the wave transfer request

        request_alarm_transfer(); // send the alarm transfer request, default into differential mode

        int period = 3000; // set the logging frequency
        logger_timer->start(period); // start the logger

    } catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }
}

/**
 * @brief GE_Monitor::request_phdb_transfer : call this function to retrieve phdb data periodically
 */
void GE_Monitor::request_phdb_transfer(int interval){
    struct datex::datex_record_phdb_req requestPkt;
    struct datex::dri_phdb_req *pRequest;

    //Clear the pkt
    memset(&requestPkt,0x00,sizeof(requestPkt));

    //Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr)+sizeof(struct datex::dri_phdb_req);
    requestPkt.hdr.r_maintype = DRI_MT_PHDB;
    requestPkt.hdr.dri_level =  0;

    //The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short) DRI_EOL_SUBR_LIST;

    //Fill the request
    pRequest = (struct datex::dri_phdb_req*)&(requestPkt.phdbr);
    pRequest->phdb_rcrd_type = DRI_PH_DISPL;
    pRequest->tx_ival = interval;
    pRequest->phdb_class_bf = DRI_PHDBCL_REQ_BASIC_MASK|DRI_PHDBCL_REQ_EXT1_MASK|DRI_PHDBCL_REQ_EXT2_MASK|DRI_PHDBCL_REQ_EXT3_MASK;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    write_buffer(payload,length);
}

/**
 * @brief GE_Monitor::request_alarm_transfer : call this function to retrieve alarm data whenever it happens
 */
void GE_Monitor::request_alarm_transfer(){
    struct datex::datex_record_alarm_req requestPkt;
    struct datex::al_tx_cmd *pRequest;

    //Clear the pkt
    memset(&requestPkt,0x00,sizeof(requestPkt));

    //Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr)+sizeof(struct datex::al_tx_cmd);
    requestPkt.hdr.r_maintype = DRI_MT_ALARM;
    requestPkt.hdr.dri_level =  0;

    //The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short) DRI_EOL_SUBR_LIST;

    //Fill the request
    pRequest = (struct datex::al_tx_cmd*)&(requestPkt.alarm_cmd);
    pRequest->cmd = DRI_AL_ENTER_DIFFMODE;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    write_buffer(payload,length);
}

/**
 * @brief GE_Monitor::request_wave_transfer : call this function to retrieve wave
 */
void GE_Monitor::request_wave_transfer(std::vector<byte> wave_id){
    // Test if samples > limitation
    int sum = 0;
    for(uint i=0;i<wave_id.size();i++){
        sum+=datex::WaveIdFreqs.find(wave_id[i])->second;
    }
    if(sum>datex::max_wave_samples_limitation){
        qDebug()<<"Samples per second exceeds maximum, this request will not be sent";
        return;
    }
    struct datex::datex_record_wave_req requestPkt;
    struct datex::dri_wave_req *pRequest;

    //Clear the pkt
    memset(&requestPkt,0x00,sizeof(requestPkt));

    //Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr)+sizeof(struct datex::dri_wave_req);
    requestPkt.hdr.r_maintype = DRI_MT_WAVE;
    requestPkt.hdr.dri_level =  0;

    //The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short) DRI_EOL_SUBR_LIST;

    //Fill the request
    pRequest = (struct datex::dri_wave_req*)&(requestPkt.wfreq);
    pRequest->req_type = WF_REQ_CONT_START;
    uint i=0;
    for(i=0;i<wave_id.size();i++){
        pRequest->type[i] = wave_id[i];
    }

    pRequest->type[i] = DRI_EOL_SUBR_LIST;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    write_buffer(payload,length);
}

/**
 * @brief GE_Monitor::request_wave_stop : call this function to stop wave transfer
 */
void GE_Monitor::request_wave_stop(){
    struct datex::datex_record_wave_req requestPkt;
    struct datex::dri_wave_req *pRequest;

    //Clear the pkt
    memset(&requestPkt,0x00,sizeof(requestPkt));

    //Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr)+sizeof(struct datex::dri_wave_req);
    requestPkt.hdr.r_maintype = DRI_MT_WAVE;
    requestPkt.hdr.dri_level =  0;

    //The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short) DRI_EOL_SUBR_LIST;

    //Fill the request
    pRequest = (struct datex::dri_wave_req*)&(requestPkt.wfreq);
    pRequest->req_type = WF_REQ_CONT_STOP;
    pRequest->type[0] = DRI_EOL_SUBR_LIST;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    write_buffer(payload,length);
}

/**
 * @brief GE_Monitor::write_buffer : call this wrapper function to format msg and send it out
 */
void GE_Monitor::write_buffer(byte* payload, int length){
    byte checksum=0;
    std::vector<byte> temptxbuff;

    temptxbuff.push_back(0x7e);

    for(int i=0;i<length;i++){
        switch(payload[i])
        {
            case(0x7e):
                temptxbuff.push_back(0x7d);
                temptxbuff.push_back(0x5e);
                checksum+=0x7d;
                checksum+=0x5e;
                break;

            case(0x7d):
                temptxbuff.push_back(0x7d);
                temptxbuff.push_back(0x5d);
                checksum+=0x7d;
                checksum+=0x5d;
                break;

            default:
                temptxbuff.push_back(payload[i]);
                checksum+=payload[i];
                break;
        }
    }

    switch(checksum){
        case 0x7e:
            temptxbuff.push_back(checksum);
            temptxbuff.push_back(0x5e);
            break;

        case 0x7d:
            temptxbuff.push_back(checksum);
            temptxbuff.push_back(0x5d);
            break;

        default:
            temptxbuff.push_back(checksum);
    }

    temptxbuff.push_back(0x7e);
    const char* payload_data = (const char*)&temptxbuff[0];
    this->local_serial_port->serial->write(payload_data, temptxbuff.size());
}

/**
 * @brief GE_Monitor::process_buffer : The serial port callback when reveive data
 */
void GE_Monitor::process_buffer(){
    QByteArray data = local_serial_port->serial->readAll();
    for (int i = 0; i < data.size(); ++i) {
        create_frame_list_from_byte(data[i]);
    }
    if(frame_buffer.size()>0){
        read_packet_from_frame();
        frame_buffer.clear();
    }
}

/**
 * @brief GE_Monitor::create_frame_list_from_byte : call this function to get raw packet out from serial data
 */
void GE_Monitor::create_frame_list_from_byte(byte b){
    //if get a byte which indicates the start of a msg
    if(b==0x7e && m_fstart){
        m_fstart = false;
        m_storestart = true;
    }

    //encounter the end
    else if(b==0x7e && m_fstart==false){
        m_fstart = true;
        m_storestart = false;
        m_storeend = true;
    }

    if(m_storestart==true){
        //encounter control byte
        if(b==0x7d)
            m_bitschiftnext = true;
        //normal byte
        else{
            //the byte before is a control byte
            if(m_bitschiftnext == true){
                m_bitschiftnext = false;
                b |=0x7c;
                b_list.push_back(b);
            }
            else if(b!=0x7e){
                b_list.push_back(b);
            }
        }
    }

    //end of msg
    else if(m_storeend){
        if(b_list.size()!=0){
            byte checksum=0x00;
            for(uint i=0;i<b_list.size()-1;i++){
                checksum+=b_list[i];
            }

            if(checksum == b_list[b_list.size()-1]){
                frame_buffer.push_back(b_list);
                std::cout<<"GE Monitor Checksum correct"<<std::endl;
                b_list.clear();
            }else{
                std::cout<<"GE Monitor Checksum wrong"<<std::endl;
                b_list.clear();
            }

            m_storeend=false;
        }
        else{
            m_storestart = true;
            m_storeend = false;
            m_fstart = false;
        }
    }

}

/**
 * @brief GE_Monitor::read_packet_from_frame : call this function to get parsed data from raw packet
 */
void GE_Monitor::read_packet_from_frame(){
    std::vector<struct datex::datex_record*> record_array;
    for(uint i=0;i<frame_buffer.size();i++){
        struct datex::datex_record* ptr= (struct datex::datex_record*)(&frame_buffer[i][0]);
        record_array.push_back(ptr);
    }

    for(uint i=0;i<frame_buffer.size();i++){
        struct datex::datex_record record = (*record_array[i]);

        // this is a PHDB record
        if(record.hdr.r_maintype == DRI_MT_PHDB){
            // time from the GE_Monitor
            //uint unixtime = record.hdr.r_time;
            struct datex::dri_phdb phdata_ptr;

            for(int j=0;j<8&&record.hdr.sr_desc[j].sr_type!=0xFF;j++){
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                byte buffer[270];
                for(int n=0;n<270;n++){
                    buffer[n] = record.rcrd.data[4+offset+n];
                }
                switch(j){
                    case 0:
                        (phdata_ptr.physdata.basic) = *(struct datex::basic_phdb*)buffer;
                        break;
                    case 1:
                        (phdata_ptr.physdata.ext1) = *(struct datex::ext1_phdb*)buffer;
                        break;
                    case 2:
                        (phdata_ptr.physdata.ext2) = *(struct datex::ext2_phdb*)buffer;
                        break;
                    case 3:
                        (phdata_ptr.physdata.ext3) = *(struct datex::ext3_phdb*)buffer;
                        break;
                }

                // get time from PC
                std::time_t pc_time = std::time(nullptr);
                pkt_timestamp = std::asctime(std::localtime(&pc_time));
                pkt_timestamp.erase(pkt_timestamp.end()-1);

                /*
                // Use time from GE_Monitor
                std::time_t temp = unixtime;
                pkg_timestamp = std::time(nullptr);
                std::tm* t = std::gmtime(&temp);
                std::stringstream ss;
                ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
                machine_timestamp = ss.str();*/
                machine_timestamp = pkt_timestamp;
                save_basic_sub_record(phdata_ptr);
                save_ext1_and_ext2_and_ext3_record(phdata_ptr);

            }
        }

        // this is a WAVE record
        else if(record.hdr.r_maintype == DRI_MT_WAVE){
            // the timestamp from GE_Monitor
            // uint unixtime = record.hdr.r_time;
            // the timestamp from pc [seconds since 01-Jan-1970]
            unsigned long int pc_time = std::time(nullptr);
            for(int j=0;j<8&&record.hdr.sr_desc[j].sr_type!=0xFF;j++){
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                int srsamplelenbytes[2];
                srsamplelenbytes[0] = record.rcrd.data[offset];
                srsamplelenbytes[1] = record.rcrd.data[offset+1];
                int sub_header_len = 6;
                int subrecordlen = 256*(int)srsamplelenbytes[1]+(int)srsamplelenbytes[0];
                int buflen = 2*subrecordlen; //(nextoffset - offset - 6);
                byte * buffer = (byte *)malloc(sizeof(byte)*buflen);
                for (int j = 0; j < buflen; j++)
                {
                    buffer[j] = record.rcrd.data[sub_header_len + j + offset];
                }
                std::vector<short> waveValList;
                std::vector<unsigned long int> TimeList;
                int samples = datex::WaveIdFreqs.find(record.hdr.sr_desc[j].sr_type)->second;
                for(int n = 0; n < buflen; n += 2){
                    waveValList.push_back((buffer[n+1])*256+(buffer[n]));
                    TimeList.push_back((unsigned long int)pc_time*1000+1000*(n/2)/samples);
                }
                WaveValResult wave_val;
                /*
                // use timestamp from GE_Monitor
                std::time_t temp = unixtime;
                std::tm* t = std::gmtime(&temp);
                std::stringstream ss;
                ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
                */
                std::time_t result = std::time(nullptr);
                pkt_timestamp = std::asctime(std::localtime(&result));
                pkt_timestamp.erase(pkt_timestamp.end()-1);

                wave_val.Timestamp = pkt_timestamp;
                wave_val.timestamp = result;
                wave_val.TimeList = TimeList;
                std::string physioId = datex::WaveIdLabels.find(record.hdr.sr_desc[j].sr_type)->second;
                wave_val.PhysioID = physioId;
                wave_val.Unitshift = get_wave_unit_shift(wave_val.PhysioID);
                wave_val.Value = waveValList;
                m_WaveValList.push_back(wave_val);
            }
        }

        else if(record.hdr.r_maintype == DRI_MT_ALARM){
            // the timestamp from GE_Monitor
            // uint unixtime = record.hdr.r_time;
            for(int j=0;j<8&&record.hdr.sr_desc[j].sr_type!=0xFF;j++){
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                byte buffer[270];
                for(int n=0;n<270;n++){
                    buffer[n] = record.rcrd.data[offset+n];
                }
                struct datex::dri_al_msg dri_al_msg_ptr;
                dri_al_msg_ptr = *(struct datex::dri_al_msg*)buffer;
                /*
                // use time from GE_Monitor
                std::time_t temp = unixtime;
                std::tm* t = std::gmtime(&temp);
                std::stringstream ss;
                ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
                */
                AlarmResult alarm[5];
                std::time_t pc_time = std::time(nullptr);
                pkt_timestamp = std::asctime(std::localtime(&pc_time));
                pkt_timestamp.erase(pkt_timestamp.end()-1);
                for(int n=0; n<5; n++){
                    alarm[n].Timestamp = pkt_timestamp;
                    alarm[n].timestamp = pc_time;
                    alarm[n].text = std::string(dri_al_msg_ptr.al_disp[n].text);
                    for(uint m=0;m<alarm[n].text.length();m++){
                        if(alarm[n].text[m]=='\n'){
                            alarm[n].text[m]=' ';
                            break;
                        }
                }
                switch(dri_al_msg_ptr.al_disp[n].color){
                    case 0:
                        alarm[n].color = "DRI_PR0";
                        break;
                    case 1:
                        alarm[n].color = "DRI_PR1";
                        break;
                    case 2:
                        alarm[n].color = "DRI_PR2";
                        break;
                    case 3:
                        alarm[n].color = "DRI_PR3";
                        break;
                }
                alarm[n].timestamp = std::time(nullptr);
                if(dri_al_msg_ptr.al_disp[n].text_changed==1){
                    m_AlarmList.push_back(alarm[n]);
                }
            }
        }
    }
    }
}
/***************************************************************************************************************************************************************/
/**
 * @brief Saving functions : call these function to save parsed data
 */

void GE_Monitor::save_data()
{
    write_to_rows();
    save_alarm_to_csv();
    save_wave_to_csv();
}

double GE_Monitor::get_wave_unit_shift(std::string physioId){
    double decimalshift = 1;
    if(physioId.find("ECG")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("INVP")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("PLETH")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("CO2")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("O2")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("RESP")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("AA")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("FLOW")!=std::string::npos)
        return(decimalshift = 0.01);
    else if(physioId.find("AWP")!=std::string::npos)
        return(decimalshift = 0.1);
    else if(physioId.find("VOL")!=std::string::npos)
        return(decimalshift = -1);
    else if(physioId.find("EEG")!=std::string::npos)
        return(decimalshift = 0.1);
    else
        return decimalshift;

}

void GE_Monitor::save_alarm_to_csv(){
    for(uint i=0; i<m_AlarmList.size(); i++){
        // Get local time
        std::time_t pc_current_timestamp = std::time(nullptr);
        /*
        std::string pkt_timestamp =std::asctime(std::localtime(&pc_current_timestamp));
        pkt_timestamp.erase(8,11);
        pkt_timestamp.pop_back();

        if(alarm_transmissionstart){
            filename_alarm = pathcsv + QString::fromStdString(std::to_string(current_pc_time)) + "_Alarm.csv";
        }
        */
        std::string row;
        bool changed=false;
        int elementcount=0;

        if((m_AlarmList[i].text.length())>0 && pc_current_timestamp> m_AlarmList[i].timestamp+timelapse
                &&  m_AlarmList[i].timestamp== m_AlarmList[0].timestamp){
            changed = true;
            elementcount+=1;
            row.append(m_AlarmList[i].Timestamp);
            row.append(",");
            row.append(std::to_string(m_AlarmList[i].timestamp));
            row.append(",");
            row.append( m_AlarmList[i].text);
            row.append(",");
            row.append(m_AlarmList[i].color);
            row.append(",\n");
            }

    if(changed){
        QFile myfile(filename_alarm);
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&row[0], row.length());
            qDebug()<<"write to alarm file";
        }
        m_AlarmList.erase(m_AlarmList.begin(), m_AlarmList.begin()+elementcount);
    }
}
}

void GE_Monitor::save_wave_to_csv(){
    for(uint i=0; i<m_WaveValList.size(); i++){
        // Get local time
        std::time_t current_pc_timestamp = std::time(nullptr);
        /*
        std::string pkt_timestamp =std::asctime(std::localtime(&current_pc_timestamp));
        pkt_timestamp.erase(8,11);
        pkt_timestamp.pop_back();
        QString filename =  pathcsv + QString::fromStdString(pkt_timestamp) + QString::fromStdString(m_WaveValList[i].PhysioID) + ".csv";
        */
        QString filename = filenames_wave[m_WaveValList[i].PhysioID];
        double decimalshift = m_WaveValList[i].Unitshift;
        std::string row;
        bool changed=false;
        int elementcount=0;
        for(uint j=0;j<m_WaveValList[i].Value.size();j++){
            if(current_pc_timestamp>m_WaveValList[i].TimeList[j]/1000+timelapse){
                changed=true;
                elementcount+=1;

                std::string wave_val = validate_wave_data(m_WaveValList[i].Value[j], decimalshift, false);
                row.append( m_WaveValList[i].Timestamp);
                row.append(",");
                row.append(wave_val);
                row.append(",");
                row.append(std::to_string(m_WaveValList[i].TimeList[j]));
                row.append(",\n");
            }
        }

        if(changed){
            QFile myfile(filename);
            if (myfile.open(QIODevice::Append)) {
                myfile.write((char*)&row[0], row.length());
                qDebug()<<"write to wave file";
                //m_WaveValList.clear();
                m_WaveValList[i].Value.erase( m_WaveValList[i].Value.begin(),  m_WaveValList[i].Value.begin()+elementcount);
                m_WaveValList[i].TimeList.erase( m_WaveValList[i].TimeList.begin(),  m_WaveValList[i].TimeList.begin()+elementcount);
            }
        }

    }

}

void GE_Monitor::save_basic_sub_record(datex::dri_phdb driSR){
    //ECG
    validate_add_data("ECG_HR", driSR.physdata.basic.ecg.hr,1,true);
    validate_add_data("ST1", driSR.physdata.basic.ecg.st1,0.01,true);
    validate_add_data("ST2", driSR.physdata.basic.ecg.st2,0.01,true);
    validate_add_data("ST3", driSR.physdata.basic.ecg.st3,0.01,true);
    validate_add_data("HR_max",  driSR.physdata.basic.ecg_extra.hr_max,1,true);
    validate_add_data("HR_min",  driSR.physdata.basic.ecg_extra.hr_min,1,true);

    //Respiration(Impedance)
    validate_add_data("RES_imp", driSR.physdata.basic.ecg.imp_rr,1,true);

    //Invasive pressure
    validate_add_data("PCWP", driSR.physdata.basic.co_wedge.pcwp, 0.01, true);
    validate_add_data("P1_PR", driSR.physdata.basic.p1.hr, 1, true);
    validate_add_data("P1_Systolic", driSR.physdata.basic.p1.sys, 0.01, true);
    validate_add_data("P1_Disatolic", driSR.physdata.basic.p1.dia, 0.01, true);
    validate_add_data("P1_Mean", driSR.physdata.basic.p1.mean, 0.01, true);
    validate_add_data("P2_PR", driSR.physdata.basic.p2.hr, 1, true);
    validate_add_data("P2_Systolic", driSR.physdata.basic.p2.sys, 0.01, true);
    validate_add_data("P2_Diastolic", driSR.physdata.basic.p2.dia, 0.01, true);
    validate_add_data("P2_Mean", driSR.physdata.basic.p2.mean, 0.01, true);
    validate_add_data("P3_PR", driSR.physdata.basic.p3.hr, 1, true);
    validate_add_data("P3_Systolic", driSR.physdata.basic.p3.sys, 0.01, true);
    validate_add_data("P3_Diastolic", driSR.physdata.basic.p3.dia, 0.01, true);
    validate_add_data("P3_Mean", driSR.physdata.basic.p3.mean, 0.01, true);
    validate_add_data("P4_PR", driSR.physdata.basic.p4.hr, 1, true);
    validate_add_data("P4_Systolic", driSR.physdata.basic.p4.sys, 0.01, true);
    validate_add_data("P4_Diastolic", driSR.physdata.basic.p4.dia, 0.01, true);
    validate_add_data("P4_Mean", driSR.physdata.basic.p4.mean, 0.01, true);
    validate_add_data("P5_PR", driSR.physdata.basic.p5.hr, 1, true);
    validate_add_data("P5_Systolic", driSR.physdata.basic.p5.sys, 0.01, true);
    validate_add_data("P5_Diastolic", driSR.physdata.basic.p5.dia, 0.01, true);
    validate_add_data("P5_Mean", driSR.physdata.basic.p5.mean, 0.01, true);
    validate_add_data("P6_PR", driSR.physdata.basic.p6.hr, 1, true);
    validate_add_data("P6_Systolic", driSR.physdata.basic.p6.sys, 0.01, true);
    validate_add_data("P6_Diastolic", driSR.physdata.basic.p6.dia, 0.01, true);
    validate_add_data("P6_Mean", driSR.physdata.basic.p6.mean, 0.01, true);


    //NIBP
    validate_add_data("NIBP_Mean", driSR.physdata.basic.nibp.hr, 1, true);
    validate_add_data("NIBP_Systolic", driSR.physdata.basic.nibp.sys, 0.01, true);
    validate_add_data("NIBP_Diastolic", driSR.physdata.basic.nibp.dia, 0.01, true);
    validate_add_data("NIBP_Mean", driSR.physdata.basic.nibp.mean, 0.01, true);

    //SpO2
    validate_add_data("SpO2_POS", driSR.physdata.basic.SpO2.SpO2, 0.01, true);
    validate_add_data("SpO2_PR", driSR.physdata.basic.SpO2.pr,1,true);

    //SvO2
    validate_add_data("SvO2", driSR.physdata.basic.svo2.svo2,1,true);

    //Temperature
    validate_add_data("T1_Temp", driSR.physdata.basic.t1.temp, 0.01, false);
    validate_add_data("T2_Temp", driSR.physdata.basic.t2.temp, 0.01, false);
    validate_add_data("T3_Temp", driSR.physdata.basic.t3.temp, 0.01, false);
    validate_add_data("T4_Temp", driSR.physdata.basic.t4.temp, 0.01, false);
    validate_add_data("TBlood", driSR.physdata.basic.co_wedge.blood_temp, 0.01, false);

    //Cardiac Output
    validate_add_data("CO", driSR.physdata.basic.co_wedge.co, 1, false);
    validate_add_data("REF", driSR.physdata.basic.co_wedge.ref, 0.01, false);


    //Gas
    validate_add_data("AA_ET", driSR.physdata.basic.aa.et, 0.01, false);
    validate_add_data("AA_FI", driSR.physdata.basic.aa.fi, 0.01, false);
    validate_add_data("AA_MAC_SUM", driSR.physdata.basic.aa.mac_sum, 0.01, false);
    validate_add_data("O2_FI",  driSR.physdata.basic.o2.fi, 0.01, false);
    validate_add_data("O2_ET", driSR.physdata.basic.o2.et, 0.01, false);
    validate_add_data("N2O_FI",  driSR.physdata.basic.n2o.fi, 0.01, false);
    validate_add_data("N2O_ET", driSR.physdata.basic.n2o.et, 0.01, false);
    validate_add_data("CO2_RR", driSR.physdata.basic.co2.rr, 1, false);
    validate_add_data("CO2_FI", driSR.physdata.basic.co2.fi, 0.01, false);
    validate_add_data("CO2_ET", driSR.physdata.basic.co2.et, 0.01, false);
    validate_add_data("AT_pre", driSR.physdata.basic.co2.amb_press, 0.1,true);

}

void GE_Monitor::save_ext1_and_ext2_and_ext3_record(datex::dri_phdb driSR){
    // ECG
    validate_add_data("PVC", driSR.physdata.ext1.arrh_ecg.pvc, 1,false);
    validate_add_data("ST_I", driSR.physdata.ext1.ecg12.stI, 0.01,false);
    validate_add_data("ST_II", driSR.physdata.ext1.ecg12.stII, 0.01,false);
    validate_add_data("ST_III", driSR.physdata.ext1.ecg12.stIII, 0.01,false);
    validate_add_data("ST_aVL", driSR.physdata.ext1.ecg12.stAVL, 0.01,false);
    validate_add_data("ST_aVR", driSR.physdata.ext1.ecg12.stAVR, 0.01,false);
    validate_add_data("ST_aVF", driSR.physdata.ext1.ecg12.stAVF, 0.01,false);
    validate_add_data("ST_V1", driSR.physdata.ext1.ecg12.stV1, 0.01, false);
    validate_add_data("ST_V2", driSR.physdata.ext1.ecg12.stV2, 0.01, false);
    validate_add_data("ST_V3", driSR.physdata.ext1.ecg12.stV3, 0.01, false);
    validate_add_data("ST_V4", driSR.physdata.ext1.ecg12.stV4, 0.01, false);
    validate_add_data("ST_V5", driSR.physdata.ext1.ecg12.stV5, 0.01, false);
    validate_add_data("ST_V6", driSR.physdata.ext1.ecg12.stV6, 0.01, false);

    //Invasive blood pressure
    validate_add_data("P7_PR", driSR.physdata.ext1.p7.hr, 1, true);
    validate_add_data("P7_Systolic", driSR.physdata.ext1.p7.sys, 0.01, true);
    validate_add_data("P7_Diastolic", driSR.physdata.ext1.p7.dia, 0.01, true);
    validate_add_data("P7_Mean", driSR.physdata.ext1.p7.mean, 0.01, true);
    validate_add_data("P8_PR", driSR.physdata.ext1.p8.hr, 1, true);
    validate_add_data("P8_Systolic", driSR.physdata.ext1.p8.sys, 0.01, true);
    validate_add_data("P8_Diastolic", driSR.physdata.ext1.p8.dia, 0.01, true);
    validate_add_data("P8_Mean", driSR.physdata.ext1.p8.mean, 0.01, true);
    validate_add_data("CCP", driSR.physdata.ext3.cpp.value, 0.01, false);
    validate_add_data("SPV", driSR.physdata.ext3.delp.spv, 0.01, false);
    validate_add_data("PPV", driSR.physdata.ext3.delp.ppv, 1, false);

    //SpO2
    validate_add_data("SpO2_PR2", driSR.physdata.ext1.SpO2_ch2.pr, 1, false);
    validate_add_data("SpO2_POS2", driSR.physdata.ext1.SpO2_ch2.SpO2, 0.01, false);

    //Cardiac Output
    validate_add_data("CI", driSR.physdata.ext3.picco.ci, 1, false);
    validate_add_data("CCO", driSR.physdata.ext3.picco.cco, 1, false);
    validate_add_data("CCI", driSR.physdata.ext3.picco.cci, 1, false);
    validate_add_data("SVR", driSR.physdata.ext3.picco.svr, 1, false);
    validate_add_data("SVRI", driSR.physdata.ext3.picco.svri, 1, false);
    validate_add_data("SV", driSR.physdata.ext3.picco.sv, 0.1, false);
    validate_add_data("SVI", driSR.physdata.ext3.picco.svi, 0.1, false);
    validate_add_data("SVV", driSR.physdata.ext3.picco.svv, 0.1, false);
    validate_add_data("GEDV", driSR.physdata.ext3.picco.gedv, 1, false);
    validate_add_data("GEDI", driSR.physdata.ext3.picco.gedi, 1, false);
    validate_add_data("EVLW", driSR.physdata.ext3.picco.evlw, 1, false);
    validate_add_data("ELWI", driSR.physdata.ext3.picco.elwi, 1, false);
    validate_add_data("ITBV", driSR.physdata.ext3.picco.itbv, 1, false);
    validate_add_data("ITBI", driSR.physdata.ext3.picco.itbi, 1, false);
    validate_add_data("CPO", driSR.physdata.ext3.picco.cpo, 1, false);
    validate_add_data("CPI", driSR.physdata.ext3.picco.cpi, 1, false);
    validate_add_data("CFI", driSR.physdata.ext3.picco.cfi, 0.01, false);
    validate_add_data("dPmax", driSR.physdata.ext3.picco.dpmax, 1, false);
    validate_add_data("GEF", driSR.physdata.ext3.picco.gef, 0.1, false);
    validate_add_data("PPV", driSR.physdata.ext3.picco.ppv, 0.1, false);
    validate_add_data("PVPI", driSR.physdata.ext3.picco.pvpi, 0.01, false);
    validate_add_data("Einj", driSR.physdata.ext3.picco.tinj, 1, false);

    //Gas
    validate_add_data("MACage", driSR.physdata.ext3.aa2.mac_age_sum, 0.01, true);
    validate_add_data("EtBal",  driSR.physdata.ext3.bal.et, 0.01, true);

    // gas exchange
    validate_add_data("VO2", driSR.physdata.ext3.gassex.vo2, 1, true);
    validate_add_data("VCO2", driSR.physdata.ext3.gassex.vco2, 1, true);
    validate_add_data("EE", driSR.physdata.ext3.gassex.ee, 1, true);
    validate_add_data("RQ", driSR.physdata.ext3.gassex.rq, 1, true);

    //SPI
    validate_add_data("SPI", driSR.physdata.ext2.spi.spiVal, 1, true);

    //BIS
    validate_add_data("BIS", driSR.physdata.ext2.eeg_bis.bis, 1, true);
    validate_add_data("BIS_BSR", driSR.physdata.ext2.eeg_bis.sr_val, 1, true);
    validate_add_data("BIS_EMG", driSR.physdata.ext2.eeg_bis.emg_val, 1, true);
    validate_add_data("BIS_SQI", driSR.physdata.ext2.eeg_bis.sqi_val, 1, true);
}

std::string GE_Monitor::validate_wave_data(short value, double decimalshift, bool rounddata){
    double d_val = (double)(value)*decimalshift;
    if(rounddata)
        d_val = round(d_val);
    std::string str = std::to_string(d_val);
    if(value< DATA_INVALID_LIMIT)
        str = '-';
    return str;
}

void GE_Monitor::validate_add_data(std::string physio_id, short value, double decimalshift, bool rounddata)
{
    double dval = (double)(value)*decimalshift;
    if (rounddata) dval = round(dval);

    std::string valuestr =std::to_string(dval);;


    if (value < DATA_INVALID_LIMIT)
    {
        valuestr = "-";
    }

    struct NumericValResult NumVal;

    NumVal.Timestamp = machine_timestamp;
    NumVal.timestamp = std::time(nullptr);
    NumVal.PhysioID = physio_id;
    NumVal.Value = valuestr;
    NumVal.DeviceID = m_DeviceID;


    m_NumericValList.push_back(NumVal);
    m_NumValHeaders.push_back(NumVal.PhysioID);

}

void GE_Monitor::write_to_rows(){
    if (m_NumericValList.size() != 0)
    {
        std::time_t current_pc_time = std::time(nullptr);
        //std::string pkt_timestamp =std::asctime(std::localtime(&current_pc_time));
        //pkt_timestamp.erase(8,11);
        //pkt_timestamp.pop_back();
        if(m_transmissionstart){
            //filename_phdb = pathcsv + QString::fromStdString(std::to_string(current_pc_time)) + "_PHDB_data.csv";
            write_to_file_header(filename_phdb);
        }

        std::string row;
        row.append("\n");
        row.append(m_NumericValList[0].Timestamp);
        row.append(",");
        row.append(std::to_string(m_NumericValList[0].timestamp));
        row.append(",");
        bool changed=false;
        int elementcount=0;

        for(uint i=0;i<m_NumericValList.size();i++){
            if(current_pc_time > m_NumericValList[i].timestamp+timelapse
                    &&  m_NumericValList[i].timestamp == m_NumericValList[0].timestamp){
                elementcount+=1;
                changed=true;
                row.append(m_NumericValList[i].Value);
                row.append(",");
            }
        }

        if(changed){
            QFile myfile(filename_phdb);
            if (myfile.open(QIODevice::Append)) {
                myfile.write((char*)&row[0], row.length());
                qDebug()<<"write to phdb";
            }
            m_NumericValList.erase(m_NumericValList.begin(), m_NumericValList.begin()+elementcount);
        }

    }
}

void GE_Monitor::write_to_file_header(QString filename)
{
    if (m_NumericValList.size() != 0 && m_transmissionstart)
    {
        std::string headers;
        headers.append("Time");
        headers.append(",");
        headers.append("time");
        headers.append(",");

        for(uint i=0;i<m_NumValHeaders.size();i++){
            headers.append(m_NumValHeaders[i]);
            headers.append(",");
        }

        QFile myfile(filename);
        if (myfile.open(QIODevice::WriteOnly)) {
            myfile.write((char*)&headers[0], headers.length());
        }
        m_transmissionstart = false;
        m_NumValHeaders.clear();
    }
}

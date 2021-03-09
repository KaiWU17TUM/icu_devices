#include "ge_monitor.h"

GE_Monitor::GE_Monitor()
{
    local_serial_port = new MySerialPort();
    local_serial_port->serial->setPortName("/dev/ttyUSB2");
    local_serial_port->serial->setBaudRate(QSerialPort::Baud19200);
    local_serial_port->serial->setDataBits(QSerialPort::Data8);
    local_serial_port->serial->setParity(QSerialPort::EvenParity);
    local_serial_port->serial->setStopBits(QSerialPort::OneStop);
    local_serial_port->serial->setFlowControl(QSerialPort::HardwareControl);
    QObject::connect(local_serial_port->serial, SIGNAL(readyRead()), this, SLOT(process_buffer()));

}


void GE_Monitor::start(){
    try {
        std::cout<<"Try to open the serial port for GE Monitor"<<std::endl;
        try_to_open_port();

        std::cout<<"Reset transfer";
        request_wave_stop();

        std::cout<<"Try to get data from GE Monitor"<<std::endl;
        request_phdb_transfer();
        request_wave_transfer();


    }  catch (const std::exception& e) {
        qDebug()<<"Error opening/writing to serial port "<<e.what();
    }


}

void GE_Monitor::request_phdb_transfer(){
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
    pRequest->tx_ival = 10;
    pRequest->phdb_class_bf = DRI_PHDBCL_REQ_BASIC_MASK|DRI_PHDBCL_REQ_EXT1_MASK|DRI_PHDBCL_REQ_EXT2_MASK|DRI_PHDBCL_REQ_EXT3_MASK;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    tx_buffer(payload,length);
}

void GE_Monitor::request_wave_transfer(){
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
    pRequest->type[0] = DRI_WF_ECG1;
    pRequest->type[1] = DRI_EOL_SUBR_LIST;

    byte* payload = (byte*)&requestPkt;
    int length = sizeof(requestPkt);
    //return payload
    tx_buffer(payload,length);

}

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
    tx_buffer(payload,length);

}



void GE_Monitor::tx_buffer(byte* payload, int length){
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
            for(int i=0;i<b_list.size()-1;i++){
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

void GE_Monitor::read_packet_from_frame(){
    std::vector<struct datex::datex_record*> record_array;
    for(int i=0;i<frame_buffer.size();i++){
        struct datex::datex_record* ptr= (struct datex::datex_record*)(&frame_buffer[i][0]);
        record_array.push_back(ptr);
    }

    for(int i=0;i<frame_buffer.size();i++){
        struct datex::datex_record record = (*record_array[i]);

        // this is a PHDB record
        if(record.hdr.r_maintype == DRI_MT_PHDB){
            uint unixtime = record.hdr.r_time;
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
                save_basic_sub_record(phdata_ptr);
                save_ext1_and_ext2_record(phdata_ptr);
            }
            write_to_rows();
        }

        // this is a WAVE record
        else if(record.hdr.r_maintype == DRI_MT_WAVE){
             uint unixtime = record.hdr.r_time;

            // for each subrecord
            for(int j=0;j<8&&record.hdr.sr_desc[j].sr_type!=0xFF;j++){
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                int nextoffset = 0;
                int srsamplelenbytes[2];
                srsamplelenbytes[0] = record.rcrd.data[offset];
                srsamplelenbytes[1] = record.rcrd.data[offset+1];
                int srheaderlen = 6;
                int subrecordlen = 256*(int)srsamplelenbytes[1]+(int)srsamplelenbytes[0]+srheaderlen;
                nextoffset = offset + 2*subrecordlen;
                int buflen = (nextoffset - offset - 6);
                byte * buffer = (byte *)malloc(sizeof(byte)*buflen);
                for (int j = 0; j < buflen; j++)
                {
                    buffer[j] = record.rcrd.data[6 + j + offset];
                }
                std::vector<short> waveValList;
                for(int n = 0; n < buflen; n += 2){
                    waveValList.push_back((buffer[n+1])*256+(buffer[n]));
                }

                WaveValResult wave_val;
                wave_val.Timestamp = unixtime;
                //wave_val.DeviceID = m_DeviceID;
                std::string physioId = datex::WaveIdLabels.find(record.hdr.sr_desc[j].sr_type)->second;
                wave_val.PhysioID = physioId;
                wave_val.Unitshift = get_wave_unit_shift(wave_val.PhysioID);
                wave_val.Value = waveValList;
                m_WaveValList.push_back(wave_val);
            }

        }
        save_wave_to_csv();

    }
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

void GE_Monitor::save_wave_to_csv(){
    for(int i=0; i<m_WaveValList.size(); i++){
        std::string filename =  pathcsv+m_WaveValList[i].PhysioID+"DataExport.csv";
        double decimalshift = m_WaveValList[i].Unitshift;
        std::string row;
        for(int j=0;j<m_WaveValList[i].Value.size();j++){
            short value = m_WaveValList[i].Value[j];
            std::string wave_val = validate_wave_data(value, decimalshift, false);
            row.append( m_WaveValList[i].Timestamp);
            row.append(",");
            row.append(wave_val);
            row.append(",\n");
        }

        QFile myfile(QString::fromStdString(filename));
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&row[0], row.length());
            qDebug()<<"write to wave file";
        }
    }
    m_WaveValList.clear();
}



void GE_Monitor::save_basic_sub_record(datex::dri_phdb driSR){
    short so1 = driSR.physdata.basic.ecg.hr;
    short so2 = driSR.physdata.basic.nibp.sys;
    short so3 = driSR.physdata.basic.nibp.dia;
    short so4 = driSR.physdata.basic.nibp.mean;
    short so5 = driSR.physdata.basic.SpO2.SpO2;
    short so6 = driSR.physdata.basic.co2.et;

    validate_add_data("ECG_HR", so1,1,true);
    validate_add_data("NIBP_Systolic", so2, 0.01, true);
    validate_add_data("NIBP_Diastolic", so3, 0.01, true);
    validate_add_data("NIBP_Mean", so4, 0.01, true);
    validate_add_data("SpO2", so5, 0.01, true);
    short et = (so6 * driSR.physdata.basic.co2.amb_press);
    validate_add_data("ET_CO2", et, 0.00001,true);

    short so7 = driSR.physdata.basic.aa.et;
    short so8 = driSR.physdata.basic.aa.fi;
    short so9 = driSR.physdata.basic.aa.mac_sum;
    word so10 = driSR.physdata.basic.aa.hdr.label;

    validate_add_data("AA_ET", so7, 0.01, false);
    validate_add_data("AA_FI", so8, 0.01, false);
    validate_add_data("AA_MAC_SUM", so9, 0.01, false);

    double so11 = driSR.physdata.basic.o2.fi;
    double so12 = driSR.physdata.basic.n2o.fi;
    double so13 = driSR.physdata.basic.n2o.et;
    double so14 = driSR.physdata.basic.co2.rr;
    double so15 = driSR.physdata.basic.t1.temp;
    double so16 = driSR.physdata.basic.t2.temp;

    double so17 = driSR.physdata.basic.p1.hr;
    double so18 = driSR.physdata.basic.p1.sys;
    double so19 = driSR.physdata.basic.p1.dia;
    double so20 = driSR.physdata.basic.p1.mean;
    double so21 = driSR.physdata.basic.p2.hr;
    double so22 = driSR.physdata.basic.p2.sys;
    double so23 = driSR.physdata.basic.p2.dia;
    double so24 = driSR.physdata.basic.p2.mean;

    double so25 = driSR.physdata.basic.flow_vol.ppeak;
    double so26 = driSR.physdata.basic.flow_vol.pplat;
    double so27 = driSR.physdata.basic.flow_vol.tv_exp;
    double so28 = driSR.physdata.basic.flow_vol.tv_insp;
    double so29 = driSR.physdata.basic.flow_vol.peep;
    double so30 = driSR.physdata.basic.flow_vol.mv_exp;
    double so31 = driSR.physdata.basic.flow_vol.compliance;
    double so32 = driSR.physdata.basic.flow_vol.rr;

    validate_add_data("O2_FI", so11, 0.01, false);
    validate_add_data("N2O_FI", so12, 0.01, false);
    validate_add_data("N2O_ET", so13, 0.01, false);
    validate_add_data("CO2_RR", so14, 1, false);
    validate_add_data("T1_Temp", so15, 0.01, false);
    validate_add_data("T2_Temp", so16, 0.01, false);


    validate_add_data("P1_HR", so17, 1, true);
    validate_add_data("P1_Systolic", so18, 0.01, true);
    validate_add_data("P1_Disatolic", so19, 0.01, true);
    validate_add_data("P1_Mean", so20, 0.01, true);
    validate_add_data("P2_HR", so21, 1, true);
    validate_add_data("P2_Systolic", so22, 0.01, true);
    validate_add_data("P2_Diastolic", so23, 0.01, true);
    validate_add_data("P2_Mean", so24, 0.01, true);

    validate_add_data("PPeak", so25, 0.01, true);
    validate_add_data("PPlat", so26, 0.01, true);
    validate_add_data("TV_Exp", so27, 0.1, true);
    validate_add_data("TV_Insp", so28, 0.1, true);
    validate_add_data("PEEP", so29, 0.01, true);
    validate_add_data("MV_Exp", so30, 0.01, false);
    validate_add_data("Compliance", so31, 0.01, true);
    validate_add_data("RR", so32, 1, true);


}

void GE_Monitor::save_ext1_and_ext2_record(datex::dri_phdb driSR){
    short so1 = driSR.physdata.ext1.ecg12.stII;
    short so2 = driSR.physdata.ext1.ecg12.stV5;
    short so3 = driSR.physdata.ext1.ecg12.stAVL;

    validate_add_data("ST_II", so1, 0.01,false);
    validate_add_data("ST_V5", so2, 0.01, false);
    validate_add_data("ST_aVL", so3, 0.01, false);

    short so4 = driSR.physdata.ext2.ent.eeg_ent;
    short so5 = driSR.physdata.ext2.ent.emg_ent;
    short so6 = driSR.physdata.ext2.ent.bsr_ent;
    short so7 = driSR.physdata.ext2.eeg_bis.bis;
    short so8 = driSR.physdata.ext2.eeg_bis.sr_val;
    short so9 = driSR.physdata.ext2.eeg_bis.emg_val;
    short so10 = driSR.physdata.ext2.eeg_bis.sqi_val;

    validate_add_data("EEG_Entropy", so4,1,true);
    validate_add_data("EMG_Entropy", so5, 1, true);
    validate_add_data("BSR_Entropy", so6, 1, true);
    validate_add_data("BIS", so7, 1, true);
    validate_add_data("BIS_BSR", so8, 1, true);
    validate_add_data("BIS_EMG", so9, 1, true);
    validate_add_data("BIS_SQI", so10, 1, true);
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

    NumVal.Timestamp = m_strTimestamp;
    NumVal.PhysioID = physio_id;
    NumVal.Value = valuestr;
    NumVal.DeviceID = m_DeviceID;

    m_NumericValList.push_back(NumVal);
    m_NumValHeaders.push_back(NumVal.PhysioID);

}

void GE_Monitor::write_to_rows(){
    if (m_NumericValList.size() != 0)
    {
        write_to_file_header();
        std::string row;
        row.append("\n");
        row.append(m_NumericValList[0].Timestamp);
        row.append(",");

        for(int i=0;i<m_NumericValList.size();i++){
            row.append(m_NumericValList[i].Value);
            row.append(",");
        }

        QFile myfile(QString::fromStdString(pathcsv+"AS3DataExport.csv"));
        if (myfile.open(QIODevice::Append)) {
            myfile.write((char*)&row[0], row.length());
            qDebug()<<"write to phdb";
        }
        m_NumericValList.clear();
    }
}


void GE_Monitor::write_to_file_header()
{
    if (m_NumericValList.size() != 0 && m_transmissionstart)
    {
        std::string headers;
        headers.append("Time");
        headers.append(",");


        for(int i=0;i<m_NumValHeaders.size();i++){
            headers.append(m_NumValHeaders[i]);
            headers.append(",");
        }

        QFile myfile(QString::fromStdString(pathcsv+"AS3DataExport.csv"));
        if (myfile.open(QIODevice::WriteOnly)) {
            myfile.write((char*)&headers[0], headers.length());
        }
        m_transmissionstart = false;
        m_NumValHeaders.clear();
    }
}

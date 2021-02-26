#include "ge_monitor.h"

GE_Monitor::GE_Monitor()
{
    local_serial_port = new MySerialPort();
    local_serial_port->serial->setPortName("/dev/ttyACM0");
    local_serial_port->serial->setBaudRate(QSerialPort::Baud115200);
    local_serial_port->serial->setDataBits(QSerialPort::Data8);
    local_serial_port->serial->setParity(QSerialPort::NoParity);
    local_serial_port->serial->setStopBits(QSerialPort::OneStop);
    local_serial_port->serial->setFlowControl(QSerialPort::NoFlowControl);
    QObject::connect(local_serial_port->serial, SIGNAL(readyRead()), this, SLOT(process_buffer()));

}


void GE_Monitor::prepare_phdb_request(){
    struct datex::datex_record_req requestPkt;
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
            printf("calculated checksum is %02x", checksum);
            printf("received checksum is %02x", b_list[b_list.size()-1]);

            if(checksum == b_list[b_list.size()-1]){
                printf("correct");
                frame_buffer.push_back(b_list);
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

void GE_Monitor::create_record_list(){
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
            }
        }

        // this is a WAVE record
        else if(record.hdr.r_maintype == DRI_MT_WAVE){
            uint unixtime = record.hdr.r_time;
            struct datex::dri_phdb phdata_ptr;

            for(int j=0;j<8&&record.hdr.sr_desc[j].sr_type!=0xFF;j++){
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                int nextoffset = 0;
                int srsamplelenbytes[2];
                srsamplelenbytes[0] = record.rcrd.data[offset];
                srsamplelenbytes[1] = record.rcrd.data[offset+1];
                int srheaderlen = 6;
                int subrecordlen = 16*(int)srsamplelenbytes[1]+(int)srsamplelenbytes[0]+srheaderlen;
                nextoffset = offset + subrecordlen;
                int buflen = (nextoffset - offset - 6);
                byte * buffer = (byte *)malloc(sizeof(byte)*buflen);
                for (int j = 0; j < buflen; j++)
                {
                    buffer[j] = record.rcrd.data[6 + j + offset];
                }

            }

        }

    }
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
    write_to_rows();

}


void GE_Monitor::validate_add_data(QString physio_id, short value, double decimalshift, bool rounddata)
{
    int val = (int)(value);
    //double dval = (Convert.ToDouble(value, CultureInfo.InvariantCulture))*decimalshift;
    //if (rounddata) dval = Math.Round(dval);

    QString valuestr = QString::number(val);;


    if (val < DATA_INVALID_LIMIT)
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
        QString row;
        row.append('\n');
        row.append(m_NumericValList[0].Timestamp);
        row.append(',');

        for(int i=0;i<m_NumericValList.size();i++){
            row.append(m_NumericValList[i].Value);
            row.append(",");
        }

        QFile myfile(pathcsv);
        if (myfile.open(QIODevice::Append)) {
            QTextStream out(&myfile);
            out << row.toUtf8();
        }

    }
}


void GE_Monitor::write_to_file_header()
{
    if (m_NumericValList.size() != 0 && m_transmissionstart)
    {
        QString headers;
        headers.append("Time");
        headers.append(",");


        for(int i=0;i<m_NumValHeaders.size();i++){
            headers.append(m_NumValHeaders[i]);
            headers.append(",");
        }

        QFile myfile(pathcsv);
        if (myfile.open(QIODevice::WriteOnly)) {
            QTextStream out(&myfile);
            out << headers.toUtf8();
        }
        m_transmissionstart = false;

    }
}

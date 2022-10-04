#include "datex_ohmeda.h"
#include "device.h"
#include <QFile>
#include <QTextStream>
Datex_ohmeda::Datex_ohmeda(std::string config_file, Device *device) : Protocol(config_file, device)
{
    load_protocol_config(config_file);
}

/**
 * @brief Datex_ohmeda::load_protocol_config: load protocol settings from config file
 * @param config_file
 */
void Datex_ohmeda::load_protocol_config(std::string config_file)
{
    QFile file(QString::fromStdString(config_file));

    // std::ifstream cfg_file(config_file);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        std::string line;
        QString Line;
        do
        {
            Line = in.readLine();
            line = Line.toStdString();
            if (line[0] == '#' || line.empty())
                continue;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            if (name == "phdb_time_interval")
            {
                phdb_data_interval = std::stoi(value);
            }

            else if (name == "wave_id")
            {
                auto delimiterPos1 = value.find("[");
                auto delimiterPos2 = value.find("]");
                value = value.substr(delimiterPos1 + 1, delimiterPos2 - delimiterPos1 - 1);
                while (auto delimiterPos = value.find(","))
                {
                    if (delimiterPos == std::string::npos)
                    {
                        auto id = std::stoi(value.substr(0, delimiterPos));
                        wave_ids.push_back(byte(id & 0x0F));
                        break;
                    }
                    auto id = std::stoi(value.substr(0, delimiterPos));
                    wave_ids.push_back(byte(id & 0x0F));
                    value = value.substr(delimiterPos + 1);
                }
            }
        } while (!Line.isNull());
    }

    // prepare files
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    filename_phdb = device->get_logger()->save_dir + std::to_string(pc_timestamp_ms) + "_PHDB_data.csv";
    filename_alarm = device->get_logger()->save_dir + std::to_string(pc_timestamp_ms) + "_Alarm.csv";
    for (auto i = 0; i < wave_ids.size(); i++)
    {
        std::string physioId = datex::WaveIdLabels.find(wave_ids[i])->second;
        std::string filename = std::to_string(pc_timestamp_ms) + "_" + physioId + ".csv";
        filenames_wave[physioId] = device->get_logger()->save_dir + filename;
    }
}

void Datex_ohmeda::write_buffer(byte *payload, int length)
{
    byte checksum = 0;
    std::vector<byte> temptxbuff;
    temptxbuff.push_back(0x7e);
    for (int i = 0; i < length; i++)
    {
        switch (payload[i])
        {
        case (0x7e):
            temptxbuff.push_back(0x7d);
            temptxbuff.push_back(0x5e);
            checksum += 0x7d;
            checksum += 0x5e;
            break;

        case (0x7d):
            temptxbuff.push_back(0x7d);
            temptxbuff.push_back(0x5d);
            checksum += 0x7d;
            checksum += 0x5d;
            break;

        default:
            temptxbuff.push_back(payload[i]);
            checksum += payload[i];
            break;
        }
    }

    switch (checksum)
    {
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
    device->write_buffer((const char *)&temptxbuff[0], temptxbuff.size());
}

void Datex_ohmeda::from_literal_to_packet(byte b)
{
    // if get a byte which indicates the start of a msg
    if (b == 0x7e && m_fstart)
    {
        m_fstart = false;
        m_storestart = true;
    }

    // encounter the end
    else if (b == 0x7e && m_fstart == false)
    {
        m_fstart = true;
        m_storestart = false;
        m_storeend = true;
    }

    if (m_storestart == true)
    {
        // encounter control byte
        if (b == 0x7d)
            m_bitschiftnext = true;
        // normal byte
        else
        {
            // the byte before is a control byte
            if (m_bitschiftnext == true)
            {
                m_bitschiftnext = false;
                b |= 0x7c;
                b_list.push_back(b);
            }
            else if (b != 0x7e)
            {
                b_list.push_back(b);
            }
        }
    }

    // end of msg
    else if (m_storeend)
    {
        if (b_list.size() != 0)
        {
            byte checksum = 0x00;
            for (uint i = 0; i < b_list.size() - 1; i++)
            {
                checksum += b_list[i];
            }
            if (checksum == b_list[b_list.size() - 1])
            {
                frame_buffer.push_back(b_list);
                std::cout << "GE Monitor Checksum correct" << std::endl;
                b_list.clear();
            }
            else
            {
                std::cout << "GE Monitor Checksum wrong" << std::endl;
                b_list.clear();
            }
            m_storeend = false;
        }
        else
        {
            m_storestart = true;
            m_storeend = false;
            m_fstart = false;
        }
    }
}

/**
 * @brief request_phdb_transfer: send request to get phdb data
 * @param interval
 * @param p
 */
void request_phdb_transfer(int interval, Datex_ohmeda *p)
{
    struct datex::datex_record_phdb_req requestPkt;
    struct datex::dri_phdb_req *pRequest;

    // Clear the pkt
    memset(&requestPkt, 0x00, sizeof(requestPkt));

    // Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr) + sizeof(struct datex::dri_phdb_req);
    requestPkt.hdr.r_maintype = DRI_MT_PHDB;
    requestPkt.hdr.dri_level = 0;

    // The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short)DRI_EOL_SUBR_LIST;

    // Fill the request
    pRequest = (struct datex::dri_phdb_req *)&(requestPkt.phdbr);
    pRequest->phdb_rcrd_type = DRI_PH_DISPL;
    pRequest->tx_ival = interval;
    pRequest->phdb_class_bf = DRI_PHDBCL_REQ_BASIC_MASK | DRI_PHDBCL_REQ_EXT1_MASK | DRI_PHDBCL_REQ_EXT2_MASK | DRI_PHDBCL_REQ_EXT3_MASK;

    byte *payload = (byte *)&requestPkt;
    int length = sizeof(requestPkt);
    // return payload
    p->write_buffer(payload, length);
}

/**
 * @brief request_alarm_transfer: send request to get alarm data
 * @param p
 */
void request_alarm_transfer(Datex_ohmeda *p)
{
    struct datex::datex_record_alarm_req requestPkt;
    struct datex::al_tx_cmd *pRequest;

    // Clear the pkt
    memset(&requestPkt, 0x00, sizeof(requestPkt));

    // Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr) + sizeof(struct datex::al_tx_cmd);
    requestPkt.hdr.r_maintype = DRI_MT_ALARM;
    requestPkt.hdr.dri_level = 0;

    // The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short)DRI_EOL_SUBR_LIST;

    // Fill the request
    pRequest = (struct datex::al_tx_cmd *)&(requestPkt.alarm_cmd);
    pRequest->cmd = DRI_AL_ENTER_DIFFMODE;

    byte *payload = (byte *)&requestPkt;
    int length = sizeof(requestPkt);
    // return payload
    p->write_buffer(payload, length);
}

/**
 * @brief request_wave_transfer: send request to get wave form data
 * @param wave_id
 * @param p
 */
void request_wave_transfer(std::vector<byte> wave_id, Datex_ohmeda *p)
{
    // Test if samples > limitation
    int sum = 0;
    for (uint i = 0; i < wave_id.size(); i++)
    {
        sum += datex::WaveIdFreqs.find(wave_id[i])->second;
    }
    if (sum > datex::max_wave_samples_limitation)
    {
        // qDebug()<<"Samples per second exceeds maximum, this request will not be sent";
        return;
    }
    struct datex::datex_record_wave_req requestPkt;
    struct datex::dri_wave_req *pRequest;

    // Clear the pkt
    memset(&requestPkt, 0x00, sizeof(requestPkt));

    // Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr) + sizeof(struct datex::dri_wave_req);
    requestPkt.hdr.r_maintype = DRI_MT_WAVE;
    requestPkt.hdr.dri_level = 0;

    // The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short)DRI_EOL_SUBR_LIST;

    // Fill the request
    pRequest = (struct datex::dri_wave_req *)&(requestPkt.wfreq);
    pRequest->req_type = WF_REQ_CONT_START;
    uint i = 0;
    for (i = 0; i < wave_id.size(); i++)
    {
        pRequest->type[i] = wave_id[i];
    }

    pRequest->type[i] = DRI_EOL_SUBR_LIST;

    byte *payload = (byte *)&requestPkt;
    int length = sizeof(requestPkt);
    // return payload
    p->write_buffer(payload, length);
}

/**
 * @brief request_wave_stop: send request and ask the device to stop sending wave data
 * @param p
 */
void request_wave_stop(Datex_ohmeda *p)
{
    struct datex::datex_record_wave_req requestPkt;
    struct datex::dri_wave_req *pRequest;

    // Clear the pkt
    memset(&requestPkt, 0x00, sizeof(requestPkt));

    // Fill the header
    requestPkt.hdr.r_len = sizeof(struct datex::datex_hdr) + sizeof(struct datex::dri_wave_req);
    requestPkt.hdr.r_maintype = DRI_MT_WAVE;
    requestPkt.hdr.dri_level = 0;

    // The pkt contains one subrecord
    requestPkt.hdr.sr_desc[0].sr_type = 0;
    requestPkt.hdr.sr_desc[0].sr_offset = (byte)0;
    requestPkt.hdr.sr_desc[1].sr_type = (short)DRI_EOL_SUBR_LIST;

    // Fill the request
    pRequest = (struct datex::dri_wave_req *)&(requestPkt.wfreq);
    pRequest->req_type = WF_REQ_CONT_STOP;
    pRequest->type[0] = DRI_EOL_SUBR_LIST;

    byte *payload = (byte *)&requestPkt;
    int length = sizeof(requestPkt);
    // return payload
    p->write_buffer(payload, length);
}

void Datex_ohmeda::send_request()
{
    request_wave_stop(this);
    request_phdb_transfer(phdb_data_interval, this);
    request_wave_transfer(wave_ids, this);
    request_alarm_transfer(this);
}

/**
 * @brief get_wave_unit_shift: get unit of wave data
 * @param physioId
 * @return
 */
double get_wave_unit_shift(std::string physioId)
{
    double decimalshift = 1;
    if (physioId.find("ECG") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("INVP") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("PLETH") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("CO2") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("O2") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("RESP") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("AA") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("FLOW") != std::string::npos)
        return (decimalshift = 0.01);
    else if (physioId.find("AWP") != std::string::npos)
        return (decimalshift = 0.1);
    else if (physioId.find("VOL") != std::string::npos)
        return (decimalshift = -1);
    else if (physioId.find("EEG") != std::string::npos)
        return (decimalshift = 0.1);
    else
        return decimalshift;
}

/**
 * @brief validate_wave_data:  validate the number
 * @param value
 * @param decimalshift
 * @param rounddata
 * @return
 */
std::string validate_wave_data(short value, double decimalshift, bool rounddata)
{
    double d_val = (double)(value)*decimalshift;
    if (rounddata)
        d_val = round(d_val);
    std::string str = std::to_string(d_val);
    if (value < DATA_INVALID_LIMIT)
        str = '-';
    return str;
}

/**
 * @brief Datex_ohmeda::validate_add_data: validate data and save the data into vector
 * @param physio_id
 * @param value
 * @param decimalshift
 * @param rounddata
 */
void Datex_ohmeda::validate_add_data(std::string physio_id, short value,
                                     double decimalshift, bool rounddata)
{
    double dval = (double)(value)*decimalshift;
    if (rounddata)
        dval = round(dval);
    std::string valuestr = std::to_string(dval);
    if (value < DATA_INVALID_LIMIT)
        valuestr = "-";

    struct NumericValueDatex NumericValueDraeger;
    NumericValueDraeger.datetime = machine_datetime;
    NumericValueDraeger.timestamp_ms = machine_timestamp;
    NumericValueDraeger.physioid = physio_id;
    NumericValueDraeger.value = valuestr;

    m_NumericValueList.push_back(NumericValueDraeger);
    m_NumValHeaders.push_back(NumericValueDraeger.physioid);
}

void Datex_ohmeda::save_basic_sub_record(datex::dri_phdb driSR)
{
    // ECG
    validate_add_data("ECG_HR", driSR.physdata.basic.ecg.hr, 1, true);
    validate_add_data("ST1", driSR.physdata.basic.ecg.st1, 0.01, true);
    validate_add_data("ST2", driSR.physdata.basic.ecg.st2, 0.01, true);
    validate_add_data("ST3", driSR.physdata.basic.ecg.st3, 0.01, true);
    validate_add_data("HR_max", driSR.physdata.basic.ecg_extra.hr_max, 1, true);
    validate_add_data("HR_min", driSR.physdata.basic.ecg_extra.hr_min, 1, true);

    // Respiration(Impedance)
    validate_add_data("RES_imp", driSR.physdata.basic.ecg.imp_rr, 1, true);

    // Invasive pressure
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

    // NIBP
    validate_add_data("NIBP_Mean", driSR.physdata.basic.nibp.hr, 1, true);
    validate_add_data("NIBP_Systolic", driSR.physdata.basic.nibp.sys, 0.01, true);
    validate_add_data("NIBP_Diastolic", driSR.physdata.basic.nibp.dia, 0.01, true);
    validate_add_data("NIBP_Mean", driSR.physdata.basic.nibp.mean, 0.01, true);

    // SpO2
    validate_add_data("SpO2_POS", driSR.physdata.basic.SpO2.SpO2, 0.01, true);
    validate_add_data("SpO2_PR", driSR.physdata.basic.SpO2.pr, 1, true);

    // SvO2
    validate_add_data("SvO2", driSR.physdata.basic.svo2.svo2, 1, true);

    // Temperature
    validate_add_data("T1_Temp", driSR.physdata.basic.t1.temp, 0.01, false);
    validate_add_data("T2_Temp", driSR.physdata.basic.t2.temp, 0.01, false);
    validate_add_data("T3_Temp", driSR.physdata.basic.t3.temp, 0.01, false);
    validate_add_data("T4_Temp", driSR.physdata.basic.t4.temp, 0.01, false);
    validate_add_data("TBlood", driSR.physdata.basic.co_wedge.blood_temp, 0.01, false);

    // Cardiac Output
    validate_add_data("CO", driSR.physdata.basic.co_wedge.co, 1, false);
    validate_add_data("REF", driSR.physdata.basic.co_wedge.ref, 0.01, false);

    // Gas
    validate_add_data("AA_ET", driSR.physdata.basic.aa.et, 0.01, false);
    validate_add_data("AA_FI", driSR.physdata.basic.aa.fi, 0.01, false);
    validate_add_data("AA_MAC_SUM", driSR.physdata.basic.aa.mac_sum, 0.01, false);
    validate_add_data("O2_FI", driSR.physdata.basic.o2.fi, 0.01, false);
    validate_add_data("O2_ET", driSR.physdata.basic.o2.et, 0.01, false);
    validate_add_data("N2O_FI", driSR.physdata.basic.n2o.fi, 0.01, false);
    validate_add_data("N2O_ET", driSR.physdata.basic.n2o.et, 0.01, false);
    validate_add_data("CO2_RR", driSR.physdata.basic.co2.rr, 1, false);
    validate_add_data("CO2_FI", driSR.physdata.basic.co2.fi, 0.01, false);
    validate_add_data("CO2_ET", driSR.physdata.basic.co2.et, 0.01, false);
    validate_add_data("AT_pre", driSR.physdata.basic.co2.amb_press, 0.1, true);
}

void Datex_ohmeda::save_ext1_and_ext2_and_ext3_record(datex::dri_phdb driSR)
{
    // ECG
    validate_add_data("PVC", driSR.physdata.ext1.arrh_ecg.pvc, 1, false);
    validate_add_data("ST_I", driSR.physdata.ext1.ecg12.stI, 0.01, false);
    validate_add_data("ST_II", driSR.physdata.ext1.ecg12.stII, 0.01, false);
    validate_add_data("ST_III", driSR.physdata.ext1.ecg12.stIII, 0.01, false);
    validate_add_data("ST_aVL", driSR.physdata.ext1.ecg12.stAVL, 0.01, false);
    validate_add_data("ST_aVR", driSR.physdata.ext1.ecg12.stAVR, 0.01, false);
    validate_add_data("ST_aVF", driSR.physdata.ext1.ecg12.stAVF, 0.01, false);
    validate_add_data("ST_V1", driSR.physdata.ext1.ecg12.stV1, 0.01, false);
    validate_add_data("ST_V2", driSR.physdata.ext1.ecg12.stV2, 0.01, false);
    validate_add_data("ST_V3", driSR.physdata.ext1.ecg12.stV3, 0.01, false);
    validate_add_data("ST_V4", driSR.physdata.ext1.ecg12.stV4, 0.01, false);
    validate_add_data("ST_V5", driSR.physdata.ext1.ecg12.stV5, 0.01, false);
    validate_add_data("ST_V6", driSR.physdata.ext1.ecg12.stV6, 0.01, false);

    // Invasive blood pressure
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

    // SpO2
    validate_add_data("SpO2_PR2", driSR.physdata.ext1.SpO2_ch2.pr, 1, false);
    validate_add_data("SpO2_POS2", driSR.physdata.ext1.SpO2_ch2.SpO2, 0.01, false);

    // Cardiac Output
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

    // Gas
    validate_add_data("MACage", driSR.physdata.ext3.aa2.mac_age_sum, 0.01, true);
    validate_add_data("EtBal", driSR.physdata.ext3.bal.et, 0.01, true);

    // gas exchange
    validate_add_data("VO2", driSR.physdata.ext3.gassex.vo2, 1, true);
    validate_add_data("VCO2", driSR.physdata.ext3.gassex.vco2, 1, true);
    validate_add_data("EE", driSR.physdata.ext3.gassex.ee, 1, true);
    validate_add_data("RQ", driSR.physdata.ext3.gassex.rq, 1, true);

    // SPI
    validate_add_data("SPI", driSR.physdata.ext2.spi.spiVal, 1, true);

    // BIS
    validate_add_data("BIS", driSR.physdata.ext2.eeg_bis.bis, 1, true);
    validate_add_data("BIS_BSR", driSR.physdata.ext2.eeg_bis.sr_val, 1, true);
    validate_add_data("BIS_EMG", driSR.physdata.ext2.eeg_bis.emg_val, 1, true);
    validate_add_data("BIS_SQI", driSR.physdata.ext2.eeg_bis.sqi_val, 1, true);
}

void Datex_ohmeda::from_packet_to_structures()
{
    std::vector<struct datex::datex_record *> record_array;
    for (uint i = 0; i < frame_buffer.size(); i++)
    {
        struct datex::datex_record *ptr = (struct datex::datex_record *)(&frame_buffer[i][0]);
        record_array.push_back(ptr);
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string pc_datetime = std::ctime(&t);
    pc_datetime.erase(pc_datetime.end() - 1);
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch())
            .count();

    for (uint i = 0; i < frame_buffer.size(); i++)
    {
        struct datex::datex_record record = (*record_array[i]);

        // this is a PHDB record
        if (record.hdr.r_maintype == DRI_MT_PHDB)
        {
            struct datex::dri_phdb phdata_ptr;

            for (int j = 0; j < 8 && record.hdr.sr_desc[j].sr_type != 0xFF; j++)
            {
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                byte buffer[270];
                for (int n = 0; n < 270; n++)
                {
                    buffer[n] = record.rcrd.data[4 + offset + n];
                }
                switch (j)
                {
                case 0:
                    (phdata_ptr.physdata.basic) = *(struct datex::basic_phdb *)buffer;
                    break;
                case 1:
                    (phdata_ptr.physdata.ext1) = *(struct datex::ext1_phdb *)buffer;
                    break;
                case 2:
                    (phdata_ptr.physdata.ext2) = *(struct datex::ext2_phdb *)buffer;
                    break;
                case 3:
                    (phdata_ptr.physdata.ext3) = *(struct datex::ext3_phdb *)buffer;
                    break;
                }

                machine_datetime = pc_datetime;
                machine_timestamp = pc_timestamp_ms;
                save_basic_sub_record(phdata_ptr);
                save_ext1_and_ext2_and_ext3_record(phdata_ptr);
            }
        }

        // this is a WAVE record
        else if (record.hdr.r_maintype == DRI_MT_WAVE)
        {
            for (int j = 0; j < 8 && record.hdr.sr_desc[j].sr_type != 0xFF; j++)
            {
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                int srsamplelenbytes[2];
                srsamplelenbytes[0] = record.rcrd.data[offset];
                srsamplelenbytes[1] = record.rcrd.data[offset + 1];

                int sub_header_len = 6;
                int subrecordlen = 256 * (int)srsamplelenbytes[1] + (int)srsamplelenbytes[0];
                int buflen = 2 * subrecordlen; //(nextoffset - offset - 6);
                byte *buffer = (byte *)malloc(sizeof(byte) * buflen);
                for (int j = 0; j < buflen; j++)
                {
                    buffer[j] = record.rcrd.data[sub_header_len + j + offset];
                }

                std::vector<short> value_list;
                std::vector<unsigned long int> timestamp_ms_list;
                int samples = datex::WaveIdFreqs.find(record.hdr.sr_desc[j].sr_type)->second;
                for (int n = 0; n < buflen; n += 2)
                {
                    value_list.push_back((buffer[n + 1]) * 256 + (buffer[n]));
                    timestamp_ms_list.push_back(pc_timestamp_ms + 1000 * (n / 2) / samples);
                }

                WaveValueDatex wave_val;
                wave_val.datetime = pc_datetime;
                wave_val.timestamp_ms = pc_timestamp_ms;
                wave_val.timestamp_ms_list = timestamp_ms_list;
                wave_val.physioid = datex::WaveIdLabels.find(record.hdr.sr_desc[j].sr_type)->second;
                wave_val.unitshift = get_wave_unit_shift(wave_val.physioid);
                wave_val.value_list = value_list;
                m_WaveValueList.push_back(wave_val);
            }
        }

        else if (record.hdr.r_maintype == DRI_MT_ALARM)
        {
            for (int j = 0; j < 8 && record.hdr.sr_desc[j].sr_type != 0xFF; j++)
            {
                int offset = (int)record.hdr.sr_desc[j].sr_offset;
                byte buffer[270];
                for (int n = 0; n < 270; n++)
                {
                    buffer[n] = record.rcrd.data[offset + n];
                }
                struct datex::dri_al_msg dri_al_msg_ptr;
                dri_al_msg_ptr = *(struct datex::dri_al_msg *)buffer;
                AlarmDatex alarm[5];
                for (int n = 0; n < 5; n++)
                {
                    alarm[n].datetime = pc_datetime;
                    alarm[n].timestamp_ms = pc_timestamp_ms;
                    alarm[n].text = std::string(dri_al_msg_ptr.al_disp[n].text);
                    for (uint m = 0; m < alarm[n].text.length(); m++)
                    {
                        if (alarm[n].text[m] == '\n')
                        {
                            alarm[n].text[m] = ' ';
                            break;
                        }
                    }
                    switch (dri_al_msg_ptr.al_disp[n].color)
                    {
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
                    if (dri_al_msg_ptr.al_disp[n].text_changed == 1)
                    {
                        m_AlarmList.push_back(alarm[n]);
                    }
                }
            }
        }
    }
}

/*************************************************************/
// functions for saving data

void Datex_ohmeda::save_data()
{
    save_numeric_to_csv(); // phdb
    save_alarm_to_csv();
    save_wave_to_csv();
}

void Datex_ohmeda::save_alarm_to_csv()
{
    std::time_t timelapse = device->get_logger()->time_delay;
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    for (uint i = 0; i < m_AlarmList.size(); i++)
    {
        bool changed = false;
        int elementcount = 0;
        std::string row;

        if ((m_AlarmList[i].text.length()) > 0 &&
            pc_timestamp_ms > m_AlarmList[i].timestamp_ms + timelapse &&
            m_AlarmList[i].timestamp_ms == m_AlarmList[0].timestamp_ms)
        {
            changed = true;
            elementcount += 1;
            row.append(m_AlarmList[i].datetime);
            row.append(",");
            row.append(std::to_string(m_AlarmList[i].timestamp_ms));
            row.append(",");
            row.append(m_AlarmList[i].text);
            row.append(",");
            row.append(m_AlarmList[i].color);
            row.append(",\n");
        }

        if (changed)
        {
            device->get_logger()->saving_to_file(filename_alarm, row);
            m_AlarmList.erase(m_AlarmList.begin(), m_AlarmList.begin() + elementcount);
        }
    }
}

void Datex_ohmeda::save_wave_to_csv()
{
    std::time_t timelapse = device->get_logger()->time_delay;
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    int empty_list_count = 0;

    for (uint i = 0; i < m_WaveValueList.size(); i++)
    {
        std::string filename = filenames_wave[m_WaveValueList[i].physioid];
        double decimalshift = m_WaveValueList[i].unitshift;
        bool changed = false;
        int elementcount = 0;
        std::string row;

        for (uint j = 0; j < m_WaveValueList[i].value_list.size(); j++)
        {
            if (pc_timestamp_ms > m_WaveValueList[i].timestamp_ms_list[j] + timelapse)
            {
                changed = true;
                elementcount += 1;
                std::string wave_val = validate_wave_data(m_WaveValueList[i].value_list[j], decimalshift, false);
                row.append(m_WaveValueList[i].datetime);
                row.append(",");
                row.append(wave_val);
                row.append(",");
                row.append(std::to_string(m_WaveValueList[i].timestamp_ms_list[j]));
                row.append(",\n");
            }
        }

        if (changed)
        {
            device->get_logger()->saving_to_file(filename, row);
            m_WaveValueList[i].value_list.erase(
                m_WaveValueList[i].value_list.begin(),
                m_WaveValueList[i].value_list.begin() + elementcount);
            m_WaveValueList[i].timestamp_ms_list.erase(
                m_WaveValueList[i].timestamp_ms_list.begin(),
                m_WaveValueList[i].timestamp_ms_list.begin() + elementcount);
        }

        if (m_WaveValueList[i].timestamp_ms_list.size() == 0)
        {
            empty_list_count += 1;
        }
    }

    if (empty_list_count > 0)
    {
        m_WaveValueList.erase(m_WaveValueList.begin(), m_WaveValueList.begin() + empty_list_count);
    }
}

void Datex_ohmeda::save_numeric_to_csv()
{
    if (m_NumericValueList.size() == 0)
        return;

    std::time_t timelapse = device->get_logger()->time_delay;
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    // Write header
    write_header(filename_phdb);

    // Write content
    bool changed = false;
    int elementcount = 0;
    std::string row;
    row.append(m_NumericValueList[0].datetime);
    row.append(",");
    row.append(std::to_string(m_NumericValueList[0].timestamp_ms));
    row.append(",");

    for (uint i = 0; i < m_NumericValueList.size(); i++)
    {
        if (m_NumericValueList[i].timestamp_ms == m_NumericValueList[0].timestamp_ms &&
            pc_timestamp_ms > (m_NumericValueList[i].timestamp_ms + timelapse))
        {
            elementcount += 1;
            changed = true;
            row.append(m_NumericValueList[i].value);
            row.append(",");
        }
    }
    row.append("\n");

    if (changed)
    {
        device->get_logger()->saving_to_file(filename_phdb, row);
        m_NumericValueList.erase(m_NumericValueList.begin(), m_NumericValueList.begin() + elementcount);
    }
}

void Datex_ohmeda::write_header(std::string filename)
{
    if (m_NumericValueList.size() != 0 && m_transmissionstart)
    {
        std::string header;
        header.append("DateTime");
        header.append(",");
        header.append("Timestamp_ms");
        header.append(",");
        for (uint i = 0; i < m_NumValHeaders.size(); i++)
        {
            header.append(m_NumValHeaders[i]);
            header.append(",");
        }
        header.append("\n");
        device->get_logger()->saving_to_file(filename, header);
        m_transmissionstart = false;
        m_NumValHeaders.clear();
    }
}

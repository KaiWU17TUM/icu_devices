#include "medibus.h"
#include "device.h"

// helper function
void add_checksum(std::vector<byte> &payload)
{
    int sum = 0;
    for (unsigned long i = 0; i < payload.size(); ++i)
    {
        sum += payload[i];
    }

    // get 8-bit sum total checksum
    byte checksum = (byte)(sum & 0xFF);
    std::vector<byte> checksum_array;
    checksum_array.push_back(checksum / 16);
    checksum_array.push_back(checksum & 0x0F);
    if (checksum_array[0] < 10)
        payload.push_back(checksum_array[0] + 0x30);
    else
        payload.push_back(checksum_array[0] - 10 + 0x41);

    if (checksum_array[1] < 10)
        payload.push_back(checksum_array[1] + 0x30);
    else
        payload.push_back(checksum_array[1] - 10 + 0x41);
}

Medibus::Medibus(std::string config_file, Device *device) : Protocol(config_file, device)
{
    load_protocol_config(config_file);
}

/**
 * @brief Medibus::load_protocol_config: load protocol specific settings from config file
 * @param config_file
 */
void Medibus::load_protocol_config(std::string config_file)
{
    QFile file(QString::fromStdString(config_file));
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        std::string line;
        QString Line;
        do
        {
            if (line[0] == '#' || line.empty())
                continue;
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            if (name == "measurement_time_interval")
            {
                measurement_time_interval = std::stoi(value);
            }
            else if (name == "alarm1_time_interval")
            {
                alarm1_time_interval = std::stoi(value);
            }
            else if (name == "alarm2_time_interval")
            {
                alarm2_time_interval = std::stoi(value);
            }
            else if (name == "alarm_low_limit_time_interval")
            {
                alarm_low_limit_time_interval = std::stoi(value);
            }
            else if (name == "alarm_high_limit_time_interval")
            {
                alarm_high_limit_time_interval = std::stoi(value);
            }
        } while (!Line.isNull());
    }
    // prepare filenames
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    filename_measurement = device->get_logger()->save_dir + (std::to_string(pc_timestamp_ms)) + "_measurement.csv";
    filename_low_limit = device->get_logger()->save_dir + (std::to_string(pc_timestamp_ms)) + "_low_limit.csv";
    filename_high_limit = device->get_logger()->save_dir + (std::to_string(pc_timestamp_ms)) + "_high_limit.csv";
    filename_alarm = device->get_logger()->save_dir + (std::to_string(pc_timestamp_ms)) + "_alarm.csv";

    alarm_low_limit_timer = new QTimer();
    QObject::connect(alarm_low_limit_timer, SIGNAL(timeout()), this, SLOT(request_alarm_low_limit()));

    alarm_high_limit_timer = new QTimer();
    QObject::connect(alarm_high_limit_timer, SIGNAL(timeout()), this, SLOT(request_alarm_high_limit()));

    alarm1_timer = new QTimer();
    QObject::connect(alarm1_timer, SIGNAL(timeout()), this, SLOT(request_alarmCP1()));

    alarm2_timer = new QTimer();
    //    QObject::connect(alarm2_timer, SIGNAL(timeout()), this, SLOT(request_alarmCP2()));

    measurement_timer = new QTimer();
    QObject::connect(measurement_timer, SIGNAL(timeout()), this, SLOT(request_measurement_cp1()));
}

void Medibus::from_literal_to_packet(byte b)
{
    if (sync_data == true)
    {
        if ((b & 0xf0) == 0xd0)
            new_data = true;
        else
            new_data = false;
        if (b_list.size() > 0 && new_data == true)
        {
            std::vector<byte> payload;
            for (unsigned long i = 0; i < b_list.size(); i++)
            {
                payload.push_back(b_list[i]);
            }
            frame_buffer.push_back(payload);
            b_list.clear();
        }
        b_list.push_back(b);
    }
    else
    {
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
                for (int i = 0; i < framelen - 2; i++)
                {
                    payload.push_back(b_list[i]);
                }
                add_checksum(payload);
                if (payload[framelen - 3] == b_list[framelen - 3] and payload[framelen - 2] == b_list[framelen - 2])
                {
                    qDebug() << "Checksum Correct!";
                    frame_buffer.push_back(payload);
                }
                else
                {
                    qDebug() << "Checksum Error!";
                }
                b_list.clear();
                m_storeend = false;
            }
        }
    }
}

void Medibus::from_packet_to_structures()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string pc_datetime = std::ctime(&t);
    pc_datetime.erase(pc_datetime.end() - 1);
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch())
            .count();

    machine_datetime = pc_datetime;
    machine_timestamp = pc_timestamp_ms;

    for (unsigned long i = 0; i < frame_buffer.size(); i++)
    {
        byte response_type = frame_buffer[i][0];
        byte command_type = frame_buffer[i][1];
        std::vector<byte> cmd;
        switch (response_type)
        {
        case 0x1b: // command received
            if (command_type == 'Q')
            { // ICC
                cmd = {0x51};
                qDebug() << "get ICC command";
                command_echo_response(cmd);
                // request_realtime_config();
                break;
            }
            else if (command_type == 'R')
            { // Device id
                cmd = {0x52};
                qDebug() << "get Device id command";
                command_echo_response(cmd); // return empty device identification
                break;
            }
            else
            {
                break;
            }

        case 0x01: // response received
            if (command_type == 'Q')
            { // ICC-Response
                qDebug() << "receive ICC-Response";
                free_flag = true;
                break;
            }
            else if (command_type == 'R')
            { // Device id
                qDebug() << "receive Device_ID-Response";
                free_flag = true;
                break;
            }
            else if (command_type == '0')
            { // Nop
                qDebug() << "receive Nop-Response";
                free_flag = true;
                break;
            }

            else if (command_type == 0x24)
            { // Response cp1
                parse_data_response_measured(frame_buffer[i], poll_request_config_measured_data_codepage1);
                qDebug() << "get measuremnt cp 1";
                free_flag = true;
                break;
            }

            else if (command_type == 0x25)
            { // Response low alarm limits
                parse_data_response_measured(frame_buffer[i], poll_request_low_alarm_limits);
                qDebug() << "get low alarm limits";
                free_flag = true;
                break;
            }

            else if (command_type == 0x26)
            { // Response high alarm limits
                parse_data_response_measured(frame_buffer[i], poll_request_high_alarm_limits);
                qDebug() << "get high alarm limits";
                free_flag = true;
                break;
            }

            else if (command_type == 0x27)
            { // Response alarm cp1
                parse_alarm(frame_buffer[i]);
                qDebug() << "get  alarm CP1";
                free_flag = true;
                break;
            }

            else if (command_type == 0x23)
            { // Response alarm cp2
                parse_alarm(frame_buffer[i]);
                qDebug() << "get alarm CP2";
                free_flag = true;
                break;
            }

            else if (command_type == ')')
            { // Device setting
                parse_data_device_settings(frame_buffer[i]);
                qDebug() << "get device setting";
                free_flag = true;
                break;
            }

            else if (command_type == '*')
            { // TextMessage
                parse_data_text_settings(frame_buffer[i]);
                qDebug() << "get TextMessage";
                free_flag = true;
                break;
            }

            else if (command_type == 0x53)
            { // Response realtime configuration
                parse_realtime_data_configs(frame_buffer[i]);
                qDebug() << "get Response realtime configuration";
                free_flag = true;
                request_realtime_data();
                break;
            }

            else if (command_type == 0x54)
            { // Response realtime data
                qDebug() << "get realtime data command, enable datastream";
                sync_data = true;
                request_sync();
                break;
            }
            else
            {
                break;
            }

        default:
            if ((response_type & 0xf0) == 0xd0)
            {
                qDebug() << "get realtime data SYNC";
                parse_realtime_data(frame_buffer[i]);
            }
            break;
        }
    }
}

void Medibus::send_request()
{
    request_icc();
    measurement_timer->start(measurement_time_interval);
    QThread::sleep(1);
    alarm1_timer->start(alarm1_time_interval);
    QThread::sleep(1);
    alarm2_timer->start(alarm2_time_interval);
    QThread::sleep(1);
    alarm_low_limit_timer->start(alarm_low_limit_time_interval);
    QThread::sleep(1);
    alarm_high_limit_timer->start(alarm_high_limit_time_interval);
    QThread::sleep(1);
}

void Medibus::write_buffer(std::vector<unsigned char> cmd)
{
    std::vector<byte> temptxbufferlist;
    if (cmd.size() != 0)
    {
        temptxbufferlist.push_back(ESCCHAR);
        for (unsigned long i = 0; i < cmd.size(); i++)
        {
            temptxbufferlist.push_back(cmd[i]);
        }
        add_checksum(temptxbufferlist);
        temptxbufferlist.push_back(CRCHAR);
        device->write_buffer((const char *)&temptxbufferlist[0], temptxbufferlist.size());
    }
}

void Medibus::parse_data_text_settings(std::vector<byte> &packetbuffer)
{
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for (unsigned long i = 2; i < framelen - 2; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();
        int textlen = 0;
        for (int i = 0; i < responselen; i = i + textlen)
        {
            std::vector<byte> DataCode(response.begin() + i, response.begin() + i + 2);
            std::vector<byte> TextLen(response.begin() + i + 2, response.begin() + i + 3);
            textlen = int(TextLen[0] - 0x30);
            std::string DataValue(response.begin() + 3 + i, response.begin() + 3 + textlen + i);
            byte datacode = 0x0;
            if (DataCode[0] <= 0x39)
                datacode += 16 * (DataCode[0] - 0x30);
            else
                datacode += 16 * (DataCode[0] - 0x41 + 10);
            if (DataCode[1] <= 0x39)
                datacode += 1 * (DataCode[1] - 0x30);
            else
                datacode += 1 * (DataCode[1] - 0x41 + 10);

            std::string physio_id;
            physio_id = TextMessages.find(datacode)->second;
            NumericValueDraeger NumVal;
            NumVal.datetime = machine_datetime;
            NumVal.timestamp_ms = machine_timestamp;
            NumVal.physioid = physio_id;
            NumVal.value = DataValue;
            m_NumericValueList.push_back(NumVal);
        }
    }
}

void Medibus::parse_data_device_settings(std::vector<byte> &packetbuffer)
{
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for (unsigned long i = 2; i < framelen - 2; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for (int i = 0; i < responselen; i = i + 7)
        {
            std::vector<byte> DataCode(response.begin() + i, response.begin() + 2 + i);
            std::string DataValue(response.begin() + 2 + i, response.begin() + 2 + 5 + i);
            byte datacode = 0x0;
            if (DataCode[0] <= 0x39)
                datacode += 16 * (DataCode[0] - 0x30);
            else
                datacode += 16 * (DataCode[0] - 0x41 + 10);
            if (DataCode[1] <= 0x39)
                datacode += 1 * (DataCode[1] - 0x30);
            else
                datacode += 1 * (DataCode[1] - 0x41 + 10);

            std::string physio_id;
            physio_id = DeviceSettings.find(datacode)->second;
            NumericValueDraeger NumVal;
            NumVal.datetime = machine_datetime;
            NumVal.timestamp_ms = machine_timestamp;
            NumVal.physioid = physio_id;
            NumVal.value = DataValue;
            m_NumericValueList.push_back(NumVal);
        }
    }
}

void Medibus::parse_data_response_measured(std::vector<byte> &packetbuffer, byte type)
{
    unsigned long framelen = packetbuffer.size();

    if (framelen != 0)
    {
        std::vector<byte> response;
        for (unsigned long i = 2; i < framelen - 2; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for (int i = 0; i < responselen; i = i + 6)
        {
            std::vector<byte> DataCode(response.begin() + i, response.begin() + 2 + i);
            std::string DataValue(response.begin() + 2 + i, response.begin() + 6 + i);
            byte datacode = 0x0;
            if (DataCode[0] <= 0x39)
                datacode += 16 * (DataCode[0] - 0x30);
            else
                datacode += 16 * (DataCode[0] - 0x41 + 10);
            if (DataCode[1] <= 0x39)
                datacode += 1 * (DataCode[1] - 0x30);
            else
                datacode += 1 * (DataCode[1] - 0x41 + 10);

            std::string physio_id;
            if (type == poll_request_config_measured_data_codepage1)
            {
                physio_id = MeasurementCP1.find(datacode)->second;
            }
            else if (type == poll_request_low_alarm_limits)
            {
                physio_id = LowLimits.find(datacode)->second;
            }
            else if (type == poll_request_high_alarm_limits)
            {
                physio_id = HighLimits.find(datacode)->second;
            }

            NumericValueDraeger NumVal;
            NumVal.type = type;
            NumVal.datetime = machine_datetime;
            NumVal.timestamp_ms = machine_timestamp;
            NumVal.physioid = physio_id;
            NumVal.value = DataValue;
            m_NumericValueList.push_back(NumVal);
        }
    }
}

void Medibus::parse_alarm(std::vector<byte> &packetbuffer)
{
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for (unsigned long i = 2; i < framelen - 2; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for (int i = 0; i < responselen; i = i + 15)
        {
            byte priority = response[i + 1];
            std::string alarmcode(response.begin() + i + 1, response.begin() + 3 + i);
            std::string AlarmPhase(response.begin() + 3 + i, response.begin() + 15 + i);

            AlarmInfo alarm;
            alarm.datetime = machine_datetime;
            alarm.timestamp_ms = machine_timestamp;
            alarm.alarmcode = alarmcode;
            alarm.alarmphrase = AlarmPhase;
            alarm.priority = std::to_string(priority);
            m_AlarmInfoList.push_back(alarm);
        }
    }
}

/**
 * @brief Medibus::command_echo_response: reply to command
 * @param cmd
 */
void Medibus::command_echo_response(std::vector<byte> &cmd)
{
    std::vector<byte> temptxbufferlist;
    temptxbufferlist.push_back(SOHCHAR);
    for (unsigned long i = 0; i < cmd.size(); i++)
    {
        temptxbufferlist.push_back(cmd[i]);
    }

    add_checksum(temptxbufferlist);
    temptxbufferlist.push_back(CRCHAR);
    const char *payload_data = (const char *)&temptxbufferlist[0];
    device->write_buffer(payload_data, temptxbufferlist.size());
}

/*************************************************************/
// functions for saving data
void Medibus::save_data()
{
    save_m_AlarmInfoList_rows();
    if (m_NumericValueList.size() != 0)
    {
        if (m_NumericValueList[0].type == poll_request_config_measured_data_codepage1)
        {
            save_numeric_value_list_to_row("MeasuredCP1");
        }
        else if (m_NumericValueList[0].type == poll_request_low_alarm_limits)
        {
            save_numeric_value_list_to_row("AlarmLow");
        }
        else if (m_NumericValueList[0].type == poll_request_high_alarm_limits)
        {
            save_numeric_value_list_to_row("AlarmHigh");
        }
    }
}

void Medibus::save_numeric_value_list_to_row(std::string datatype)
{
    if (m_NumericValueList.size() == 0)
        return;

    std::time_t timelapse = device->get_logger()->time_delay;
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    std::string filename;
    if (datatype == "MeasuredCP1")
    {
        filename = filename_measurement;
    }
    else if (datatype == "AlarmLow")
    {
        filename = filename_low_limit;
    }
    else if (datatype == "AlarmHigh")
    {
        filename = filename_high_limit;
    }

    write_numeric_value_list_header(datatype, filename);

    bool changed = false;
    int elementcount = 0;
    std::string row;
    row.append(m_NumericValueList[0].datetime);
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
        device->get_logger()->saving_to_file(filename, row);
        qDebug() << "write data to file";
        m_NumericValueList.erase(m_NumericValueList.begin(), m_NumericValueList.begin() + elementcount);
    }
}

void Medibus::save_m_AlarmInfoList_rows()
{
    if (m_AlarmInfoList.size() == 0)
        return;

    std::time_t timelapse = device->get_logger()->time_delay;
    unsigned long int pc_timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    bool changed = false;
    int elementcount = 0;
    std::string row;

    for (uint i = 0; i < m_AlarmInfoList.size(); i++)
    {
        if (m_AlarmInfoList[i].timestamp_ms == m_AlarmInfoList[0].timestamp_ms &&
            pc_timestamp_ms > (m_AlarmInfoList[i].timestamp_ms + timelapse))
        {
            elementcount += 1;
            changed = true;
            row.append(m_AlarmInfoList[i].datetime);
            row.append(",");
            row.append(m_AlarmInfoList[i].alarmcode);
            row.append(",");
            row.append(m_AlarmInfoList[i].priority);
            row.append(",");
            row.append(m_AlarmInfoList[i].alarmphrase);
            row.append(",\n");
        }
    }

    if (changed)
    {
        device->get_logger()->saving_to_file(filename_alarm, row);
        qDebug() << "write alarm to file";
        m_AlarmInfoList.erase(m_AlarmInfoList.begin(), m_AlarmInfoList.begin() + elementcount);
    }
}

void Medibus::write_numeric_value_list_header(std::string datatype, std::string filename)
{
    if (numeric_value_list_header_selector(datatype))
    {
        std::string header;
        header.append("Time");
        header.append(",");
        for (unsigned long i = 0; i < m_NumericValueList.size(); i++)
        {
            if (m_NumericValueList[i].timestamp_ms == m_NumericValueList[0].timestamp_ms)
            {
                header.append(m_NumericValueList[i].physioid);
                header.append(",");
            }
        }
        header.append("\n");
        device->get_logger()->saving_to_file(filename, header);
    }
}

bool Medibus::numeric_value_list_header_selector(std::string datatype)
{
    bool writeheader = true;
    if (datatype == "MeasuredCP1")
    {
        if (m_transmissionstart)
            m_transmissionstart = false;
        else
            writeheader = false;
    }
    else if (datatype == "DeviceSettings")
    {
        if (m_transmissionstart2)
            m_transmissionstart2 = false;
        else
            writeheader = false;
    }
    else if (datatype == "TextMessages")
    {
        if (m_transmissionstart3)
            m_transmissionstart3 = false;
        else
            writeheader = false;
    }
    else if (datatype == "AlarmLow")
    {
        if (m_transmissionstart4)
            m_transmissionstart4 = false;
        else
            writeheader = false;
    }
    else if (datatype == "AlarmHigh")
    {
        if (m_transmissionstart5)
            m_transmissionstart5 = false;
        else
            writeheader = false;
    }
    return writeheader;
}

//##############################################################################################################################################
// The following code is for real time data retrieving, right now I did not figure out a way to get real time data and other data at
// simutaneously. The following code works well on its own.
/**
 * @brief Medibus::parse_realtime_data_configs
 * @param packetbuffer
 */
void Medibus::parse_realtime_data_configs(std::vector<byte> &packetbuffer)
{
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        for (unsigned long i = 2; i < framelen - 2; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();

        for (int i = 0; i < responselen; i = i + 23)
        {
            std::vector<byte> DataCode(response.begin() + i, response.begin() + 2 + i);
            std::string Interval(response.begin() + 2 + i, response.begin() + 10 + i);
            std::string MinimalV(response.begin() + 10 + i, response.begin() + 15 + i);
            std::string MaximalV(response.begin() + 15 + i, response.begin() + 20 + i);
            std::string MaxBin(response.begin() + 20 + i, response.begin() + 23 + i);
            byte datacode = 0x0;
            if (DataCode[0] <= 0x39)
                datacode += 16 * (DataCode[0] - 0x30);
            else
                datacode += 16 * (DataCode[0] - 0x41 + 10);
            if (DataCode[1] <= 0x39)
                datacode += 1 * (DataCode[1] - 0x30);
            else
                datacode += 1 * (DataCode[1] - 0x41 + 10);

            std::string physio_id;
            physio_id = RealtimeConfigs.find(datacode)->second;
            RealtimeCfg local_cfg;
            local_cfg.id = physio_id;

            int interval = 0;
            for (uint n = 0; n < Interval.length(); n++)
            {
                if (Interval[Interval.length() - 1 - n] == 0x20)
                    break;
                else
                {
                    interval += pow(10, n) * (Interval[Interval.length() - 1 - n] - 0x30);
                }
            }
            local_cfg.interval = interval;

            int minimal_v = 0;
            for (uint n = 0; n < MinimalV.length(); n++)
            {
                if (MinimalV[MinimalV.length() - 1 - n] == 0x20 || MinimalV[MinimalV.length() - 1 - n] == 0x2D)
                    break;
                else
                {
                    minimal_v += pow(10, n) * (MinimalV[MinimalV.length() - 1 - n] - 0x30);
                }
                if (MinimalV[0] == 0x2D)
                    minimal_v = -1 * minimal_v;
            }
            local_cfg.minimal_val = minimal_v;

            int maximal_v = 0;
            for (uint n = 0; n < MaximalV.length(); n++)
            {
                if (MaximalV[MaximalV.length() - 1 - n] == 0x20 || MaximalV[MaximalV.length() - 1 - n] == 0x2D)
                    break;
                else
                {
                    maximal_v += pow(10, n) * (MaximalV[MaximalV.length() - 1 - n] - 0x30);
                }
                if (MaximalV[0] == 0x2D)
                    maximal_v = -1 * maximal_v;
            }
            local_cfg.maximal_val = maximal_v;

            int max_bin = 0;
            for (uint n = 0; n < MaxBin.length(); n++)
            {
                if (MaxBin[MaxBin.length() - 1 - n] == ' ')
                    break;
                else
                {
                    max_bin += pow(16, n) * (MaxBin[MaxBin.length() - 1 - n] - 0x30);
                }
            }
            local_cfg.max_bin = max_bin;
            m_RealtimeCfgList.push_back(local_cfg);
        }
    }
}

void Medibus::parse_realtime_data(std::vector<byte> &packetbuffer)
{
    unsigned long framelen = packetbuffer.size();
    if (framelen != 0)
    {
        std::vector<byte> response;
        byte sync_byte = packetbuffer[0];
        for (unsigned long i = 1; i < framelen - 1; i++)
        {
            response.push_back(packetbuffer[i]);
        }
        int responselen = response.size();
        int value_index = 0;
        std::string physio_id;
        std::string data_value;

        for (int i = 0; i < responselen; i = i + 2)
        {
            std::vector<byte> DataValue(response.begin() + i, response.begin() + 2 + i);
            if ((DataValue[0] & 0xc0) == 0x80)
            { // sync data
                while ((sync_byte) & ((0x01 << value_index) == 1))
                {
                    byte datacode = realtime_data_list[value_index];
                    physio_id = RealtimeConfigs.find(datacode)->second;
                    byte front_num = (0x3f & DataValue[0]);
                    byte back_num = (0x3f & DataValue[1]);
                    int value = int(front_num) + int(back_num) * (64);
                    float value2 = 0;

                    for (uint j = 0; j < m_RealtimeCfgList.size(); j++)
                    {
                        if (m_RealtimeCfgList[j].id == physio_id)
                        {
                            value2 = m_RealtimeCfgList[j].minimal_val + value * (m_RealtimeCfgList[j].maximal_val - m_RealtimeCfgList[j].minimal_val) / float(m_RealtimeCfgList[j].max_bin);
                            break;
                        }
                    }

                    data_value = std::to_string(value2);

                    NumericValueDraeger local_NumVal;
                    local_NumVal.datetime = machine_datetime;
                    local_NumVal.timestamp_ms = machine_timestamp;
                    local_NumVal.physioid = physio_id;
                    local_NumVal.value = data_value;

                    m_NumericValueList.push_back(local_NumVal);
                    value_index = value_index + 1;
                }
            }
            else if ((DataValue[0] & 0xc0) == 0xc0)
            { // sync command
                qDebug() << "cmd";
                // TODO::deal with sync command
            }
        }
        // save_numeric_value_list_to_row("RealtimeData");
        // m_NumericValueList.clear();
    }
}

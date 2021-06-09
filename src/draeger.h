#ifndef DRAEGER_H
#define DRAEGER_H


#include <vector>
#include <map>

typedef unsigned char byte;

#define ESCCHAR 0x1B
#define CRCHAR 0x0D
#define SOHCHAR 0x01


#define poll_request_icc_msg  0x51
#define poll_request_deviceid  0x52
#define request_realtime_configuration 0x53
#define poll_request_no_operation  0x30
#define poll_request_stop_com 0x55

#define poll_request_config_measured_data_codepage1  0x24
#define poll_request_low_alarm_limits 0x25
#define poll_request_high_alarm_limits 0x26
#define poll_request_alarmCP1 0x27
#define poll_request_alarmCP2 0x2E


#define poll_request_config_realtime_data  0x54
#define poll_request_current_date_time  0x28
#define poll_request_device_settings  0x29
#define poll_request_text_messages  0x2a

const std::vector<unsigned char> realtime_transmission_cmd {0x54, 0x30, 0x30, 0x30, 0x32, 0x30, 0x36, 0x30,0x33};

struct NumVal{
    std::string Timestamp;
    std::string PhysioID;
    std::string Value;
};

struct AlarmInfo{
    std::string Timestamp;
    std::string AlarmPhrase;
    std::string Priority;
    std::string AlarmCode;
};

struct RealtimeCfg{
    std::string id;
    int interval;
    int minimal_val;
    int maximal_val;
    int max_bin;
};

typedef std::pair<byte, std::string> stringpair_t;
const std::map<byte, std::string> LowLimits{
    stringpair_t(0xb8, "RespiratoryMinuteVolume"),
    stringpair_t(0xb9, "RespiratoryMinuteVolumeFrac"),
    //CO2
    stringpair_t(0xDB,"EndTidalCO2Percent"), //pct
    stringpair_t(0xE3,"EndTidalCO2kPa"),//kPa
    stringpair_t(0xE6,"EndTidalCO2mmHg"),//mmHg
    //SPO2
    stringpair_t(0xE1,"PulseRateOximeter"), //OnePerMin
    stringpair_t(0xEB,"OxygenSaturation"),  //pct

};

const std::map<byte, std::string> HighLimits{
    stringpair_t(0x7d, "PeakAirwayPressure"),
    stringpair_t(0x82, "ExpiratoryTidalVolume"),
    stringpair_t(0xb8, "RespiratoryMinuteVolume"),
    stringpair_t(0xb9, "RespiratoryMinuteVolumeFrac"),
    stringpair_t(0xd6, "RespiratoryRate"),
    //CO2
    stringpair_t(0xDB,"EndTidalCO2Percent"), //pct
    stringpair_t(0xE3,"EndTidalCO2kPa"),//kPa
    stringpair_t(0xE6,"EndTidalCO2mmHg"),//mmHg
    //SPO2
    stringpair_t(0xE1,"PulseRateOximeter"), //OnePerMin
    stringpair_t(0xEB,"OxygenSaturation"),  //pct
};


const std::map<byte, std::string> MeasurementCP1{
    //Airway related
    stringpair_t(0x05, "BreathingPressure"),  //mbar
    stringpair_t(0x06, "ComplianceFrac"), //mlPerMBar
    stringpair_t(0x07,"Compliance"), //LPerBar
    stringpair_t(0x08,"Resistance"),//mbarPerLPerSec
    stringpair_t(0x0B,"ResistanceFrac"),
    stringpair_t(0x71,"MinimalAirwayPressure"),
    stringpair_t(0x72,"OcclusionPressure"),
    stringpair_t(0x73,"MeanBreathingPressure"), //mbar
    stringpair_t(0x74,"PlateauPressure"),//mbar
    stringpair_t(0x78,"PEEPBreathingPressure"), //mbar
    stringpair_t(0x79,"IntrinsicPEEPBreathingPressure"),
    stringpair_t(0x7D,"PeakBreathingPressure"),  //mbar
    stringpair_t(0x81,"TrappedVolume"),
    stringpair_t(0x82,"TidalVolumeFrac"),
    stringpair_t(0xB5,"SpontaneousRespiratoryRate"),
    stringpair_t(0xB7,"SpontaneousMinuteVolume"),
    stringpair_t(0x7A,"SpontaneousMinuteVolumeFrac"),
    stringpair_t(0xB8,"RespiratoryMinuteVolume"),
    stringpair_t(0xB9,"RespiratoryMinuteVolumeFrac"),//L
    stringpair_t(0xC1,"AirwayTemperature"),
    stringpair_t(0xD6,"RespiratoryRate"),
    stringpair_t(0x8B,"InspiratorySpontaneousSupportVolume"),
    stringpair_t(0x8D,"NegativeInspiratoryForce"),
    stringpair_t(0xC9,"RapidShallowBreathingIndex"),


    //CO2 related
    stringpair_t(0x09,"CarbonDioxideProduction"),//mLPerMin
    stringpair_t(0x89,"DeadSpace"),
    stringpair_t(0x8A,"RelativeDeadSpace"),
    stringpair_t(0xDB,"EndTidalCO2Percent"), //pct
    stringpair_t(0xE3,"EndTidalCO2kPa"),//kPa
    stringpair_t(0xE6,"EndTidalCO2mmHg"),//mmHg
    stringpair_t(0xF0,"InspO2"),//pct

    //SPO2
    stringpair_t(0xE1,"PulseRateOximeter"), //OnePerMin
    stringpair_t(0xEB,"OxygenSaturation"),  //pct
};

const std::map<byte, std::string> DeviceSettings
{
    stringpair_t(0x01, "Oxygen"),  //pct
    stringpair_t(0x02, "MaxInpirationFlow"),  //LPerMin
    stringpair_t(0x04, "InspTidalVolume"),  //L
    stringpair_t(0x05, "InspiratoryTime"),  //sec
    stringpair_t(0x07, "IPart"),  //None
    stringpair_t(0x08, "EPart"),  //None
    stringpair_t(0x09, "FrequencyIMV"),  //OnePerMin
    stringpair_t(0x0A, "FrequencyIPPV"),  //OnePerMin
    stringpair_t(0x0B, "PEEP"),  //mbar
    stringpair_t(0x0C, "IntermittentPEEP"),  //mbar
    stringpair_t(0x0D, "BIPAPLowPressure"),  //mbar
    stringpair_t(0x0E, "BIPAPHighPressure"),  //mbar
    stringpair_t(0x0F, "BIPAPLowTime"),  //sec
    stringpair_t(0x10, "BIPAPHighTime"),  //sec
    stringpair_t(0x11, "ApneaTime"),  //sec
    stringpair_t(0x12, "PressureSupportPressure"),  //mbar
    stringpair_t(0x13, "MaxInspirationAirwayPressure"),  //mbar
    stringpair_t(0x15, "TriggerPressure"),  //mbar
    stringpair_t(0x16, "TachyapneaFrequency"),  //OnePerMin
    stringpair_t(0x17, "TachyapneaDuration"),  //sec
    stringpair_t(0x27, "InspPause_InspTime"),  //pct
    stringpair_t(0x29, "FlowTrigger"),  //LPerMin
    stringpair_t(0x2E, "ASBRamp"),  //sec
    stringpair_t(0x2F, "FreshgasFlow"),  //mLPerMin
    stringpair_t(0x40, "VT"),  //mL
    stringpair_t(0x42, "MinimalFrequency"),  //OnePerMin
    stringpair_t(0x44, "BackupTidalVolume"),  //L
    stringpair_t(0x45, "InspiratoryPressure"),  //mbar
    stringpair_t(0x4A, "Age"),  //yr
    stringpair_t(0x4B, "Weight"),  //kg
    stringpair_t(0x4C, "InspiratoryFlow"),  //L/sec
    stringpair_t(0x4E, "Tdisconnect"),  //sec
    stringpair_t(0x4F, "FlowAcceleration"),  //mbar/sec
};
const std::map<byte, std::string> TextMessages{
    stringpair_t(0x01, "VentModeIPPV"),
    stringpair_t(0x02, "VentModeIPPVAssist"),
    stringpair_t(0x04, "VentModeCPPV"),
    stringpair_t(0x05, "VentModeCPPVAssist"),
    stringpair_t(0x06, "VentModeSIMV"),
    stringpair_t(0x07, "VentModeSIMVASB"),
    stringpair_t(0x08, "SB"),
    stringpair_t(0x09, "ASB"),
    stringpair_t(0x0A, "CPAP"),
    stringpair_t(0x0B, "CPAP_ASB"),
    stringpair_t(0x0C, "MMV"),
    stringpair_t(0x0D, "MMV_ASB"),
    stringpair_t(0x0E, "BIPAP"),
    stringpair_t(0x0F, "SYNCHRON_MASTER"),
    stringpair_t(0x10, "SYNCHRON_SLAVE"),
    stringpair_t(0x11, "APNEA_VENTILATION"),
    stringpair_t(0x12, "DS"),
    stringpair_t(0x18, "BIPAP_SMV"),
    stringpair_t(0x19, "BIPAP_SMV_ASB"),
    stringpair_t(0x1A, "BIPAP_APRV"),
    stringpair_t(0x1E, "VentStandby"),
    stringpair_t(0x20, "Adults"),
    stringpair_t(0x21, "Neonates"),
    stringpair_t(0x22, "CO2InmmHg"),
    stringpair_t(0x23, "CO2InkPa"),
    stringpair_t(0x24, "CO2InPercent"),
    stringpair_t(0x25, "AnesGasHalothane"),
};
const std::map<byte, std::string> RealtimeConfigs{
    stringpair_t(0x00, "AirwayPressure"),
    stringpair_t(0x01, "Flow"),
    stringpair_t(0x02, "Pleth"),
    stringpair_t(0x03, "Resp"),
    stringpair_t(0x06, "ExpCo2_1"),
    stringpair_t(0x07, "ExpCo2_2"),
    stringpair_t(0x08, "ExpCo2_3"),
    stringpair_t(0x20, "InspFlow"),
    stringpair_t(0x21, "ExpFlow"),
    stringpair_t(0x24, "ExpVolume"),
};

#endif // DRAEGER_H

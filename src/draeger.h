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
#define poll_request_no_operation  0x30
#define poll_request_stop_com 0x55
#define poll_request_config_measured_data_codepage1  0x24
#define poll_request_config_measured_data_codepage2  0x2B
#define poll_request_config_realtime_data  0x54

#define poll_request_current_date_time  0x28
#define poll_request_device_settings  0x29
#define poll_request_text_messages  0x2a

struct NumVal{
    std::string Timestamp;
    std::string PhysioID;
    std::string Value;
};

typedef std::pair<byte, std::string> stringpair_t;
const std::map<byte, std::string> MeasurementCP1{
    stringpair_t(0x05, "BreathingPressure"),  //mbar
    stringpair_t(0x06, "ComplianceFrac"), //mlPerMBar
    stringpair_t(0x07,"Compliance"), //LPerBar
    stringpair_t(0x08,"Resistance"),//mbarPerLPerSec
    stringpair_t(0x09,"CarbonDioxideProduction"),//mLPerMin
    stringpair_t(0x0B,"ResistanceFrac"),
    stringpair_t(0x2A,"rSquared"), //None
    stringpair_t(0x50,"InspHalothanekPa"),  //kPa
    stringpair_t(0x51,"ExpHalothanekPa"),//kPa
    stringpair_t(0x52,"InspEnfluranekPa"),  //kPa
    stringpair_t(0x53,"ExpEnfluranekPa"),//kPa
    stringpair_t(0x54,"InspIsofluranekPa"), //kPa
    stringpair_t(0x55,"ExpIsofluranekPa"), //kPa
    stringpair_t(0x56,"InspDesfluranekPa"), //kPa
    stringpair_t(0x57,"ExpDesfluranekPa"), //kPa
    stringpair_t(0x58,"InspSevofluranekPa"),//kPa
    stringpair_t(0x59,"ExpSevofluranekPa"), //kPa
    stringpair_t(0x5A,"InspAgentkPa"), //kPa
    stringpair_t(0x5B,"ExpAgentkPa"),//kPa
    stringpair_t(0x5C,"InspAgent2kPa"), //kPa
    stringpair_t(0x5D,"ExpAgent2kPa"), //kPa
    stringpair_t(0x64,"O2Uptake"),  //TenMlPerMin
    stringpair_t(0x69,"PlateauTime"),//sec
    stringpair_t(0x6B,"AmbientPressure"),//mbar
    stringpair_t(0x6F,"InspiratoryTime"),//sec
    stringpair_t(0x71,"MinimalAirwayPressure"),
    stringpair_t(0x72,"OcclusionPressure"),
    stringpair_t(0x73,"MeanBreathingPressure"), //mbar
    stringpair_t(0x74,"PlateauPressure"),//mbar
    stringpair_t(0x76,"FlowPeak"),//mL/sec
    stringpair_t(0x78,"PEEPBreathingPressure"), //mbar
    stringpair_t(0x79,"IntrinsicPEEPBreathingPressure"),
    stringpair_t(0x7A,"SpontaneousMinuteVolumeFrac"),
    stringpair_t(0x7B,"RRmand"),//OnePerMin
    stringpair_t(0x7D,"PeakBreathingPressure"),  //mbar
    stringpair_t(0x7E,"VTmand"),//L
    stringpair_t(0x7F,"VTspon"),//L
    stringpair_t(0x81,"TrappedVolume"),
    stringpair_t(0x82,"TidalVolumeFrac"),
    stringpair_t(0x83,"VTemand"),//mL
    stringpair_t(0x84,"VTespon"),//mL
    stringpair_t(0x85,"InspmandatoryTidalVolumeVTimand"),//mL
    stringpair_t(0x88,"TidalVolume"),//mL
    stringpair_t(0x89,"DeadSpace"),
    stringpair_t(0x8A,"RelativeDeadSpace"),
    stringpair_t(0x8B,"InspiratorySpontaneousSupportVolume"),
    stringpair_t(0xAC,"InspMAC"),//None
    stringpair_t(0xAD,"ExpMAC"),// None
    stringpair_t(0xAE,"InspDesfluranePct"), //pct
    stringpair_t(0xAF,"ExpDesfluranePct"), //pct
    stringpair_t(0xB0,"InspSevofluranePct"), //pct
    stringpair_t(0xB1,"ExpSevofluranePct"),  //pct
    stringpair_t(0xB2,"Leakage"),//mLPerMin
    stringpair_t(0xB3,"LeakageRelPctleak"), //pct
    stringpair_t(0xB4,"RespiratoryRatePressure"),//OnePerMin
    stringpair_t(0xB5,"SpontaneousRespiratoryRate"),
    stringpair_t(0xB6,"SpontaneousFractionMinVoPctMVsponMVtotal"), //pct
    stringpair_t(0xB7,"SpontaneousMinuteVolume"),
    stringpair_t(0xB8,"RespiratoryMinuteVolume"),
    stringpair_t(0xB9,"RespiratoryMinuteVolumeFrac"),//L
    stringpair_t(0xBD,"ApneaDuration"), //sec
    stringpair_t(0xC1,"AirwayTemperature"),
    stringpair_t(0xC4,"DeltaO2"),//pct
    stringpair_t(0xC6,"BatteryCapacity"),//%
    stringpair_t(0xC9,"RapidShallowBreathingIndex"),
    stringpair_t(0xD5,"RespiratoryRateCO2"),//OnePerMin
    stringpair_t(0xD6,"RespiratoryRate"),
    stringpair_t(0xD7,"RespiratoryRateVolumePerFlow"), //OnePerMin
    stringpair_t(0xD9,"RespiratoryRateDerived"), //OnePerMin
    stringpair_t(0xDA,"InspCO2Pct"),//pct
    stringpair_t(0xDB,"EndTidalCO2Percent"), //pct
    stringpair_t(0xDD,"N2OFlow"),//mLPerMin
    stringpair_t(0xDE,"AirFlow"),//mLPerMin
    stringpair_t(0xDF,"PulseRateDerived"),  //OnePerMin
    stringpair_t(0xE1,"PulseRateOximeter"), //OnePerMin
    stringpair_t(0xE2,"O2Flow"),//mLPerMin
    stringpair_t(0xE3,"EndTidalCO2kPa"),//kPa
    stringpair_t(0xE5,"InspCO2mmHg"),//mmHg
    stringpair_t(0xE6,"EndTidalCO2mmHg"),//mmHg
    stringpair_t(0xE7,"ItoE_Ipart"),//None
    stringpair_t(0xE8,"ItoE_Epart"),//None
    stringpair_t(0xE9,"InspAgentPct"),  //pct
    stringpair_t(0xEA,"ExpAgentPct"),//pct
    stringpair_t(0xEB,"OxygenSaturation"),  //pct
    stringpair_t(0xED,"InspAgent2Pct"), //pct
    stringpair_t(0xEE,"ExpAgent2Pct"),  //pct
    stringpair_t(0xEF,"ExpO2"), //pct
    stringpair_t(0xF0,"InspO2"),//pct
    stringpair_t(0xF4,"InspHalothanePct"), //pct
    stringpair_t(0xF5,"ExpHalothanePct"),//pct
    stringpair_t(0xF6,"InspEnfluranePct"), //pct
    stringpair_t(0xF7,"ExpEnfluranePct"),//pct
    stringpair_t(0xF8,"InspIsofluranePct"),  //pct
    stringpair_t(0xF9,"ExpIsofluranePct"),  //pct
    stringpair_t(0xFB,"InspN2OPct"), //pct
    stringpair_t(0xFC,"ExpN2OPct"), //pct
    stringpair_t(0xFF,"InspCO2kPa"),//kPa*/

};
const std::map<byte, std::string> MeasurementCP2{
    stringpair_t(0x00, "VTspon"),  //mL
    stringpair_t(0x06, "ElastanceE"),  //mbarPerL
    stringpair_t(0x07, "Tau"),  //sec
    stringpair_t(0x21, "ExpiratoryTidalVolumeVTe"),  //mL
    stringpair_t(0x22, "InspiratoryTidalVolumeVTi"),  //mL
    stringpair_t(0x23, "EIP"),  //mbar
    stringpair_t(0x7D, "Tlowmax"),  //sec
    stringpair_t(0x7B, "PressureVariability"),  //pct
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
#endif // DRAEGER_H

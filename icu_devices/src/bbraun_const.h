#ifndef BBRAUN_CONST_H
#define BBRAUN_CONST_H

#include <map>

#define SOHCHAR 0x01
#define STXCHAR 0x02
#define ETXCHAR 0x03
#define ETBCHAR 0x17
#define EOTCHAR 0x04
#define RSCHAR 0x1E
#define DCHAR 0x44
#define ECHAR 0x45
#define dCHAR 0x64
#define eCHAR 0x65
#define XCHAR 0x58
#define xCHAR 0x78
#define ACKCHAR 0x06
#define NAKCHAR 0x15

typedef unsigned char byte;

typedef std::pair<std::string, std::string> stringpair_t2;
static std::map<std::string, std::string> GeneralParameters{
    stringpair_t2("FMNOR", "Number of connected Space pillars"),
    stringpair_t2("FMSTAT", "State of SpaceCom"),
    stringpair_t2("FMDONGLE", "Hardware protection"),
    stringpair_t2("FMBANY", "Last scanned barcode"),
    stringpair_t2("FMRT", "Type of a segment"),
    stringpair_t2("FMRSTAT", "State of a segment"),
};
static std::map<std::string, std::string> InfusionPumpParameters{
    stringpair_t2("GNNEW", "Name of the pump"),
    stringpair_t2("GNMODEL", "Name of the pump model"),

    stringpair_t2("INRT", "Rate"),

    stringpair_t2("INVTB", "Volume to be infused"),

    stringpair_t2("INPSOLL", "Pressure value"),

    stringpair_t2("INSOL", "Medication long name"),

    stringpair_t2("INSOLSN", "Medication short name"),

    stringpair_t2("INSOLID", "Medication ID"),

    stringpair_t2("INSYR", "Size of syringe"),

    stringpair_t2("INVOLAC", "Actual volume in syringe"),

    stringpair_t2("INRMT", "Remaining time until end of infusion"),

    stringpair_t2("INVI", "Volume infused"),

    stringpair_t2("INTIME", "Infusion time"),

    stringpair_t2("INAKKU", "Battery time"),

    stringpair_t2("INSTBY", "Rest of standby time"),

    stringpair_t2("INBORT", "Rate of bolus"),

    stringpair_t2("INBOVAL", "Bolus volume delivered by pump"),

    stringpair_t2("INDOCAL", "Dose Mode active"),

    stringpair_t2("INDCON", "Drug concentration"),

    stringpair_t2("INDCONU", "Drug concentration unit"),

    stringpair_t2("INDORT", "dose rate"),

    stringpair_t2("INDORTU", "Dose rate unit"),

    stringpair_t2("INP1", "Actual pressure setting  pressure alarm"),

    stringpair_t2("INP2", "Actual pressure in % of the maximum pressure"),

    stringpair_t2("INM1", "Ready for infusion"),

    stringpair_t2("INM2", "Bolus active"),

    stringpair_t2("INM3", "Active pumping"),

    stringpair_t2("INM4", "CC-function"),

    stringpair_t2("INM5", "Bolus function is released"),

    stringpair_t2("INM6", "Standby function on"),

    stringpair_t2("INM7", "Data-Lock on"),

    stringpair_t2("INM8", "Power supply"),

    stringpair_t2("INM9", "Disposable changed"),

    stringpair_t2("INM10", "KOR/KVO-function"),

    stringpair_t2("INM11", "Without drop –sensor- control"),

    stringpair_t2("INM12", "Softlimit low undergone"),

    stringpair_t2("INM13", " Softlimit high overgone"),

    stringpair_t2("INA1", "Battery empty"),

    stringpair_t2("INA2", "Syringe empty"),

    stringpair_t2("INA3", "Pressure alarm"),

    stringpair_t2("INA4", "Standby expire in two minutes"),

    stringpair_t2("INA5", "Standby over"),

    stringpair_t2("INA6", "CC-alarm"),

    stringpair_t2("INA7", "End of volume"),

    stringpair_t2("INA8", "Time over"),

    stringpair_t2("INA9", "Syringe alarm"),

    stringpair_t2("INA10", "Not used"),

    stringpair_t2("INA11", "Battery pre-alarm"),

    stringpair_t2("INA12", "Syringe empty pre-alarm"),

    stringpair_t2("INA13", "Door of pump open"),

    stringpair_t2("INA14", "Drop alarm"),

    stringpair_t2("INA15", "Air alarm"),

    stringpair_t2("INA16", "KVO end"),

    stringpair_t2("INNR", "Unique identifier"),

    stringpair_t2("INDTNR", "DiaNet-type number of device"),

    stringpair_t2("INSERNUM", "Serial number of device"),

    stringpair_t2("DSSTATUS", "Operation condition of a pump"),

    stringpair_t2("INBOCNT", "Counter for bolus given between two requests"),

    stringpair_t2("INAVRT", "Average rate between two requests"),
};
static std::map<std::string, std::string> AdditionalParameters{
    stringpair_t2("VERSION", "Actual version number of protocol"),

    stringpair_t2("OPMODE", "Operation mode of pump"),

    stringpair_t2("DATE", "Actual date set in SpaceCom"),

    stringpair_t2("TIME", "Actual time set in SpaceCom"),

    stringpair_t2("TIMEOFINF", "Pre selected time of infusion"),

    stringpair_t2("ALARM1", "Alarm1"),

    stringpair_t2("ALARM2", "Alarm2"),

    stringpair_t2("STATUS1", "Condition1"),

    stringpair_t2("STATUS2", "Condition2"),

    stringpair_t2("PREALARM", "Pre alarm"),

    stringpair_t2("DGDSTATUS", "Status of DOSEGURAD"),

    stringpair_t2("DGDALERTSLL", "DoseGuard alert of soft limit low"),

    stringpair_t2("DGDALERTSLH", "DoseGuard alert of soft limit high"),

    stringpair_t2("DGDALERTHLH", "DoseGuard alert of hard limit high"),

    stringpair_t2("DISPCHG", "Disposable change"),

    stringpair_t2("PATIDBADGE", "Scanned patient ID from badge"),

    stringpair_t2("PATIDDRUG", "Scanned patient ID from drug label"),

    stringpair_t2("PATID", " Stored patient id in pump"),

    stringpair_t2("PATW", "Patient weight(kg}"),

    stringpair_t2("PATH", "Height of patient(cm)"),

    stringpair_t2("PATAGE", "Age of the patient"),

    stringpair_t2("PATSEX", "Sex of the patient"),

    stringpair_t2("BOLTYPE", "Type of given bolus"),

    stringpair_t2("BOLRTSLLOW", "Bolus rate soft limit low"),

    stringpair_t2("BOLRTSLHIGH", "Bolus rate soft limit high"),

    stringpair_t2("BOLRTHLHIGH", "Bolus rate hard limit high"),

    stringpair_t2("BOLVOLSLLOW", "Bolus volume soft limit value low"),

    stringpair_t2("BOLVOLSLHIGH", " Bolus volume soft limit value high"),

    stringpair_t2("BOLVOLHLHIGH", "Bolus volume hard limit value"),

    stringpair_t2("BOLVOLLIMUT", "Unit of all bolus volume limits"),

    stringpair_t2("ACTCHAN", "active channel"),

    stringpair_t2("RUNSTATE", "Status of a pump in a bit field"),

    stringpair_t2("SLLOW", " Soft limit low value"),

    stringpair_t2("SLHIGH", "Soft limit high value"),

    stringpair_t2("HLHIGH", "Hard limit high value"),

    stringpair_t2("RTLIMUT", "Unit of rate limit"),

    stringpair_t2("LASTDA", "Last device alarm given by pump"),

    stringpair_t2("DRUGCAT", "Drug category"),

    stringpair_t2("DLNAME", "Name of Drug list"),

    stringpair_t2("DLCRETIME", "Creation time of Drug List"),

    stringpair_t2("NURSEID", "Scanned Nurse identifier"),

    stringpair_t2("DRUGORDERNO", "Pharmacy System drug order number"),

    stringpair_t2("DISPNAME", "Brand of selected disposable"),

    stringpair_t2("INSOLPRIO", "Alarm priority defined for drug"),

    stringpair_t2("INSOLCOL", "Color definition for drug"),

    stringpair_t2("TCIMOD", "Selected TCI mode"),

    stringpair_t2("TCIMODEL", "Selected TCI model"),

    stringpair_t2("TCIAWA", "Calculated wake up time"),

    stringpair_t2("TCIPLA", "actual plasma value"),

    stringpair_t2("TCIEFF", "actual effect value"),

    stringpair_t2("TCITAR", "entered target value for plasma or effect concentration"),

    stringpair_t2("TCIUT", "Unit of TCI values"),

    stringpair_t2("DRROUNDED", "Dose rate was rounded"),

    stringpair_t2("PCAAD", "PCA A/D ratio(%)"),

    stringpair_t2("PCABR", "Number of PCA boluses requested"),

    stringpair_t2("PCALOT", "Set lock out time"),

    stringpair_t2("PCABA", "Absolute value of applicated boluses"),

    stringpair_t2("PROGSTAT", "Step number of the programming mode"),

    stringpair_t2("AUTOPROGSTATE", "State of AutoProgramming mechanism"),

    stringpair_t2("LASTAUTOPROGERR", "Last error reported by AutoProgramming"),
};

struct NumericValueBbraun
{
    std::string datetime;
    unsigned long int timestamp_ms;
    std::string relativetime;
    std::string address;
    std::string parametertype;
    std::string physioid;
    std::string value;
};

#endif // BBRAUN_CONST_H

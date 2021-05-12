#ifndef DATEX_H
#define DATEX_H


#include <map>

#define DATA_INVALID_LIMIT -32001

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
namespace datex {

#define DRI_MT_PHDB 0
#define DRI_MT_WAVE 1
#define DRI_MT_ALARM 4
#define DRI_MT_NETWORK 5
#define DRI_MT_FO 8

#define DRI_EOL_SUBR_LIST 0xFF

#pragma pack(1)
struct sr_desc{
    short sr_offset;
    byte sr_type;
};

struct datex_hdr{
    short r_len;
    byte r_nbr;
    byte dri_level;
    unsigned short plug_id;
    unsigned int r_time;
    byte n_subnet;
    byte reserved2;
    unsigned short reserved3;
    short r_maintype;
    struct sr_desc sr_desc[8];
};

//////////////////////////////////////////

#define DRI_PHDBCL_REQ_BASIC_MASK 0x0000
#define DRI_PHDBCL_DEBY_BASIC_MASK 0x0001
#define DRI_PHDBCL_REQ_EXT1_MASK 0x0002
#define DRI_PHDBCL_REQ_EXT2_MASK 0x0004
#define DRI_PHDBCL_REQ_EXT3_MASK 0x0008


#define DRI_PH_DISPL 1
#define DRI_PH_10s_TREND 2
#define DRI_PH_60s_TREND 3
#define DRI_PH_AUX_INFO 4


/*Measurement***********************************************************/
#define DATA_INVALID -32767
#define DATA_NOT_UPDATED -32766
#define DATA_UNDER_RANGE -32764
#define DATA_OVER_RANGE -32763
#define DATA_NOT_CALIBRATED -32762

/**
 * @brief WavesIDLabels
 */
typedef std::pair<byte, std::string> stringpair_t;
const std::map<byte, std::string> WaveIdLabels{
     stringpair_t(0, "DRI_WF_CMD"),
     stringpair_t(1, "DRI_WF_ECG1"),
     stringpair_t(2, "DRI_WF_ECG2"),
     stringpair_t(3, "DRI_WF_ECG3"),
     stringpair_t(4, "DRI_WF_INVP1"),
     stringpair_t(5, "DRI_WF_INVP2"),
     stringpair_t(6, "DRI_WF_INVP3"),
     stringpair_t(7, "DRI_WF_INVP4"),
     stringpair_t(8, "DRI_WF_PLETH"),
     stringpair_t(9, "DRI_WF_CO2"),
     stringpair_t(10, "DRI_WF_O2"),
     stringpair_t(11, "DRI_WF_N2O"),
     stringpair_t(12, "DRI_WF_AA"),
     stringpair_t(13, "DRI_WF_AWP"),
     stringpair_t(14, "DRI_WF_FLOW"),
     stringpair_t(15, "DRI_WF_RESP"),
     stringpair_t(16, "DRI_WF_INVP5"),
     stringpair_t(17, "DRI_WF_INVP6"),
     stringpair_t(18, "DRI_WF_EEG1"),
     stringpair_t(19, "DRI_WF_EEG2"),
     stringpair_t(20, "DRI_WF_EEG3"),
     stringpair_t(21, "DRI_WF_EEG4"),
     stringpair_t(23, "DRI_WF_VOL"),
     stringpair_t(24, "DRI_WF_TONO_PRESS"),
     stringpair_t(29, "DRI_WF_SPI_LOOP_STATUS"),
     stringpair_t(32, "DRI_WF_ENT_100"),
     stringpair_t(35, "DRI_WF_EEG_BIS"),
};

struct phdb_status_bits{
    unsigned int exists :1;
    unsigned int active :1;
    unsigned int reserved :14;
    unsigned int reserved2 :16;
};

union phdb_status{
    struct phdb_status_bits status_bits;
    dword status_dw;
    short stat_f[2];
};

struct group_hdr{
    //union phdb_status status;
    dword status;
    word label; //different for each group class
};

/*Basic Class***********************************************************/

//Ecg_group
struct ecg_group{
    struct group_hdr hdr;
    short hr;
    short st1;
    short st2;
    short st3;
    short imp_rr;
};

//invasive pressure group p_group
struct p_group{
    struct group_hdr hdr;
    short sys;
    short dia;
    short mean;
    short hr;
};

//non-invasive blood pressure group nibp_group
struct nibp_group{
    struct group_hdr hdr;
    short sys;
    short dia;
    short mean;
    short hr;
};

//Temperature group
struct t_group{
    struct group_hdr hdr;
    short temp;
};

//SpO2
struct SpO2_pl_group{
    struct group_hdr hdr;
    short SpO2;
    short pr;
    short ir_amp;
    short SvO2;
};

//CO2 group
struct co2_group{
    struct group_hdr hdr;
    short et;
    short fi;
    short rr;
    short amb_press;
};

//O2 group
struct o2_group{
    struct group_hdr hdr;
    short et;
    short fi;
};

//N2O Group
struct n2o_group
{
    struct group_hdr hdr;
    short et;
    short fi;
};

//anesthesia agent group
struct aa_group
{
    struct group_hdr hdr;
    short et;
    short fi;
    short mac_sum;
};

//flow and volume group
struct flow_vol_group
{
    struct group_hdr hdr;
    short rr;
    short ppeak;
    short peep;
    short pplat;
    short tv_insp;
    short tv_exp;
    short compliance;
    short mv_exp;
};

//cardiac output & Wedge Pressure Group
struct co_wedge_group
{
    struct group_hdr hdr;
    short co;
    short blood_temp;
    short ref;
    short pcwp;
};

//NMT Group
struct nmt_group
{
    struct group_hdr hdr;
    short t1;
    short tratio;
    short ptc;
};

//ECG Extra Group
struct ecg_extra_group
{
    short hr_ecg;
    short hr_max;
    short hr_min;
};

//SvO2
struct svo2_group
{
    struct group_hdr   hdr;
    short svo2;
};


#pragma pack(1)
struct basic_phdb{
    struct ecg_group ecg;
    struct p_group p1;
    struct p_group p2;
    struct p_group p3;
    struct p_group p4;

    struct nibp_group nibp;
    struct t_group t1;
    struct t_group t2;
    struct t_group t3;
    struct t_group t4;

    struct SpO2_pl_group SpO2;
    struct co2_group co2;
    struct o2_group o2;
    struct n2o_group n2o;
    struct aa_group aa;
    struct flow_vol_group flow_vol;
    struct co_wedge_group co_wedge;
    struct nmt_group nmt;
    struct ecg_extra_group ecg_extra;
    struct svo2_group svo2_group;
    struct p_group p5;
    struct p_group p6;

    byte reserved[2];
};

/*EXT1 Class*/
struct arrch_ecg_group{
    struct group_hdr    hdr;
    short               hr;
    short               rr_time;
    dword               arrch_status_bf;
    short               reserved[16];
};

struct ecg_12_group{
    struct group_hdr    hdr;
    short               stI;
    short               stII;
    short               stIII;
    short               stAVL;
    short               stAVR;
    short               stAVF;
    short               stV1;
    short               stV2;
    short               stV3;
    short               stV4;
    short               stV5;
    short               stV6;
};

struct ext1_phdb{
    struct arrch_ecg_group ecg;
    struct ecg_12_group    ecg12;
    struct p_group         p78[2];
    struct SpO2_pl_group   SpO2_ch2;
    struct t_group         t56[2];
    byte                   reserved[134];
};

//*EXT2 Class*//
struct nmt2_group{
    struct group_hdr    hdr;
    short               count;
    short               nmt_t1;
    short               nmt_t2;
    short               nmt_t3;
    short               nmt_t4;
    short               nmt_resv1;
    short               nmt_resv2;
    short               nmt_resv3;
    short               nmt_resv4;
};

struct eeg_channel{
    short   ampl;
    short   sef;
    short   mf;
    short   delta_proc;
    short   theta_proc;
    short   alpha_proc;
    short   beta_proc;
    short   bsr;
};

struct  eeg_group{
    struct group_hdr    hdr;
    short               femg;
    struct  eeg_channel eeg1;
    struct  eeg_channel eeg2;
    struct  eeg_channel eeg3;
    struct  eeg_channel eeg4;
};

struct eeg_bis_group{
    struct group_hdr    hdr;
    short               bis;
    short               sqi_val;
    short               emg_val;
    short               sr_val;
    short               reserved1;
};

struct entropy_group{
    struct group_hdr    hdr;
    short               eeg_ent;
    short               emg_ent;
    short               bsr_ent;
    short               reserved[8];
};

struct eeg2_group{
    struct group_hdr    hdr;
    byte                common_reference;
    byte                montage_label_ch_1_m;
    byte                montage_label_ch_1_p;
    byte                montage_label_ch_2_m;
    byte                montage_label_ch_2_p;
    byte                montage_label_ch_3_m;
    byte                montage_label_ch_3_p;
    byte                montage_label_ch_4_m;
    byte                montage_label_ch_4_p;
    byte                reserved_byte;
    short               reserved[8];
};

struct spi_group{
    struct group_hdr    hdr;
    short               spiVal;
    short               reserved[4];
};

struct ext2_phdb{
    struct nmt2_group   nmt2;
    struct eeg_group    eeg;
    struct eeg_bis_group    eeg_bis;
    struct entropy_group    ent;
    byte                reserved1[58];
    struct eeg2_group   eeg2;
    struct spi_group    spi;
    byte                reserved2[24];
};

/*EXT3  Class*/
struct gasex_group{
    struct group_hdr    hdr;
    short               vo2;
    short               vco2;
    short               ee;
    short               rq;
};

struct flow_vol_group2{
    struct group_hdr    hdr;
    short               ipeep;
    short               pmean;
    short               raw;
    short               mv_insp;
    short               epeep;
    short               mv_spont;
    short               ie_ratio;
    short               insp_time;
    short               exp_time;
    short               static_compliance;
    short               static_pplat;
    short               static_peepe;
    short               static_peepi;
    short               reserved[7];
};

struct bal_gas_group{
    struct group_hdr    hdr;
    short               et;
    short               fi;
};

struct tono_group{
    struct  group_hdr   hdr;
    short   prco2;
    short   pr_et;
    short   pr_pa;
    short   pr_delay;
    short   phi;
    short   phi_delay;
    short   amb_press;
    short   cpma;
};

struct aa2_group{
    struct group_hdr    hdr;
    short               mac_age_sum;
    byte                reserved[16];
};

struct delp_group{
    struct group_hdr    hdr;
    short               spv;
    short               ppv;
};

struct cpp_group{
    struct group_hdr    hdr;
    short               value;
};

struct picco_group{
    struct group_hdr    hdr;
    short               cci;
    short               cco;
    short               cfi;
    short               ci;
    short               co;
    short               cpi;
    short               cpo;
    short               dpmax;
    short               elwi;
    short               evlw;
    short               gedi;
    short               gedv;
    short               gef;
    short               itbi;
    short               itbv;
    short               ppv;
    short               pvpi;
    short               sv;
    short               svi;
    short               svr;
    short               svri;
    short               svv;
    short               tblood;
    short               tinj;
};

struct ext3_phdb{
    struct gasex_group  gassex;
    struct flow_vol_group2  flow_vol2;
    struct bal_gas_group  bal;
    struct tono_group  tono;
    struct aa2_group  aa2;
    struct delp_group delp;
    struct cpp_group cpp;
    struct cpp_group cpp2;
    struct picco_group picco;
    byte               reserved[74];
};

/**/



//278 byte
struct dri_phdb{
    dword time;
    union{
        struct basic_phdb basic;
        struct ext1_phdb ext1;
        struct ext2_phdb ext2;
        struct ext3_phdb ext3;
    }physdata;
    byte marker;
    byte reserved;
    word cl_drilvl_subt; //contains dri_phdb_class
};


struct ph_srcrds{
    byte ph_subrec[5*sizeof(struct dri_phdb)]; //one physiological database record accomodates up to five subrecords
};
struct wr_srcrds{
    // byte ph_subrec[5*sizeof(struct dri_phdb)]; //one physiological database record accomodates up to five subrecords
    short   *data;
};


struct nw_srcrds{
//    struct nw_login_msg a1;
//    struct nw_pat_descr a2;
byte ph_subrec[5*sizeof(struct dri_phdb)];//one physiological database record accomodates up to five subrecords
};
struct fo_srcrds{
    byte ph_subrec[5*sizeof(struct dri_phdb)]; //one physiological database record accomodates up to five subrecords
};


enum dri_phdb_class{
    DRI_PHDBCL_BASIC,
    DRI_PHDBCL_EXT1,
    DRI_PHDBCL_EXT2,
    DRI_PHDBCL_EXT3,
};

struct aux_phdb_info{
    dword nibp_time;
    short reserved1;
    dword time;
    dword pcwp_time;
    short pat_bsa;
    byte reserved[98];
};

#pragma pack(1)
struct dri_phdb_req{
    byte phdb_rcrd_type;
    short tx_ival;
    unsigned int phdb_class_bf;
    short reserved;
};

//Valid waveform subrecord types
#define DRI_AL_XMIT_STATUS 0
#define DRI_AL_ENTER_DIFFMODE 2
#define DRI_AL_EXIT_DIFFMODE 3

#pragma pack(1)
struct al_tx_cmd{
    short cmd;
    short reserved[5];
};




struct datex_record_phdb_req{
    struct datex_hdr hdr;
    struct dri_phdb_req phdbr;
};

struct datex_record_alarm_req{
    struct datex_hdr hdr;
    struct al_tx_cmd alarm_cmd;
};
///////////////////////////////////////////////////////////

//Valid waveform subrecord types
#define DRI_WF_CMD 0
#define DRI_WF_ECG1 1
#define DRI_WF_ECG2 2
#define DRI_WF_ECG3 3
#define DRI_WF_INCP1 4
//TODO::fill the data constants
#pragma pack(1)
struct dri_wave_req{
    short       req_type;
    short       res;
    byte        type[8];
    short        reserved[10];
};

struct datex_record_wave_req{
    struct datex_hdr hdr;
    struct dri_wave_req wfreq;
};

#define WF_REQ_CONT_START 0
#define WF_REQ_CONT_STOP 1


struct wf_hdr{
    short   act_len;
    word    status;
    word    label;
};

struct ext_wf_hdr{
    short   size_in_bytes;
    short   status_1;
    short   status_2;
    short   status_3;
    short   status_4;
    short   status_5;
    short   status_6;
};

/*alarm*/
typedef enum {
   F, T
}
boolean;

enum dri_silience_info{
    DRI_SI_NONE = 0,
    DRI_SI_APNEA = 1,
    DRI_SI_ASY = 2,
    DRI_SI_APNEA_ASY = 3,
    DRI_SI_ALL = 4,
    DRI_SI_2MIN = 5,
    DRI_SI_5MIN = 6,
    DRI_SI_20S = 7
};

enum dri_alarm_color{
    DRI_PR0 = 0,
    DRI_PR1 = 1,
    DRI_PR2 = 2,
    DRI_PR3 = 3,
};

#pragma pack(1)
struct al_disp_al{
    char text[80];
    short text_changed;
    short color;
    short color_changed;
    short   reserved[6];
};

#pragma pack(1)
struct dri_al_msg{
    short reserved;
    short sound_on_off;
    short reserved2;
    short reserved3;
    short silience_info;
    struct al_disp_al   al_disp[5];
    short   reserved4[5];

};

struct al_srcrds{
    struct dri_al_msg   local_dri_al_msg;
};

/*network managemnet*/
struct nw_dev_descr
{
    byte    ethernet_addr[6];
    short   dev_type;
    short   dri_level;
    short   reserved[10];
};

struct nw_login_msg{
    struct nw_dev_descr nw_dev_descr2;
};



struct nw_pat_descr{
    char    pat_1stname[30];
    char    pat_2ndname[40];
    char    pat_id[40];
    char    middle_name[30];
    short   gender;
    short   age_years;
    short   age_days;
    short   age_hours;
    short   height;
    short   height_unit;
    short   weight;
    short   weight_unit;
    short   year_birth_date;
    short   month_birth_date;
    short   day_birth_date;
    short   hour_birth_date;
    short   bsa;
    char    location[32];
    char    issuer[32];
    short   change_src;
    short   reserved[59];
};

struct datex_record{
    struct datex_hdr hdr;
    union{
        struct wr_srcrds wf_rcrd;
        struct ph_srcrds ph_rcrd;
        struct al_srcrds al_rcrd;
        struct nw_srcrds nw_rcrd;
        struct fo_srcrds fo_rcrd;
        byte            data[1450];
    }rcrd;
};



}
#endif // DATEX_H

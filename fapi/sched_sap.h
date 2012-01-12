#ifndef _SCHED_SAP_H_
#define _SCHED_SAP_H_

/***********************************************************************************/
/* LTE MAC Scheduler Interface                                                     */
/* Specification v1.11                                                             */
/* Femto Forum Technical Document                                                  */
/*                                                                                 */
/* Document number:  FF_Tech_001_v1.11                                             */
/* Date issued:      12-10-2010                                                    */
/* Document status:  Document for public distribution                              */
/*                                                                                 */
/* Copyright                                                                       */
/* This document is subject to copyright owned by the Femto Forum Ltd and/or       */
/* licensed to the Femto Forum Ltd by its contributing members. You may use and    */
/* distribute this document free of charge provided that you comply with the       */
/* provisions set out in this notice. Other than this limited licence, you are not */
/* granted any further right, interest or title in this document and the Femto     */
/* Forum Ltd and/or its contributing members shall at all times remain the sole    */
/* owner(s) of the copyright in this document.                                     */
/***********************************************************************************/

#ifndef __KERNEL__
#ifndef bool
#define bool int
#endif	/* bool */
#endif	/* __KERNEL__ */


/**
 *  4.4  Constants
 */
#define MAX_SCHED_CFG_LIST   10
#define MAX_LC_LIST          10
#define MAX_CTRL_BUF_LIST    4
#define MAX_BUFF_ADD_LIST    30
#define MAX_RACH_LIST        30
#define MAX_PAGING_LIST      30
#define MAX_DL_INFO_LIST     30
#define MAX_BUILD_DATA_LIST  30
#define MAX_BUILD_RAR_LIST   10
#define MAX_BUILD_BC_LIST    3
#define MAX_ULINFO_LIST      30
#define MAX_DCI_LIST         30
#define MAX_PHICH_LIST       30
#define MAX_TB_LIST          2
#define MAX_RLC_PDU_LIST     30
#define MAX_NR_LCG           4
#define MAX_MBSFN_CONFIG     5
#define MAX_SI_MSG_LIST      32
#define MAX_SI_MSG_SIZE      65535
#define MAX_CE_LIST          30
#define MAX_CQI_LIST         30
#define MAX_UE_SELECTED_SB   6
#define MAX_HL_SB            25
#define MAX_SINR_RB_LIST     100
#define MAX_SR_LIST          30
#define MAX_MAC_CE_LIST      30

enum result_e
{
    SUCCESS, FAILURE
};

/**
 *     4.3  Parameters
 */

/**
 * \brief See section 4.3.1 dlDciListElement
 */
struct dlDciListElement
{
    uint16_t rnti;                   // 1..65535
    uint32_t rb_bitmap;              // 32bit bitmap
    uint8_t  rb_shift;               // 0,1
    uint8_t  res_alloc;              // 0,1,2
    uint8_t  nr_of_tbs;              // 1..2

    uint16_t tbs_size [MAX_TB_LIST]; // 0..
    uint8_t  mcs [MAX_TB_LIST];      // 0..31
    uint8_t  ndi [MAX_TB_LIST];      // 0..1
    uint8_t  rv [MAX_TB_LIST];       // 0..3

    uint8_t cce_index;               // 0..88
    uint8_t aggr_level;              // 1,2,4,8
    uint8_t precoding_info;          // 2 antenna_ports: 0..6

    enum format_e
    {
        e1,e1A,e1B,e1C,e1D,e2,e2A,e2B
    } format;

    int8_t  tpc;                     // -4,-1,0,1,3,4
    uint8_t harq_process;            // 0..7
    uint8_t dai;                     // 1,2,3,4

    enum vrb_format_e
    {
        VRB_DISTRIBUTED, VRB_LOCALIZED
    } vrb_format;

    bool tb_swap;                    // TRUE, FALSE
    bool sps_release;                //  TRUE, FALSE
    bool pdcch_order;                // TRUE, FALSE

    uint8_t preamble_index;          // 0..63
    uint8_t prach_mask_index;        // 0..15

    enum n_gap_e
    {
        GAP1, GAP2
    } n_gap;

    uint8_t tbs_idx;                 // 2,3
    uint8_t dl_power_offset;         // 0,1
    int8_t  pdcch_power_offset;      // -6..4
};

/**
 * \brief See section 4.3.2 ulDciListElement
 */
struct ulDciListElement
{
    uint16_t rnti;                    // 1..65535
    uint8_t  rb_start;                // 0..99
    uint8_t  rb_len;                  // 1..100
    uint16_t tb_size;                 // 2..
    uint8_t  mcs;                     // 0..32
    uint8_t  ndi;                     // 0..1
    uint8_t  cce_index;               // 0..88
    uint8_t  aggr_level;              // 1,2,4,8
    uint8_t  ue_tx_antenna_selection; // 0,1,3
    bool     hopping;                 // TRUE, FALSE
    uint8_t  n_2_dmrs;                // 0..7
    int8_t   tpc;                     // -4,-1,0,1,3,4
    bool     cqi_request;             // TRUE, FALSE
    uint8_t  ul_index;                // 0,1,2,3
    uint8_t  dai;                     // 1,2,3,4
    uint8_t  freq_hopping;            // 0..4
    int8_t   pdcch_power_offset;      // -6..4
};

#  if 0
/**
 * \brief See section 4.3.3 vendorSpecificListElement
 */
struct vendorSpecificListElement
{
    enum type_e
    {
        PF_WEIGHT1, PF_WEIGHT2, CQI_AVG_FACTOR
    } type;
      /* PF_WEIGHT1  The first weight used by a proportional fair scheduler */
      /* PF_WEIGHT2  The second weight used by a proportional fair scheduler */
      /* CQI_AVG_FACTOR  The factor used for averaging CQIs in the scheduler. */

    uint32_t length;  // unsigned integer
    struct XXX value; // depends on type  struct
};
#  endif

/**
 * \brief See section 4.3.4 logicalChannelConfigListElement
 */
struct logicalChannelConfigListElement
{
    uint8_t logicalChannelIdentity;     // 1..10
    uint8_t logicalChannelGroup;        // 0..3, 4
    enum direction_e
    {
        DIR_UL, DIR_DL, DIR_BOTH
    } direction;

    enum qosBearerType_e
    {
        QBT_NON_GBR, QBT_GBR
    } qosBearerType;

    uint8_t qci;                        // 0..255
    // The QCI is coded as defined in 36.413, i.e the value
    // indicates one less than the actual QCI value.

    uint64_t e_RAB_MaximumBitrateUL;    // 0..10000000000
    uint64_t e_RAB_MaximumBitrateDL;    // 0..10000000000
    uint64_t e_RAB_GuaranteedBitrateUL; // 0..10000000000
    uint64_t e_RAB_GuaranteedBitrateDL; // 0..10000000000
};

/**
 * \brief See section 4.3.5 addBufferListElement
 */
struct addBufferListElement
{
    uint16_t length;    // 1..65535
    uint32_t timestamp; // 32bit
};

/**
 * \brief See section 4.3.6 rachListElement
 */
struct rachListElement
{
    uint16_t rnti;          // 1..65535
    uint16_t estimatedSize; // 56,144,208,256
};

/**
 * \brief See section 4.3.7 phichListElement
 */
struct phichListElement
{
    uint16_t rnti; // 1..65535

    enum phich_e
    {
        PhiACK, PhiNACK
    } phich;
};

/**
 * \brief See section 4.3.9 rlcPDU_ListElement
 */
struct rlcPDU_ListElement
{
    uint8_t logicalChannelIdentity; // 0..10
    uint16_t size;                  // 1..9420
};

/**
 * \brief See section 4.3.8 buildDataListElement
 */
struct buildDataListElement
{
    uint16_t rnti;               //  1..65535
    struct dlDciListElement dci; // see section 4.3.1

    uint8_t ceBitmap [MAX_TB_LIST]; // bitmap  TA, DRX, CE
    uint8_t nr_rlcPDU_List;         // 1..MAX_RLC_PDU_LIST

    struct rlcPDU_ListElement rlcPDU_List [MAX_RLC_PDU_LIST][MAX_TB_LIST]; // See 4.3.9
};

/**
 * \brief See section 4.3.10 buildRAR_ListElement
 */
struct buildRAR_ListElement
{
    uint16_t rnti;              // 1..65535
    uint32_t grant;             // integer

    struct dlDciListElement dci; // see 4.3.1
};

/**
 * \brief See section 4.3.11 buildBroadcastListElement
 */
struct buildBroadcastListElement
{
    enum broadcast_type_e
    {
        BCCH, PCCH
    } type;

    uint8_t index;              // 0..63 :  0  SIB1, 1..31  SIx, 32..63 - PCCH

    struct dlDciListElement dci; // see 4.3.1
};

/**
 * \brief See section 4.3.12 ulInfoListElement
 */
struct ulInfoListElement
{
    uint16_t rnti;                        // 1..65535
    uint16_t ulReception [MAX_LC_LIST+1]; //  0..65535

    enum receptionStatus_e
    {
        Ok, notOk, NotValid
    } receptionStatus;

    int8_t tpc;                           // -4,-1,0,1,3,4
};

/**
 * \brief See section 4.3.13 srListElement
 */
struct srListElement
{
    uint16_t rnti; // 1..65535
};

/**
 * \brief See section 4.3.14 macCE_ListElement
 */
struct macCE_ListElement
{
    uint16_t rnti;                         // 1..65535

    enum maCE_Type_e
    {
        BSR, PHR, CRNTI
    } maCE_Type;

      /**
       * \brief See section 4.3.15 macCE_Value
       */
    union
    {
        uint8_t phr;                       // 0..63,64
        uint8_t crnti;                     // crnti
        uint8_t bufferStatus [MAX_NR_LCG]; // 0..63, 64
    } macCE_Value;
};


/**
 * \brief See section 4.3.16 drxConfig
 */
struct drxConfig
{
    uint8_t onDurationTimer;
    // 1,2,3,4,5,6,8,10,20,30,40,50,60,80,100,200
    uint16_t drx_InactivityTimer;
    // 1,2,3,4,5,6,8,10,20,30,40,50,60,80,100,200,300,500,750,1280,1920,2560
    uint8_t drx_RetransmissionTimer;
    // 1,2,4,6,8,16,24,33
    uint16_t longDRX_Cycle;
    // 10,20,32,40,64,80,128,160,256,320, 512,640,1024,1280,2048,2560
    uint16_t longDRX_CycleStartOffset;
    // 0..2559
    uint16_t shortDRX_Cycle;
    // 2,5,8,10,16,10,21,40,64,80,128,160,256,320,512,640, OFF
    uint8_t drxShortCycleTimer; // 1..16
};

/**
 * \brief See section 4.3.17 spsConfig
 */
struct spsConfig
{
    uint16_t semiPersistSchedIntervalUL;
    // 10,20,32,40,64,80,128,160,320,640  subframes.
    uint16_t semiPersistSchedIntervalDL;
    // 10,20,32,40,64,80,128,160,320,640  subframes.

    uint8_t  numberOfConfSPS_Proces;         // 1..8

    uint8_t  n1_PUCCH_AN_PersistentListSize; // 0..4
    uint16_t n1_PUCCH_AN_PersistentList[4];  // 0..2047

    uint8_t  implicitReleaseAfter;           // 2,3,4,8
};

/**
 * \brief See section 4.3.18 srRequest
 */
enum action_e
{
    setup,release
};

struct srRequest
{
    enum action_e action;

    uint8_t schedInterval; // 5,10,20,40,80
    uint8_t dsr_TransMax;  // 4,8,16,32,64
};

/**
 * \brief See section 4.3.19 cqiRequest
 */
struct cqiRequest
{
    enum action_e action;

    uint16_t cqiSchedInterval; // 1,2,5,10,20,32,40,64,80,128,160 subframes.
    uint8_t riSchedInterval;   // 1,2,4,8,16,20 subframes.
};

/**
 * \brief See section 4.3.20 ue_Capabilities
 */
struct ue_Capabilities
{
    bool halfDuplex;      // UE only supports half-duplex  FDD operation
    bool intraSF_hopping; // UE support of intra-subframe hopping
    bool type2_sb_1;      // UE supports type 2 hopping with n_sb > 1
    uint8_t ueCategory;   // 1..5
    bool resAllocType1;   // UE support for resource allocation type 1.
};

/**
 * \brief See section 4.3.22 siMessageListElement
 */
struct siMessageListElement
{
    uint16_t periodicity; // 8, 16, 32, 64, 128, 256, 512 radio frames
    uint16_t length;      // 1..MAX_SI_MSG_SIZE
};

/**
 * \brief See section 4.3.21 siConfiguration
 */
struct siConfiguration
{
    uint16_t sfn;                                                // 0..1023
    uint16_t sib1Length;                                         // 1..MAX_SI_MSG_SIZE
    uint8_t siWindowLength;                                      // 1,2,5,10,15,20,40

    uint8_t nrSI_Message_List;                                   // 0..MAX_SI_MSG_LIST
    struct siMessageListElement siMessageList [MAX_SI_MSG_LIST]; // See 4.3.22
};

/**
 * \brief See section 4.3.23 dlInfoListElement
 */
struct dlInfoListElement
{
    uint16_t rnti;          // 1..65535
    uint8_t harqProcessId;  // 0..7,8

    uint8_t nr_harq_status; // 1.. MAX_TB_LIST
    enum harqStatus_e {
        HarqACK, HarqNACK, HarqDTX
    } harqStatus [MAX_TB_LIST];
};

/**
 * \brief See section 4.3.26 ueSelected
 */
struct ueSelected
{
    uint8_t sbList [MAX_UE_SELECTED_SB]; // 0..24
    uint8_t sb_pmi;                      // 0..15
    uint8_t sb_cqi [MAX_TB_LIST];        // 0..15
};

/**
 * \brief See section 4.3.27 higherLayerSelected
 */
struct higherLayerSelected
{
    uint8_t sb_pmi;              // 0..63
    uint8_t sb_cqi[MAX_TB_LIST]; // 0..15
};

/**
 * \brief See section 4.3.28 bwPart
 */
struct bwPart
{
    uint8_t bwPartIndex; // 1..4
    uint8_t sb;          // 0..11
    uint8_t cqi;         // 0..15
};

/**
 * \brief See section 4.3.24 cqiListElement
 */
struct cqiListElement
{
    uint16_t rnti;                                                  // 1..65535
    uint8_t ri;                                                     // 1..4

    enum cqiType_e
    {
        P10,P11,P20,P21, A12, A22, A20, A30, A31
    } cqiType;


    uint8_t wb_cqi[MAX_TB_LIST];                                    // 0..15
    uint8_t wb_pmi;                                                 // 0..15

    /**
     * \brief See section 4.3.25 sbMeasResult
     */
    union
    {
        struct ueSelected ueSelected;                               // see 4.3.26
        struct higherLayerSelected higherLayerSelected [MAX_HL_SB]; // see 4.3.27
        struct bwPart bwPart;                                       // see 4.3.28
    } sbMeasResult;
};

/**
 * \brief See section 4.3.29 ulCQI
 */
struct ulCQI
{
    uint16_t sinr [MAX_SINR_RB_LIST];  // -20.0..30. Sxxxxxxxxxxx.xx dB
    enum measurement_type_e
    {
        SRS, PUSCH, PUCCH_1, PUCCH_2, PRACH
    } type;
    /*
      SRS  measurement on SRS
      PUSCH  measurement on PUSCH
      PUCCH_1  measurement done on Format 1 resource
      PUCCH_2  measurement done on Format 2 resource
      PRACH  measurement done on PRACH
    */
};

/**
 * \brief See section 4.3.30 pagingInfoListElement
 */
struct pagingInfoListElement
{
    uint8_t  pagingIndex;        // 32..63
    uint16_t pagingMessageSize;  // 0..
    uint8_t  pagingSubframe;     // 0..9
};

/**
*   CSCHED  MAC Scheduler Control SAP
*
* CSCHED_CELL_CONFIG_REQ
*     4.1.1  (re-)configure MAC scheduler with cell
*         configuration and scheduler configuration. The cell configuration will
*         also setup the BCH, BCCH, PCCH and CCCH LC configuration.
* CSCHED_CELL_CONFIG_CNF
*         4.1.2  Cell configuration and scheduler
*         configuration applied
*
* CSCHED_UE_CONFIG_REQ
*         4.1.3  (re-)configure  MAC scheduler with UE
*         specific parameters. A UE can only be
*         configured when a cell configuration has
*         been received.
* CSCHED_UE_CONFIG_CNF
*         4.1.4  UE specific configuration applied
*
* CSCHED_LC_CONFIG_REQ
*         4.1.5  (re-)configure MAC scheduler with logical
*         channel configuration. A logical channel
*         can only be configured when a UE
*         configuration has been received.
* CSCHED_LC_CONFIG_CNF
*         4.1.6  Logical channel configuration applied
*
* CSCHED_LC_RELEASE_REQ
*         4.1.7  release a logical channel in the MAC
*         scheduler. A logical channel can only be
*         released if it has been configured
*       previously.
* CSCHED_LC_RELEASE_CNF
*       4.1.8  Logical Channel specific configuration removed
*
* CSCHED_UE_RELEASE_REQ
*       4.1.9  release a UE in the MAC scheduler. The
*       release of the UE configuration implies
*       the release of LCs, which are still active. A
*       UE can only be released if it has been configured previously.
* CSCHED_UE_RELEASE_CNF
*       4.1.10  UE specific configuration removed
*
* CSCHED_UE_CONFIG_UPDATE_IND
*       4.1.11  update of UE specific parameters from MAC scheduler to RRC
*
* CSCHED_CELL_CONFIG_UPDATE_IND
*       4.1.12  update of ctrl channel configuration from MAC scheduler to RRC
*/


enum cyclicPrefixLength_e
{
    CP_Normal, CP_Extended
};

/**
 * \brief See section 4.1.1 CSCHED_CELL_CONFIG_REQ
 */
struct CSCHED_CELL_CONFIG_REQ
{
    uint8_t pusch_HoppingOffset;                                 // 0..98
    enum hoppingMode_e
    {
        inter,
        interintra
    }hoppingMode;


    uint8_t n_SB;                                                // 1,2,3,4

    enum phichResource_e
    {
        PHICH_R_ONE_SIXTH,
        PHICH_R_HALF,
        PHICH_R_ONE,
        PHICH_R_TWO
    } phichResource;

    enum phichDuration_e
    {
        DurationNormal, DurationExtended
    } phichDuration;

    uint8_t initialNrOf_PDCCH_OFDM_Symbols;                      // 0..4
    struct siConfiguration siConfiguration;                      // See. 4.3.21

    uint8_t ul_Bandwidth;                                        // 6,15,25,50,75,100
    uint8_t dl_Bandwidth;                                        // 6,15,25,50,75,100

    enum cyclicPrefixLength_e ul_CyclicPrefixLength;
    enum cyclicPrefixLength_e dl_CyclicPrefixLength;

    uint8_t antennaPortsCount;                                   // 1,2,4

    enum duplexMode_e
    {
        DM_TDD, DM_FDD
    } duplexMode;

    uint8_t subframeAssignment;                                  // 0..6
    uint8_t specialSubframePatterns;                             // 0..8

    bool    mbsfn_SubframeConfigPresent;                         // 4.2.1
    uint8_t mbsfn_SubframeConfig_RFPeriod [MAX_MBSFN_CONFIG];    // 1,2,4,8,16.32
    uint8_t mbsfn_SubframeConfig_RFOffset [MAX_MBSFN_CONFIG];    // 0..7
    uint8_t mbsfn_SubframeConfig_SFallocation [MAX_MBSFN_CONFIG];// bitmap 0..9

    uint8_t prachConfigurationIndex;                             // 0..63
    uint8_t prach_FreqOffset;                                    // 0..94
    uint8_t ra_ResponseWindowSize;                               // 2..8,10
    uint8_t mac_ContentionResolutionTimer;                       // 8,16,24,32,40,48,56,64

    uint8_t  maxHARQ_Msg3Tx;                                     // 1..8
    uint16_t n1PUCCH_AN;                                         // 0..2047
    uint8_t  deltaPUCCH_Shift;                                   // 1..3

    uint8_t nRB_CQI;                                             // 0..98
    uint8_t nCS_AN;                                              // 0..7
    uint8_t srsSubframeConfiguration;                            // 0..15

    uint8_t srsSubframeOffset;                                   // 0..9
    uint8_t srsBandwidthConfiguration;                           // 0..7
    bool    srsMaxUpPts;                                         // TRUE, FALSE

    enum enable64QAM_e
    {
        MOD_16QAM, MOD_64QAM
    } enable64QAM;

    uint8_t nr_vendorSpecificList;                               // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.2 CSCHED_CELL_CONFIG_CNF
 */
struct CSCHED_CELL_CONFIG_CNF
{
    enum result_e result;

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.3 CSCHED_UE_CONFIG_REQ
 */
struct CSCHED_UE_CONFIG_REQ
{
    uint16_t rnti;                               // 1..65535
    bool reconfigureFlag;                        // TRUE, FALSE
    bool drxConfigPresent;                       // TRUE, FALSE
    struct drxConfig drxConfig;                  // see 4.3.16
    uint16_t timeAlignmentTimer;                 // 500,750,1280,1920,2560,5120

    enum measGapConfigPattern_e
    {
        MGP_GP1, MGP_GP2, OFF
    } measGapConfigPattern;

    uint8_t measGapConfigSubframeOffs;           // 0..79
    bool spsConfigPresent;                       // TRUE, FALSE

    struct spsConfig spsConfig;                  // see 4.3.17     The SPS configuration
    bool srConfigPresent;                        // TRUE, FALSE
    struct srRequest srConfig;                   // see 4.3.18     The SR configuration request
    bool cqiConfigPresent;                       // TRUE, FALSE
    struct cqiRequest cqiConfig;                 // see 4.3.19     The CQI configuration request
    uint8_t transmissionMode;                    // 1..7

    uint64_t ue_AggregatedMaximumBitrateUL;      // 0..10000000000
    uint64_t ue_AggregatedMaximumBitrateDL;      // 0..10000000000

    struct ue_Capabilities ue_Capabilities;      // see 4.3.20     The UE capabilities

    enum ue_TransmitAntennaSelection_e
    {
        none, openLoop, closedLoop
    } ue_TransmitAntennaSelection;

    bool    ttiBundling;                         // TRUE, FALSE
    uint8_t maxHARQ_Tx;                          // 1..8,10,12,16,20,24, 28
    uint8_t betaOffset_ACK_Index;                // 0..15
    uint8_t betaOffset_RI_Index;                 // 0..15
    uint8_t betaOffset_CQI_Index;                // 0..15
    bool    ackNackSRS_SimultaneousTransmission; // TRUE, FALSE

    bool simultaneousAckNackAndCQI;              // TRUE, FALSE

    enum aperiodicCQI_RepMode_e
    {
        rm12, rm20,rm22, rm30, rm31, None
    } aperiodicCQI_RepMode;

    enum tdd_AckNackFeedbackMode_e
    {
        bundling, multiplexing
    } tdd_AckNackFeedbackMode;

    uint8_t ackNackRepetitionFactor;             // 0,2,4,6

    uint8_t nr_vendorSpecificList;               // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.4 CSCHED_UE_CONFIG_CNF
 */
struct CSCHED_UE_CONFIG_CNF
{
    uint16_t rnti;                 // 1..65535
    enum result_e result;

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.5 CSCHED_LC_CONFIG_REQ
 */
struct CSCHED_LC_CONFIG_REQ
{

    uint16_t rnti;                          // 1..65535
    bool reconfigureFlag;                   // TRUE, FALSE
    uint8_t nr_logicalChannelConfigList;    // 1..MAX_LC_LIST

    struct logicalChannelConfigListElement
    logicalChannelConfigList [MAX_LC_LIST]; // see 4.3.4

    uint8_t nr_vendorSpecificList;          // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.6 CSCHED_LC_CONFIG_CNF
 */
struct CSCHED_LC_CONFIG_CNF
{
    uint16_t rnti;                               // 1..65535

    enum result_e result;

    uint8_t nr_logicalChannelIdentity;           // 1..MAX_LC_LIST
    uint8_t logicalChannelIdentity[MAX_LC_LIST]; // range 1..10  channel ID

    uint8_t nr_vendorSpecificList;               // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.7 CSCHED_LC_RELEASE_REQ
 */
struct CSCHED_LC_RELEASE_REQ
{
    uint16_t rnti;                                 // 1..65535
    uint8_t  nr_logicalChannelIdentity;            // 1..MAX_LC_LIST
    uint8_t  logicalChannelIdentity [MAX_LC_LIST]; // range 1..10

    uint8_t nr_vendorSpecificList;                 // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.8 CSCHED_LC_RELEASE_CNF
 */
struct CSCHED_LC_RELEASE_CNF
{
    uint16_t rnti;                                // 1..65535

    enum result_e result;

    uint8_t nr_LogicalChannelIdentity;            // 1..MAX_LC_LIST
    uint8_t logicalChannelIdentity [MAX_LC_LIST]; // range 1..10

    uint8_t nr_vendorSpecificList;                // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.9 CSCHED_UE_RELEASE_REQ
 */
struct CSCHED_UE_RELEASE_REQ
{
    uint16_t rnti;                 // 1..65535

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.10 CSCHED_UE_RELEASE_CNF
 */
struct CSCHED_UE_RELEASE_CNF
{
    uint16_t rnti;                 // 1..65535

    enum result_e result;

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.11 CSCHED_UE_CONFIG_UPDATE_IND
 */
struct CSCHED_UE_CONFIG_UPDATE_IND
{
    uint16_t rnti;                 // 1..65535
    uint8_t transmissionMode;      // 1..7

    bool spsRequestPresent;        // TRUE, FALSE
    struct spsConfig spsRequest;         // see 4.3.17     The SPS configuration request

    bool srRequestPresent;         // TRUE, FALSE
    struct srRequest srRequest;          // see 4.3.18     The SR configuration request

    bool cqiRequestPresent;        // TRUE, FALSE
    struct cqiRequest cqiRequest;         // see 4.3.19     The CQI configuration request

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.1.12 CSCHED_CELL_CONFIG_UPDATE_IND
 */
struct CSCHED_CELL_CONFIG_UPDATE_IND
{
    uint8_t prbUtilizationDL;      // 0..100
    uint8_t prbUtilizationUL;      // 0..100

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};


/**
*   4.2  SCHED - MAC Scheduler SAP
*
*  SCHED_DL_RLC_BUFFER_REQ
*   4.2.1  update buffer status of logical channel data in
*   RLC. The update rate with which the buffer status
*   is updated in the scheduler is outside of the scope
*   of the document.
*
*  SCHED_DL_PAGING_BUFFER_REQ
*   4.2.2  update buffer status of paging messages.
*
*  SCHED_DL_MAC_BUFFER_REQ
*   4.2.3  update buffer status of MAC control elements.
*   The update rate with which the buffer status is
*   updated in the scheduler is outside of the scope
*   of the document.
*
*  SCHED_DL_TRIGGER_REQ
*   4.2.4  Starts the DL MAC scheduler for this subframe
*
*  SCHED_DL_RACH_INFO_REQ
*   4.2.5  Provides RACH reception information to the scheduler
*
*  SCHED_DL_CQI_INFO_REQ
*   4.2.6  Provides CQI measurement report information to
*   the scheduler
*
*  SCHED_DL_CONFIG_IND
*   4.2.5  triggers building of DL MAC PDUs and Subframe
*   Configuration in MAC
*
*  SCHED_UL_TRIGGER_REQ
*   4.2.8  Starts the UL MAC scheduler for this subframe
*
*  SCHED_UL_NOISE_INTERFERENCE_REQ
*   4.2.9  Provides Noise and interference measurement
*   information to the scheduler
*
*  SCHED_UL_SR_INFO_REQ
*   4.2.10  Provides scheduling request reception
*   information to the scheduler
*
*  SCHED_UL_MAC_CTRL_INFO_REQ
*   4.2.11  Provides mac control information
*   (power headroom, ul buffer status) to the scheduler
*
*  SCHED_UL_CQI_INFO_REQ
*   4.2.12  Provides UL CQI measurement information to the scheduler
*
*  SCHED_UL_CONFIG_IND
*   4.2.9  passes the UL scheduling decision (Format 0 DCIs) to MAC
*/

/**
 * \brief See section 4.2.1 SCHED_DL_RLC_BUFFER_REQ
 */
struct SCHED_DL_RLC_BUFFER_REQ
{
    uint16_t rnti;                           // 1..65535
    uint8_t  logicalChannelIdentity;         // 0..10

    uint32_t rlc_TransmissionQueueSize;      // 0..4294967295  transmission queue in
    uint16_t rlc_TransmissionQueueHOL_Delay; // 0..65535  new transmissions in
    uint32_t rlc_RetransmissionQueueSize;    // 0..4294967295  retransmission queue
    uint16_t rlc_RetransmissionHOL_Delay;    // 0..65535  retransmissions in ms
    uint16_t rlc_StatusPDU_Size;             // 0..65535  pending STATUS

    uint8_t nr_vendorSpecificList;           // 0..MAX_SCHED_CFG_LIST  elements in the next
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.2 SCHED_DL_PAGING_BUFFER_REQ
 */
struct SCHED_DL_PAGING_BUFFER_REQ
{
    uint8_t  nr_pagingInfoList;               //  0..MAX_PAGING_LIST
    uint16_t pagingInfoList[MAX_PAGING_LIST]; // nr_pagingInfoList

    uint8_t nr_vendorSpecificList;            // 0..MAX_SCHED_CFG_LIST  elements in the next
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.3 SCHED_DL_MAC_BUFFER_REQ
 */
struct SCHED_DL_MAC_BUFFER_REQ
{
    uint16_t rnti;                 //  1..65535

    uint8_t ceBitmap;              // TA, DRX, CR  bitmap  to be sent by the MAC.

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.4 SCHED_DL_TRIGGER_REQ
 */
struct SCHED_DL_TRIGGER_REQ
{
    uint16_t sfn_sf;                                        // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_dlInfoList;                                  // 0..MAX_DL_INFO_LIST
    struct dlInfoListElement dlInfoList [MAX_DL_INFO_LIST]; //See 4.3.23

    uint8_t nr_vendorSpecificList;                          // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.2.5 SCHED_DL_RACH_INFO_REQ
 */
struct SCHED_DL_RACH_INFO_REQ
{
    uint16_t sfn_sf;                                 // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_rachList;                             // 0..MAX_RACH_LIST  list
    struct rachListElement rachList [MAX_RACH_LIST]; // See 4.3.6

    uint8_t nr_vendorSpecificList;                   // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.6 SCHED_DL_CQI_INFO_REQ
 */
struct SCHED_DL_CQI_INFO_REQ
{
    uint16_t sfn_sf;                              // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_cqiList;                           // 0..MAX_CQI_LIST
    struct cqiListElement cqiList [MAX_CQI_LIST]; // See 4.3.24

    uint8_t nr_vendorSpecificList;                // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList[0];
};

/**
 * \brief See section 4.2.7 SCHED_DL_CONFIG_IND
 */
struct SCHED_DL_CONFIG_IND
{
    uint8_t nr_buildDataList;               // 0.. MAX_BUILD_DATA_LIST
    uint8_t nr_buildRAR_List;               // 0.. MAX_BUILD_RAR_LIST
    uint8_t nr_buildBroadcastList;          // 0.. MAX_BUILD_BC_LIST

    struct buildDataListElement
    buildDataList [MAX_BUILD_DATA_LIST];    // See 4.3.8
    struct buildRAR_ListElement
    buildRAR_List [MAX_BUILD_RAR_LIST];     // See 4.3.10
    struct buildBroadcastListElement
    buildBroadcastList [MAX_BUILD_BC_LIST]; // See 4.3.11

    uint8_t nrOf_PDCCH_OFDM_Symbols;        // 0..4

    uint8_t nr_vendorSpecificList;          // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.8 SCHED_UL_TRIGGER_REQ
 */
struct SCHED_UL_TRIGGER_REQ
{
    uint16_t sfn_sf;               // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_ulInfoList;         // 0.. MAX_ULINFO_LIST
    struct ulInfoListElement
    ulInfoList[MAX_ULINFO_LIST];   // See 4.3.12

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.9 SCHED_UL_NOISE_INTERFERENCE_REQ
 */
struct SCHED_UL_NOISE_INTERFERENCE_REQ
{
    uint16_t sfn_sf;               // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint16_t rip;                  // -126.0..-75.0  SXXXXXXX.XXXXXXXX  36.214. in dBm.
    uint16_t tnp;                  // -146.0..-75    SXXXXXXX.XXXXXXXX  dBm

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.10 SCHED_UL_SR_INFO_REQ
 */
struct SCHED_UL_SR_INFO_REQ
{
    uint16_t sfn_sf;                           // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_srList;                         // 0..MAX_SR_LIST
    struct srListElement srList [MAX_SR_LIST]; // See 4.3.13

    uint8_t nr_vendorSpecificList;             // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.11 SCHED_UL_MAC_CTRL_INFO_REQ
 */
struct SCHED_UL_MAC_CTRL_INFO_REQ
{
    uint16_t sfn_sf;                                       // 0..16377  bit 0-3 SF, bit 4-13 SFN

    uint8_t nr_macCE_List;                                 // 0..MAX_MAC_CE_LIST
    struct  macCE_ListElement macCE_List[MAX_MAC_CE_LIST]; // See 4.3.14

    uint8_t nr_vendorSpecificList;                         // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.12 SCHED_UL_CQI_INFO_REQ
 */
struct SCHED_UL_CQI_INFO_REQ
{
    uint16_t sfn_sf;               // 0..16377  bit 0-3 SF, bit 4-13 SFN

    struct ulCQI ulCqi;            //  see 4.3.29

    uint8_t nr_vendorSpecificList; // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

/**
 * \brief See section 4.2.13 SCHED_UL_CONFIG_IND
 */
struct SCHED_UL_CONFIG_IND
{
    uint8_t nr_dciList;                                // 0..MAX_DCI_LIST
    uint8_t nr_phichList;                              // 0..MAX_PHICH_LIST

    struct ulDciListElement dciList [MAX_DCI_LIST];    // See 4.3.2
    struct phichListElement phichList[MAX_PHICH_LIST]; // See 4.3.7

    uint8_t nr_vendorSpecificList;                     // 0..MAX_SCHED_CFG_LIST
    uint8_t vendorSpecificList [0];
};

#endif /* _SCHED_SAP_H_ */

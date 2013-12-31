#ifndef _FAPI_CL_CPHY_SAP_H_
#define _FAPI_CL_CPHY_SAP_H_

#include <l1_l2_iface.h>

/**
   PHY controll SAP as defined in SmallCell reference
   architecture, P5 - the PHY mode control interface

 CFAPI SAP Messages:
   FAPI_L1_PARAM_REQ       = 0x00, // See Section 3.2.1.1
   FAPI_L1_PARAM_RESP      = 0x01, // See Section 3.2.1.2
   FAPI_L1_CONFIG_REQ      = 0x02, // See Section 3.2.2.1
   FAPI_L1_CONFIG_RESP     = 0x03, // See Section 3.2.2.2
   FAPI_L1_START_REQ       = 0x04, // See Section 3.2.4.1
   FAPI_L1_STOP_REQ        = 0x05, // See Section 3.2.5.1
   FAPI_L1_STOP_IND        = 0x06, // See Section 3.2.5.2

*/

struct lte_cphy_config {
    uint16_t DuplexingMode;
    /* PCFICH Power Offset            2 */
    uint16_t PCFICH_PowerOffset;
    /* P-B                            3 */
    uint16_t P_B;
    /* DL Cyclic Prefix Type          4 */
    uint16_t DL_CyclicPrefixType;
    /* UL Cyclic Prefix Type          5 */
    uint16_t UL_CyclicPrefixType;
    /* RF Config                      All TLVs in this grouping */
    uint16_t DownlinkChannelBandwidth;
    uint16_t UplinkChannelBandwidth;
    uint16_t ReferenceSignalPower;
    uint16_t TxAntennaPorts;
    uint16_t RxAntennaPorts;
    /* PHICH Config                   All TLVs in this grouping */
    uint16_t PHICH_Resource;
    uint16_t PHICH_Duration;
    uint16_t PHICH_PowerOffset;
    /* SCH Config                     All TLVs in this grouping */
    uint16_t PrimarySynchronizationSignal_EPRE_EPRERS;
    uint16_t SecondarySynchronizationSignal_EPRE_EPRERS;
    uint16_t PhysicalCell_ID;
    /* PRACH Config                   All TLVs in this grouping */
    uint16_t ConfigurationIndex;
    uint16_t RootSequenceIndex;
    uint16_t ZeroCorrelationZoneConfiguration;
    uint16_t HighSpeedFlag;
    uint16_t FrequencyOffset;
    /* PUSCH Config                   All TLVs in this grouping */
    uint16_t HoppingMode;
    uint16_t HoppingOffset;
    uint16_t NumberOfSubBands;
    /* PUCCH Config                   All TLVs in this grouping */
    uint16_t Delta_PUCCH_Shift;
    uint16_t N_CQI_RB;
    uint16_t N_AN_CS;
    uint16_t N1Pucch_AN;
    /* SRS Config                     All TLVs in this grouping */
    uint16_t SRS_BandwidthConfiguration;
#if FAPI_TDD_MODE
    // TDD only
    uint16_t MaxUpPTS;
#endif
    uint16_t SRS_SubframeConfiguration;
    uint16_t SRS_AckNack_SRS_SimultaneousTransmission;
    /* Uplink Reference Signal Config All TLVs in this grouping */
    uint16_t Uplink_RS_Hopping;
    uint16_t GroupAssignmentDeltaSequence_ShiftPattern;
    uint16_t CyclicShift1For_DMRS;
    /* Data Report Mode               50 */
    uint16_t DataReportMode;
    uint16_t SFN_SF;
};


/** 
 * Return pointer to current CPHY configuration
 * 
 * @return chpy_config
 */
struct lte_cphy_config * lte_cphy_get_config (void);

/** 
 * Module initialization
 * 
 * 
 * @return OK if 0
 */
int lte_cphy_sap_init (void);

/** 
 * Handle CPHY SAP API request.
 * STOP_REQ and STOP_REQ processed asynchronosly if return is 0,
 * otherwise response parameter can be used imidiatelly.
 * 
 * @param request  - request
 * @param response - place to put response
 * 
 * @return 0  OK and response need to be send,
 *         -ENOMEM on memory allocation
 *         -1 error to interface PHY
 */
int lte_cphy_request(fapi_l1_msg_hdr_t* request, fapi_l1_msg_hdr_t* response);

/** 
 * Manage CPHY state from outside, need for transition in RUNNING state
 * 
 * @param phy_state - new state
 */
void lte_cphy_set_state (FAPI_PHY_State_e phy_state);

/**
 * Used to get PHY state
 * 
 * @return FAPI_PHY_State_e
 */
FAPI_PHY_State_e lte_cphy_get_state (void);

#endif /* _FAPI_CL_CPHY_SAP_H_ */

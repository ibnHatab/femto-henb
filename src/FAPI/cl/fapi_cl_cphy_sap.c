
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef MX_ARM_ENABLED
#include "appprintf.h"
#endif

#include <LtePhyL2Api.h>

#include <l1_types.h>
#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

#include "fapi_cl.h"
#include "fapi_cl_conv.h"

#include "fapi_cl_cphy_sap.h"

/**
   Defines TLV constrain in regards of CPHY state;
   Encode value offset in config structure
 */
struct supported_tlv {
    FAPI_L1_TLV_TAG_e tag; // TLV tag
    size_t config_offset;  // offset in config structure
    int state;             // can be used in STATE bitmap
} supported_tlvs [] = {
    { FAPI_L1_DuplexingMode                              ,
      offsetof(struct lte_cphy_config, DuplexingMode),
      PHY_STATE_IDLE},
    { FAPI_L1_PCFICH_PowerOffset                         , offsetof(struct lte_cphy_config, PCFICH_PowerOffset),                         PHY_STATE_IDLE},
    { FAPI_L1_P_B                                        , offsetof(struct lte_cphy_config, P_B),                                        PHY_STATE_IDLE},
    { FAPI_L1_DL_CyclicPrefixType                        , offsetof(struct lte_cphy_config, DL_CyclicPrefixType),                        PHY_STATE_IDLE},
    { FAPI_L1_UL_CyclicPrefixType                        , offsetof(struct lte_cphy_config, UL_CyclicPrefixType),                        PHY_STATE_IDLE},
    { FAPI_L1_DownlinkChannelBandwidth                   , offsetof(struct lte_cphy_config, DownlinkChannelBandwidth),                   PHY_STATE_IDLE},
    { FAPI_L1_UplinkChannelBandwidth                     , offsetof(struct lte_cphy_config, UplinkChannelBandwidth),                     PHY_STATE_IDLE},
    { FAPI_L1_ReferenceSignalPower                       , offsetof(struct lte_cphy_config, ReferenceSignalPower),                       PHY_STATE_IDLE},
    { FAPI_L1_TxAntennaPorts                             , offsetof(struct lte_cphy_config, TxAntennaPorts),                             PHY_STATE_IDLE},
    { FAPI_L1_RxAntennaPorts                             , offsetof(struct lte_cphy_config, RxAntennaPorts),                             PHY_STATE_IDLE},
    { FAPI_L1_PHICH_Resource                             , offsetof(struct lte_cphy_config, PHICH_Resource),                             PHY_STATE_IDLE},
    { FAPI_L1_PHICH_Duration                             , offsetof(struct lte_cphy_config, PHICH_Duration),                             PHY_STATE_IDLE},
    { FAPI_L1_PHICH_PowerOffset                          , offsetof(struct lte_cphy_config, PHICH_PowerOffset),                          PHY_STATE_IDLE},
    { FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS   , offsetof(struct lte_cphy_config, PrimarySynchronizationSignal_EPRE_EPRERS),   PHY_STATE_IDLE},
    { FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS , offsetof(struct lte_cphy_config, SecondarySynchronizationSignal_EPRE_EPRERS), PHY_STATE_IDLE},
    { FAPI_L1_PhysicalCell_ID                            , offsetof(struct lte_cphy_config, PhysicalCell_ID),                            PHY_STATE_IDLE},
    { FAPI_L1_ConfigurationIndex                         , offsetof(struct lte_cphy_config, ConfigurationIndex),                         PHY_STATE_IDLE},
    { FAPI_L1_RootSequenceIndex                          , offsetof(struct lte_cphy_config, RootSequenceIndex),                          PHY_STATE_IDLE},
    { FAPI_L1_ZeroCorrelationZoneConfiguration           , offsetof(struct lte_cphy_config, ZeroCorrelationZoneConfiguration),           PHY_STATE_IDLE},
    { FAPI_L1_HighSpeedFlag                              , offsetof(struct lte_cphy_config, HighSpeedFlag),                              PHY_STATE_IDLE},
    { FAPI_L1_FrequencyOffset                            , offsetof(struct lte_cphy_config, FrequencyOffset),                            PHY_STATE_IDLE},
    { FAPI_L1_HoppingMode                                , offsetof(struct lte_cphy_config, HoppingMode),                                PHY_STATE_IDLE},
    { FAPI_L1_HoppingOffset                              , offsetof(struct lte_cphy_config, HoppingOffset),                              PHY_STATE_IDLE},
    { FAPI_L1_NumberOfSubBands                           , offsetof(struct lte_cphy_config, NumberOfSubBands),                           PHY_STATE_IDLE},
    { FAPI_L1_Delta_PUCCH_Shift                          , offsetof(struct lte_cphy_config, Delta_PUCCH_Shift),                          PHY_STATE_IDLE},
    { FAPI_L1_N_CQI_RB                                   , offsetof(struct lte_cphy_config, N_CQI_RB),                                   PHY_STATE_IDLE},
    { FAPI_L1_N_AN_CS                                    , offsetof(struct lte_cphy_config, N_AN_CS),                                    PHY_STATE_IDLE},
    { FAPI_L1_N1Pucch_AN                                 , offsetof(struct lte_cphy_config, N1Pucch_AN),                                 PHY_STATE_IDLE},
    { FAPI_L1_SRS_BandwidthConfiguration                 , offsetof(struct lte_cphy_config, SRS_BandwidthConfiguration),                 PHY_STATE_IDLE},
#if FAPI_TDD_MODE
    { FAPI_L1_MaxUpPTS                                   , offsetof(struct lte_cphy_config, MaxUpPTS),                                   PHY_STATE_IDLE},
#endif
    { FAPI_L1_SRS_SubframeConfiguration                  , offsetof(struct lte_cphy_config, SRS_SubframeConfiguration),                  PHY_STATE_IDLE},
    { FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission   , offsetof(struct lte_cphy_config, SRS_AckNack_SRS_SimultaneousTransmission),   PHY_STATE_IDLE},
    { FAPI_L1_Uplink_RS_Hopping                          , offsetof(struct lte_cphy_config, Uplink_RS_Hopping),                          PHY_STATE_IDLE},
    { FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern  , offsetof(struct lte_cphy_config, GroupAssignmentDeltaSequence_ShiftPattern),  PHY_STATE_IDLE},
    { FAPI_L1_CyclicShift1For_DMRS                       , offsetof(struct lte_cphy_config, CyclicShift1For_DMRS),                       PHY_STATE_IDLE},
    { FAPI_L1_DataReportMode                             , offsetof(struct lte_cphy_config, DataReportMode),                             PHY_STATE_IDLE},
    { FAPI_L1_SFN_SF                                     , offsetof(struct lte_cphy_config, SFN_SF),                                     PHY_STATE_IDLE | PHY_STATE_RUNNING },
};
const size_t supported_tlvs_size = sizeof (supported_tlvs) / sizeof (struct supported_tlv);

static __inline int find_supported_tlv (FAPI_L1_TLV_TAG_e tag)
{
    int i;
    for (i = 0; i < supported_tlvs_size; i++)
        if (tag == supported_tlvs[i].tag)
            return i;
    return -1;
}

int check_supported_tlvs(l1_tlv_word_t tlvs[], uint8_t numberOfTLVs,
                         FAPI_PHY_State_e state,
                         uint8_t *tlv_errors)
{
    int ret = 0, i, idx;
    for (i = 0; i < numberOfTLVs; i++) {
        if ((idx = find_supported_tlv(tlvs[i].tag)) < 0){
            tlv_errors [tlvs[i].tag] |= FAPI_TLV_UNSUPPORTED;
            ret ++;
        } else {
            if (! (supported_tlvs[idx].state & state)) {
                tlv_errors [tlvs[i].tag] |= FAPI_TLV_UNSUPPORTED;
                ret ++;            
            }
        }
    }
    
    return ret;
}


/**
 * storing reflected phy state and capabiliies
 */
struct cphy_sap_info {
    FAPI_PHY_State_e phy_state;
    struct lte_cphy_config config;
} sap_info =
{
     /* .phy_state =  */ PHY_STATE_IDLE,
     /* .config                                         = */ {
        /* Duplexing Mode                 1 */
        /* .DuplexingMode                              = */ FAPI_L1_DM_FDD,
        /* PCFICH Power Offset            2 */
        /* .PCFICH_PowerOffset                         = */ DB_TO_FAPI(1),
        /* P-B                            3 */
        /* .P_B                                        = */ 0,
        /* DL Cyclic Prefix Type          4 */
        /* .DL_CyclicPrefixType                        = */ CP_NORMAL,
        /* UL Cyclic Prefix Type          5 */
        /* .UL_CyclicPrefixType                        = */ CP_NORMAL,
        /* +RF Config                      All TLVs in this grouping */
        /* .DownlinkChannelBandwidth                   = */ 50,
        /* .UplinkChannelBandwidth                     = */ 15,
        /* .ReferenceSignalPower                       = */ 0,
        /* .TxAntennaPorts                             = */ 2,
        /* .RxAntennaPorts                             = */ 2,
        /* PHICH Config                   All TLVs in this grouping */
        /* .PHICH_Resource                             = */ PHICH_R_ONE_SIXTH,
        /* .PHICH_Duration                             = */ PHICH_D_NORMAL,
        /* .PHICH_PowerOffset                          = */ 0,
        /* SCH Config                     All TLVs in this grouping */
        /* .PrimarySynchronizationSignal_EPRE_EPRERS   = */ 0,
        /* .SecondarySynchronizationSignal_EPRE_EPRERS = */ 0,
        /* .PhysicalCell_ID                            = */ 0,
        /* PRACH Config                   All TLVs in this grouping */
        /* .ConfigurationIndex                         = */ 0x0C,
        /* .RootSequenceIndex                          = */ 0x16,
        /* .ZeroCorrelationZoneConfiguration           = */ 1,
        /* .HighSpeedFlag                              = */ HS_UNRESTRICTED_SET,
        /* .FrequencyOffset                            = */ 0,
        /* PUSCH Config                   All TLVs in this grouping */
        /* .HoppingMode                                = */ HM_INTER_SF,
        /* .HoppingOffset                              = */ 1,
        /* .NumberOfSubBands                           = */ 2,
        /* PUCCH Config                   All TLVs in this grouping */
        /* .Delta_PUCCH_Shift                          = */ 1,
        /* .N_CQI_RB                                   = */ 0,
        /* .N_AN_CS                                    = */ 0,
        /* .N1Pucch_AN                                 = */ 0,
        /* SRS Config                     All TLVs in this grouping */
        /* .SRS_BandwidthConfiguration                 = */ 0,
#if FAPI_TDD_MODE
        // TDD only
        /* .MaxUpPTS                                   = */ MAX_UP_PTS_DISABLED,
#endif
        /* .SRS_SubframeConfiguration                  = */ 15,
        /* .SRS_AckNack_SRS_SimultaneousTransmission   = */ SRS_NO_SIMULTANEOUS_TRANSMISSION,
        /* Uplink Reference Signal Config All TLVs in this grouping */
        /* .Uplink_RS_Hopping                          = */ RS_NO_HOPPING,
        /* .GroupAssignmentDeltaSequence_ShiftPattern  = */ 0,
        /* .CyclicShift1For_DMRS                       = */ 0,
        /* Data Report Mode               50 */
        /* .DataReportMode                             = */ 1,
        /* .SFN_SF                                     = */ 0
    }
};

const size_t lte_cphy_config_size = sizeof(struct lte_cphy_config) / sizeof(uint16_t);

struct lte_cphy_config * lte_cphy_get_config (void) {
    return &sap_info.config;
}

int lte_cphy_sap_init (void)
{
    sap_info.phy_state = PHY_STATE_IDLE;
    return 0;
}

void lte_cphy_set_state (FAPI_PHY_State_e phy_state)
{
    sap_info.phy_state = phy_state;
}

FAPI_PHY_State_e lte_cphy_get_state (void)
{
    return sap_info.phy_state;
}


int lte_cphy_decode_config (struct lte_cphy_config * config,
                            l1_tlv_word_t* tlvs, uint8_t numberOfTLVs)
{
    int errors = 0;
    int i;

    for (i = 0; i<numberOfTLVs; i++)
    {
        uint16_t tlv_val = tlvs[i].val.word;

        switch (tlvs[i].tag) {
            /* Duplexing Mode                 1 */
        case FAPI_L1_DuplexingMode: config->DuplexingMode = tlv_val;                         break;
            /* PCFICH Power Offset            2 */
        case FAPI_L1_PCFICH_PowerOffset: config->PCFICH_PowerOffset = tlv_val;               break;
            /* P-B                            3 */
        case FAPI_L1_P_B: config->P_B = tlv_val;                                             break;
            /* DL Cyclic Prefix Type          4 */
        case FAPI_L1_DL_CyclicPrefixType: config->DL_CyclicPrefixType = tlv_val;             break;
            /* UL Cyclic Prefix Type          5 */
        case FAPI_L1_UL_CyclicPrefixType: config->UL_CyclicPrefixType = tlv_val;             break;
            /* +RF Config                      All TLVs in this grouping */
        case FAPI_L1_DownlinkChannelBandwidth: config->DownlinkChannelBandwidth = tlv_val;   break;
        case FAPI_L1_UplinkChannelBandwidth: config->UplinkChannelBandwidth = tlv_val;       break;
        case FAPI_L1_ReferenceSignalPower: config->ReferenceSignalPower = tlv_val;           break;
        case FAPI_L1_TxAntennaPorts: config->TxAntennaPorts = tlv_val;                       break;
        case FAPI_L1_RxAntennaPorts: config->RxAntennaPorts = tlv_val;                   break;
            /* PHICH Config                   All TLVs in this grouping */
        case FAPI_L1_PHICH_Resource: config->PHICH_Resource = tlv_val;                       break;
        case FAPI_L1_PHICH_Duration: config->PHICH_Duration = tlv_val;                       break;
        case FAPI_L1_PHICH_PowerOffset: config->PHICH_PowerOffset = tlv_val;                 break;
            /* SCH Config                     All TLVs in this grouping */
        case FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS:
            config->PrimarySynchronizationSignal_EPRE_EPRERS = tlv_val;                      break;
        case FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS:
            config->SecondarySynchronizationSignal_EPRE_EPRERS = tlv_val;                    break;
        case FAPI_L1_PhysicalCell_ID: config->PhysicalCell_ID = tlv_val;                     break;
            /* PRACH Config                   All TLVs in this grouping */
        case FAPI_L1_ConfigurationIndex: config->ConfigurationIndex = tlv_val;               break;
        case FAPI_L1_RootSequenceIndex: config->RootSequenceIndex = tlv_val;                 break;
        case FAPI_L1_ZeroCorrelationZoneConfiguration:
            config->ZeroCorrelationZoneConfiguration = tlv_val;                              break;
        case FAPI_L1_HighSpeedFlag: config->HighSpeedFlag = tlv_val;                         break;
        case FAPI_L1_FrequencyOffset: config->FrequencyOffset = tlv_val;                     break;
            /* PUSCH Config                   All TLVs in this grouping */
        case FAPI_L1_HoppingMode: config->HoppingMode = tlv_val;                             break;
        case FAPI_L1_HoppingOffset: config->HoppingOffset = tlv_val;                         break;
        case FAPI_L1_NumberOfSubBands: config->NumberOfSubBands = tlv_val;                   break;
            /* PUCCH Config                   All TLVs in this grouping */
        case FAPI_L1_Delta_PUCCH_Shift: config->Delta_PUCCH_Shift = tlv_val;                 break;
        case FAPI_L1_N_CQI_RB: config->N_CQI_RB = tlv_val;                                   break;
        case FAPI_L1_N_AN_CS: config->N_AN_CS = tlv_val;                                     break;
        case FAPI_L1_N1Pucch_AN: config->N1Pucch_AN = tlv_val;                               break;
            /* SRS Config                     All TLVs in this grouping */
        case FAPI_L1_SRS_BandwidthConfiguration:
            config->SRS_BandwidthConfiguration = tlv_val;       break;
#if FAPI_TDD_MODE
            // TDD only
        case FAPI_L1_MaxUpPTS: config->MaxUpPTS = tlv_val;                                   break;
#endif
        case FAPI_L1_SRS_SubframeConfiguration:
            config->SRS_SubframeConfiguration = tlv_val; break;
        case FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission:
            config->SRS_AckNack_SRS_SimultaneousTransmission = tlv_val;                      break;
            /* Uplink Reference Signal Config All TLVs in this grouping */
        case FAPI_L1_Uplink_RS_Hopping: config->Uplink_RS_Hopping = tlv_val;                 break;
        case FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern:
            config->GroupAssignmentDeltaSequence_ShiftPattern = tlv_val;                     break;
        case FAPI_L1_CyclicShift1For_DMRS: config->CyclicShift1For_DMRS = tlv_val;           break;
            /* Data Report Mode               50 */
        case FAPI_L1_DataReportMode: config->DataReportMode = tlv_val;                       break;
        case FAPI_L1_SFN_SF: config->SFN_SF = tlv_val;  break;
        default:
            errors ++;
        }
    }

    return errors;
}

int lte_cphy_encode_config (l1_tlv_word_t* tlvs, struct lte_cphy_config * config)
{
    int errors = 0;
    int i;
    for (i = 0; i < supported_tlvs_size; i++)
    {
        tlvs [i].tag = supported_tlvs [i].tag;
        tlvs [i].len = 2;
        tlvs[i].val.word = *(uint16_t*)((uint8_t*) config + supported_tlvs [i].config_offset);
    }
    return errors;
}

int lte_cphy_request(fapi_l1_msg_hdr_t* request, fapi_l1_msg_hdr_t* response)
{
    int ret = 0;
    CLDBG("cl takes action FAPI_L1_[%d], PHY_STATE_[%d]", request->msgType, sap_info.phy_state);
    switch (request->msgType)
    {
    case FAPI_L1_PARAM_REQ: {
        /**
           IDLE       The PHY indicates which capabilities it supports
           CONFIGURED The PHY returns its current configuration
           RUNNING    The PHY returns invalid state
        */
        //        struct fapi_l1_param_response* response = 0;

        
        switch (sap_info.phy_state) {
        case PHY_STATE_IDLE:
        case PHY_STATE_CONFIGURED: // falltrought
        {                
            if (l1_alloc_param_response (MSG_OK, (struct fapi_l1_param_response*)response,
                                         lte_cphy_config_size) < 0)
                return -ENOMEM;
            lte_cphy_encode_config (((struct fapi_l1_param_response*)response)->tlvs,
                                    lte_cphy_get_config());
        } break;
        case PHY_STATE_RUNNING:
            if (l1_alloc_param_response (MSG_INVALID_STATE,
                                         (struct fapi_l1_param_response*)response, 0) < 0)
                return -ENOMEM;
            break;
        }
    } break;
    case FAPI_L1_CONFIG_REQ: {
        /**
           PHY in IDLE state: The CONFIG.request message did not contain all the mandatory
           TLVs. It should be resent with the correct information. The PHY remains in the
           IDLE state and all received TLVs have been ignored by the PHY.

           PHY in CONFIGURED state: The CONFIG.request message included invalid, or
           unsupported, TLVs. It should be resent with the correct information. The PHY
           remains in the CONFIGURED state, but all received TLVs have been ignored by the
           PHY.

           PHY in RUNNING state:
           The CONFIG.request message included invalid TLVs, or TLVs which can not be
           modified when the PHY is running.  It should be resent with the correct
           information. The PHY remains in the RUNNING state, but all received TLVs have
           been ignored by the PHY.
        */
        uint8_t tlv_errors [FAPI_L1_TLV_MAX] = { FAPI_TLV_NOERROR };
        struct fapi_l1_config_request *fapi =
            (struct fapi_l1_config_request *) request;

        CLDBG("CPHY CONFIG_REQ in %d, %d tlv's",
              sap_info.phy_state, fapi->numberOfTLVs);

        switch (sap_info.phy_state) {
        case PHY_STATE_IDLE:
        case PHY_STATE_CONFIGURED: {
            PINITPARM apiInit = 0;
            PGENMSGDESC genHrd = (PGENMSGDESC) fapi_cl_phy_alloc_msg ();

            if (!genHrd)
                return -ENOMEM;

            genHrd->msgSpecific = sizeof(INITPARM);
            genHrd->msgType = PHY_INIT_REQ;
            genHrd->phyEntityId = 0;
            apiInit = (PINITPARM) &genHrd[1];


            if (sap_info.phy_state == PHY_STATE_IDLE &&
                fapi->numberOfTLVs != supported_tlvs_size)
                ret |= FAPI_TLV_MISSING;

            ret |= conv_apiInit_from_CONFIG_request((struct fapi_l1_config_request*) request,
                                                     apiInit, tlv_errors);

            if (ret) {
                if (l1_alloc_config_response (MSG_INVALID_CONFIG,
                                              (struct fapi_l1_config_response *)response,
                                              fapi, tlv_errors) < 0)
                    return -ENOMEM;
            } else {
                // configure PHY
                CLDBG("PHY_INIT_REQ : cl -> phy");
                if (fapi_cl_phy_send_msg (genHrd->msgType,
                                          genHrd,
                                          genHrd->msgSpecific + sizeof(GENMSGDESC)) < 0) {

                    l1_alloc_config_response (MSG_INVALID_CONFIG,
                                              (struct fapi_l1_config_response *)response,
                                              NULL, NULL);
                } else {
                    // save TLVS
                    lte_cphy_decode_config (&sap_info.config, fapi->tlvs, fapi->numberOfTLVs);
                    
                    l1_alloc_config_response (MSG_OK,
                                              (struct fapi_l1_config_response *)response,
                                              NULL, NULL);
                    
                    if (sap_info.phy_state == PHY_STATE_IDLE)
                        sap_info.phy_state = PHY_STATE_CONFIGURED;

                }
            }
        } break;
        case PHY_STATE_RUNNING: {
            ret = check_supported_tlvs(fapi->tlvs, fapi->numberOfTLVs,
                                       lte_cphy_get_state(), tlv_errors);
            if (ret) {
                if (l1_alloc_config_response (MSG_INVALID_CONFIG,
                                              (struct fapi_l1_config_response *)response,
                                              fapi, tlv_errors) < 0)
                    return -ENOMEM;
            } else {
                int idx = find_tlv_idx(FAPI_L1_SFN_SF, fapi->tlvs, fapi->numberOfTLVs);
                if (idx >= 0) {
                    sap_info.config.SFN_SF = fapi->tlvs[idx].val.word;
                    cl_set_sfnsf(sap_info.config.SFN_SF);
                }

                if (l1_alloc_config_response (MSG_OK,
                                              (struct fapi_l1_config_response *)response,
                                              NULL, NULL) < 0)
                    return -ENOMEM;
            }
        } break;
        }
    } break;

    case FAPI_L1_START_REQ: {
        switch (sap_info.phy_state) {
        case PHY_STATE_IDLE:
        case PHY_STATE_RUNNING: {
            if (l1_alloc_error_indication (MSG_INVALID_STATE, FAPI_L1_START_REQ,
                                           (struct fapi_l1_error_indication *)response) < 0)
                return -ENOMEM;
        } break;
        case PHY_STATE_CONFIGURED: {
            PSTARTREQ startReq = (PSTARTREQ) fapi_cl_phy_alloc_msg ();
            if (!startReq)
                return -ENOMEM;

            startReq->msgType = PHY_START_REQ;
            startReq->phyEntityId = 0;


#if CL_PHY_TIMER_RESOLUTION > 0
            startReq->mode   = AUTO_TIMER;
            startReq->count  = CL_PHY_TIMER_MODE_COUNT; // mlog
            startReq->period = CL_PHY_TIMER_RESOLUTION;
#else	/* !( CL_PHY_TIMER_RESOLUTION > 0 ) */
            startReq->mode  = AUTO_PCIE;
            startReq->count  = 0;
            startReq->period = 0;
#endif	/* !( CL_PHY_TIMER_RESOLUTION > 0 ) */

            if (fapi_cl_phy_send_msg (startReq->msgType,
                                      startReq,
                                      sizeof(STARTREQ)) < 0)
                return -1;
            CLDBG("PHY_START_REQ : cl -> phy");
        } break;
        }
    } break;

    case FAPI_L1_STOP_REQ: {
        switch (sap_info.phy_state) {
        case PHY_STATE_IDLE:
        case PHY_STATE_CONFIGURED: {
            if (l1_alloc_error_indication (MSG_INVALID_STATE, FAPI_L1_START_REQ,
                                           (struct fapi_l1_error_indication *)response) < 0)
                return -ENOMEM;
        } break;
        case PHY_STATE_RUNNING: {
            PSTARTREQ stopReq = (PSTARTREQ) fapi_cl_phy_alloc_msg ();
            if (!stopReq)
                return -ENOMEM;

            stopReq->mode = 0;
            stopReq->msgType = PHY_STOP_REQ;
            stopReq->phyEntityId = 0;

            if (fapi_cl_phy_send_msg(stopReq->msgType,
                                     stopReq,
                                     sizeof(STARTREQ)))
                return -1;
            CLDBG("PHY_STOP_REQ : cl -> phy");

            if (l1_alloc_stop_indication ((struct fapi_l1_stop_indication*)response) < 0)
                return -ENOMEM;

            sap_info.phy_state = PHY_STATE_CONFIGURED;
        } break;
        }
    } break;

    default:
        BUG_ON(0 && "Unknown msgType");
    }

    return 0;
}

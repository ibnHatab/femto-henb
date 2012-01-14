
#include "l1_types.h"
#include "l1_l2_iface.h"

/* 3.2  Configuration Messages Validaion */

/**
 * @brief Database for mmessage onsistency validation
 */
struct FAPI_L1_MessageValidator {
    FAPI_L1_State_e state;           // current state
    FAPI_L1_MessageTypeID_e message; // incomin message
    uint8_t validTags [FAPI_L1_MAX_TLV_PER_MESSAGE];
};

#define RL_CONFIG_ALL_TLVS_IN_THIS_GROUPING     \
    FAPI_L1_DownlinkChannelBandwidth,           \
        FAPI_L1_UplinkChannelBandwidth,         \
        FAPI_L1_ReferenceSignalPower,           \
        FAPI_L1_TxAntennaPorts,                 \
        FAPI_L1_Rx_Antenna_Ports

#define PHICH_CONFIG_ALL_TLVS_IN_THIS_GROUPING  \
    FAPI_L1_PHICH_Resource,                     \
        FAPI_L1_PHICH_Duration,                 \
        FAPI_L1_PHICH_PowerOffset

#define SCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING                \
    FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS,       \
        FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS, \
        FAPI_L1_PhysicalCell_ID

#define PRACH_CONFIG_ALL_TLVS_IN_THIS_GROUPING      \
    FAPI_L1_ConfigurationIndex,                     \
        FAPI_L1_RootSequenceIndex,                  \
        FAPI_L1_ZeroCorrelationZoneConfiguration,   \
        FAPI_L1_HighSpeedFlag,                      \
        FAPI_L1_FrequencyOffset

#define PUSCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING  \
    FAPI_L1_HoppingMode,                        \
        FAPI_L1_HoppingOffset,                  \
        FAPI_L1_NumberOfSubBands

#define PUCCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING  \
    FAPI_L1_Delta_PUCCH_Shift,                  \
        FAPI_L1_N_CQI_RB,                       \
        FAPI_L1_N_AN_CS,                        \
        FAPI_L1_N1Pucch_AN

#define SRS_CONFIG_ALL_TLVS_IN_THIS_GROUPING                \
    FAPI_L1_BandwidthConfiguration,                         \
        FAPI_L1_MaxUpPTS,                                   \
        FAPI_L1_SRS_SubframeConfiguration,                  \
        FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission

#define UPLINK_REFERENCE_SIGNAL_CONFIG_ALL_TLVS_IN_THIS_GROUPING    \
    FAPI_L1_Uplink_RS_Hopping,                                      \
        FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern,          \
        FAPI_L1_CyclicShift1For_DMRS

#define TDD_FRAME_STRUCTURE_CONFIG_ALL_TLVS_IN_THIS_GROUPING    \
    FAPI_L1_SubframeAssignment,                                 \
        FAPI_L1_SpecialSubframePatterns,                        \
        FAPI_L1_DownlinkBandwidthSupport,                       \
        FAPI_L1_UplinkBandwidthSupport,                         \
        FAPI_L1_DownlinkModulationSupport,                      \
        FAPI_L1_UplinkModulationSupport,                        \
        FAPI_L1_PHY_AntennaCapability,                          \
        FAPI_L1_DataReportMode,                                 \
        FAPI_L1_SFN_SF,                                         \
        FAPI_L1_PHY_State

  // Table 6: TLVs included in PARAM.response for TDD when PHY is in IDLE state
#define PARAM_RESPONSE_TDD_PHY_IDLE_STATE_LIST {    \
        FAPI_L1_PHY_State,                          \
            FAPI_L1_DownlinkBandwidthSupport,       \
            FAPI_L1_UplinkBandwidthSupport,         \
            FAPI_L1_DownlinkModulationSupport,      \
            FAPI_L1_UplinkModulationSupport,        \
            FAPI_L1_PHY_AntennaCapability           \
            }

// Table 7: TLVs included in PARAM.response for TDD when PHY is in CONFIGURED state
#define PARAM_RESPONSE_TDD_PHY_CONFIGURED_STATE_LIST {                  \
        FAPI_L1_PHY_State,                                              \
            FAPI_L1_DownlinkBandwidthSupport,                           \
            FAPI_L1_UplinkBandwidthSupport,                             \
            FAPI_L1_DownlinkModulationSupport,                          \
            FAPI_L1_UplinkModulationSupport,                            \
            FAPI_L1_PHY_AntennaCapability,                              \
            FAPI_L1_DuplexingMode,                                      \
            FAPI_L1_PCFICH_PowerOffset,                                 \
            FAPI_L1_P_B,                                                \
            FAPI_L1_DL_CyclicPrefixType,                                \
            FAPI_L1_UL_CyclicPrefixType,                                \
            FAPI_L1_DataReportMode,                                     \
                                                                        \
            RL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                        \
            PHICH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            PRACH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUSCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUCCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SRS_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            UPLINK_REFERENCE_SIGNAL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,   \
            TDD_FRAME_STRUCTURE_CONFIG_ALL_TLVS_IN_THIS_GROUPING        \
            }

// Table 12: TLVs included in CONFIG.request for TDD for IDLE and CONFIGURED states
// Mandatory TLVs – These must be included when the PHY is in IDLE state, they may
// also be included when the PHY is in CONFIGURED state.
// Optional TLVs – These may be included when the PHY is in either IDLE or CONFIGURED
// state. Currently there are no optional TLVs.
#define CONFIG_REQUEST_TDD_IDLE_AND_CONFIGURED_STATE_LIST {             \
        FAPI_L1_DuplexingMode,                                          \
            FAPI_L1_PCFICH_PowerOffset,                                 \
            FAPI_L1_P_B,                                                \
            FAPI_L1_DL_CyclicPrefixType,                                \
            FAPI_L1_UL_CyclicPrefixType,                                \
            FAPI_L1_DataReportMode,                                     \
                                                                        \
            RL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                        \
            PHICH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            PRACH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUSCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUCCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SRS_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            UPLINK_REFERENCE_SIGNAL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,   \
            TDD_FRAME_STRUCTURE_CONFIG_ALL_TLVS_IN_THIS_GROUPING        \
            }

// Table 8: TLVs included in PARAM.response for FDD when PHY is in IDLE state
#define PARAM_RESPONSE_FDD_PHY_IDLE_STATE_LIST {    \
        FAPI_L1_PHY_State,                          \
            FAPI_L1_DownlinkBandwidthSupport,       \
            FAPI_L1_UplinkBandwidthSupport,         \
            FAPI_L1_DownlinkModulationSupport,      \
            FAPI_L1_UplinkModulationSupport,        \
            FAPI_L1_PHY_AntennaCapability,          \
            }

// Table 9: TLVs included in PARAM.response for FDD when PHY is in CONFIGURED state
#define PARAM_RESPONSE_FDD_PHY_CONFIGURED_STATE_LIST {                  \
        FAPI_L1_PHY_State,                                              \
            FAPI_L1_DownlinkBandwidthSupport,                           \
            FAPI_L1_UplinkBandwidthSupport,                             \
            FAPI_L1_DownlinkModulationSupport,                          \
            FAPI_L1_UplinkModulationSupport,                            \
            FAPI_L1_PHY_AntennaCapability,                              \
            FAPI_L1_DuplexingMode,                                      \
            FAPI_L1_PCFICH_PowerOffset,                                 \
            FAPI_L1_P_B,                                                \
            FAPI_L1_DL_CyclicPrefixType,                                \
            FAPI_L1_UL_CyclicPrefixType,                                \
            FAPI_L1_DataReportMode,                                     \
                                                                        \
            RL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                        \
            PHICH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            PRACH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUSCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUCCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SRS_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            UPLINK_REFERENCE_SIGNAL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,   \
            }

// Table 13: TLVs included in CONFIG.request for FDD for IDLE and CONFIGURED states
// Mandatory TLVs – These must be included when the PHY is in IDLE state, they may
// also be included when the PHY is in CONFIGURED state.
// Optional TLVs – These may be included when the PHY is in either IDLE or CONFIGURED
// state. Currently there are no optional TLVs.
#define CONFIG_REQUEST_FDD_IDLE_AND_CONFIGURED_STATE_LIST {             \
        FAPI_L1_DuplexingMode,                                          \
            FAPI_L1_PCFICH_PowerOffset,                                 \
            FAPI_L1_P_B,                                                \
            FAPI_L1_DL_CyclicPrefixType,                                \
            FAPI_L1_UL_CyclicPrefixType,                                \
            FAPI_L1_DataReportMode,                                     \
                                                                        \
            RL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                        \
            PHICH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            PRACH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUSCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            PUCCH_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                     \
            SRS_CONFIG_ALL_TLVS_IN_THIS_GROUPING,                       \
            UPLINK_REFERENCE_SIGNAL_CONFIG_ALL_TLVS_IN_THIS_GROUPING,   \
            }

// Table 14: TLVs permitted in CONFIG.request in the RUNNING state
//  Other TLVs are FFS (For F**** Sake???)
#define CONFIG_REQUEST_RUNNING_STATE_STATE_LIST {   \
        FAPI_L1_SFN_SF                              \
            }

  // Validation table for TDD
struct FAPI_L1_MessageValidator FAPI_L1_MessageValidatorTable_TDD [] = {
    { FAPI_L1_ST_Idle, FAPI_L1_PARAM_response,
      PARAM_RESPONSE_TDD_PHY_IDLE_STATE
    },
    { FAPI_L1_ST_Configured, FAPI_L1_PARAM_response,
      PARAM_RESPONSE_TDD_PHY_CONFIGURED_STATE_LIST
    },

    { FAPI_L1_ST_Idle, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_TDD_IDLE_AND_CONFIGURED_STATE_LIST
    },
    { FAPI_L1_ST_Configured, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_TDD_IDLE_AND_CONFIGURED_STATE_LIST
    },

    { FAPI_L1_ST_Running, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_RUNNING_STATE_STATE_LIST
    },
};

  // Validation table for FDD
struct FAPI_L1_MessageValidator FAPI_L1_MessageValidatorTable_FDD [] = {
    { FAPI_L1_ST_Idle, FAPI_L1_PARAM_response,
      PARAM_RESPONSE_FDD_PHY_IDLE_STATE
    },
    { FAPI_L1_ST_Configured, FAPI_L1_PARAM_response,
      PARAM_RESPONSE_FDD_PHY_CONFIGURED_STATE_LIST
    },

    { FAPI_L1_ST_Idle, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_FDD_IDLE_AND_CONFIGURED_STATE_LIST
    },
    { FAPI_L1_ST_Configured, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_FDD_IDLE_AND_CONFIGURED_STATE_LIST
    },

    { FAPI_L1_ST_Running, FAPI_L1_CONFIG_request,
      CONFIG_REQUEST_RUNNING_STATE_STATE_LIST
    },
};

inline uint8_t* getMessageValidatorCtx (const struct FAPI_L1_MessageValidator *table,
                                        size_t table_length,
                                        FAPI_L1_State_e state,
                                        FAPI_L1_MessageTypeID_e message)
{
    int n;
    for (n = 0; n < table_length; n++)
        if (table->state == state && table->message == message)
            return table->validTags;
    return NULL;
}

void* getValidTagsForMessage (FAPI_L1_DuplexingMode_e duplexingMode,
                              FAPI_L1_State_e state,
                              FAPI_L1_MessageTypeID_e message)
{
    switch (duplexingMode) {
    case FAPI_L1_DM_TDD:
        return getMessageValidatorCtx (FAPI_L1_MessageValidatorTable_TDD,
                                       sizeof (FAPI_L1_MessageValidatorTable_TDD)
                                       / sizeof (FAPI_L1_MessageValidator),
                                       state, message);
    case FAPI_L1_DM_FDD:
        return getMessageValidatorCtx (FAPI_L1_MessageValidatorTable_FDD,
                                       sizeof (FAPI_L1_MessageValidatorTable_FDD)
                                       / sizeof (FAPI_L1_MessageValidator),
                                       state, message);
    case FAPI_L1_DM_HD_FDD:
    default:
        return NULL;
    }
}

bool isTagValig (uint8_t tag, void* validTags) {
    uint8_t *table = (uint8_t *) validTags;
    while (*table) {
        if (tag == *table)
            return true;
        table ++;
    }
    return false;
}


#include <sys/types.h>
#include <stdint.h>

#include <fapi/l1_tlv.h>

struct L1_TLV_Size_t {
    uint8_t   tag;
    uint8_t   length; //  (in bytes)x
} FAPI_L1_TLV_SizeTable [] = {
    /* 0 */  { 0                                                  , 0xFF },
    /* 1 */  { FAPI_L1_DuplexingMode                              , TT_UINT16_T },
    /* 2 */  { FAPI_L1_PCFICH_PowerOffset                         , TT_UINT16_T },
    /* 3 */  { FAPI_L1_P_B                                        , TT_UINT16_T },
    /* 4 */  { FAPI_L1_DL_CyclicPrefixType                        , TT_UINT16_T },
    /* 5 */  { FAPI_L1_UL_CyclicPrefixType                        , TT_UINT16_T },
    /* 6 */  { FAPI_L1_DownlinkChannelBandwidth                   , TT_UINT16_T },
    /* 7 */  { FAPI_L1_UplinkChannelBandwidth                     , TT_UINT16_T },
    /* 8 */  { FAPI_L1_ReferenceSignalPower                       , TT_UINT16_T },
    /* 9 */  { FAPI_L1_TxAntennaPorts                             , TT_UINT16_T },
    /* 10 */ { FAPI_L1_Rx_Antenna_Ports                           , TT_UINT16_T },
    /* 11 */ { FAPI_L1_PHICH_Resource                             , TT_UINT16_T },
    /* 12 */ { FAPI_L1_PHICH_Duration                             , TT_UINT16_T },
    /* 13 */ { FAPI_L1_PHICH_PowerOffset                          , TT_UINT16_T },
    /* 14 */ { FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS   , TT_UINT16_T },
    /* 15 */ { FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS , TT_UINT16_T },
    /* 16 */ { FAPI_L1_PhysicalCell_ID                            , TT_UINT16_T },
    /* 17 */ { FAPI_L1_ConfigurationIndex                         , TT_UINT16_T },
    /* 18 */ { FAPI_L1_RootSequenceIndex                          , TT_UINT16_T },
    /* 19 */ { FAPI_L1_ZeroCorrelationZoneConfiguration           , TT_UINT16_T },
    /* 20 */ { FAPI_L1_HighSpeedFlag                              , TT_UINT16_T },
    /* 21 */ { FAPI_L1_FrequencyOffset                            , TT_UINT16_T },
    /* 22 */ { FAPI_L1_HoppingMode                                , TT_UINT16_T },
    /* 23 */ { FAPI_L1_HoppingOffset                              , TT_UINT16_T },
    /* 24 */ { FAPI_L1_NumberOfSubBands                           , TT_UINT16_T },
    /* 25 */ { FAPI_L1_Delta_PUCCH_Shift                          , TT_UINT16_T },
    /* 26 */ { FAPI_L1_N_CQI_RB                                   , TT_UINT16_T },
    /* 27 */ { FAPI_L1_N_AN_CS                                    , TT_UINT16_T },
    /* 28 */ { FAPI_L1_N1Pucch_AN                                 , TT_UINT16_T },
    /* 29 */ { FAPI_L1_BandwidthConfiguration                     , TT_UINT16_T },
    /* 30 */ { FAPI_L1_MaxUpPTS                                   , TT_UINT16_T },
    /* 31 */ { FAPI_L1_SRS_SubframeConfiguration                  , TT_UINT16_T },
    /* 32 */ { FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission   , TT_UINT8_T  },
    /* 33 */ { FAPI_L1_Uplink_RS_Hopping                          , TT_UINT16_T },
    /* 34 */ { FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern  , TT_UINT16_T },
    /* 35 */ { FAPI_L1_CyclicShift1For_DMRS                       , TT_UINT16_T },
    /* 36 */ { FAPI_L1_SubframeAssignment                         , TT_UINT16_T },
    /* 37 */ { FAPI_L1_SpecialSubframePatterns                    , TT_UINT16_T },
    /* 38 */ { 0                                                  , 0xFF },
    /* 39 */ { 0                                                  , 0xFF },
    /* 40 */ { FAPI_L1_DownlinkBandwidthSupport                   , TT_UINT16_T },
    /* 41 */ { FAPI_L1_UplinkBandwidthSupport                     , TT_UINT16_T },
    /* 42 */ { FAPI_L1_DownlinkModulationSupport                  , TT_UINT16_T },
    /* 43 */ { FAPI_L1_UplinkModulationSupport                    , TT_UINT16_T },
    /* 44 */ { FAPI_L1_PHY_AntennaCapability                      , TT_UINT16_T },
    /* 45 */ { 0                                                  , 0xFF },
    /* 46 */ { 0                                                  , 0xFF },
    /* 47 */ { 0                                                  , 0xFF },
    /* 48 */ { 0                                                  , 0xFF },
    /* 49 */ { 0                                                  , 0xFF },
    /* 50 */ { FAPI_L1_DataReportMode                             , TT_UINT16_T },
    /* 51 */ { FAPI_L1_SFN_SF                                     , TT_UINT16_T },
    /* 52 */ { 0                                                  , 0xFF },
    /* 53 */ { 0                                                  , 0xFF },
    /* 54 */ { 0                                                  , 0xFF },
    /* 55 */ { 0                                                  , 0xFF },
    /* 56 */ { 0                                                  , 0xFF },
    /* 57 */ { 0                                                  , 0xFF },
    /* 58 */ { 0                                                  , 0xFF },
    /* 59 */ { 0                                                  , 0xFF },
    /* 60 */ { FAPI_L1_PHY_State                                  , TT_UINT16_T },
    /* 61 */ { 0                                                  , 0xFF },
    /* 62 */ { 0                                                  , 0xFF },
    /* 63 */ { 0                                                  , 0xFF },
    /* 64 */ { 0                                                  , 0xFF },
    /* 65 */ { 0                                                  , 0xFF },
    /* 66 */ { 0                                                  , 0xFF },
    /* 67 */ { 0                                                  , 0xFF },
    /* 68 */ { 0                                                  , 0xFF },
    /* 69 */ { 0                                                  , 0xFF },
    /* 70 */ { 0                                                  , 0xFF },
    /* 71 */ { 0                                                  , 0xFF },
    /* 72 */ { 0                                                  , 0xFF },
    /* 73 */ { 0                                                  , 0xFF },
    /* 74 */ { 0                                                  , 0xFF },
    /* 75 */ { 0                                                  , 0xFF },
    /* 76 */ { 0                                                  , 0xFF },
    /* 77 */ { 0                                                  , 0xFF },
    /* 78 */ { 0                                                  , 0xFF },
    /* 79 */ { 0                                                  , 0xFF },
    /* 80 */ { 0                                                  , 0xFF },
    /* 81 */ { 0                                                  , 0xFF },
    /* 82 */ { 0                                                  , 0xFF },
    /* 83 */ { 0                                                  , 0xFF },
    /* 84 */ { 0                                                  , 0xFF },
    /* 85 */ { 0                                                  , 0xFF },
    /* 86 */ { 0                                                  , 0xFF },
    /* 87 */ { 0                                                  , 0xFF },
    /* 88 */ { 0                                                  , 0xFF },
    /* 89 */ { 0                                                  , 0xFF },
    /* 90 */ { 0                                                  , 0xFF },
    /* 91 */ { 0                                                  , 0xFF },
    /* 92 */ { 0                                                  , 0xFF },
    /* 93 */ { 0                                                  , 0xFF },
    /* 94 */ { 0                                                  , 0xFF },
    /* 95 */ { 0                                                  , 0xFF },
    /* 96 */ { 0                                                  , 0xFF },
    /* 97 */ { 0                                                  , 0xFF },
    /* 98 */ { 0                                                  , 0xFF },
    /* 99 */ { 0                                                  , 0xFF },
    /*100 */ { FAPI_L1_Handle                                    , TT_UINT32_T },
    /*101 */ { FAPI_L1_RNTI                                      , TT_UINT16_T },
    /*102 */ { FAPI_L1_CQI_PUCCH_ResourceIndex                   , TT_UINT16_T },
    /*103 */ { FAPI_L1_CQI_PMI_ConfigIndex                       , TT_UINT16_T },
    /*104 */ { FAPI_L1_CQI_RI_ConfigIndex                        , TT_UINT16_T },
    /*105 */ { FAPI_L1_CQI_SimultaneousAckNackAnd_CQI            , TT_UINT8_T },
    /*106 */ { FAPI_L1_AN_RepetitionFactor                       , TT_UINT8_T },
    /*107 */ { FAPI_L1_AN_n1PUCCH_ANRep                          , TT_UINT16_T },
    /*108 */ { FAPI_L1_TDD_AckNackFeedbackMode                   , TT_UINT8_T },
    /*109 */ { FAPI_L1_SRS_Bandwidth                             , TT_UINT8_T },
    /*110 */ { FAPI_L1_SRS_Hopping_Bandwidth                     , TT_UINT8_T },
    /*111 */ { FAPI_L1_FrequencyDomainPosition                   , TT_UINT8_T },
    /*112 */ { FAPI_L1_SRS_Duration                              , TT_UINT8_T },
    /*113 */ { FAPI_L1_ISRS_SRS_ConfigIndex                      , TT_UINT16_T },
    /*114 */ { FAPI_L1_TransmissionComb                          , TT_UINT8_T },
    /*115 */ { FAPI_L1_SoundingReferenceCyclicShift              , TT_UINT8_T },
    /*116 */ { FAPI_L1_SR_PUCCH_ResourceIndex                    , TT_UINT16_T },
    /*117 */ { FAPI_L1_SR_ConfigIndex                            , TT_UINT8_T },
    /*118 */ { FAPI_L1_SPS_DL_ConfigSchedulingInterval           , TT_UINT16_T },
    /*119 */ { FAPI_L1_SPS_DL_n1PUCCH_AN_Persistent              , TT_UINT16_T },
    /*120 */ { 0                                                 , 0xFF }
};


ssize_t tlv_encode_tag(uint8_t tag, uint8_t *value, uint8_t *buffer, size_t offset)
{
    return -1;
}

ssize_t tlv_decode_tag(uint8_t *tag, uint8_t *value, uint8_t *buffer, size_t offset)
{
    return -1;
}

ssize_t tlv_copy_tag(uint8_t tag, uint8_t *src, uint8_t *dst)
{
    return -1;
}


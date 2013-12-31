
static l1_tlv_word_t SISO_CONFIG_REQUEST_TLVS [] = {
    /* Duplexing Mode                 1 */
    {FAPI_L1_DuplexingMode                              , 2, {FAPI_L1_DM_FDD}},
    /* PCFICH Power Offset            2 */
    {FAPI_L1_PCFICH_PowerOffset                         , 2, {0x1b42}}, // --
    /* P-B                            3 */
    {FAPI_L1_P_B                                        , 2, {0}},
    /* DL Cyclic Prefix Type          4 */
    {FAPI_L1_DL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* UL Cyclic Prefix Type          5 */
    {FAPI_L1_UL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* +RF Config                      All TLVs in this grouping */
    {FAPI_L1_DownlinkChannelBandwidth                   , 2, {50}},
    {FAPI_L1_UplinkChannelBandwidth                     , 2, {15}},
    {FAPI_L1_ReferenceSignalPower                       , 2, {.word=0x1b42}},
    {FAPI_L1_TxAntennaPorts                             , 2, {1}},
    {FAPI_L1_RxAntennaPorts                           , 2, {1}},
    /* PHICH Config                   All TLVs in this grouping */
    {FAPI_L1_PHICH_Resource                             , 2, {PHICH_R_ONE_SIXTH}},
    {FAPI_L1_PHICH_Duration                             , 2, {PHICH_D_NORMAL}},
    {FAPI_L1_PHICH_PowerOffset                          , 2, {0}},
    /* SCH Config                     All TLVs in this grouping */
    {FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS   , 2, {.word=6255}},
    {FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS , 2, {.word=6255}},
    {FAPI_L1_PhysicalCell_ID                            , 2, {0}},
    /* PRACH Config                   All TLVs in this grouping */
    {FAPI_L1_ConfigurationIndex                         , 2, {0x0C}},
    {FAPI_L1_RootSequenceIndex                          , 2, {0x16}},
    {FAPI_L1_ZeroCorrelationZoneConfiguration           , 2, {1}}, // Cyclic shift
    {FAPI_L1_HighSpeedFlag                              , 2, {HS_UNRESTRICTED_SET}},
    {FAPI_L1_FrequencyOffset                            , 2, {0}},
    /* PUSCH Config                   All TLVs in this grouping */
    {FAPI_L1_HoppingMode                                , 2, {HM_INTER_SF}},
    {FAPI_L1_HoppingOffset                              , 2, {1}},
    {FAPI_L1_NumberOfSubBands                           , 2, {2}},

    /* PUCCH Config                   All TLVs in this grouping */
    {FAPI_L1_Delta_PUCCH_Shift                          , 2, {1}}, //??
    {FAPI_L1_N_CQI_RB                                   , 2, {0}}, //??
    {FAPI_L1_N_AN_CS                                    , 2, {0}}, // 2b
    {FAPI_L1_N1Pucch_AN                                 , 2, {0}}, //??
    /* SRS Config                     All TLVs in this grouping */
    {FAPI_L1_SRS_BandwidthConfiguration                     , 2, {0}},
#if 0
    {FAPI_L1_MaxUpPTS                                   , 2, {MAX_UP_PTS_DISABLED}}, // TDD only
#endif
    {FAPI_L1_SRS_SubframeConfiguration                  , 2, {15}},
    {FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission   , 2, {SRS_NO_SIMULTANEOUS_TRANSMISSION}},
    /* Uplink Reference Signal Config All TLVs in this grouping */
    {FAPI_L1_Uplink_RS_Hopping                          , 2, {RS_NO_HOPPING}},
    {FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern  , 2, {0}},
    {FAPI_L1_CyclicShift1For_DMRS                       , 2, {0}},
    /* Data Report Mode               50 */
    {FAPI_L1_DataReportMode                             , 2, {1}},
    {FAPI_L1_SFN_SF                                     , 2, {0}}
};
static size_t SISO_CONFIG_REQUEST_TLVS_num = sizeof(SISO_CONFIG_REQUEST_TLVS) / sizeof(l1_tlv_word_t);


//// EMPTY VECTORS //// EMPTY VECTORS //// EMPTY VECTORS //// EMPTY VECTORS //// 

struct fapi_l1_dl_config_request dl_empty_config = {
    .Length = offsetof (struct DL_PDU_Configuration, DL_PDU) + sizeof(struct fapi_l1_bch_pdu),
    .NumberOf_PDCCH_OFDM_Symbols = 2,
    .NumberOf_DCIs = 0,
    .NumberOf_PDUs = 0,
    .NumberOf_PDSCH_RNTIs = 0,
    .TransmissionPowerFor_PCFICH = 0,
};

struct fapi_l1_ul_config_request ul_empty_config = {
    .Length = 0,
    .NumberOf_PDUs = 0,
    .RACH_PRACH = 0,
    .SRS_Present = 0
};

struct fapi_l1_tx_request tx_empty_request = {
    .NumberOf_PDUs = 0,
};


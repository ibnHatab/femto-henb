
// struct UL_PDU_Configuration {
// .PDU_Type = 0,
// .PDU_Size = 0,
// .UL_PDU [1] = 0,
// };
struct fapi_l1_ul_config_request ul_config_request_data = {
    .SFN_SF = INVALID_SFNSF,
    .Length = 0,
    .NumberOf_PDUs = 0,
    .RACH_PRACH = 1,
    .SRS_Present = 1,
    .UL_PDU_Configuration = {}
};
// 0: ULSCH, see Section 3.3.1.3.1.
#define ULSCH_PDU_DATA  {                       \
        .Handle = 42,                           \
            .Size = 0,                          \
            .RNTI = 0xFFFF,                     \
            .ResourceBlockStart = 99,           \
            .NumberOfResourceBlocks = 1,        \
            .ModulationType = MD_QPSK,          \
            .CyclicShift2For_DMRS = 7,          \
            .FrequencyHoppingEnabledFlag = 1,   \
            .FrequencyHoppingBits = 3,          \
            .NewDataIndication = 1,             \
            .RedundancyVersion = 3,             \
            .HARQ_ProcessNumber = 7,            \
            .UL_TX_Mode = 1,                    \
            .Current_TX_NB = 0,                 \
            .N_Srs = 1,                         \
            }

struct fapi_l1_ulsch_pdu ulsch_pdu_data = ULSCH_PDU_DATA;

// Table 48: CQI_RI Information
#define CQI_RI_INFORMATION_DATA  {              \
        .DL_CQI_PMI_SizeRank_Eq_1 = 255,        \
            .DL_CQI_PMI_SizeRank_Gt_1 = 255,    \
            .RI_Size = 2,                       \
            .DeltaOffset_CQI = 15,              \
            .DeltaOffset_RI = 15,               \
            } 
// 3.3.1.3.5  Initial Transmission Parameters
#define INITIAL_TRANSMISSION_PARAMETERS_DATA  {         \
        .N_SrsInitial = 1,                              \
            .InitialNumberOfResourceBlocks = 100,       \
            }
// 1: ULSCH_CQI_RI, see Section 3.3.1.3.2.
struct fapi_l1_ulsch_cqi_ri_pdu ulsch_cqi_ri_pdu_data = {
    .ULSCH_PDU = ULSCH_PDU_DATA,
    .CQI_RI_Information = CQI_RI_INFORMATION_DATA,
    .InitialTransmissionParameters = INITIAL_TRANSMISSION_PARAMETERS_DATA
};
// Table 50: HARQ Information
#define HARQ_INFORMATION_DATA {                 \
        .HARQ_Size = 2,                         \
            .DeltaOffset_HARQ = 15,              \
            .ACK_NACK_Mode = 0,                 \
            }
// 2: ULSCH_HARQ, see Section 3.3.1.3.3.
struct fapi_l1_ulsch_harq_pdu ulsch_harq_pdu_data = {
    .ULSCH_PDU = ULSCH_PDU_DATA,
    .HARQ_Information = HARQ_INFORMATION_DATA,
    .InitialTransmissionParameters = INITIAL_TRANSMISSION_PARAMETERS_DATA,
} ;
// 3: ULSCH_CQI_HARQ_RI, see Section 3.3.1.3.4.
struct fapi_l1_ulsch_cqi_harq_ri_pdu ulsch_cqi_harq_ri_pdu_data = {
    .ULSCH_PDU = ULSCH_PDU_DATA,
    .CQI_RI_Information = CQI_RI_INFORMATION_DATA,
    .HARQ_Information = HARQ_INFORMATION_DATA,
    .InitialTransmissionParameters = INITIAL_TRANSMISSION_PARAMETERS_DATA,
} ;
// 3.3.1.3.13  CQI Information Table 60
#define CQI_INFORMATION_DATA  {                 \
        .PUCCH_Index = 1185,                    \
            .DL_CQI_PMI_Size = 255,             \
            } 
// 4: UCI_CQI, see Section 3.3.1.3.6.
struct fapi_l1_uci_cqi_pdu uci_cqi_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .CQI_Information = CQI_INFORMATION_DATA,
} ;
// 3.3.1.3.14  SR Information Table 61
#define SR_INFORMATION_DATA  {                  \
        .PUCCH_Index = 42,                       \
            }
// 5: UCI_SR, see Section 3.3.1.3.7.
struct fapi_l1_uci_sr_pdu uci_sr_pdu_data = {
    .Handle = 0,
    .RNTI = 0xFFFF,
    .SR_Information = SR_INFORMATION_DATA,
} ;
// 3.3.1.3.15  HARQ Information
// Table 63: HARQ information for FDD
#define HARQ_INFORMATION_FDD_DATA {             \
        .PUCCH_Index = 0,                       \
            .HARQ_Size  = 0,                    \
            }

// 6: UCI_HARQ, see Section 3.3.1.3.8.
struct fapi_l1_uci_harq_pdu uci_harq_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .HARQ_Information.fdd = HARQ_INFORMATION_FDD_DATA
} ;
// 7: UCI_SR_HARQ, see Section 3.3.1.3.9.
struct fapi_l1_uci_sr_harq_pdu uci_sr_harq_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .SR_Information = SR_INFORMATION_DATA,
    .HARQ_Information.fdd = HARQ_INFORMATION_FDD_DATA
} ;
// 8: UCI_CQI_HARQ, see Section 3.3.1.3.10.
struct fapi_l1_uci_cqi_harq_pdu uci_cqi_harq_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .CQI_Information = CQI_INFORMATION_DATA,
    .HARQ_Information.fdd = HARQ_INFORMATION_FDD_DATA
} ;
// 9 : UCI_CQI_SR, see Section  3.3.1.3.11
struct fapi_l1_uci_cqi_sr_pdu uci_cqi_sr_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .CQI_Information = CQI_INFORMATION_DATA,
    .SR_Information = SR_INFORMATION_DATA,
} ;
// 10 : UCI_CQI_SR_HARQ, see Section  3.3.1.3.11
struct fapi_l1_uci_cqi_sr_harq_pdu uci_cqi_sr_harq_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
    .CQI_Information = CQI_INFORMATION_DATA,
    .SR_Information = SR_INFORMATION_DATA,
    .HARQ_Information.fdd = HARQ_INFORMATION_FDD_DATA
} ;
// 11: SRS, see Section 3.3.1.3.16.
struct fapi_l1_srs_pdu srs_pdu_data = {
    .Handle = 0,
    .Size = 0,
    .RNTI = 0,
    .SRS_Bandwidth  = 0,
    .FrequencyDomainPosition = 0,
    .SRS_HoppingBandwidth = 0,
    .TransmissionComb = 0,
    .ISRS_SRS_ConfigIndex = 0,
    .SoundingReferenceCyclicShift = 0,
} ;
// 12 : HARQ_BUFFER , see Section 3.3.1.3.17
struct fapi_l1_harq_buffer_pdu harq_buffer_pdu_data = {
    .Handle = 0,
    .RNTI = 0,
} ;

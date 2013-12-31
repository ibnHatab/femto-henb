
uint8_t mac_bch_pdu [] = {0x62, 0xD8, 0x00};

struct fapi_l1_bch_pdu bch_pdu = {
    .Length = sizeof(mac_bch_pdu),
    .PDU_Index = 1,
    .TransmissionPower = 0,
};

struct DL_PDU_Configuration dl_bch_pdu_configuration [] ={
    {
        .PDU_Type = BCH_PDU_TYPE,
        .PDU_Size = sizeof(struct fapi_l1_bch_pdu)
        + offsetof(struct DL_PDU_Configuration, DL_PDU)
    }
};


struct fapi_l1_dl_config_request dl_bch_config = {
    .Length = 0,
    .NumberOf_PDCCH_OFDM_Symbols = 2,
    .NumberOf_DCIs = 0,
    .NumberOf_PDUs = 0,
    .NumberOf_PDSCH_RNTIs = 0,
    .TransmissionPowerFor_PCFICH = 0,
};

  //// UL //// UL //// UL //// UL //// UL //// UL //// UL 
struct fapi_l1_ul_config_request ul_bch_config = {
    .Length = 0,
    .NumberOf_PDUs = 0,
    .RACH_PRACH = 0,
    .SRS_Present = 0
};


  //// TX req //// TX req //// TX req //// TX req //// 
struct fapi_l1_dl_pdu_config dl_bch_pdu_config [] = {
    {
        .PDU_Length = 1 * sizeof(mac_bch_pdu),
        .PDU_Index = 1,
        .numTLV = 1,
        .tlvs = {
            {
                .tag = 1,
                .length = sizeof(mac_bch_pdu),
                .value = 0 // API msg
            }
        }
    }
};

struct fapi_l1_tx_request tx_bch_request = {
    .NumberOf_PDUs = 1,
};

  //// DATA TEMPLATE  //// DATA TEMPLATE  //// DATA TEMPLATE  //// DATA TEMPLATE ////

// struct DL_PDU_Configuration DL_PDU_Configuration_data = {
//     .PDU_Type = 0,
//     .PDU_Size = 0,
//     .DL_PDU [1] = 0,
// };

struct fapi_l1_dl_config_request dl_config_request_data = {
    .SFN_SF = INVALID_SFNSF,
    .Length = 0,
    .NumberOf_PDCCH_OFDM_Symbols = 4,
    .NumberOf_DCIs = 0,
    .NumberOf_PDUs = 0,
    .NumberOf_PDSCH_RNTIs = 0,
    .TransmissionPowerFor_PCFICH = 10000,
};
  // 0: DCI DL PDU, see Section 3.3.1.2.1.
struct fapi_l1_dci_dl_pdu dci_dl_pdu_data = {
    .DCI_Format = DCI_2A,
    .CCE_Index = 88,
    .AggregationLevel = 8,
    .RNTI = 0xFFFF,
    .ResourceAllocationType = 1,
    .VirtualResourceBlockAssignmentFlag = 1,
    .ResourceBlockCoding = 1,
    .MCS_1 = 31,
    .RedundancyVersion_1 = 3,
    .NewDataIndicator_1 = 1,
    .TransportBlockToCodewordSwapFlag = 1,
    .MCS_2 = 31,
    .RedundancyVersion_2 = 1,
    .NewDataIndicator_2 = 0,
    .HARQ_Process = 15,
    .TPMI = 3,
    .PMI = 1,
    .PrecodingInformation = 7,
    .TPC = 3,
    .DownlinkAssignmentIndex = 4,
    .N_GAP = N_GAP1,
    .TransportBlockSizeIndex = 31,
    .DownlinkPowerOffset = 1,
    .Allocate_PRACH_Flag = 1,
    .PreambleIndex  = 63,
    .PRACH_MaskIndex = 15,
    .RNTI_type = 1,
    .TransmissionPower = 10000
};
  // 1: BCH PDU, see Section 3.3.1.2.2.
struct fapi_l1_bch_pdu bch_pdu_data = {
    .Length = 0,
    .PDU_Index = 0,
    .TransmissionPower = 10000
};
  // 2: MCH PDU, see Section 3.3.1.2.3.
struct fapi_l1_mch_pdu mch_pdu_data = {
    .Length = 0,
    .PDU_Index = 0,
    .RNTI = 0xFFFF,
    .ResourceAllocationType = 2,
    .ResourceBlockCoding = 0,
    .Modulation = MD_64QAM,
    .TransmissionPower = 10000
};
  // Table 43: BfVectorType Structure
struct fapi_l1_bf_vector_type bf_vector_type_data [] = {
    {
    .subbandIndex = 0,
    .numAntennas = 1,
    .bfValue = 0xAA00 & 0xCC
    },
    
};
  // 3: DLSCH PDU, see Section 3.3.1.2.4.
struct fapi_l1_dlsch_pdu dlsch_pdu_data = {
    .Length = 0,
    .PDU_Index = 0,
    .RNTI = 0xFFFF,
    .ResourceAllocationType = 2,
    .VirtualResourceBlockAssignmentFlag = 1,
    .ResourceBlockCoding = 2,
    .Modulation = MD_64QAM,
    .RedundancyVersion = 3,
    .TransportBlocks = 2,
    .TransportBlockToCodewordSwapFlag = 1,
    .TransmissionScheme = SINGLE_ANTENNA_PORT_5,
    .NumberOfLayers = 4,
    .NumberOfSubbands = 13,
    .CodebookEntries = {{15}},
    .UE_CategoryCapacity = 5,
    .P_A = 7,
    .DeltaPowerOffsetIndex = 1,
    .N_GAP = N_GAP2,
    .N_PRB = 1,
    .numBfPRBperSubban = 1,
    .bfVector = {}, // bf_vector_type_data
};
  // 4: PCH PDU, see Section 3.3.1.2.5.
struct fapi_l1_pch_pdu pch_pdu_data = {
    .Length = 0,
    .PDU_Index = 0,
    .P_RNTI = 0xFFFF,
    .ResourceAllocationType = 2,
    .VirtualResourceBlockAssignment = 1,
    .ResourceBlockCoding = 2,
    .MCS = 0,
    .RedundancyVersion = 0,
    .NumberOfTransportBlocks = 1,
    .TransportBlockToCodewordSwapFlag = 0,
    .TransmissionScheme = SINGLE_ANTENNA_PORT_0,
    .NumberOfLayers = 4,
    .CodebookIndex = 0,
    .UE_CategoryCapacity = 5,
    .P_A = 7,
    .TransmissionPower = 10000,
    .N_PRB = 1,
};

//// TX request //// TX request //// TX request //// TX request //// TX request ////

// struct fapi_l1_dl_pdu_tlv {
//     uint16_t tag;
//     uint16_t length;
//     uint32_t value;
// } __packed;

struct fapi_l1_dl_pdu_config dl_pdu_config_data = {
    .PDU_Length = 0,
    .PDU_Index = 0,
    .numTLV = 0,
    .tlvs = {}
};
// See Section 3.3.2.1
struct fapi_l1_tx_request tx_request_data = {
    .SFN_SF = INVALID_SFNSF,
    .NumberOf_PDUs = 0,
    .TX_Config = {}
};

//// HI_DCI0 //// HI_DCI0 //// HI_DCI0 //// HI_DCI0 ////

struct fapi_l1_hi_pdu fapi_l1_hi_pdu_data = {
    .ResourceBlockStart = 100,
    .CyclicShift2For_DMRS = 7,
    .HI_Value = HI_ACK,
    .I_PHICH = 0,
    .TransmissionPower = 10000,
};

struct fapi_l1_dci_ul_pdu fapi_l1_dci_ul_pdu_data = {
    .DCI_Format = 2,
    .CCE_Index = 88,
    .AggregationLevel = 8,
    .RNTI = 42,
    .ResourceBlockStart = 100,
    .NumberOfResourceBlocks = 100,
    .MCS = 31,
    .CyclicShift2For_DMRS = 7,
    .FrequencyEnabledFlag = 1,
    .FrequencyHoppingBits = 3,
    .NewDataIndication = 1,
    .UE_TX_AntennaSelection = 2,
    .TPC = 3,
    .CQI_Request = 1,
    .UL_Index = 3,
    .DL_AssignmentIndex = 4,
    .TPC_Bitmap = 0,
};

struct fapi_l1_hi_dci0_request fapi_l1_hi_dci0_request_data = {
    .SFN_SF = 0,
    .NumberOf_DCI = 0,
    .NumberOf_HI = 0,
    .DCI_HI_PDU_Configuration = {}
};

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Femto Forum Ltd
 *
 * LTE eNB L1 API Definition v1.1
 * Femto Forum Technical Document
 *
 * Document number:  FF_Tech_002_v1.11
 * Date issued:      12-10-2010
 * Document status:  Document for public distribution
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ibn Hatab <lib.aca55a@gmail.com>
 */

#ifndef _L1_L2_IFACE_H_
#define _L1_L2_IFACE_H_

#include <fapi/l1_types.h>

/** Table 3: General L1 API message format */

typedef struct fapi_l1_message {
    uint8_t  typeID;
    uint8_t  lengthVendorSspecific;
    uint16_t length;
} fapi_l1_message_t;

enum FAPI_L1_MessageTypeID_e {
    FAPI_L1_PARAM_request       = 0x00, // See Section 3.2.1.1
    FAPI_L1_PARAM_response      = 0x01, // See Section 3.2.1.2
    FAPI_L1_CONFIG_request      = 0x02, // See Section 3.2.2.1
    FAPI_L1_CONFIG_response     = 0x03, // See Section 3.2.2.2
    FAPI_L1_START_request       = 0x04, // See Section 3.2.4.1
    FAPI_L1_STOP_request        = 0x05, // See Section 3.2.5.1
    FAPI_L1_STOP_indication     = 0x06, // See Section 3.2.5.2
    FAPI_L1_UE_CONFIG_request   = 0x07, // See Section 3.2.6.1
    FAPI_L1_UE_CONFIG_response  = 0x08, // See Section 3.2.6.2
    FAPI_L1_ERROR_indication    = 0x09, // See Section 3.2.8.1
    FAPI_L1_UE_RELEASE_request  = 0x0a, // See Section 3.2.7.1
    FAPI_L1_UE_RELEASE_response = 0x0b, // See Section 3.2.7.2
    // RESERVED  0x0c-0x7f
    FAPI_L1_DL_CONFIG_request   = 0x80, // See Section 3.3.1.2
    FAPI_L1_UL_CONFIG_request   = 0x81, // See Section 3.3.1.3
    FAPI_L1_SUBFRAME_indication = 0x82, // See Section 3.3.1.1
    HI_DCI0_request             = 0x83, //  See Section 3.3.1.3
    FAPI_L1_TX_request          = 0x84, // See Section 3.3.2.1
    FAPI_L1_HARQ_indication     = 0x85, // See Section 3.3.3.2
    FAPI_L1_CRC_indication      = 0x86, // See Section 3.3.3.3
    FAPI_L1_RX_ULSCH_indication = 0x87, // See Section 3.3.3.1
    FAPI_L1_RACH_indication     = 0x88, // See Section 3.3.3.4
    FAPI_L1_SRS_indication      = 0x89, // See Section 3.3.3.7
    FAPI_L1_RX_SR_indication    = 0x8a, // See Section 3.3.3.4
    FAPI_L1_RX_CQI_indication   = 0x8b, // See Section 3.3.3.5
    // RESERVED  0x8c-0xff
};

enum FAPI_L1_MessageErrors_e {
    MSG_OK = 0,
    MSG_INVALID_STATE,
    MSG_INVALID_CONFIG,
};

// See Section 3.2.1.1
struct fapi_l1_param_request {
    fapi_l1_message_t hdr;
    // pass
};
// See Section 3.2.1.2
struct fapi_l1_param_response {
    fapi_l1_message_t hdr;
    uint8_t errorCode;      // See Table 86.
    uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
    uint8_t TLVs[0];        // Variable  See Table 6 to Table 9.
};
// See Section 3.2.2.1
struct fapi_l1_config_request {
    fapi_l1_message_t hdr;
    uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
    uint8_t TLVs[0];        // Variable  See Table 12 to Table 14.
};
// See Section 3.2.2.2
struct fapi_l1_config_response {
    fapi_l1_message_t hdr;
    uint8_t errorCode;      // See Table 86.
    uint8_t numberOfInvalidOrUnsupportedTLVs;
    uint8_t numberOfMissingTLVs;
    uint8_t TLVs[0];
    // A list of invalid or unsupported TLVs each TLV is presented in its entirety.
    // A list of missing TLVs  each TLV is presented in its entirety.
};
// See Section 3.2.4.1
struct fapi_l1_start_request {
    fapi_l1_message_t hdr;
    // The message length in the generic header = 0.
};
// See Section 3.2.5.1
struct fapi_l1_stop_request {
    fapi_l1_message_t hdr;
    // The message length in the generic header = 0.
};
// See Section 3.2.5.2
struct fapi_l1_stop_indication {
    fapi_l1_message_t hdr;
    // The message length in the generic header = 0.
};
// See Section 3.2.6.1
struct fapi_l1_ue_config_request {
    fapi_l1_message_t hdr;
    uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
    uint8_t TLVs[0];        // Variable  See Table 22.
};
// See Section 3.2.6.2
struct fapi_l1_ue_config_response {
    fapi_l1_message_t hdr;
    uint8_t errorCode;      // See Table 86.
    uint8_t numberOfInvalidOrUnsupportedTLVs;
    uint8_t numberOfMissingTLVs;
    uint8_t TLVs[0];
    // A list of invalid or unsupported TLVs each TLV is presented in its entirety.
    // A list of missing TLVs  each TLV is presented in its entirety.
};
// See Section 3.2.8.1
struct fapi_l1_error_indication {
    fapi_l1_message_t hdr;
    uint8_t messageID;
    // Indicate which message received by the PHY has an error.
    uint8_t errorCode;                    // see 3.4 for value.
    union {
        // Table 32: MSG_INVALID_STATE and MSG_BCH_MISSING
        struct {/* pass */} MsgBchMissing;
        // Table 33: SFN_OUT_OF_SYNC and MSG_INVALID_SFN
        struct {
            uint16_t receivedSFN_SF;          // The SFN/SF value received in the message
            uint16_t expectedSFN_SF;          // value the PHY was expecting to receive
        } SfnOutOfSync;
        // Table 34: MSG_PDU_ERR
        struct {
            uint8_t subErrorCode;             // see Section 3.4.1.
            uint8_t direction;                // 0 = DL, 1 = UL
            uint16_t RNTI;                    // The RNTI in the received PDU.
            // If the error occurred in a MCH, or BCH, PDU this value is set to 0
            uint8_t PDU_Type;
            // The PDU Type parameter specified in both DL/UL subframe
        } MsgPduErr;
        // Table 35: MSG_HI_ERR
        struct {
            uint8_t subErrorCode;             // see Section 3.4.1.
            uint8_t PHICH_Lowest_UL_RB_Index; // The PHICH RB Index parameters in each HI PDU
        } MsgHiErr;
        // Table 36: MSG_TX_ERR
        struct {
            uint8_t subErrorCode;             // see Section 3.4.1.
            uint16_t PDU_Index;               // The PDU index parameter specified for each PDU
        } MsgTxErr;
    } error;
};
// See Section 3.2.7.1
struct fapi_l1_ue_release_request {
    fapi_l1_message_t hdr;
    uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
    uint8_t TLVs[0];        // Variable  See Table 28.
};
// See Section 3.2.7.2
struct fapi_l1_ue_release_response {
    fapi_l1_message_t hdr;
    uint8_t errorCode;      // MSG_INVALID_CONFIG, MSG_INVALID_STATE, MSG_OK
    uint8_t numberOfInvalidOrUnsupportedTLVs;
    uint8_t numberOfMissingTLVs;
    uint8_t TLVs[0];
    // A list of invalid or unsupported TLVs each TLV is presented in its entirety.
    // A list of missing TLVs  each TLV is presented in its entirety.
};
/**
 * The following combinations of PDUs are required:
 *  A BCH PDU does not have an associated DCI PDU
 *  A PCH PDU requires an associated DCI PDU
 *  A MCH PDU requires an associated DCI PDU
 *  A DLSCH allocated with Semi-Persistent Scheduling may not have an associated DCI PDU
 *  A DLSCH for a unique RNTI requires an associated DCI PDU.
 *    Therefore, 2 DLSCH for the same RNTI onlyrequire 1 DCI PDU
 */
// See Section 3.3.1.2
struct fapi_l1_dl_config_request {
    fapi_l1_message_t hdr;

    uint16_t SFN_SF;
    // A 16-bit value where,
    //   [15:4] SFN, range 0  1023
    //   [3:0] SF, range 0  9

    uint16_t Length;
    // The length of the downlink subframe configuration.
    // Range 0  65535.

    uint8_t NumberOf_PDCCH_OFDM_Symbols;
    // The number of OFDM symbols for the PDCCH.
    // See [8] section 6.7.
    // Value:0  4

    uint8_t NumberOfDCIs;
    // The number of DCI PDUs included in this message.
    // Range: 0  255

    uint16_t NumberOf_PDUs;
    // Number of PDUs that are included in this message.
    // Range 0  514

    uint8_t NumberOf_PDSCH_RNTIs;
    // Number of unique RNTIs sent on the PDSCH.
    // - a PCH PDU will have an unique RNTI and should be included in this value
    // - a DLSCH PDU can be one transport block sent to a UE with
    //   an unique RNTI. This RNTI should be included in this value
    // - a DLSCH PDU can be one of two transport blocks sent to a
    //   UE. In this case the two DLSCH PDUs will share the same
    //   RNTI. Only one RNTI should be included in this value.

    uint16_t TransmissionPowerFor_PCFICH;
    // Offset to the reference signal power.
    // Value: 0  10000, representing -6 dB to 4 dB in 0.001 dB
    // steps.

    // For Number of PDUs
    struct {
        uint8_t PDU_Type;
        // 0: DCI DL PDU, see Section 3.3.1.2.1.
        // 1: BCH PDU, see Section 3.3.1.2.2.
        // 2: MCH PDU, see Section 3.3.1.2.3.
        // 3: DLSCH PDU, see Section 3.3.1.2.4.
        // 4: PCH PDU, see Section 3.3.1.2.5.

        uint8_t PDU_Size;
        // Size of the PDU control information (in bytes).
        // This length value includes the 2 bytes required for the PDU
        // type and PDU size parameters.

        uint_8 DL_PDU [0];
        // See Sections 3.3.1.2.1 to 3.3.1.2.4.
    } DL_PDU_Configuration [0];
};
// 0: DCI DL PDU, see Section 3.3.1.2.1.
struct fapi_l1_dci_dl_pdu {

    uint8_t DCI_Format;
    // Format of the DCI
    // 0 = 1
    // 1 = 1A
    // 2 = 1B
    // 3 = 1C
    // 4 = 1D
    // 5 = 2
    // 6 = 2A
    // CCE Index  uint8_t  CCE index used to send the DCI.
    // Value: 0  88

    uint8_t AggregationLevel;
    // The aggregation level used
    // Value: 1,2,4,8

    uint16_t RNTI;
    // The RNTI used for identifying the UE when receiving the PDU
    // Valid for all DCI formats
    // Value: 1  65535.

    uint8_t ResourceAllocationType;
    // Resource allocation type/header
    // Valid for DCI formats: 1,2,2A
    // 0=type 0
    // 1=type 1

    uint8_t VirtualResourceBlockAssignmentFlag;
    //   Type of virtual resource block used
    // Valid for DCI formats: 1A,1B,1D
    // 0 = localized
    // 1 = distributed

    uint32_t ResourceBlockCoding;
    // The encoding for the resource blocks. It's coding is dependent
    // on whether resource allocation type 0, 1, 2 is in use.
    // Resource allocation type 0 is explicitly signalled for DCI
    // formats 1, 2, 2A
    // Resource allocation type 1 is explicitly signalled for DCI
    // formats 1, 2, 2A
    // Resource allocation type 2 is implicit for DCI
    // formats 1A, 1B, 1C, 1D
    // See [6] section 7.1.6 for the encoding used for each format.
    // Valid for DCI formats: 1,1A,1B,1C,1D ,2,2A
    // See [6] section 7.1.6 for the encoding used for each format
    // and a variable-length bitstring generated.  Further
    // information on the location of this bitstring within the
    // resource block coding 32-bit parameter is implementation-
    // specific

    uint8_t MCS_1;
    // The modulation and coding scheme for 1st transport block
    // Valid for DCI formats: 1,1A,1B,1C,1D ,2,2A
    // Value: 0  31

    uint8_t RedundancyVersion_1;
    //  The redundancy version for 1st transport block.
    // Valid for DCI formats: 1,1A,1B,1C,1D ,2,2A
    // Value: 0  3

    uint8_t NewDataIndicator_1;
    // The new data indicator for 1st transport block.
    // Valid for DCI formats: 1,1A,1B,1C,1D ,2,2A

    uint8_t TransportBlockToCodewordSwapFlag;
    // Indicates the mapping of transport block to codewords
    // Valid for DCI formats: 2,2A
    // 0 = no swapping
    // 1 = swapped

    uint8_t MCS_2;
    // The modulation and coding scheme for 2nd transport block.
    // Valid for DCI formats: 2,2A
    // Value: 0  31

    uint8_t RedundancyVersion_2;
    // The redundancy version for 2nd transport block.
    // Valid for DCI formats: 2,2A
    // Value: 0  3

    uint8_t NewDataIndicator_2;
    // The new data indicator for 2nd transport block.
    // Valid for DCI formats: 2,2A

    uint8_t HARQ_Process;
    // HARQ process number
    // Valid for DCI formats: 1,1A,1B,1D,2,2A
    // Value: 0 15

    uint8_t TPMI;
    // The codebook index to be used for precoding
    // Valid for DCI formats: 1B,1D
    // 2 antenna_ports: 0  3
    // 4 antenna_ports: 0  15

    uint8_t PMI;
    // Confirmation for precoding
    // Valid for DCI formats: 1B
    // 0 = use precoding indicated in TPMI field
    // 1 = use precoding indicated in last PMI report on PUSCH

    uint8_t PrecodingInformation;
    // Precoding information
    // Valid for DCI formats: 2,2A
    // 2 antenna_ports: 0  7
    // 4 antenna_ports: 0  63

    uint8_t TPC;
    // Tx power control command for PUCCH.
    // Valid for DCI formats: 1,1A,1B,1D,2,2A
    // Value:  0,1,2,3
    // In case of DCI format 1A and RNTI=SI-RNTI,RA-RNTI or P-RNTI
    // the TPC values are either 0,1 representing N_PRB=2 and
    // N_PRB =3 respectively. In case of SPS-C-RNTI it represents the
    // PUCCH resource index.

    uint8_t DownlinkAssignmentIndex;
    // The downlink assignment index. Only used in TDD mode,
    // value ignored for FDD.
    // Valid for DCI formats: 1,1A,1B,1D,2,2A
    // Value: 1,2,3,4

    uint8_t N_GAP;
    // Used in virtual resource block distribution
    // Valid for DCI formats: 1A,1B,1C,1D
    // 0= N_GAP1
    // 1= N_GAP2

    uint8_t TransportBlockSizeIndex;
    // The transport block size
    // Valid for DCI formats: 1C
    // Value: 0  31

    uint8_t DownlinkPowerOffset;
    // Indicates the DL power offset type for multi-user MIMO
    // transmission
    // Valid for DCI formats: 1D
    // Value: 0  1

    uint8_t Allocate_PRACH_Flag;
    // Indicates that PRACH procedure is initiated
    // Valid for DCI formats: 1A
    // 0 = false
    // 1=true

    uint8_t PreambleIndex ;
    // The preamble index to be used on the PRACH
    // Valid for DCI formats: 1A
    // Value: 0  63

    uint8_t PRACH_MaskIndex;
    // The mask index to be used on the PRACH
    // Valid for DCI formats: 1A
    // Value: 0  15

    uint8_t RNTI_type;
    // RNTI type
    // Valid for DCI format 1, 1A,2,2A
    // 1 =  C-RNTI
    // 2 =  RA-RNTI, P-RNTI, or SI-RNTI.
    // 3 = SPS-CRNTI

    uint16_t TransmissionPower;
    // Offset to the reference signal power.
    // Value: 0  10000, representing -6 dB to 4 dB in 0.001 dB
    // steps.
};
// 1: BCH PDU, see Section 3.3.1.2.2.
struct fapi_l1_bch_pdu {
    uint16_t Length;
    // The length (in bytes) of the associated MAC PDU, delivered in
    // TX.request. This should be the actual length of the MAC
    // PDU, which may not be a multiple of 32-bits.

    uint16_t     PDU_Index;
    // This is a count value which is incremented every time a BCH,
    // MCH, PCH or DLSCH PDU is included in the
    // DL_CONFIG.request message.
    // This value is repeated in TX.request and associates the
    // control information to the data.
    // It is reset to 0 for every subframe
    // Range 0  65535

    uint16_t TransmissionPower;
    // Offset to the reference signal power.
    // Value: 0  10000, representing -6 dB to 4 dB in 0.001 dB steps.
};
// 2: MCH PDU, see Section 3.3.1.2.3.
struct fapi_l1_mch_pdu {
    uint16_t Length;
    // The length (in bytes) of the associated MAC PDU, delivered in
    // TX.request.  This should be the actual length of the MAC
    // PDU, which may not be a multiple of 32-bits

    uint16_t PDU_Index;
    // This is a count value which is incremented every time a BCH,
    // MCH, PCH or DLSCH PDU is included in the
    // DL_CONFIG.request message.
    // This value is repeated in TX.request and associates the
    // control information to the data.
    // It is reset to 0 for every subframe
    // Range 0  65535

    uint16_t RNTI;
    // The RNTI associated with the MCH
    // See [3] section 5.1.4
    // Value: 1  65535.

    uint8_t ResourceAllocationType;
    // Resource allocation type
    // See [6] section 7.1.6
    // 0 = type 0
    // 1 = type 1
    // 2 = type 2

    uint32_t ResourceBlockCoding;
    // The encoding for the resource blocks. It's coding is dependent
    // on whether resource allocation type 0, 1, 2 is in use.
    // See [6] section 7.1.6 for the encoding used for each format.

    uint8_t Modulation;
    // 2: QPSK
    // 4: 16QAM
    // 6: 64QAM

    uint16_t TransmissionPower;
    // Offset to the reference signal power.
    // Value: 0  10000, representing -6 dB to 4 dB in 0.001 dB
    // steps.
};

// Table 43: BfVectorType Structure
struct fapi_l1_bf_vector_type {
    uint8_t subbandIndex;
    // Index of subband for which the following beam forming
    // vector is applied

    uint8_t numAntennas;
    // Number of physical antennas
    // For each physical antenna

    uint16_t bfValue;
    // Beam forming vector element for physical antenna #i real 8
    // bits followed by imaginery 8 bits
};
// 3: DLSCH PDU, see Section 3.3.1.2.4.
struct fapi_l1_dlsch_pdu {
    uint16_t Length;
    // The length (in bytes) of the associated MAC PDU, delivered in
    // TX.request. This should be the actual length of the MAC
    // PDU, which may not be a multiple of 32-bits.

    uint16_t PDU_Index;
    // This is a count value which is incremented every time a BCH,
    // MCH, PCH or DLSCH PDU is included in the
    // DL_CONFIG.request message.
    // This value is repeated in TX.request and associates the
    // control information to the data.
    // It is reset to 0 for every subframe
    // Range 0  65535

    uint16_t RNTI;
    // The RNTI associated with the UE
    // See [3] section 5.1.4
    // Value: 1  65535.

    uint8_t ResourceAllocationType;
    // Resource allocation type
    // See [3] section 7.1.6
    // 0 = type 0
    // 1 = type 1
    // 2 = type 2
    // VirtualResourceBlockAssignmentFlag  uint8_t
    // Type of virtual resource block used. This should match the
    // value sent in the DCI Format 1A, 1B, 1D PDU which allocated
    // this grant.
    // See [6] section 7.1.6.3
    // 0 = localized
    // 1 = distributed

    uint32_t ResourceBlockCoding;
    // The encoding for the resource blocks. It's coding is dependent
    // on whether resource allocation type 0, 1, 2 is in use. This
    // should match the value sent in the DCI Format PDU which
    // allocated this grant.
    // See [6] section 7.1.6 for the encoding used for each format.

    uint8_t Modulation;
    // 2: QPSK
    // 4: 16QAM
    // 6: 64QAM

    uint8_t RedundancyVersion;
    // HARQ redundancy version. This should match the value sent in
    // the DCI Format PDU which allocated this grant.
    // Value: 0  3.

    uint8_t TransportBlocks;
    // The transport block transmitted to this RNTI. A value of 2
    // indicates this is the second transport block for either DCI
    // format 2 or 2A. For other DCI values this field will always be 1.
    // Value: 1  2

    uint8_t TransportBlockToCodewordSwapFlag;
    // Indicates the mapping of transport block to codewords. This
    // should match the value sent in the DCI Format 2, 2A PDU
    // which allocated this grant.
    // 0 = no swapping
    // 1 = swapped

    uint8_t TransmissionScheme;
    // The MIMO mode used in the PDU
    // See [6] section 7.1.
    // 0: SINGLE_ANTENNA_PORT_0,
    // 1: TX_DIVERSITY,
    // 2: LARGE_DELAY_CDD,
    // 3: CLOSED_LOOP_SPATIAL_MULTIPLEXING,
    // 4: MULTI_USER_MIMO,
    // 5: CLOSED_LOOP_RANK_1_PRECODING,
    // 6: SINGLE_ANTENNA_PORT_5.

    uint8_t NumberOfLayers;
    // The number of layers used in transmission
    // See [8] section 6.3.3
    // Value: 1  4

    uint8_t NumberOfSubbands;
    // Only valid when transmission scheme = 3, 4, 5.
    // Defines the number of subbands and codebooks used for PMI.
    // If value=1 then a single PMI value is supplied which should be
    // used over all RB
    // Value 0 -> 13

    struct {
        uint8_t CodebookIndex;
        // Only valid when transmission scheme = 3, 4, 5.
        // Defines the codebook used.
        // When antenna port = 1: NA
        // When antenna port = 2: 0..3
        // When antenna port = 4: 0..15
    } NumberOfSubbandEntries [FAPI_L1_NUMBER_OF_SUBBANDS_MAX]; //FIXME: 13

    uint8_t UE_CategoryCapacity;
    // The UE capabilities category
    // See [10] section 4.1.
    // Value:1  5

    uint8_t P_A;
    // The ratio of PDSCH EPRE to cell-specific RS EPRE among PDSCH
    // REs in all the OFDM symbols not containing cell-specific RS in dB.
    // See [6], section 5.2.
    // 0: -6dB
    // 1: -4.77dB
    // 2: -3dB
    // 3: -1.77dB
    // 4: 0dB
    // 5: 1dB
    // 6: 2dB
    // 7: 3dB

    uint8_t DeltaPowerOffsetIndex;
    // Delta power offset, value: 0..1,
    // Refer to:
    // Table 7.1.5-1 in [6] for Multi-user MIMO mode. Takes value
    // zero for all other modes.

    uint8_t N_GAP;
    // Used in virtual resource block distribution
    // 0= NGAP1
    // 1=  NGAP2

    uint8_t N_PRB;
    // Used with DCI format 1A and RNTI=SI-RNTI or RA-RNTI. This
    // should match the value sent in the TPC field of the DCI 1A PDU
    // which allocated this grant.
    // 0: NPRB = 2
    // 1: NPRB = 3

    uint8_t numBfPRBperSubban;
    // Number of PRBs that are treated as one subband
    // numBfVector  uint8_t  Number of beam forming vectors
    // One beam forming vector is specified for each subband

    struct fapi_l1_bf_vector_type bfVector[0];
    // BfVectorType[numBfVector];
    // Beam forming vectors, see Table 43.
};
// 4: PCH PDU, see Section 3.3.1.2.5.
struct fapi_l1_pch_pdu {

    uint16_t Length;
    //  The length (in bytes) of the associated MAC PDU, delivered in
    // TX.request. This should be the actual length of the MAC
    // PDU, which may not be a multiple of 32-bits.

    uint16_t PDU_Index;
    // This is a count value which is incremented every time a BCH,
    // MCH, PCH or DLSCH PDU is included in the
    // DL_CONFIG.request message.
    // This value is repeated in TX.request and associates the
    // control information to the data.
    // It is reset to 0 for every subframe
    // Range 0  65535

    uint16_t P_RNTI;
    // The P-RNTI associated with the paging
    // See [3] section 5.1.4
    // Value: 0xFFFE

    uint8_t ResourceAllocationType;
    // Resource allocation type
    // See [6] section 7.1.6
    // 2 = type 2

    uint8_t VirtualResourceBlockAssignment;
    // Type of virtual resource block used. This should match the
    // value sent in the DCI Format 1A, 1B, 1D PDU which allocated
    // flag  this grant.
    // See [6] section 7.1.6.3
    // 0 = localized
    // 1 = distributed

    uint32_t ResourceBlockCoding;
    // The encoding for the resource blocks. It's coding is dependent
    // on whether resource allocation type 0, 1, 2 is in use. This
    // should match the value sent in the DCI Format PDU which
    // allocated this grant.
    // See [6] section 7.1.6 for the encoding used for each format.

    uint8_t MCS;
    // For PCH PDU only QPSK modulation is allowed.
    // 0: QPSK

    uint8_t RedundancyVersion;
    // For PCH PDU only redundancy version 0 is allowed
    // Value: 0

    uint8_t NumberOfTransportBlocks;
    // The number of transport blocks transmitted to this RNTI. Only
    // 1 transport block is sent on the PCH per subframe.
    // Value: 1

    uint8_t TransportBlockToCodewordSwapFlag;
    // Reserved.  This parameter is not used on the PCH transport
    // channel.

    uint8_t TransmissionScheme;
    // The MIMO mode used in the PDU
    // See [6] section 7.1.
    // 0: SINGLE_ANTENNA_PORT_0,
    // 1: TX_DIVERSITY,
    // 6: SINGLE_ANTENNA_PORT_5.

    uint8_t NumberOfLayers;
    // The number of layers used in transmission
    // See [8] section 6.3.3
    // Value: 1  4

    uint8_t CodebookIndex;
    // Reserved.  This parameter is not used on the PCH transport
    // channel.

    uint8_t UE_CategoryCapacity;
    // Reserved.  This parameter is not used on the PCH transport
    // channel.

    uint8_t P_A;
    // The ratio of PDSCH EPRE to cell-specific RS EPRE among
    // PDSCH REs in all the OFDM symbols not containing cell-
    // specific RS in dB.
    // See [6], section 5.2.
    // 0: -6dB
    // 1: -4.77dB
    // 2: -3dB
    // 3: -1.77dB
    // 4: 0dB
    // 5: 1dB
    // 6: 2dB
    // 7: 3dB

    uint16_t TransmissionPower;
    // Offset to the reference signal power.
    // Value: 0  10000, representing -6 dB to 4 dB in 0.001 dB steps.

    uint8_t N_PRB;
    // Used with DCI 1A format. This should match the value sent in
    // the TPC field of the DCI 1A PDU which allocated this grant.
    // 0: NPRB = 2
    // 1: NPRB = 3

};

//// UL //// UL //// UL //// UL //// UL //// UL //// UL //// UL //// UL ////
struct fapi_l1_ul_config_request {
    fapi_l1_message_t hdr;
};

struct fapi_l1_subframe_indication {
    fapi_l1_message_t hdr;
    uint16_t SFN_SF;
    // A 16-bit value where, [15:4] SFN, range 0  1023; [3:0] SF, range 0  9
};

struct fapi_l1_hi_dci0_request {
    fapi_l1_message_t hdr;
};

struct fapi_l1_tx_request {
    fapi_l1_message_t hdr;
};

struct fapi_l1_harq_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_crc_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_rx_ulsch_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_rach_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_srs_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_rx_sr_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_rx_cqi_indication {
    fapi_l1_message_t hdr;
};



#endif /* _L1_L2_IFACE_H_ */

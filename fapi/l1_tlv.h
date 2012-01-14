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

#ifndef _L1_TLV_H_
#define _L1_TLV_H_

/** 3.2.3  Configuration TLVs
    The configuration TLVs that are used  in the PARAM and CONFIG message exchanges.

    Type      Description  Table 17: TLV format
    -------------------------------------------
    uint8_t   Tag
    uint8_t   Length (in bytes)
    uint16_t  Value
*/

enum FAPI_L1_Configuration_TLV_TAG_e {
    FAPI_L1_DuplexingMode = 1,
    // Type of duplexing mode
    // Value: 0 : TDD, 1 : FDD, 2 : HD_FDD

    FAPI_L1_PCFICH_PowerOffset = 2,
    // The power per antenna of the PCFICH with respect to the
    // reference signal.
    // Value: 0 - 10000, represents -6dB to 4dB in steps 0.001dB

    FAPI_L1_P_B = 3,
    // Refers to downlink power allocation.
    // See [6] section 5.2 Value is an index into the referenced table.
    // Value: 0 - 3

    FAPI_L1_DL_CyclicPrefixType = 4,
    // Cyclic prefix type, used for DL
    // See [8] section 5.2.1
    // 0: CP_NORMAL,
    // 1: CP_EXTENDED.

    FAPI_L1_UL_CyclicPrefixType = 5,
    // Cyclic prefix type, used for UL
    // See [8] section 5.2.1
    // 0: CP_NORMAL,
    // 1: CP_EXTENDED.

    /******* RFConfig *******/

    FAPI_L1_DownlinkChannelBandwidth = 6,
    // Downlink channel bandwidth in resource blocks.
    // See [7] section 5.6.
    // Value: 6,15, 25, 50, 75, 100

    FAPI_L1_UplinkChannelBandwidth = 7,
    // Uplink channel bandwidth in resource blocks.
    // See [7] section 5.6
    // Value: 6,15, 25, 50, 75,100

    FAPI_L1_ReferenceSignalPower = 8,
    // Normalized value levels (relative) to accommodate different
    // absolute Tx Power used by eNb.
    // Value: 0 - 255
    // Representing 0dB to -63.75dB in -0.25dB steps.

    FAPI_L1_TxAntennaPorts = 9,
    // The number of cell specific transmit antenna ports.
    // See [8] section 6.2.1.
    // Value:1,2,4

    FAPI_L1_Rx_Antenna_Ports = 10,
    // The number of cell specific receive antenna ports.
    // See [8] section 6.2.1.
    // Value: 1, 2, 4

    /******* PHICH Config *******/

    FAPI_L1_PHICH_Resource = 11,
    // The number of resource element groups used for PHICH.
    // See [8] section 6.9.
    // 0: PHICH_R_ONE_SIXTH
    // 1: PHICH_R_HALF
    // 2: PHICH_R_ONE
    // 3: PHICH_R_TWO

    FAPI_L1_PHICH_Duration = 12,
    // The PHICH duration for MBSFN and non-MBSFN sub-frames.
    // See [8] section 6.9
    // 0: PHICH_D_NORMAL
    // 1: PHICH_D_EXTENDED

    FAPI_L1_PHICH_PowerOffset = 13,
    // The power per antenna of the PHICH with respect to the
    // reference signal.
    // Value: 0-> 10000, represents -6dB to 4dB in steps 0.001dB

    /******* SCH Config *******/

    FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS = 14,
    // The power of synchronization signal with respect to the reference signal,
    // Value: 0 - 9999 represents -6dB to 4dB in step 0.001dB

    FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS = 15,
    // The power of synchronization signal with respect to the
    // reference signal,
    // Value: 0 - 9999 represents -6dB to 4dB in step 0.001dB

    FAPI_L1_PhysicalCell_ID = 16,
    // The Cell ID sent with the synchronization signal.
    // See [8] section 6.11.
    // Value: 0 - 503

    /******* PRACH Config *******/

    FAPI_L1_ConfigurationIndex = 17,
    // Provides information about the location and format of the PRACH.
    // See [8] section 5.7. Table 5.7.1-2 for FDD, Table 5.7.1-3 for TDD
    // The value is an index into the referenced tables.
    // Value: 0 - 63

    FAPI_L1_RootSequenceIndex = 18,
    // PRACH Root sequence index.
    // See [8] section 5.7.2.
    // Value: 0 - 837

    FAPI_L1_ZeroCorrelationZoneConfiguration = 19,
    // Equivalent to Ncs, see [8] section 5.7.2.
    // TDD: 0 - 6
    // FDD: 0 - 15

    FAPI_L1_HighSpeedFlag = 20,
    // Indicates if unrestricted, or restricted, set of preambles is used.
    // See [8] section 5.7.2.
    // 0: HS_UNRESTRICTED_SET
    // 1: HS_RESTRICTED_SET

    FAPI_L1_FrequencyOffset = 21,
    // The first physical resource block available for PRACH. see [8]
    // section 5.7.1
    // Value: 0 - UL_channel_bandwidth  6

    /******* PUSCH Config *******/

    FAPI_L1_HoppingMode = 22,
    // If hopping is enabled indicates the type of hopping used.
    // See [8] section 5.3.4
    // 0: HM_INTER_SF
    // 1: HM_INTRA_INTER_SF

    FAPI_L1_HoppingOffset = 23,
    // The offset used if hopping is enabled.
    // See [8] section 5.3.4
    // Value: 0 - 98

    FAPI_L1_NumberOfSubBands = 24,
    // The number of sub-bands used for hopping.
    // See [8] section 5.3.4.
    // Value: 1 - 4

    /******* PUCCH Config *******/

    FAPI_L1_Delta_PUCCH_Shift = 25,
    // The cyclic shift difference.
    // See [8] section 5.4.1.
    // Value: 1 - 3

    FAPI_L1_N_CQI_RB = 26,
    // The bandwidth, in units of resource blocks, that is available for
    // use by PUCCH formats 2/2a/2b transmission in each slot.
    // See Section 5.4 in [8].
    // Value: 0 - 98

    FAPI_L1_N_AN_CS = 27,
    // The number of cyclic shifts used for PUCCH formats 1/1a/1b in a
    // resource block with a mix of formats 1/a/1/ab and 2/2a/2b. See
    // Section 5.4 in [8].
    // Value: 0 - 7

    FAPI_L1_N1Pucch_AN = 28,
    // N(1)_PUCCH , see [6] section 10.1
    // Value: 0 - 2047

    /******* SRS Config *******/

    FAPI_L1_BandwidthConfiguration = 29,
    // The available SRS bandwidth of the cell.
    // See [8] section 5.5.3
    // The value is an index into the referenced table.
    // Value: 0 - 7

    FAPI_L1_MaxUpPTS = 30,
    // Used for TDD only and indicates how SRS operates in UpPTS
    // subframes.
    // See [8] section 5.5.3.2 and [6] section 8.2
    // 0: Disabled
    // 1: Enabled

    FAPI_L1_SRS_SubframeConfiguration = 31,
    // The subframe configuration. Needed if semi-static configuration
    // is held in PHY.
    // Value: 0 - 15

    FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission = 32,
    // Indicates if SRS and ACK/NACK can be received in the same
    // subframe. Needed if semi-static configuration is held in PHY.
    // 0: no simultaneous transmission
    // 1: simultaneous transmission

    /******* Uplink Reference Signal Config *******/

    FAPI_L1_Uplink_RS_Hopping = 33,
    // Indicates the type of hopping to use.
    // See [8] section 5.5.1.
    // 0: RS_NO_HOPPING
    // 1: RS_GROUP_HOPPING
    // 2: RS_SEQUENCE_HOPPING

    FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern = 34,
    // The sequence shift pattern used if group hopping is enabled.
    // See [8] section 5.5.1
    // Values: 0 - 29

    FAPI_L1_CyclicShift1For_DMRS = 35,
    // Specifies the cyclic shift for the reference signal used in the cell.
    // See [8] section 5.5.1.
    // The value is an index into the referenced table.
    // Value: 0 - 7

    /******* TDD Frame Structure Config *******/

    FAPI_L1_SubframeAssignment = 36,
    // For TDD mode only, indicates the DL/UL subframe structure.
    // See [8] section 4.2.
    // Value: 0 - 6

    FAPI_L1_SpecialSubframePatterns = 37,
    // For TDD mode only. Length of fields DwPTS, GP and UpPTS.
    // See [8] section 4.2.
    // Value: 0 - 8

    FAPI_L1_DownlinkBandwidthSupport = 40,
    // The PHY downlink channel bandwidth capability (in resource blocks).
    // See [7] section 5.6
    // Value: bitX :0 = no support, 1= support.
    // Bit0: 6
    // Bit1: 15
    // Bit2: 25
    // Bit3: 50
    // Bit4: 75
    // Bit5: 100

    FAPI_L1_UplinkBandwidthSupport = 41,
    // The PHY uplink channel bandwidth capability (in resource blocks).
    // See [7] section 5.6
    // Value: bitX :0 = no support, 1= support.
    // Bit0: 6
    // Bit1: 15
    // Bit2: 25
    // Bit3: 50
    // Bit4: 75
    // Bit5: 100

    FAPI_L1_DownlinkModulationSupport = 42,
    // The PHY downlink modulation capability.
    // Value: bitX :0 = no support, 1= support.
    // Bit0: QPSK
    // Bit1: 16QAM
    // Bit2: 64QAM

    FAPI_L1_UplinkModulationSupport = 43,
    // The PHY uplink modulation capability.
    // Value: bitX :0 = no support, 1= support.
    // Bit0: QPSK
    // Bit1: 16QAM
    // Bit2: 64QAM

    FAPI_L1_PHY_AntennaCapability = 44,
    // Number of antennas supported.
    // Value: 1, 2, 4

    FAPI_L1_DataReportMode = 50,
    // The data report mode for the uplink data.
    // 0: A CRC.indication message is sent in every subframe.  If
    // ULSCH data has been processed, the CRC.indication contains
    // CRC results for the subframe. The CRC results are, also, given in
    // the RX.indication message.
    // 1: The CRC.indication message is not sent. The CRC results
    // are given in the RX.indication message.

    FAPI_L1_SFN_SF = 51,
    // The future SFN/SF subframe where the TLVs included in the
    // message should be applied.
    // A 16-bit value where,
    // [15:4] SFN, range 0 - 1023
    // [3:0] SF, range 0 - 9
    // These TLVs are used by L1 to report its current status.

    FAPI_L1_PHY_State = 60,
    // Indicates the current operational state of the PHY.
    // 0 = IDLE
    // 1 = CONFIGURED
    // 2 = RUNNING

    /** 3.2.6.4  UE Configuration TLVs
        The configuration TLVs that are used in the UE_CONFIG message exchanges.

        Type      Description  Table 25: TLV format
        -------------------------------------------
        uint8_t   Tag
        uint8_t   Length (in bytes)
        variable  Value  (bytes % 4 == 0)

        XXX: Note that Value is variable in UE message.
        XXX: Threre is oportunity to optimize encoder using this destinction.
    */

    FAPI_L1_Handle = 100,
    // An opaque handling to associate the received information in RX.indication

    FAPI_L1_RNTI = 101,
    // The RNTI used for identifying the UE when receiving the PDU
    // See [3] section 5.1.4
    // Value: 1 - 65535.

    /******* CQI Config *******/

    FAPI_L1_CQI_PUCCH_ResourceIndex = 102,
    // The PUCCH resource for periodc CQI reporting.
    // Value: 0 - 1185.

    FAPI_L1_CQI_PMI_ConfigIndex = 103,
    // The periodic PMI reporting configuration.
    // Value: 0 - 1023.

    FAPI_L1_CQI_RI_ConfigIndex = 104,
    // The periodic RI reporting configuration.
    // Value: 0 - 1023.

    FAPI_L1_CQI_SimultaneousAckNackAnd_CQI = 105,
    // Indicates if simultaneous transmission of CQI and ACK/NACK is  allowed.
    // Value:
    // 0: no PUCCH Format 2a/2b
    // 1: PUCCH Format 2a/2b can be used

    /******* ACK/NACK Config *******/

    FAPI_L1_AN_RepetitionFactor = 106,
    // The ACK/NACK repetition  factor
    // Value: 2,4,6

    FAPI_L1_AN_n1PUCCH_ANRep = 107,
    // The ACK/NACK repetition PUCCH resource index
    // Value: 0- 2047

    FAPI_L1_TDD_AckNackFeedbackMode = 108,
    // The TDD ACK/NACK Feedback Mode
    // Value:
    // 0: bundling
    // 1: multiplexing

    /******* SRS Config *******/

    FAPI_L1_SRS_Bandwidth = 109,
    // SRS Bandwidth. This value is fixed for a UE and allocated in RRC
    // connection setup.
    // See [8] section 5.5.3.2
    // Value: 0 - 3

    FAPI_L1_SRS_Hopping_Bandwidth = 110,
    // Configures the frequency hopping on the SRS. This value is fixed
    // for a UE and allocated in RRC connection setup.
    // See [8] section 5.5.3.2.
    // Value 0 - 3

    FAPI_L1_FrequencyDomainPosition = 111,
    // Frequency-domain position, NRRC This value is fixed for a UE and
    // allocated in RRC connection setup.
    // See [8] section 5.5.3.2
    // Value: 0 - 23

    FAPI_L1_SRS_Duration = 112,
    // The duration of the SRS configuration
    // Value:
    // 0: once
    // 1: indefinite

    FAPI_L1_ISRS_SRS_ConfigIndex = 113,
    // Defines the periodicity and subframe location of the SRS.
    // SRS Configuration Index. This value is fixed for a UE and
    // allocated in RRC connection setup.
    // See [6] section 8.2.
    // Value: 0 - 1023.

    FAPI_L1_TransmissionComb = 114,
    // Configures the frequency location of the SRS. This value is fixed
    // for a UE and allocated in RRC connection setup.
    // Value: 0 -  1

    FAPI_L1_SoundingReferenceCyclicShift = 115,
    // Configures the SRS sequence generation. This value is fixed for a
    // UE and allocated in RRC connection setup.
    // See [8] section 5.5.3.1.
    // Value: 0 - 7

    /******* SR Config *******/

    FAPI_L1_SR_PUCCH_ResourceIndex = 116,
    // The scheduling request PUCCH resource index.
    // Value: 0 - 2047.

    FAPI_L1_SR_ConfigIndex = 117,
    // The scheduling request configuration index.
    // Value: 0 - 155.

    /******* SPS Config *******/

    FAPI_L1_SPS_DL_ConfigSchedulingInterval = 118,
    // SPS Configuration Interval
    // Value: 10,20,32,40,64,80,128,160,320,640

    FAPI_L1_SPS_DL_n1PUCCH_AN_Persistent = 119,
    // The SPS PUCCH AN Resource configuration.
    // The TLV can be repeated four times.
    // Value: 0 - 2047.
};

/** \brief This enumeration serve as:
    type_TAG or SIZE in bytes in encoder
    or BINARY FLAG used for messagege validation
*/
enum L1_TLV_TAG_TYPE_e {
    TT_UINT8_T  = 0x1,
    TT_UINT16_T = 0x2,
    TT_UINT32_T = 0x4,
};


/**
 * @brief tlv_encode_tag function encode TLV into memory pointer with given offset
 *
 * @param tag    - FAPI_L1_Configuration_TLV_TAG_e or FAPI_L1_UE_Configuration_TLV_TAG_e
 * @param value  - pointer to the value of tag
 * @param buffer - encoding buffer
 * @param offset - buffer offset
 *
 * @return consumed buffer in bytes or -1 on error
 */
ssize_t tlv_encode_tag(uint8_t tag, uint8_t *value, uint8_t *buffer, size_t offset);

/**
 * @brief tlv_decode_tag function decode TLV from memory pointer on given offset
 *
 * @param tag    - out value for FAPI_L1_Configuration_TLV_TAG_e or FAPI_L1_UE_Configuration_TLV_TAG_e
 * @param value  - pointer to the value of tag
 * @param buffer - encoding buffer
 * @param offset - buffer offset
 *
 * @return addvances bytes or -1 on error
 */
ssize_t tlv_decode_tag(uint8_t *tag, uint8_t *value, uint8_t *buffer, size_t offset);

/**
 * Copy tag from one place to another
 *
 * @param tag - target tag
 * @param src - source memory
 * @param dst - destination memory
 *
 * @return addvances bytes or -1 on error
 */
ssize_t tlv_copy_tag(uint8_t tag, uint8_t *src, uint8_t *dst);

#endif /*_L1_TLV_H_ */

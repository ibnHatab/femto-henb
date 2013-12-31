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

#ifndef _L1_TYPES_H_
#define _L1_TYPES_H_

#ifndef __KERNEL__
#ifndef _cplusplus
#define bool int
#define false 0
#define true  1
#endif	/* bool */
#endif	/* __KERNEL__ */

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/* There is some amount of overlap with <sys/types.h> as known by inet code */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
#endif

#define INVALID_SFNSF (1 << 15)

/// The SFN/SF of the SUBFRAME this information was received in.
typedef union {
    uint16_t sfn_sf; // A 16-bit  value where,
    struct
    {
        uint16_t sf  : 4;  // [3:0] SF, range 0 - 9       Sub-Frame a.k.a subframeNumber :FIXME
        uint16_t sfn : 12; // [15:4] SFN, range 0 - 1023  Sustem Frame Number a.k.a frameNumber
    } __packed val;
} __packed l1_sfn_sf_t;


#define FAPI_L1_NUMBER_OF_SUBBANDS_MAX 13

typedef enum {
    FAPI_L1_DM_TDD                   = 0,
    FAPI_L1_DM_FDD                   = 1,
    FAPI_L1_DM_HD_FDD                = 2
} FAPI_DuplexingMode_e;

typedef enum {
    DIR_DL                           = 0,
    DIR_UL                           = 1,
    DIR_BOTH                         = 2
} FAPI_Direction_e;

typedef enum {
    DCI_1                            = 0,
    DCI_1A                           = 1,
    DCI_1B                           = 2,
    DCI_1C                           = 3,
    DCI_1D                           = 4,
    DCI_2                            = 5,
    DCI_2A                           = 6
} FAPI_DCI_Format_e;

typedef enum {
    N_GAP1                           = 0,
    N_GAP2                           = 1
} FAPI_N_GAP_e;

// The PHY downlink modulation capability.
// Value: bitX :0                    = no support, 1= support.
typedef enum {
    MD_QPSK                          = 0x02,    // Bit0: 
    MD_16QAM                         = 0x04,    // Bit1: 
    MD_64QAM                         = 0x04     // Bit2: 
} FAPI_Modulation_e;

typedef enum {
    SINGLE_ANTENNA_PORT_0            = 0, // 0: SINGLE_ANTENNA_PORT_0,
    TX_DIVERSITY                     = 1, // 1: TX_DIVERSITY,
    LARGE_DELAY_CDD                  = 2, // 2: LARGE_DELAY_CDD,
    CLOSED_LOOP_SPATIAL_MULTIPLEXING = 3, // 3: CLOSED_LOOP_SPATIAL_MULTIPLEXING,
    MULTI_USER_MIMO                  = 4, // 4: MULTI_USER_MIMO,
    CLOSED_LOOP_RANK_1_PRECODING     = 5, // 5: CLOSED_LOOP_RANK_1_PRECODING,
    SINGLE_ANTENNA_PORT_5            = 6  // 6: SINGLE_ANTENNA_PORT_5.
} FAPI_TransmissionScheme_e;

// The TDD ACK/NACK Feedback Mode
typedef enum {
    ACK_NACK_BUNDLING                = 0,
    ACK_NACK_MULTIPLEXING            = 1,
    ACK_NACK_SPECIAL_BUNDLING        = 2
} FAPI_ACK_NACK_Mode_e;

// Cyclic prefix type, used for UL
// See [8] section 5.2.1
typedef enum {
    CP_NORMAL                        = 0,
    CP_EXTENDED                      = 1
} FAPI_CyclicPrefixType_e;

// The number of resource element groups used for PHICH.
// See [8] section 6.9.
typedef enum {
    PHICH_R_ONE_SIXTH                = 0,
    PHICH_R_HALF                     = 1,
    PHICH_R_ONE                      = 2,
    PHICH_R_TWO                      = 3
} FAPI_PHICH_Resource_e;

// The PHICH duration for MBSFN and non-MBSFN sub-frames.
// See [8] section 6.9
typedef enum {
    PHICH_D_NORMAL                   = 0,
    PHICH_D_EXTENDED                 = 1
} FAPI_PHICH_Duration_e;

// Indicates if unrestricted, or restricted, set of preambles is used.
// See [8] section 5.7.2.
typedef enum {
    HS_UNRESTRICTED_SET              = 0,
    HS_RESTRICTED_SET                = 1
} FAPI_HighSpeedFlag_e;

// If hopping is enabled indicates the type of hopping used.
// See [8] section 5.3.4
typedef enum {
    HM_INTER_SF                      = 0,
    HM_INTRA_INTER_SF                = 1
} FAPI_HoppingMode_e;

// Used for TDD only and indicates how SRS operates in UpPTS
// subframes.
// See [8] section 5.5.3.2 and [6] section 8.2
typedef enum {
    MAX_UP_PTS_DISABLED              = 0,
    MAX_UP_PTS_ENABLED               = 1
} FAPI_MaxUpPTS_e;

// Indicates if SRS and ACK/NACK can be received in the same
// subframe. Needed if semi-static configuration is held in PHY.
typedef enum {
    SRS_NO_SIMULTANEOUS_TRANSMISSION = 0,
    SRS_SIMULTANEOUS_TRANSMISSION    = 1
} FAPI_SRS_AckNack_SRS_SimultaneousTransmission_e;

// Indicates the type of hopping to use.
// See [8] section 5.5.1.
typedef enum {
    RS_NO_HOPPING                    = 0,
    RS_GROUP_HOPPING                 = 1,
    RS_SEQUENCE_HOPPING              = 2
} FAPI_Uplink_RS_Hopping_e;

// Indicates the current operational state of the PHY.
typedef enum {
  PHY_STATE_IDLE                   = 1 << 0,
  PHY_STATE_CONFIGURED             = 1 << 1,
  PHY_STATE_RUNNING                = 1 << 2
} FAPI_PHY_State_e;

typedef enum {
    // Bit0: 6
    BW_RB_6   = 1 << 0,
    // Bit1: 15
    BW_RB_15  = 1 << 1,
    // Bit2: 25
    BW_RB_25  = 1 << 2,
    // Bit3: 50
    BW_RB_50  = 1 << 3,
    // Bit4: 75
    BW_RB_75  = 1 << 4,
    // Bit5: 100
    BW_RB_100 = 1 << 5,
} BW_SUPPORT_RB_Map_e;

typedef enum {
    DCI_DL_PDU_TYPE = 0, // 0: DCI DL PDU, see Section 3.3.1.2.1.
    BCH_PDU_TYPE    = 1, // 1: BCH PDU,    see Section 3.3.1.2.2.
    MCH_PDU_TYPE    = 2, // 2: MCH PDU,    see Section 3.3.1.2.3.
    DLSCH_PDU_TYPE  = 3, // 3: DLSCH PDU,  see Section 3.3.1.2.4.
    PCH_PDU_TYPE    = 4  // 4: PCH PDU,    see Section 3.3.1.2.5.
} DL_PDU_Type_e;

typedef enum {
  ULSCH_PDU_TYPE             = 0, //  see Section 3.3.1.3.1.
  ULSCH_CQI_RI_PDU_TYPE      = 1, //  see Section 3.3.1.3.2.
  ULSCH_HARQ_PDU_TYPE        = 2, //  see Section 3.3.1.3.3.
  ULSCH_CQI_HARQ_RI_PDU_TYPE = 3, //  see Section 3.3.1.3.4.
  UCI_CQI_PDU_TYPE           = 4, //  see Section 3.3.1.3.6.
  UCI_SR_PDU_TYPE            = 5, //  see Section 3.3.1.3.7.
  UCI_HARQ_PDU_TYPE          = 6, //  see Section 3.3.1.3.8.
  UCI_SR_HARQ_PDU_TYPE       = 7, //  see Section 3.3.1.3.9.
  UCI_CQI_HARQ_PDU_TYPE      = 8, //  see Section 3.3.1.3.10.
  UCI_CQI_SR_PDU_TYPE        = 9, //  see Section  3.3.1.3.11
  UCI_CQI_SR_HARQ_PDU_TYPE   = 10, //  see Section  3.3.1.3.11
  SRS_PDU_TYPE               = 11, //  see Section 3.3.1.3.16.
  HARQ_BUFFER_PDU_TYPE       = 12, //  see Section 3.3.1.3.17
} UL_PDU_Type_e;

typedef enum {
  HI_PDU_TYPE = 0,    // see Section 3.3.1.4.1.
  DCI_UL_PDU_TYPE = 1 // see Section 3.3.1.4.2.
} HI_DCI0_Type_e;

typedef enum {
  HI_NACK = 0,
  HI_ACK = 1
} HI_ACK_NACK_e;

// A flag indicating if a CRC error was detected.
typedef enum {
  CRC_CORRECT = 0,
  CRC_ERROR = 1
} CRC_ERROR_e;

#endif


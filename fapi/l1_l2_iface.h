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

struct fapi_l1_param_request {
  fapi_l1_message_t hdr;
  // pass
};

struct fapi_l1_param_response {
  fapi_l1_message_t hdr;
  uint8_t errorCode;      // See Table 86.
  uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
  uint8_t TLVs[0];        // Variable  See Table 6 to Table 9.
};

struct fapi_l1_config_request {
  fapi_l1_message_t hdr;
  uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
  uint8_t TLVs[0];        // Variable  See Table 12 to Table 14.
};

struct fapi_l1_config_response {
  fapi_l1_message_t hdr;
  uint8_t errorCode;      // See Table 86.
  uint8_t numberOfInvalidOrUnsupportedTLVs;
  uint8_t numberOfMissingTLVs;
  uint8_t TLVs[0];
  // A list of invalid or unsupported TLVs each TLV is presented in its entirety.
  // A list of missing TLVs  each TLV is presented in its entirety.
};

struct fapi_l1_start_request {
  fapi_l1_message_t hdr;
  // The message length in the generic header = 0.
};

struct fapi_l1_stop_request {
  fapi_l1_message_t hdr;
  // The message length in the generic header = 0.
};

struct fapi_l1_stop_indication {
  fapi_l1_message_t hdr;
  // The message length in the generic header = 0.
};

struct fapi_l1_ue_config_request {
  fapi_l1_message_t hdr;
  uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
  uint8_t TLVs[0];        // Variable  See Table 22.
};

struct fapi_l1_ue_config_response {
    fapi_l1_message_t hdr;
  uint8_t errorCode;      // See Table 86.
  uint8_t numberOfInvalidOrUnsupportedTLVs;
  uint8_t numberOfMissingTLVs;
  uint8_t TLVs[0];
  // A list of invalid or unsupported TLVs each TLV is presented in its entirety.
  // A list of missing TLVs  each TLV is presented in its entirety.
};

struct fapi_l1_error_indication {
    fapi_l1_message_t hdr;
};

struct fapi_l1_ue_release_request {
  fapi_l1_message_t hdr;
  uint8_t numberOfTLVs;   // Number of TLVs contained in the message body.
  uint8_t TLVs[0];        // Variable  See Table 28.
};

struct fapi_l1_ue_release_response {
    fapi_l1_message_t hdr;
};

struct fapi_l1_dl_config_request {
    fapi_l1_message_t hdr;
};

struct fapi_l1_ul_config_request {
    fapi_l1_message_t hdr;
};

struct fapi_l1_subframe_indication {
    fapi_l1_message_t hdr;
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

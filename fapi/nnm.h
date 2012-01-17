/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Femto Forum Ltd
 *
 * LTE Network Monitor Mode
 * Specification v1.01
 * Femto Forum Technical Document
 *
 * Document number:   FF_Tech_003_v1.01
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

#ifndef _NNM_H_
#define _NNM_H_

typedef struct fapi_nnm_message {
  uint8_t  typeID;
  uint16_t length;
} fapi_nnm_message_t;

enum FAPI_NNM_MessageTypeID_e {
  NMM_RSSI_MEAS_REQ        = 01,  // 6.1.1 Triggers RSSI measurements
  // in the requested band. Optionally a list of carriers can be configured.
  NMM_RSSI_MEAS_IND        = 02, // 6.1.2  Returns the measurement on a carrier.
  NMM_RSSI_MEAS_CNF        = 03, // 6.1.3  Indicates the end of the measurements.
  NMM_STOP_RSSI_MEAS_REQ   = 04, // 6.1.4  Stops the RSSI measurements.
  NMM_CELL_SEARCH_REQ      = 05, // 6.1.5  Triggers the search for cells on the
  // specified carrier. Optionally a list of cells can be specified.
  NMM_CELL_SEARCH_IND      = 06, // 6.1.6  Returns a found cell.
  NMM_CELL_SEARCH_CNF      = 07, // 6.1.7  Indicates the end of the cell search.
  NMM_STOP_CELL_SEARCH_REQ = 08, // 6.1.8  Stops the cell search.
  NMM_PBCH_CONFIG_REQ      = 09, // 6.1.9  Triggers reading of the PBCH.
  NMM_PBCH_CONFIG_CNF      = 10, // 6.1.10  Confirms the PBCH configuration.
  NMM_PBCH_DATA_IND        = 11, // 6.1.11  Returns the data read from PBCH.  For
  //    each NMM_PBCH_CONFIG_REQ zero or one NMM_PBCH_DATA_IND are received.
  NMM_STOP_PBCH_REQ        = 12, // 6.1.12  Stop PBCH reading.
  NMM_SIB1_CONFIG_REQ      = 13, // 6.1.13  Requests SIB1 reading on EARFCN and PCI
  //   configured with last NMM_PBCH_CONFIG_REQ.
  NMM_SIB1_CONFIG_CNF      = 14, // 6.1.14  Confirm SIB1 configuration.
  NMM_BCCH_CONFIG_REQ      = 15, // 6.1.15  Requests reading of the BCCH other than
  //   SIB1 reading on EARFCN and PCI configured with last NMM_PBCH_CONFIG_REQ.
  NMM_BCCH_CONFIG_CNF      = 16, // 6.1.16  Confirm BCCH configuration.
  NMM_BCCH_DATA_IND        = 17, // 6.1.17  Returns the data read from BCCH including SIB1.
  NMM_STOP_BCCH_REQ        = 18, // 6.1.18  Stop BCCH and/or SIB1 reading.
};

// 8 Constants
#defien MAX_CARRIER_LIST    100
#defien MAX_PCI_LIST        50
#defien MAX_SI_LIST         32
#defien MAX_BCCH_SIZE       2048
#defien MAX_CELL_FOUND_LIST 16
#defien MAX_BCCH_DATA_LIST  32
#defien MAX_VENDOR_LIST     16

//// 7  Parameters //// 7  Parameters ////
// 7.1  CellFoundElement
struct CellFoundElement {
  uint8_t rsrp; //  0..128(-144..-16)
  // The measured RSRP value in dBm. See [3].

  uint16_t pci; // 0..503
  // The physical cell identity of the cell which the NMM synchronized to.
};
// 7.2  BcchDataElement
struct BcchDataElement {

uint16_t len; // 0..MAX_BCCH_SIZE
//   The length of the BCCH payload.

uint16_t sfn; // 0..1023
//   The radio frame the data was received on.

uint8_t sf; // 0..9
//   The subframe the data was received on.

  uint8_t data[MAX_BCCH_SIZE];
  // The BCCH payload. The payload is mapped
  // according to 36.212 (Byte 0, bit 8 is mapped to a0).
};
// 7.3  VendorSpecificListElement
struct VendorSpecificListElement {
  uint32_t type;     // Indicating the type of the value.
  uint32_t length;   //  The length of the actual value.
  uint8_t  value[0]; //  struct  The actual value which will be set
};

// 6.1.1  NMM_RSSI_MEAS_REQ Parameters
struct NMM_RSSI_MEAS_REQ {
  fapi_nnm_message_t hdr;

  uint8_t freqBandIndicator; // 1..64
  // The E-UTRA band for which the carrierList
  // applies. See [1] table 5.5-1

  uint8_t measurementPeriod; // 1..255
  // The length of time the NMM measure RSSI
  // over. In 10ms radio frames.

  uint8_t nr_earfcnList; // 0..MAX_CARRIER_LIST
  // The number of carriers which should be
  // measured. In case no carrier (value 0) is
  // specified all carriers in the band shall be
  // measured.

  uint16_t earfcnList[MAX_CARRIER_LIST]; // 0..65535
  // The list of carriers to be measured. See  [1].

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.2 NMM_RSSI_MEAS_IND parameters
struct NMM_RSSI_MEAS_IND {
  fapi_nnm_message_t hdr;

  uint8_t rssi; // 0..128
  // (-144..-16) The measured RSSI in dBm in steps of 1dB.

  uint16_t earfcn; // 0..65535
  // The carrier measured. See [1].

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.3 NMM_RSSI_MEAS_CNF Parameters
struct NMM_RSSI_MEAS_CNF {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.4 NMM_STOP_RSSI_MEAS_REQ Parameters
struct NMM_STOP_RSSI_MEAS_REQ {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.5 NMM_CELL_SEARCH_REQ Parameters
struct NMM_CELL_SEARCH_REQ {
  fapi_nnm_message_t hdr;

  uint16_t earfcn; // 1..65535
  // The earfcn for which cells should be measured.

  bool  exhaustiveSearch;  // TRUE,FALSE
  // NMM should try to find all cells on the carrier.

  uint8_t nr_pciList; // 0..MAX_PCI_LIST
  // The number of cells in the pciList. If 0 all cells on the carrier should be found.
  // Otherwise, depending on exhaustiveSearch flag, only the given
  // pciList is searched or the pciList is used for indicating a priority list.

  uint16_t pciList[MAX_PCI_LIST];  // 0..503  array of uint16_t
  // The list of cells which should be searched.

  uint8_t measBandwidth; // 6,15,25,50,75,100
  // The number of resource blocks which should be used for measuring RSRP. See [3].

  uint8_t measurementPeriod; // 1..255
  // The length of time the NMM measure
  // RSSI over. In 10ms radio frames.

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.6 NMM_CELL_SEARCH_IND Parameters
struct NMM_CELL_SEARCH_IND {
  fapi_nnm_message_t hdr;

  uint8_t nr_cellFoundList; // 0..MAX_CELL_FOUND_LIST
  //   The number of cells indicated in this message.

  struct CellFoundElement cellFoundList[MAX_CELL_FOUND_LIST];
  // Array of CellFoundElement  array of struct  The cell information from the NMM.  see 7.1

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.

};

// 6.1.7 NMM_CELL_SEARCH_CNF Parameters
struct NMM_CELL_SEARCH_CNF {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.8 NMM_STOP_CELL_SEARCH_REQ Parameters
struct NMM_STOP_CELL_SEARCH_REQ {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.9 NMM_PBCH_CONFIG_REQ Parameters
struct NMM_PBCH_CONFIG_REQ {
  fapi_nnm_message_t hdr;

  uint16_t pci; // 0..503
  // The physical cell identity of the cell which the NMM should read the PBCH.

  uint16_t earfcn; // 0..65535
  // The EARFCN for the cell which the
  // NMM should read the PBCH.

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.10 NMM_PBCH_CONFIG_CNF Parameters
struct NMM_PBCH_CONFIG_CNF {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.11 NMM_PBCH_DATA_IND Parameters
struct NMM_PBCH_DATA_IND {
  fapi_nnm_message_t hdr;

  uint16_t pci; // 0..503
  // The physical cell identity of the cell
  // which the NMM synchronized to.

  enum cause_e {
    MIB_OK,
    MIB_FAILED
  } cause ;
  // enum  Indicates if MIB reading was successful or not. In case reading failed the next
  // parameters are invalid.

  uint8_t numTx; // 1,2,4
  //   The number of transmit antennas of the cell.

  uint16_t sfn; // 0..1023
  //   The system frame number of the cell.

  enum phichDuration_e {
    PD_NORMAL,
    PD_EXTENDED
  } phichDuration;
  // enum  The PHICH duration of the cell.

  enum phichResource_e {
    PR_ONE_SIXTH,
    PR_ONE_HALF,
    PR_ONE,
    PR_TWO
  } phichResource;
  // enum  The PHICH resource size of the cell.

  uint8_t dl_SystemBandwidth; // 6,15,25,50,75,100
  // DL transmission bandwidth in RBs.


  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.12 NMM_STOP_PBCH_REQ Parameters
struct NMM_STOP_PBCH_REQ {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.13 NMM_SIB1_CONFIG_REQ Parameters
struct NMM_SIB1_CONFIG_REQ {
  fapi_nnm_message_t hdr;

  uint8_t retryCount; // 1..8
  // The number of periods for which decoding of a SIB1 should be retried.

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.14 NMM_SIB1_CONFIG_CNF Paramerers
struct NMM_SIB1_CONFIG_CNF {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.15 NMM_BCCH_CONFIG_REQ Parameters
struct NMM_BCCH_CONFIG_REQ {
  fapi_nnm_message_t hdr;

  uint8_t nr_siPeriodicityList; // 0..MAX_SI_LIST
  // The number of SIs to read.

  uint16_t siPeriodicityList [MAX_SI_LIST]; //    80,160,320,640,1280,2560,5120
  // array of  The SI periodicity of all requested SIs.

  uint8_t siWindow; // 1,2,5,10,15,20,40
  // The SI window.

  uint8_t retryCount; // 1..8
  // The number of siWindows for which decoding of a SI should be retried. The
  // retiries are counted per siPeriodicity.

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.16 NMM_BCCH_CONFIG_CNF Parameters
struct NMM_BCCH_CONFIG_CNF {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.17 NMM_BCCH_DATA_IND Parameters
struct NMM_BCCH_DATA_IND {
  fapi_nnm_message_t hdr;

  uint8_t nr_bcchDataList; // 0..MAX_BCCH_DATA_LIST
  //   The number of BCCHs in this message .

  struct BcchDataElement bcchDataList[MAX_BCCH_DATA_LIST];
  //  Array of  array of struct  The list of decoded  BCCH information.

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

// 6.1.18 NMM_STOP_BCCH_REQ Parameters
struct NMM_STOP_BCCH_REQ {
  fapi_nnm_message_t hdr;

  uint8_t nr_vendorSpecificList; // 0..MAX_VENDOR_LIST
  // The number of elements in the next array.

  uint8_t vendorSpecificList[0];
  // Contains vendor specific configuration.
};

#endif /* _NNM_H_ */

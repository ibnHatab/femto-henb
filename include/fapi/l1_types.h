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
#ifndef bool
#define bool int
#define false 0
#define true  1
#endif	/* bool */
#endif	/* __KERNEL__ */

#define FAPI_L1_NUMBER_OF_SUBBANDS_MAX 13

enum FAPI_L1_DuplexingMode_e {
    FAPI_L1_DM_TDD = 0,
    FAPI_L1_DM_FDD = 1,
    FAPI_L1_DM_HD_FDD = 2
};

enum FAPI_L1_Direction_e {
  DIR_DL = 0,
  DIR_UL = 1
};

#define MAKE_SFN_SF (SFN, SF) do { SFN << 3 | SF } while (0)
#define GET_SFN (SFN_SF)      do { SFN_SF >> 3 } while (0)
#define GET_SF (SFN_SF)       do { SFN_SF & 0x7 } while (0)

enum FAPI_L1_DCI_Format_e {
  DCI_1  = 0,
  DCI_1A = 1,
  DCI_1B = 2,
  DCI_1C = 3,
  DCI_1D = 4,
  DCI_2  = 5,
  DCI_2A = 6
};

enum FAPI_L1_N_GAP_e {
  N_GAP1 = 0,
  N_GAP2 = 1
};

enum FAPI_L1_Modulation_e {
  MD_QPSK  = 2,
  MD_16QAM = 4,
  MD_64QAM = 6
};

enum FAPI_L1_TransmissionScheme_e {
  SINGLE_ANTENNA_PORT_0            = 0, // 0: SINGLE_ANTENNA_PORT_0,
  TX_DIVERSITY                     = 1, // 1: TX_DIVERSITY,
  LARGE_DELAY_CDD                  = 2, // 2: LARGE_DELAY_CDD,
  CLOSED_LOOP_SPATIAL_MULTIPLEXING = 3, // 3: CLOSED_LOOP_SPATIAL_MULTIPLEXING,
  MULTI_USER_MIMO                  = 4, // 4: MULTI_USER_MIMO,
  CLOSED_LOOP_RANK_1_PRECODING     = 5, // 5: CLOSED_LOOP_RANK_1_PRECODING,
  SINGLE_ANTENNA_PORT_5            = 6  // 6: SINGLE_ANTENNA_PORT_5.
};

enum FAPI_L1_ACK_NACK_Mode_e {
  ACK_NACK_BUNDLING         = 0,
  ACK_NACK_MULTIPLEXING     = 1,
  ACK_NACK_SPECIAL_BUNDLING = 2
};

#endif /* _L1_TYPES_H_ */

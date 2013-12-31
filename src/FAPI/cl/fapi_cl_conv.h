#ifndef _FAPI_CL_CONV_H_
#define _FAPI_CL_CONV_H_

#include <LtePhyL2Api.h>
#include <l1_tlv.h>

/* FAPI PHICH power : Value 0 -> 10000, -6 - 4 dB in step 0.001db */
/* MSPD Power: Value -32768(-0x8000) ->1536(0x600); -128(0x80) to 6 dB in step 1/256(0x100)db */
#define POW_FROM_FAPI(val) (((val * 1024 / 1000) - 6 * 1024) >> 2)
#define POW_FROM_MSPD(val) ((val << 2) + 6 * 1024) * 1000 / 1024
#define DB_TO_FAPI(val) ((uint16_t)((val + 6) * 1000))

#define CELL_GRP(v)   (uint16_t)(v / 3)
#define CELL_LAYER(v) (v % 3)

#define ENABLED  1
#define DISABLED 0

/** 
 * This function check array of TLV's againt array of tags for being propper subset.
 * 
 * @param tlvs          - TLV's array to check
 * @param numberOfTLVs  - number of TLVS 
 * @param array         - reference tags
 * @param num           - number of tags
 * @param tlv_errors    - array, indexed by TLV tags, containing
 *                        error flags defined by cl_conv_error_e
 *
 * @return error status, OK if 0 
 */
int conv_check_supported(l1_tlv_word_t tlvs[], uint8_t numberOfTLVs,
                         FAPI_L1_TLV_TAG_e array[], uint8_t num,
                         uint8_t *tlv_errors);

/** 
 * Convert FAPI PHY configuration into API PHY Init
 * 
 * @param fapi          - FAPI
 * @param api           - API
 * @param tlv_errors    - array, indexed by TLV tags, containing
 *                        error flags defined by cl_conv_error_e
 * 
 * @return error status, OK if 0 
 */
int conv_apiInit_from_CONFIG_request (struct fapi_l1_config_request* fapi,
                                       PINITPARM api,
                                       uint8_t *tlv_errors);

/** 
 * Format generic header and element count.
 * 
 * @param fapi - FAPI
 * @param api  - API
 * 
 * @return OK if 0
 */
int conv_dlSubframeDescriptor_from_DL_CONFIG_request
(struct fapi_l1_dl_config_request * fapi, PDLSUBFRDESC api, size_t *msgSize);

int conv_ulSubframeDescriptor_from_UL_CONFIG_request
(struct fapi_l1_ul_config_request * fapi, PULSUBFRDESC api, size_t *msgSize);

int conv_HiandDciUlMessageDescriptor_from_HI_DCI0_request
(struct fapi_l1_hi_dci0_request * fapi, PHIADCIULMSGDESC hiadci);

int conv_HiInfoDescriptor_from_HI_PDU(struct DCI_HI_PDU_Configuration *ul_pdu,
                                      PHIINFOMSGDESC hi_info);

int conv_DciUlSduHeaderDescriptor_from_DCI_UL_PDU(struct DCI_HI_PDU_Configuration *ul_pdu,
                                                  PDCIULSDUMSG dci_info, U32 *msgSize);


int conv_RX_XX_indication_from_RxStatusInd (PRXSTATUSIND api,
                                            fapi_l1_msg_hdr_t *fapi);



void estimateSize_RX_ULSCH_indication_from_RxSduInd (PRXSDUIND api,
                                                     uint16_t *fapi_msg_size,
                                                     uint16_t *cqi_ind_size,
                                                     uint16_t *crc_ind_size);
int conv_RX_ULSCH_indication_from_RxSduInd (PRXSDUIND api,
                                            fapi_l1_msg_hdr_t *fapi,
                                            struct fapi_l1_rx_cqi_indication *cqi_ind,
                                            struct fapi_l1_crc_indication *crc_ind);

#endif /* _FAPI_CL_CONV_H_ */

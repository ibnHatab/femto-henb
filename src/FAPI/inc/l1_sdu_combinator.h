#ifndef _L1_SDU_COMBINATOR_H_
#define _L1_SDU_COMBINATOR_H_


/**
   XL_CONFIG.request structure combinator applied as chain call
   a.k.a native interface in oop (by keeping object piinter last in argument
   list we help propper code formating).

   Combinators doesn't inforce protocol. It should be followed explicitly.
   Call order is:
           4 > of_dl_conf(sfn_sf,
                   3> dl_conf_add_pdu( &bch_pdu, BCH_PDU_TYPE_SIZE,
                                   2>  dl_conf_add_pdu( &bch_pdu, BCH_PDU_TYPE_SIZE,
                                                    1>  format_dl_conf (msg_buff, &dl_bch_config))));

    Since PDU order not important we, basically, need to keep `format_dl_conf' first and `of_dl_conf'
    last in the call chain.
 */

struct fapi_l1_dl_config_request *
format_dl_conf (uint8_t *buff, struct fapi_l1_dl_config_request * from_conf);

struct fapi_l1_dl_config_request *
of_dl_conf (uint16_t SFN_SF, struct fapi_l1_dl_config_request * conf);

struct fapi_l1_dl_config_request *
dl_conf_add_pdu (uint8_t * DL_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                 struct fapi_l1_dl_config_request * conf);

struct fapi_l1_dl_config_request *
dl_conf_pdu_tail (uint8_t * data, uint8_t size, struct fapi_l1_dl_config_request * conf);
/// Having one tail per PDU is a limit; parent PDU size account for tail size

#define DCI_DL_PDU_TYPE_SIZE                            \
    DCI_DL_PDU_TYPE, sizeof(struct fapi_l1_dci_dl_pdu)
#define BCH_PDU_TYPE_SIZE                               \
    BCH_PDU_TYPE, sizeof(struct fapi_l1_bch_pdu)
#define MCH_PDU_TYPE_SIZE                               \
    MCH_PDU_TYPE, sizeof(struct fapi_l1_mch_pdu)
#define DLSCH_PDU_TYPE_SIZE                             \
    DLSCH_PDU_TYPE, sizeof(struct fapi_l1_dlsch_pdu)
#define PCH_PDU_TYPE_SIZE                               \
    PCH_PDU_TYPE, sizeof(struct fapi_l1_pch_pdu)


/**
 * Iterate over PDU's part of DL config message
 *
 * @param dl_conf - DL_CONF SDU
 * @param dl_pdu - the PDU * to use as a iteration entry
 */
#define dl_pdu_for_each_entry(dl_conf, dl_pdu)                          \
    int i; for (i = 0,                                                  \
                    dl_pdu = &dl_conf->DL_PDU_Configuration[0];         \
                i < dl_conf->NumberOf_DCIs + dl_conf->NumberOf_PDUs;    \
                i++,                                                    \
                    dl_pdu = (struct DL_PDU_Configuration *)((uint8_t*)dl_pdu + dl_pdu->PDU_Size))

#define dl_pdu_assert_bounds(dl_conf, dl_pdu)                           \
    BUG_ON(((char*)dl_pdu) - ((char*)dl_conf->DL_PDU_Configuration) + dl_pdu->PDU_Size > dl_conf->Length)

  //dl_pdu = (struct DL_PDU_Configuration *)((uint8_t*)dl_pdu + dl_pdu->PDU_Size))

//// UL CONFIG //// UL CONFIG //// UL CONFIG //// UL CONFIG //// UL CONFIG ////

struct fapi_l1_ul_config_request *
format_ul_conf (uint8_t *buff, struct fapi_l1_ul_config_request * from_conf);

struct fapi_l1_ul_config_request *
of_ul_conf (uint16_t SFN_SF, struct fapi_l1_ul_config_request * conf);

struct fapi_l1_ul_config_request *
ul_conf_add_pdu (uint8_t * UL_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                 struct fapi_l1_ul_config_request * conf);

struct fapi_l1_ul_config_request *
ul_conf_pdu_tail (uint8_t * data, uint8_t size, struct fapi_l1_ul_config_request * conf);

#define ULSCH_PDU_TYPE_SIZE                             \
    ULSCH_PDU_TYPE, sizeof(struct fapi_l1_ulsch_pdu)
#define ULSCH_CQI_RI_PDU_TYPE_SIZE                                      \
    ULSCH_CQI_RI_PDU_TYPE, sizeof(struct fapi_l1_ulsch_cqi_ri_pdu)
#define ULSCH_HARQ_PDU_TYPE_SIZE                                \
    ULSCH_HARQ_PDU_TYPE, sizeof(struct fapi_l1_ulsch_harq_pdu)
#define ULSCH_CQI_HARQ_RI_PDU_TYPE_SIZE                                 \
    ULSCH_CQI_HARQ_RI_PDU_TYPE, sizeof(struct fapi_l1_ulsch_cqi_harq_ri_pdu)
#define UCI_CQI_PDU_TYPE_SIZE                                   \
    UCI_CQI_PDU_TYPE, sizeof(struct fapi_l1_uci_cqi_pdu)
#define UCI_SR_PDU_TYPE_SIZE                            \
    UCI_SR_PDU_TYPE, sizeof(struct fapi_l1_uci_sr_pdu)
#define UCI_HARQ_PDU_TYPE_SIZE                                  \
    UCI_HARQ_PDU_TYPE, sizeof(struct fapi_l1_uci_harq_pdu)
#define UCI_SR_HARQ_PDU_TYPE_SIZE                                       \
    UCI_SR_HARQ_PDU_TYPE, sizeof(struct fapi_l1_uci_sr_harq_pdu)
#define UCI_CQI_HARQ_PDU_TYPE_SIZE                                      \
    UCI_CQI_HARQ_PDU_TYPE, sizeof(struct fapi_l1_uci_cqi_harq_pdu)
#define UCI_CQI_SR_PDU_TYPE_SIZE                                \
    UCI_CQI_SR_PDU_TYPE, sizeof(struct fapi_l1_uci_cqi_sr_pdu)
#define UCI_CQI_SR_HARQ_PDU_TYPE_SIZE                                   \
    UCI_CQI_SR_HARQ_PDU_TYPE, sizeof(struct fapi_l1_uci_cqi_sr_harq_pdu)
#define SRS_PDU_TYPE_SIZE                               \
    SRS_PDU_TYPE, sizeof(struct fapi_l1_srs_pdu)
#define HARQ_BUFFER_PDU_TYPE_SIZE                                       \
    HARQ_BUFFER_PDU_TYPE, sizeof(struct fapi_l1_harq_buffer_pdu)

/**
 * Iterate over PDU's part of UL config message
 *
 * @param ul_conf - UL_CONF SDU
 * @param ul_pdu - the PDU * to use as a iteration entry
 */
// FIXME: pass i as paramater
#define ul_pdu_for_each_entry(ul_conf, ul_pdu)                          \
    int i; for (i = 0,                                                  \
                    ul_pdu = &ul_conf->UL_PDU_Configuration[0];         \
                i < ul_conf->NumberOf_PDUs;                             \
                i++,                                                    \
                    ul_pdu = (struct UL_PDU_Configuration *)((uint8_t*)ul_pdu + ul_pdu->PDU_Size))

#define ul_pdu_assert_bounds(ul_conf, ul_pdu)                           \
    BUG_ON(((char*)ul_pdu) - ((char*)ul_conf->UL_PDU_Configuration) + ul_pdu->PDU_Size > ul_conf->Length)

//// TX Request //// TX Request //// TX Request //// TX Request //// TX Request ////
struct fapi_l1_tx_request *
format_tx_req (uint8_t *buff, struct fapi_l1_tx_request * from_req);

struct fapi_l1_tx_request *
of_tx_req (uint16_t SFN_SF, struct fapi_l1_tx_request * req);

struct fapi_l1_tx_request *
tx_req_add_pdu (uint16_t tag, uint16_t PDU_Index,
                uint8_t * DL_PDU, uint16_t PDU_Length,
                struct fapi_l1_tx_request * req);

/**
 * Iterate over PDU's part of DL config message
 *
 * @param tx_req - TX_REQUEST SDU
 * @param dl_pdu - the PDU * to use as a iteration entry
 */
#define tx_req_for_each_pdu(tx_req, dl_pdu)                             \
    int i; for (i = 0,                                                  \
                    dl_pdu = &tx_req->TX_Config[0];                     \
                i < tx_req->NumberOf_PDUs;                              \
                i++,                                                    \
                    dl_pdu = (struct fapi_l1_dl_pdu_config *) ((uint8_t*)dl_pdu + dl_pdu->PDU_Length))


//// HI_DCI0 Request //// HI_DCI0 Request //// HI_DCI0 Request //// HI_DCI0 Request ////
struct fapi_l1_hi_dci0_request *
format_hi_dci0_req (uint8_t *buff, struct fapi_l1_hi_dci0_request * from_req);

struct fapi_l1_hi_dci0_request *
of_hi_dci0_req (uint16_t SFN_SF, struct fapi_l1_hi_dci0_request * req);

struct fapi_l1_hi_dci0_request *
hi_dci0_req_add_pdu (uint8_t * DCI_HI_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                     struct fapi_l1_hi_dci0_request * req);

// 0: HI PDU, see Section 3.3.1.4.1.
// 1: DCI UL PDU, see Section 3.3.1.4.2.

#define HI_PDU_TYPE_SIZE                        \
    HI_PDU_TYPE, sizeof(struct fapi_l1_hi_pdu)
#define DCI_UL_PDU_TYPE_SIZE                            \
    DCI_UL_PDU_TYPE, sizeof(struct fapi_l1_dci_ul_pdu)

/**
 * Iterate over PDU's part of DL config message
 *
 * @param i - loop index
 * @param hi_dci0_req - HI_DCI0_REQUEST SDU
 * @param dl_pdu - the PDU * to use as a iteration entry
 */
#define hi_dci0_req_for_each_pdu(i, hi_dci0_req, dci_pdu)               \
    for (i = 0,                                                         \
             dci_pdu = &hi_dci0_req->DCI_HI_PDU_Configuration[0];       \
         i < hi_dci0_req->NumberOf_HI + hi_dci0_req->NumberOf_DCI;      \
         i++,                                                           \
             dci_pdu = (struct DCI_HI_PDU_Configuration *)((uint8_t*)dci_pdu + dci_pdu->PDU_Size))


#endif /* _L1_SDU_COMBINATOR_H_ */

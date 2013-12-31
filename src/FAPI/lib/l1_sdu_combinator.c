
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

#include <l1_sdu_combinator.h>

#if MX_ARM_ENABLED
#include <4gmx.h>
  //#include <4gmx_const.h>
#else	/* !( MX_ARM_ENABLED ) */
#include <sys/param.h>
#define ROUND roundup
#endif

  //// DL CONFIG //// DL CONFIG //// DL CONFIG //// DL CONFIG //// DL CONFIG ////
struct fapi_l1_dl_config_request *
format_dl_conf (uint8_t *buff, struct fapi_l1_dl_config_request * from_conf){

    struct fapi_l1_dl_config_request* conf = (struct fapi_l1_dl_config_request*) buff;
    if (from_conf) {
        memcpy(buff, from_conf, sizeof(struct fapi_l1_dl_config_request));
    } else {
        conf->SFN_SF = INVALID_SFNSF;
        conf->Length = 0;
        conf->NumberOf_PDCCH_OFDM_Symbols = 4;
        conf->NumberOf_DCIs = 0;
        conf->NumberOf_PDUs = 0;
        conf->NumberOf_PDSCH_RNTIs = 0;
        conf->TransmissionPowerFor_PCFICH = 0;        
    }
    
    return (struct fapi_l1_dl_config_request*) buff;
}


struct fapi_l1_dl_config_request *
of_dl_conf (uint16_t SFN_SF, struct fapi_l1_dl_config_request * conf){
    conf->SFN_SF = SFN_SF;

    l1_msg_alloc_generic (FAPI_L1_DL_CONFIG_REQ,
                          (fapi_l1_msg_hdr_t *) conf,
                          conf->Length 
                          + offsetof (struct fapi_l1_dl_config_request, DL_PDU_Configuration));

    return conf;
}

struct fapi_l1_dl_config_request *
dl_conf_add_pdu (uint8_t * DL_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                 struct fapi_l1_dl_config_request * conf){
    // copy
    struct DL_PDU_Configuration * pdu =
        (struct DL_PDU_Configuration *) ((uint8_t*) conf->DL_PDU_Configuration + conf->Length);

    pdu->PDU_Type = PDU_Type;
    pdu->PDU_Size = PDU_Size + offsetof(struct DL_PDU_Configuration, DL_PDU);

    if (DL_PDU) 
        memcpy(pdu->DL_PDU, DL_PDU, PDU_Size);

    // update
    if (PDU_Type == DCI_DL_PDU_TYPE)
        conf->NumberOf_DCIs ++;
    else
        conf->NumberOf_PDUs ++;

    conf->Length += pdu->PDU_Size;
    return conf;
}

struct fapi_l1_dl_config_request *
dl_conf_pdu_tail (uint8_t * data, uint8_t size,
                 struct fapi_l1_dl_config_request * conf) {
    uint8_t* pData = ((uint8_t*) conf->DL_PDU_Configuration) + conf->Length - size;

    if (data) 
        memcpy(pData, data, size);
    
    return conf;
}


//// UL CONFIG //// UL CONFIG //// UL CONFIG //// UL CONFIG //// UL CONFIG ////

struct fapi_l1_ul_config_request *
format_ul_conf (uint8_t *buff, struct fapi_l1_ul_config_request * from_conf){

    struct fapi_l1_ul_config_request* conf = (struct fapi_l1_ul_config_request*) buff;
        
    if (from_conf) {
        memcpy(buff, from_conf, sizeof(struct fapi_l1_ul_config_request));
    } else {
        conf->SFN_SF = INVALID_SFNSF;
        conf->Length = 0;
        conf->NumberOf_PDUs = 0;
        conf->RACH_PRACH =  0;
        conf->SRS_Present = 0; 
    }
    return conf;
}


struct fapi_l1_ul_config_request *
of_ul_conf (uint16_t SFN_SF, struct fapi_l1_ul_config_request * conf){
    conf->SFN_SF = SFN_SF;

    l1_msg_alloc_generic (FAPI_L1_UL_CONFIG_REQ,
                          (fapi_l1_msg_hdr_t *) conf,
                          conf->Length 
                          + offsetof (struct fapi_l1_ul_config_request, UL_PDU_Configuration));

    return conf;
}

struct fapi_l1_ul_config_request *
ul_conf_add_pdu (uint8_t * UL_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                 struct fapi_l1_ul_config_request * conf){
    // copy
    struct UL_PDU_Configuration * pdu =
        (struct UL_PDU_Configuration *) ((uint8_t*) conf->UL_PDU_Configuration + conf->Length);

    pdu->PDU_Type = PDU_Type;
    pdu->PDU_Size = PDU_Size + offsetof(struct UL_PDU_Configuration, UL_PDU);

    if (UL_PDU) 
        memcpy(pdu->UL_PDU, UL_PDU, PDU_Size);

    // update
    conf->NumberOf_PDUs ++;
    conf->Length += pdu->PDU_Size;
    
    return conf;
}

struct fapi_l1_ul_config_request *
ul_conf_pdu_tail (uint8_t * data, uint8_t size,
                 struct fapi_l1_ul_config_request * conf) {
    uint8_t* pData = ((uint8_t*) conf->UL_PDU_Configuration + conf->Length - size);

    if (data) 
        memcpy(pData, data, size);
    
    return conf;
}

//// TX Request //// TX Request //// TX Request //// TX Request //// TX Request ////

struct fapi_l1_tx_request *
format_tx_req (uint8_t *buff, struct fapi_l1_tx_request * from_req){

    if (from_req) {
        memcpy(buff, from_req, sizeof(struct fapi_l1_tx_request));
    } else {
        ((struct fapi_l1_tx_request*) buff)->SFN_SF = INVALID_SFNSF;
        ((struct fapi_l1_tx_request*) buff)->NumberOf_PDUs = 0;
    }

    l1_msg_alloc_generic (FAPI_L1_TX_REQ,
                          (fapi_l1_msg_hdr_t *) buff,
                          offsetof (struct fapi_l1_tx_request, TX_Config));

    return (struct fapi_l1_tx_request*) buff;
}

struct fapi_l1_tx_request *
of_tx_req (uint16_t SFN_SF, struct fapi_l1_tx_request * req){
    req->SFN_SF = SFN_SF;
    return req;
}

struct fapi_l1_tx_request *
tx_req_add_pdu (uint16_t tag, uint16_t PDU_Index,
                uint8_t * DL_PDU, uint16_t PDU_Length,
                struct fapi_l1_tx_request * req){
    // copy
    struct fapi_l1_dl_pdu_config * pdu =
        (struct fapi_l1_dl_pdu_config *) ((uint8_t*)req + req->hdr.length);
    
    pdu->PDU_Index = PDU_Index;
    pdu->numTLV = 1; // Only one TLV carried per DL PDU config in combiner implementation
    pdu->PDU_Length = offsetof(struct fapi_l1_dl_pdu_config, tlvs)
        + offsetof(struct fapi_l1_dl_pdu_tlv, value)
        + (tag == 0 ? ROUND(PDU_Length, 4) : sizeof(uint32_t));

    pdu->tlvs[0].tag = tag;
    pdu->tlvs[0].length = PDU_Length;

    if (tag == 0) {
        if (DL_PDU) memcpy((void*)&pdu->tlvs[0].value, DL_PDU, PDU_Length);
    } else {
        pdu->tlvs[0].value = (uint32_t) DL_PDU;
    }

    // update
    req->NumberOf_PDUs ++;
    req->hdr.length += pdu->PDU_Length;

    return req;
}


//// HI_DCI0 Request //// HI_DCI0 Request //// HI_DCI0 Request //// HI_DCI0 Request ////

struct fapi_l1_hi_dci0_request *
format_hi_dci0_req (uint8_t *buff, struct fapi_l1_hi_dci0_request * from_req){

    if (from_req) {
        memcpy(buff, from_req, sizeof(struct fapi_l1_hi_dci0_request));
    } else {
        ((struct fapi_l1_hi_dci0_request*) buff)->SFN_SF = INVALID_SFNSF;
        ((struct fapi_l1_hi_dci0_request*) buff)->NumberOf_DCI = 0;
        ((struct fapi_l1_hi_dci0_request*) buff)->NumberOf_HI = 0;
    }

    l1_msg_alloc_generic (FAPI_L1_HI_DCI0_REQ,
                          (fapi_l1_msg_hdr_t *) buff,
                          offsetof (struct fapi_l1_hi_dci0_request, DCI_HI_PDU_Configuration));
    
    return (struct fapi_l1_hi_dci0_request*) buff;
}

struct fapi_l1_hi_dci0_request *
of_hi_dci0_req (uint16_t SFN_SF, struct fapi_l1_hi_dci0_request * req){
    req->SFN_SF = SFN_SF;
    return req;
}

struct fapi_l1_hi_dci0_request *
hi_dci0_req_add_pdu (uint8_t * DCI_HI_PDU, uint16_t PDU_Type, uint8_t PDU_Size,
                     struct fapi_l1_hi_dci0_request * req){
    // copy
    struct DCI_HI_PDU_Configuration * pdu =
        (struct DCI_HI_PDU_Configuration *) ((uint8_t*)req + req->hdr.length);

    pdu->PDU_Type = PDU_Type;
    pdu->PDU_Size = PDU_Size + offsetof(struct DCI_HI_PDU_Configuration, DCI_HI_PDU);

    if (DCI_HI_PDU) 
        memcpy(pdu->DCI_HI_PDU, DCI_HI_PDU, PDU_Size);

    // update
    if (PDU_Type == DCI_UL_PDU_TYPE)
        req->NumberOf_DCI ++;
    else if (PDU_Type == HI_PDU_TYPE)
        req->NumberOf_HI ++;
    else 
        return 0;

    req->hdr.length += pdu->PDU_Size;
    return req;
}



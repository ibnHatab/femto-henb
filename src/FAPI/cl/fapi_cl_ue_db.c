#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#ifdef MX_ARM_ENABLED
#include "appprintf.h"
#endif	/* defined MX_ARM_ENABLED */ 

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

#include "fapi_cl.h"

#include "fapi_cl_ue_db.h"

/**
   Used to preinitialize the UE config's
   May have been preset to mmatch PHY configuration
 */
struct cl_ue_conf default_ue_conf = {
    /* .Handle                          =  */ CL_UE_HND_POISON,
    /* .RNTI                            =  */ 0,
    /* CQI Config All TLVs in this grouping */
    /* .CQI_PUCCH_ResourceIndex         =  */ 0,
    /* .CQI_PMI_ConfigIndex             =  */ 0,
    /* .CQI_RI_ConfigIndex              =  */ 0,
    /* .CQI_SimultaneousAckNackAnd_CQI  =  */ 0,
    /* ACK/NACK Config All TLVs in this grouping */
    /* .AN_RepetitionFactor             =  */ 0,
    /* .AN_n1PUCCH_ANRep                =  */ 0,
    /* SRS Config All TLVs in this grouping */
    /* .SRS_Bandwidth                   =  */ 1,
    /* .SRS_Hopping_Bandwidth           =  */ 0,
    /* .FrequencyDomainPosition         =  */ 0,
    /* .SRS_Duration                    =  */ 0,
    /* .ISRS_SRS_ConfigIndex            =  */ 0,
    /* .TransmissionComb                =  */ 0,
    /* .SoundingReferenceCyclicShift    =  */ 0,
    /* SR Config All TLVs in this grouping */
    /* .SR_PUCCH_ResourceIndex          =  */ 0,
    /* .SR_ConfigIndex                  =  */ 0,
    /* SPS Config All TLVs in this grouping */
    /* .SPS_DL_ConfigSchedulingInterval =  */ 0,
    /* .SPS_DL_n1PUCCH_AN_Persistent    =  */ 0
};


struct cl_ue_db_info {
    struct cl_ue_conf ue_conf [CL_UE_PERSIST_INFO_MAX];
    uint16_t active_ue_num;
};

static struct cl_ue_db_info db_info;

int cl_ue_db_init ()
{
    int i;
    db_info.active_ue_num = 0;
    for (i = 0; i < CL_UE_PERSIST_INFO_MAX; i++) {
        memcpy (&db_info.ue_conf[i], &default_ue_conf, sizeof(default_ue_conf));
    }
    return 0;
}

static int cl_alloc_ue_conf (uint16_t Handle)
{
    int i;
    BUG_ON(Handle == CL_UE_HND_POISON);
    for (i = 0; i < CL_UE_PERSIST_INFO_MAX; i++) {
        if (db_info.ue_conf[i].Handle == CL_UE_HND_POISON) {
            db_info.ue_conf[i].Handle = Handle;
            db_info.active_ue_num ++;
            return i;
        }
    }
    return -1;
}

static int cl_find_ue (uint16_t Handle)
{
    int i;
    BUG_ON(Handle == CL_UE_HND_POISON);
    for (i = 0; i < CL_UE_PERSIST_INFO_MAX; i++)
        if (db_info.ue_conf[i].Handle == Handle)
            return i;
    return -1;
}

struct cl_ue_conf * cl_find_ue_conf (uint16_t handle)
{
    int idx = cl_find_ue (handle);
    if (idx < 0) 
        return NULL;
    
    return &db_info.ue_conf [idx];
}

static int cl_free_ue_conf (uint16_t Handle)
{
    int i;
    BUG_ON(Handle == CL_UE_HND_POISON);
    for (i = 0; i < CL_UE_PERSIST_INFO_MAX; i++) {
        if (db_info.ue_conf[i].Handle == Handle) {
            memcpy (&db_info.ue_conf[i], &default_ue_conf, sizeof(default_ue_conf));
            db_info.active_ue_num --;
            return i;
        }
    }
    return -1;
}

static int cl_config_ue (uint16_t Handle, l1_tlv_word_t *tlvs, uint16_t numberOfTLVs,
                         uint8_t *tlv_errors)
{
    int ret = 0, i;
    struct cl_ue_conf *ue_conf;

    int idx = cl_find_ue (Handle);

    if (idx < 0) /*and*/ if ((idx = cl_alloc_ue_conf (Handle)) < 0)
                     return -ENOMEM;

    ue_conf = &db_info.ue_conf [idx];

    for (i = 0; i < numberOfTLVs; i++) {
        switch (tlvs[i].tag) {
        case FAPI_L1_Handle: BUG_ON(tlvs[i].val.word != ue_conf->Handle); break;
        case FAPI_L1_RNTI: ue_conf->RNTI = tlvs[i].val.word;
            break;
            /* CQI Config All TLVs in this grouping */
        case FAPI_L1_CQI_PUCCH_ResourceIndex:
            ue_conf->CQI_PUCCH_ResourceIndex = tlvs[i].val.word;
            break;
        case FAPI_L1_CQI_PMI_ConfigIndex:
            ue_conf->CQI_PMI_ConfigIndex = tlvs[i].val.word;
            break;
        case FAPI_L1_CQI_RI_ConfigIndex:
            ue_conf->CQI_RI_ConfigIndex = tlvs[i].val.word;
            break;
        case FAPI_L1_CQI_SimultaneousAckNackAnd_CQI:
            ue_conf->CQI_SimultaneousAckNackAnd_CQI = tlvs[i].val.byte[0];
            break;
            /* ACK/NACK Config All TLVs in this grouping */
        case FAPI_L1_AN_RepetitionFactor:
            ue_conf->AN_RepetitionFactor = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_AN_n1PUCCH_ANRep:
            ue_conf->AN_n1PUCCH_ANRep = tlvs[i].val.word;
            break;
#if 0
        case FAPI_L1_TDD_AckNackFeedbackMode:
            ue_conf->TDD_AckNackFeedbackMode = tlvs[i].val.byte[0];
            break;
#endif
            /* SRS Config All TLVs in this grouping */
        case FAPI_L1_SRS_Bandwidth:
            ue_conf->SRS_Bandwidth = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_SRS_Hopping_Bandwidth:
            ue_conf->SRS_Hopping_Bandwidth = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_FrequencyDomainPosition:
            ue_conf->FrequencyDomainPosition = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_SRS_Duration:
            ue_conf->SRS_Duration = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_ISRS_SRS_ConfigIndex:
            ue_conf->ISRS_SRS_ConfigIndex = tlvs[i].val.word;
            break;
        case FAPI_L1_TransmissionComb:
            ue_conf->TransmissionComb = tlvs[i].val.byte[0];
            break;
        case FAPI_L1_SoundingReferenceCyclicShift:
            ue_conf->SoundingReferenceCyclicShift = tlvs[i].val.byte[0];
            break;
            /* SR Config All TLVs in this grouping */
        case FAPI_L1_SR_PUCCH_ResourceIndex:
            ue_conf->SR_PUCCH_ResourceIndex = tlvs[i].val.word;
            break;
        case FAPI_L1_SR_ConfigIndex:
            ue_conf->SR_ConfigIndex = tlvs[i].val.byte[0];
            break;
            /* SPS Config All TLVs in this grouping */
        case FAPI_L1_SPS_DL_ConfigSchedulingInterval:
            ue_conf->SPS_DL_ConfigSchedulingInterval = tlvs[i].val.word;
            break;
        case FAPI_L1_SPS_DL_n1PUCCH_AN_Persistent:
            ue_conf->SPS_DL_n1PUCCH_AN_Persistent = tlvs[i].val.word;
            break;
        default:
            BUG_ON(tlvs[i].tag > FAPI_L1_TLV_MAX);
            ret |= FAPI_TLV_UNSUPPORTED;
            tlv_errors [tlvs[i].tag] |= FAPI_TLV_UNSUPPORTED;
        }
    }

    return -ret;
}

int cl_ue_config_req (struct fapi_l1_ue_config_request* req,
                      struct fapi_l1_ue_config_response* response)
{
    uint8_t tlv_errors [FAPI_L1_TLV_MAX] = { FAPI_TLV_NOERROR };

    int idx = find_tlv_idx (FAPI_L1_Handle, req->tlvs, req->numberOfTLVs);

    if (idx < 0) {
        tlv_errors [FAPI_L1_Handle] |= FAPI_TLV_MISSING;
        l1_alloc_ue_config_response (MSG_INVALID_CONFIG, response, req, tlv_errors);
        return -MSG_INVALID_CONFIG;
    } else {
        if (cl_config_ue (req->tlvs[idx].val.word, req->tlvs,
                          req->numberOfTLVs, tlv_errors) < 0) {
            l1_alloc_ue_config_response (MSG_INVALID_CONFIG, response, req, tlv_errors);
            return -MSG_INVALID_CONFIG;
        } else {
            l1_alloc_ue_config_response (MSG_OK, response, NULL, NULL);        
        }
    }

    return 0;
}


int cl_ue_release_req (struct fapi_l1_ue_release_request* req,
                       struct fapi_l1_ue_release_response* response)
{
    uint8_t tlv_errors [FAPI_L1_TLV_MAX] = { FAPI_TLV_NOERROR };

    int idx = find_tlv_idx (FAPI_L1_Handle, req->tlvs, req->numberOfTLVs);

    if (idx < 0) {
        tlv_errors [FAPI_L1_Handle] |= FAPI_TLV_MISSING;
        l1_alloc_ue_release_response (MSG_INVALID_CONFIG, response, req, tlv_errors);
        return -MSG_INVALID_CONFIG;
    } else {
        if (cl_free_ue_conf (req->tlvs[idx].val.word) < 0) {
            tlv_errors[FAPI_L1_Handle] |= FAPI_TLV_WRONG_VALUE;
            l1_alloc_ue_release_response (MSG_INVALID_STATE, response, req, tlv_errors);
            return -MSG_INVALID_STATE;
        } else {
            l1_alloc_ue_release_response (MSG_OK, response, NULL, NULL);        
        }
    }
    
    return 0;
}


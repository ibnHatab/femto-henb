#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_types.h>

#include "l1_msg_alloc.h"

/** 
 * Allocate for message type. Fills in generic header.
 * When n testing mode looks on ** and if NULL do malloc. 
 * 
 * @param type - message type
 * @param msg  - message
 * @param size - size
 * 
 * @return 
 */
/* __inline */ int l1_msg_alloc_generic(FAPI_L1_MessageTypeID_e type,
                                  fapi_l1_msg_hdr_t *msg, size_t size)
{

    (msg)->msgType = type;
    (msg)->lengthVendorSpecific = 0;
    (msg)->length = size;

    return 0;
}

int l1_alloc_config_request (struct fapi_l1_config_request* request,
                          l1_tlv_word_t tlvs[], uint8_t numberOfTLVs)
{
    uint16_t msgLength = sizeof(struct fapi_l1_config_request)
        + numberOfTLVs * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_CONFIG_REQ, (fapi_l1_msg_hdr_t *)request, msgLength);

    if (numberOfTLVs) {
        memcpy((request)->tlvs, tlvs, numberOfTLVs * sizeof(l1_tlv_word_t));
        (request)->numberOfTLVs = numberOfTLVs;
    }

    return 0;
}

static void count_tlv_errors(uint8_t *tlv_errors,
                             uint8_t *numberOfInvalidOrUnsupportedTLVs,
                             uint8_t *numberOfMissingTLVs)
{
    int i;
    for (i = 0; i < FAPI_L1_TLV_MAX; i++) {
        if (tlv_errors [i] & FAPI_TLV_MISSING) numberOfMissingTLVs ++;
        if (tlv_errors [i] & (FAPI_TLV_WRONG_VALUE | FAPI_TLV_UNSUPPORTED))
            numberOfInvalidOrUnsupportedTLVs ++;
    }
}

static void save_tlv_errors(l1_tlv_word_t *requestTLVs,
                            uint16_t requestNumberOfTLVs,
                            l1_tlv_word_t *responseTLVs,
                            uint8_t numberOfInvalidOrUnsupportedTLVs,
                            uint8_t numberOfMissingTLVs,
                            uint8_t *tlv_errors)
{
    int i;
    l1_tlv_word_t *invalidOrUnsupportedTLVs, *missingTLVs;

    int invalidIdx = 0, missingIdx = 0;

    invalidOrUnsupportedTLVs = responseTLVs;
    missingTLVs = &responseTLVs[numberOfInvalidOrUnsupportedTLVs];

    for (i = 0; i < FAPI_L1_TLV_MAX; i++)
    {
        if (tlv_errors [i] & (FAPI_TLV_WRONG_VALUE | FAPI_TLV_UNSUPPORTED)) {
            int idx = find_tlv_idx ((FAPI_L1_TLV_TAG_e)i,
                                    requestTLVs, requestNumberOfTLVs);

            requestTLVs = &requestTLVs[idx];
            invalidOrUnsupportedTLVs [invalidIdx].tag = requestTLVs->tag;
            invalidOrUnsupportedTLVs [invalidIdx].len = requestTLVs->len;
            invalidOrUnsupportedTLVs [invalidIdx].val.word = requestTLVs->val.word;
            invalidIdx ++;
        }

        if (tlv_errors [i] & FAPI_TLV_MISSING) {
            missingTLVs[missingIdx].tag = (FAPI_L1_TLV_TAG_e)i;
            missingTLVs[missingIdx].len = 2;
            missingTLVs[missingIdx].val.word = 0;
            missingIdx ++;
        }
    }
}

int l1_alloc_config_response (FAPI_L1_ErrorCodes_e errorCode,
                          struct fapi_l1_config_response* response,
                          struct fapi_l1_config_request* request,
                          uint8_t *tlv_errors)
{
    uint8_t numberOfInvalidOrUnsupportedTLVs = 0;
    uint8_t numberOfMissingTLVs = 0;
    uint16_t msgLength;
    int tlvsTotal;

    if (errorCode != MSG_OK && tlv_errors != 0)
        count_tlv_errors(tlv_errors,
                         &numberOfInvalidOrUnsupportedTLVs,
                         &numberOfMissingTLVs);

    tlvsTotal = numberOfInvalidOrUnsupportedTLVs + numberOfMissingTLVs;

    msgLength = sizeof(struct fapi_l1_config_response) + tlvsTotal * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_CONFIG_RESP, (fapi_l1_msg_hdr_t *)response, msgLength);

    (response)->errorCode = errorCode;
    (response)->numberOfInvalidOrUnsupportedTLVs = numberOfInvalidOrUnsupportedTLVs;
    (response)->numberOfMissingTLVs = numberOfMissingTLVs;

    if (tlvsTotal)
        save_tlv_errors(request->tlvs, request->numberOfTLVs,
                        (response)->tlvs,
                        numberOfInvalidOrUnsupportedTLVs,
                        numberOfMissingTLVs,
                        tlv_errors);
    return 0;
}

int l1_alloc_param_request (struct fapi_l1_param_request* request)
{
    return l1_msg_alloc_generic (FAPI_L1_PARAM_REQ,
                                 (fapi_l1_msg_hdr_t *) request,
                                 sizeof(struct fapi_l1_param_request));
}

int l1_alloc_param_response (FAPI_L1_ErrorCodes_e errorCode,
                             struct fapi_l1_param_response* response,
                             uint8_t numberOfTLVs)
{
    uint16_t msgLength = sizeof(struct fapi_l1_param_response)
        + numberOfTLVs * sizeof(l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_PARAM_RESP, (fapi_l1_msg_hdr_t *)response, msgLength);

    (response)->errorCode = errorCode;
    (response)->numberOfTLVs = numberOfTLVs;

    return 0;
}

int l1_alloc_start_request (struct fapi_l1_start_request* request)
{
    return l1_msg_alloc_generic (FAPI_L1_START_REQ,
                                 (fapi_l1_msg_hdr_t *)request,
                                 sizeof(struct fapi_l1_start_request));
}

int l1_alloc_stop_request (struct fapi_l1_stop_request* request)
{
    return l1_msg_alloc_generic (FAPI_L1_STOP_REQ,
                                 (fapi_l1_msg_hdr_t *)request,
                                 sizeof(struct fapi_l1_stop_request));
}

int l1_alloc_error_indication (FAPI_L1_ErrorCodes_e errorCode,
                           FAPI_L1_MessageTypeID_e messageID,
                           struct fapi_l1_error_indication* indication)
{
    uint16_t msgLength = sizeof(struct fapi_l1_error_indication);

    l1_msg_alloc_generic (FAPI_L1_ERROR_IND, (fapi_l1_msg_hdr_t *)indication, msgLength);

    (indication)->errorCode = errorCode;
    (indication)->messageID = messageID;

    return 0;
}

int l1_alloc_stop_indication (struct fapi_l1_stop_indication* indication)
{
    return l1_msg_alloc_generic (FAPI_L1_STOP_IND,
                                 (fapi_l1_msg_hdr_t *)indication,
                                 sizeof(struct fapi_l1_stop_indication));
}

int l1_alloc_ue_config_request (struct fapi_l1_ue_config_request* request,
                                l1_tlv_word_t tlvs[], uint8_t numberOfTLVs)
{
    uint16_t msgLength = sizeof(struct fapi_l1_ue_config_request)
        + numberOfTLVs * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_UE_CONFIG_REQ, (fapi_l1_msg_hdr_t *)request, msgLength);

    if (numberOfTLVs) {
        memcpy((request)->tlvs, tlvs, numberOfTLVs * sizeof(l1_tlv_word_t));
        (request)->numberOfTLVs = numberOfTLVs;
    }

    return 0;
}

int l1_alloc_ue_config_response (FAPI_L1_ErrorCodes_e errorCode,
                             struct fapi_l1_ue_config_response* response,
                             struct fapi_l1_ue_config_request* request,
                             uint8_t *tlv_errors)
{
    uint8_t numberOfInvalidOrUnsupportedTLVs = 0;
    uint8_t numberOfMissingTLVs = 0;
    uint16_t msgLength;
    int tlvsTotal;

    if (errorCode != MSG_OK && tlv_errors != 0)
        count_tlv_errors(tlv_errors,
                         &numberOfInvalidOrUnsupportedTLVs,
                         &numberOfMissingTLVs);

    tlvsTotal = numberOfInvalidOrUnsupportedTLVs + numberOfMissingTLVs;

    msgLength = sizeof(struct fapi_l1_ue_config_response) + tlvsTotal * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_UE_CONFIG_RESP, (fapi_l1_msg_hdr_t *)response, msgLength);

    (response)->errorCode = errorCode;
    (response)->numberOfInvalidOrUnsupportedTLVs = numberOfInvalidOrUnsupportedTLVs;
    (response)->numberOfMissingTLVs = numberOfMissingTLVs;

    if (tlvsTotal)
        save_tlv_errors(request->tlvs, request->numberOfTLVs,
                        (response)->tlvs,
                        numberOfInvalidOrUnsupportedTLVs,
                        numberOfMissingTLVs,
                        tlv_errors);
    return 0;
}

int l1_alloc_ue_release_request (struct fapi_l1_ue_release_request* request,
                                l1_tlv_word_t tlvs[], uint8_t numberOfTLVs)
{
    uint16_t msgLength = sizeof(struct fapi_l1_ue_release_request)
        + numberOfTLVs * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_UE_RELEASE_REQ, (fapi_l1_msg_hdr_t *)request, msgLength);

    if (numberOfTLVs) {
        memcpy((request)->tlvs, tlvs, numberOfTLVs * sizeof(l1_tlv_word_t));
        (request)->numberOfTLVs = numberOfTLVs;
    }

    return 0;
}

int l1_alloc_ue_release_response (FAPI_L1_ErrorCodes_e errorCode,
                             struct fapi_l1_ue_release_response* response,
                             struct fapi_l1_ue_release_request* request,
                             uint8_t *tlv_errors)
{
    uint8_t numberOfInvalidOrUnsupportedTLVs = 0;
    uint8_t numberOfMissingTLVs = 0;
    uint16_t msgLength;
    int tlvsTotal;

    if (errorCode != MSG_OK && tlv_errors != 0)
        count_tlv_errors(tlv_errors,
                         &numberOfInvalidOrUnsupportedTLVs,
                         &numberOfMissingTLVs);

    tlvsTotal = numberOfInvalidOrUnsupportedTLVs + numberOfMissingTLVs;

    msgLength = sizeof(struct fapi_l1_ue_release_response) + tlvsTotal * sizeof (l1_tlv_word_t);

    l1_msg_alloc_generic (FAPI_L1_UE_RELEASE_RESP, (fapi_l1_msg_hdr_t *)response, msgLength);

    (response)->errorCode = errorCode;
    (response)->numberOfInvalidOrUnsupportedTLVs = numberOfInvalidOrUnsupportedTLVs;
    (response)->numberOfMissingTLVs = numberOfMissingTLVs;

    if (tlvsTotal)
        save_tlv_errors(request->tlvs, request->numberOfTLVs,
                        (response)->tlvs,
                        numberOfInvalidOrUnsupportedTLVs,
                        numberOfMissingTLVs,
                        tlv_errors);
    return 0;
}


int l1_alloc_subframe_indication (uint16_t sfn_sf,
                                  struct fapi_l1_subframe_indication* indication)
{
    uint16_t msgLength = sizeof(struct fapi_l1_subframe_indication);

    l1_msg_alloc_generic (FAPI_L1_SUBFRAME_IND, (fapi_l1_msg_hdr_t *)indication, msgLength);

    (indication)->SFN_SF = sfn_sf;

    return 0;
}

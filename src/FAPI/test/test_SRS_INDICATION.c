#include <embUnit/embUnit.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>
#include <l1_sdu_combinator.h>

#include <fapi_cl.h>
#include <fapi_cl_cphy_sap.h>
#include <fapi_cl_conv.h>
#include <fapi_cl_ue_db.h>

#include <api_storage.h>

void send_CONFIG_REQUEST(l1_tlv_word_t tlvs[], size_t num);
extern l1_tlv_word_t SISO_CONFIG_REQUEST_TLVS [];
extern size_t SISO_CONFIG_REQUEST_TLVS_num;

extern unsigned char out_srs_0008_PHY_RXSTATUS_IND_bin[];
extern unsigned int out_srs_0008_PHY_RXSTATUS_IND_bin_len;

static l1_tlv_word_t UE_CONFIG_TLVS [] = {
    {FAPI_L1_Handle                          , 2, {42}},
    {FAPI_L1_RNTI                            , 2, {.word = 51}},
    /* CQI Config All TLVs in this grouping */
    {FAPI_L1_CQI_PUCCH_ResourceIndex         , 2, {0}},
    {FAPI_L1_CQI_PMI_ConfigIndex             , 2, {0}},
    {FAPI_L1_CQI_RI_ConfigIndex              , 2, {0}},
    {FAPI_L1_CQI_SimultaneousAckNackAnd_CQI  , 1, {0}},
    /* ACK/NACK Config All TLVs in this grouping */
    {FAPI_L1_AN_RepetitionFactor             , 1, {2}},
    {FAPI_L1_AN_n1PUCCH_ANRep                , 2, {0}},
#if 0
    {FAPI_L1_TDD_AckNackFeedbackMode         , 1, {0}},
#endif
    /* SRS Config All TLVs in this grouping */
    {FAPI_L1_SRS_Bandwidth                   , 1, {3}},
    {FAPI_L1_SRS_Hopping_Bandwidth           , 1, {3}},
    {FAPI_L1_FrequencyDomainPosition         , 1, {23}},
    {FAPI_L1_SRS_Duration                    , 1, {1}},
    {FAPI_L1_ISRS_SRS_ConfigIndex            , 2, {.word = 1023}},
    {FAPI_L1_TransmissionComb                , 1, {1}},
    {FAPI_L1_SoundingReferenceCyclicShift    , 1, {7}},
    /* SR Config All TLVs in this grouping */
    {FAPI_L1_SR_PUCCH_ResourceIndex          , 2, {.word = 2047}},
    {FAPI_L1_SR_ConfigIndex                  , 1, {155}},
    /* SPS Config All TLVs in this grouping */
    {FAPI_L1_SPS_DL_ConfigSchedulingInterval , 2, {.word = 640}},
    {FAPI_L1_SPS_DL_n1PUCCH_AN_Persistent    , 2, {.word = 2047}},
};
static size_t UE_CONFIG_TLVS_num = sizeof(UE_CONFIG_TLVS) / sizeof(l1_tlv_word_t);

static void setUp(void)
{
    /* initialize */
    cl_init ();
    lte_cphy_sap_init ();
    cl_ue_db_init ();

    lte_cphy_set_state (PHY_STATE_IDLE);
}

static void tearDown(void)
{
	/* terminate */
}

/*embunit:impl=+ */
static void testConvert_SRS(void)
{
    char msgBuff[BUFSIZ];
    struct fapi_l1_srs_indication * fapi = (struct fapi_l1_srs_indication *) msgBuff;

    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_srs_0008_PHY_RXSTATUS_IND_bin;

    RXSTATUSIND * api = (RXSTATUSIND *) ((uint8_t*)&apiLogInfo [1] + 8);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTATUS_IND, api->msgType);
                      
    TEST_ASSERT_EQUAL_INT(0, conv_RX_XX_indication_from_RxStatusInd (api, (fapi_l1_msg_hdr_t *)fapi));

    TEST_ASSERT_EQUAL_INT(api->frameNum, fapi->SFN_SF.val.sfn);
    TEST_ASSERT_EQUAL_INT(api->subFrameNum, fapi->SFN_SF.val.sf);

    TEST_ASSERT_EQUAL_INT(api->eventNumber, fapi->NumberOf_UEs);

    PRX_SRS_STATUS_EVENT rx_srs = (PRX_SRS_STATUS_EVENT) &api->pEventList;
    struct fapi_l1_ue_srs_config * ue_srs = &fapi->UEs[0];
    
    TEST_ASSERT_EQUAL_INT(rx_srs->numberRb, ue_srs->NumberOfResourceBlocks);        
}

static void testProgram_SRS(void)
{
    // (A) Configure PHY
    send_CONFIG_REQUEST(SISO_CONFIG_REQUEST_TLVS,
                        SISO_CONFIG_REQUEST_TLVS_num);

    

    // (A) Configure UE
    {
        uint8_t ue_msg_buff[BUFSIZ];
        uint8_t rsp_msg_buff[BUFSIZ];
        struct fapi_l1_ue_config_request* request
            = (struct fapi_l1_ue_config_request*) ue_msg_buff;
        struct fapi_l1_ue_config_response* response
            = (struct fapi_l1_ue_config_response*) rsp_msg_buff;

        l1_alloc_ue_config_request (request, UE_CONFIG_TLVS, UE_CONFIG_TLVS_num);
        cl_ue_config_req (request, response);

        TEST_ASSERT_EQUAL_INT(MSG_OK, response->errorCode);
    }
    // (B) Program SRS
    struct fapi_l1_srs_pdu srs_pdu_4 = {
        .Handle = 4,
        .Size = 0,
        .RNTI = 51,
        .SRS_Bandwidth  = 0,
        .FrequencyDomainPosition = 0,
        .SRS_HoppingBandwidth = 0,
        .TransmissionComb = 0,
        .ISRS_SRS_ConfigIndex = 0,
        .SoundingReferenceCyclicShift = 0,
    };

    struct fapi_l1_ul_config_request * ul_conf_srs;
    {
        uint8_t msg_buff[BUFSIZ];
        uint8_t api_buff[BUFSIZ];
        ul_conf_srs = of_ul_conf(0 << 4 | 0,
                                 ul_conf_add_pdu((uint8_t*) &srs_pdu_4,
                                                 SRS_PDU_TYPE_SIZE,
                                                 format_ul_conf (msg_buff, NULL)));

        PULSUBFRDESC apiRX_v = (PULSUBFRDESC) api_buff;
        size_t msgSize = 0;

        TEST_ASSERT_EQUAL_INT(0,
                              conv_ulSubframeDescriptor_from_UL_CONFIG_request (
                                  ul_conf_srs, apiRX_v, &msgSize));        
    }
    // (C) Detect SRS from API trace
    struct fapi_l1_srs_indication * fapi;
    {
        char fapiBuff[BUFSIZ];
        fapi = (struct fapi_l1_srs_indication *) fapiBuff;

        API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_srs_0008_PHY_RXSTATUS_IND_bin;        
        RXSTATUSIND * api = (RXSTATUSIND *) ((uint8_t*)&apiLogInfo [1] + 8);
        TEST_ASSERT_EQUAL_INT(PHY_RXSTATUS_IND, api->msgType);
                      
        TEST_ASSERT_EQUAL_INT(0,
                              conv_RX_XX_indication_from_RxStatusInd (api, (fapi_l1_msg_hdr_t *)fapi));
        
        // Check
        TEST_ASSERT_EQUAL_INT(srs_pdu_4.Handle, fapi->UEs[0].Handle);
        TEST_ASSERT_EQUAL_INT(srs_pdu_4.RNTI, fapi->UEs[0].RNTI);
        TEST_ASSERT_EQUAL_INT(ul_conf_srs->SFN_SF, fapi->SFN_SF.sfn_sf);
        
    }

    
    
}

/*embunit:impl=- */
TestRef test_SRS_INDICATION_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testConvert_SRS",testConvert_SRS),
		new_TestFixture("testProgram_SRS",testProgram_SRS),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_SRS_INDICATION,"test_SRS_INDICATION",setUp,tearDown,fixtures);
	return (TestRef)&test_SRS_INDICATION;
};

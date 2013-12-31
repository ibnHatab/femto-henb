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

extern unsigned char out_siso_0038_PHY_RXSDU_IND_bin[];
extern unsigned int out_siso_0038_PHY_RXSDU_IND_bin_len;


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
static void testConvert(void)
{
    // Allocate memory
    char msgBuff[3][BUFSIZ];
    l1_sfn_sf_t sfn_sf = { .val.sfn = 887, .val.sf = 7};
    uint16_t channelId = 0;
    uint32_t Handle = 42;
    uint16_t RNTI = 1024;

    void *cur = go_next_rx_subframe_layer (sfn_sf);

    link_rx_request_info(cur, channelId, Handle, RNTI, ULSCH_PDU_TYPE);

    fapi_l1_msg_hdr_t *fapi
        = (fapi_l1_msg_hdr_t *) msgBuff[0];
    struct fapi_l1_rx_cqi_indication *cqi_ind
        = (struct fapi_l1_rx_cqi_indication *) msgBuff[1];
    struct fapi_l1_crc_indication *crc_ind
        = (struct fapi_l1_crc_indication *) msgBuff[2];

    // Read reference trace
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_siso_0038_PHY_RXSDU_IND_bin;

    PRXSDUIND api = (PRXSDUIND) ((uint8_t*)&apiLogInfo [1]);
    TEST_ASSERT_EQUAL_INT(PHY_RXSDU_IND, api->msgType);

    // Test Estimate size
    uint16_t fapi_msg_size, cqi_ind_size, crc_ind_size;

    estimateSize_RX_ULSCH_indication_from_RxSduInd (api,
                                                    &fapi_msg_size,
                                                    &cqi_ind_size,
                                                    &crc_ind_size);

    // Test convertion
    int ret = conv_RX_ULSCH_indication_from_RxSduInd (api, fapi, cqi_ind, crc_ind);
    TEST_ASSERT_EQUAL_INT(0, ret);

    TEST_ASSERT_EQUAL_INT(crc_ind_size, crc_ind->hdr.length);
    TEST_ASSERT_EQUAL_INT(fapi_msg_size, fapi->length);

    if (api->chanType == PUCCH) {
        TEST_ASSERT_EQUAL_INT(cqi_ind_size, cqi_ind->hdr.length);
    }

    struct fapi_l1_ulsch_pdu_config * PDU = 
        ((struct fapi_l1_rx_ulsch_indication *) fapi )->RX_Config;

    TEST_ASSERT_EQUAL_INT(Handle, PDU->Handle);
    TEST_ASSERT_EQUAL_INT(RNTI, PDU->RNTI);
}

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

static void testSendReceive(void)
{
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
    }

    // (B) Send CONFIG for ULSCH
    // (B.1) simulate subframe programming received
    l1_sfn_sf_t sfn_sf = { .val.sfn = 887, .val.sf = 7};
    go_next_rx_subframe_layer (sfn_sf);

    // SISO #33 RXVector 887/7
    struct fapi_l1_ul_config_request * ul_conf_ulsch;
    {
        uint8_t msgBuff[BUFSIZ];
        uint8_t apiBuff[BUFSIZ];
        size_t msgSize = 0;

        PULSUBFRDESC apiRX_v = (PULSUBFRDESC) apiBuff;

        struct fapi_l1_ul_config_request ul_config_request_data = {
            .SFN_SF = INVALID_SFNSF,
            .Length = 0,
            .NumberOf_PDUs = 0,
            .RACH_PRACH =  0,
            .SRS_Present = 1
        };

        struct fapi_l1_ulsch_pdu  ulsch_pdu_data =
            {
                .Handle = 42,
                .Size = 0,
                .RNTI = 51,
                .ResourceBlockStart = 99,
                .NumberOfResourceBlocks = 1,
                .ModulationType = MD_QPSK,
                .CyclicShift2For_DMRS = 0,
                .FrequencyHoppingEnabledFlag = 1,
                .FrequencyHoppingBits = 3,
                .NewDataIndication = 1,
                .RedundancyVersion = 0,
                .HARQ_ProcessNumber = 7,
                .UL_TX_Mode = 1,
                .Current_TX_NB = 0,
                .N_Srs = 1,
            };

        ul_conf_ulsch = of_ul_conf(sfn_sf.sfn_sf,
                                   ul_conf_add_pdu((uint8_t*) &ulsch_pdu_data, ULSCH_PDU_TYPE_SIZE,
                                   format_ul_conf (msgBuff, &ul_config_request_data)));


        TEST_ASSERT_EQUAL_INT(0,
                              conv_ulSubframeDescriptor_from_UL_CONFIG_request (
                                  ul_conf_ulsch, apiRX_v, &msgSize));
    }

    // (C) Receive PUSCH PDU
    // SISO # 38 RXSDU 887/7
    struct fapi_l1_rx_ulsch_indication * ulsch_ind;
    {
        char msgBuff[3][BUFSIZ];

        fapi_l1_msg_hdr_t *fapi
            = (fapi_l1_msg_hdr_t *) msgBuff[0];
        ulsch_ind = (struct fapi_l1_rx_ulsch_indication *) fapi;
        struct fapi_l1_rx_cqi_indication *cqi_ind
            = (struct fapi_l1_rx_cqi_indication *) msgBuff[1];
        struct fapi_l1_crc_indication *crc_ind
            = (struct fapi_l1_crc_indication *) msgBuff[2];

        // Read reference trace
        API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_siso_0038_PHY_RXSDU_IND_bin;

        PRXSDUIND api = (PRXSDUIND) ((uint8_t*)&apiLogInfo [1]);
        TEST_ASSERT_EQUAL_INT(PHY_RXSDU_IND, api->msgType);

        TEST_ASSERT_EQUAL_INT(0,
                              conv_RX_ULSCH_indication_from_RxSduInd (api, fapi, cqi_ind, crc_ind));
    }

    // (D) Check channel mapping
    TEST_ASSERT_EQUAL_INT(ul_conf_ulsch->SFN_SF, ulsch_ind->SFN_SF);

    struct fapi_l1_ulsch_pdu* ulsch_pdu =
        (struct fapi_l1_ulsch_pdu*) ul_conf_ulsch->UL_PDU_Configuration[0].UL_PDU;
    TEST_ASSERT_EQUAL_INT(ulsch_pdu->Handle,
                          ulsch_ind->RX_Config[0].Handle);
    TEST_ASSERT_EQUAL_INT(ulsch_pdu->RNTI,
                          ulsch_ind->RX_Config[0].RNTI);
}

/*embunit:impl=- */
TestRef test_PUSH_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
            new_TestFixture("testConvert",testConvert),
                new_TestFixture("testSendReceive",testSendReceive),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_PUSH,"test_PUSH",setUp,tearDown,fixtures);
	return (TestRef)&test_PUSH;
};

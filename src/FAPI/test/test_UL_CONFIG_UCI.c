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
extern l1_tlv_word_t MIMO_CONFIG_REQUEST_TLVS [];
extern size_t MIMO_CONFIG_REQUEST_TLVS_num;

extern unsigned char out_siso_0178_PHY_RXSTART_REQ_bin[];
extern unsigned int out_siso_0178_PHY_RXSTART_REQ_bin_len;

extern unsigned char out_srs_0004_PHY_RXSTART_REQ_bin[];
extern unsigned int out_srs_0004_PHY_RXSTART_REQ_bin_len;


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

static void test_UL_CONFIG_UCI(struct fapi_l1_ul_config_request * fapi,
                               unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PULSUBFRDESC apiRX_v = (PULSUBFRDESC) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);
    size_t msgSize = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_ulSubframeDescriptor_from_UL_CONFIG_request (
                              fapi, apiRX_v, &msgSize));

      //    save_api_fragment (PHY_RXSTART_REQ, storage, "/tmp/ulsch-uci.api", msgSize);


    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    PULSUBFRDESC apiRX_v_Ref = (PULSUBFRDESC) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, genhdr->msgType);

    PULCTRLCHDESC ctrlChRef = (PULCTRLCHDESC) ((uint8_t*)apiRX_v_Ref) + apiRX_v_Ref->offsetULCtrlChannels;
    PULCTRLCHDESC ctrlCh = (PULCTRLCHDESC) ((uint8_t*)apiRX_v) + apiRX_v->offsetULCtrlChannels;

    TEST_ASSERT_EQUAL_BIN(ctrlChRef, ctrlCh, sizeof(ULCTRLCHDESC));

    /* // FIXUP HEADER */
    apiRX_v_Ref->offsetRachCtrlStruct = apiRX_v->offsetRachCtrlStruct = 0; 
    apiRX_v_Ref->offsetULCtrlChannels = apiRX_v->offsetULCtrlChannels = 0; 
    apiRX_v_Ref->offsetsrsInfo = apiRX_v->offsetsrsInfo = 0; 
    apiRX_v_Ref->offsetCustomFeatures = apiRX_v->offsetCustomFeatures = 0; 

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, offsetof(ULSUBFRDESC, ulSfrCtrl));

    apiRX_v_Ref->ulSfrCtrl.pad = 0;
        
    TEST_ASSERT_EQUAL_BIN(&apiRX_v_Ref->ulSfrCtrl,
                          &apiRX_v->ulSfrCtrl, sizeof(ULSUBFRCMNCTRL));
}

/*embunit:impl=+ */
/*
  IQ:SISO, API:#178 SFN: 893,0
*/

static void testPUCCH(void)
{
    uint8_t msg_buff[BUFSIZ];
    uint8_t ue_msg_buff[BUFSIZ];
    uint8_t rsp_msg_buff[BUFSIZ];

    // (A) Configure PHY

    send_CONFIG_REQUEST(SISO_CONFIG_REQUEST_TLVS,
                        SISO_CONFIG_REQUEST_TLVS_num);

    lte_cphy_set_state (PHY_STATE_RUNNING);

   // (B) Check UL CH config
    struct fapi_l1_ul_config_request ul_config_request_data = {
    .SFN_SF = INVALID_SFNSF,
    .Length = 0,
    .NumberOf_PDUs = 0,
    .RACH_PRACH =  1,
    .SRS_Present = 1
    };

#define HARQ_INFORMATION_FDD_DATA {             \
        .PUCCH_Index = 0,                       \
            .HARQ_Size  = 0,                    \
            }
#define SR_INFORMATION_DATA  {                   \
        .PUCCH_Index = 0,                       \
            }
#define CQI_INFORMATION_DATA  {                 \
        .PUCCH_Index = 0,                       \
            .DL_CQI_PMI_Size = 0,               \
            } 
    
    struct fapi_l1_uci_cqi_sr_harq_pdu uci_cqi_sr_harq_pdu_data = {
    .Handle = 42,
    .RNTI = 51,
    .CQI_Information = CQI_INFORMATION_DATA,
    .SR_Information = SR_INFORMATION_DATA,
    .HARQ_Information.fdd = HARQ_INFORMATION_FDD_DATA
    };

    
    struct fapi_l1_ul_config_request * ul_conf_ulsch =
        of_ul_conf(893 << 4 | 0,
                   ul_conf_add_pdu((uint8_t*) &uci_cqi_sr_harq_pdu_data,
                                   UCI_CQI_SR_HARQ_PDU_TYPE_SIZE,
                                   format_ul_conf (msg_buff, &ul_config_request_data)));

    // Configure UE
    struct fapi_l1_ue_config_request* request
        = (struct fapi_l1_ue_config_request*) ue_msg_buff;
    struct fapi_l1_ue_config_response* response
        = (struct fapi_l1_ue_config_response*) rsp_msg_buff;

    l1_alloc_ue_config_request (request, UE_CONFIG_TLVS, UE_CONFIG_TLVS_num);
    cl_ue_config_req (request, response);

    TEST_ASSERT_EQUAL_INT(MSG_OK, response->errorCode);

    test_UL_CONFIG_UCI(ul_conf_ulsch,
                       out_siso_0178_PHY_RXSTART_REQ_bin,
                       out_siso_0178_PHY_RXSTART_REQ_bin_len);

}


static void test_UL_CONFIG_SRS(struct fapi_l1_ul_config_request * fapi,
                               unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PULSUBFRDESC apiRX_v = (PULSUBFRDESC) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);
    size_t msgSize = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_ulSubframeDescriptor_from_UL_CONFIG_request (
                              fapi, apiRX_v, &msgSize));

    //    save_api_fragment (PHY_RXSTART_REQ, storage, "/tmp/srs.api", msgSize);


    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) ((uint8_t*)&apiLogInfo [1] + 8); // +8 for new header
    PULSUBFRDESC apiRX_v_Ref = (PULSUBFRDESC) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, genhdr->msgType);

    PSRSDED srsChRef = (PSRSDED) ((uint8_t*)apiRX_v_Ref) + apiRX_v_Ref->offsetsrsInfo;
    PSRSDED srsCh = (PSRSDED) ((uint8_t*)apiRX_v) + apiRX_v->offsetsrsInfo;

    TEST_ASSERT_EQUAL_BIN(srsChRef, srsCh, sizeof(SRSDED));

    /* // FIXUP HEADER */
    apiRX_v_Ref->offsetRachCtrlStruct = apiRX_v->offsetRachCtrlStruct = 0; 
    apiRX_v_Ref->offsetULCtrlChannels = apiRX_v->offsetULCtrlChannels = 0; 
    apiRX_v_Ref->offsetsrsInfo = apiRX_v->offsetsrsInfo = 0; 
    apiRX_v_Ref->offsetCustomFeatures = apiRX_v->offsetCustomFeatures = 0; 

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, offsetof(ULSUBFRDESC, ulSfrCtrl));

}

static void testSRS(void)
{
    uint8_t msg_buff[BUFSIZ];
    uint8_t ue_msg_buff[BUFSIZ];
    uint8_t rsp_msg_buff[BUFSIZ];

    // (A) Configure PHY

    send_CONFIG_REQUEST(MIMO_CONFIG_REQUEST_TLVS,
                        MIMO_CONFIG_REQUEST_TLVS_num);

    lte_cphy_set_state (PHY_STATE_RUNNING);

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

   // (B) Check UL CH config
    struct fapi_l1_ul_config_request ul_config_request_data = {
        .SFN_SF = INVALID_SFNSF,
        .Length = 0,
        .NumberOf_PDUs = 0,
        .RACH_PRACH =  1,
        .SRS_Present = 1
    };


    struct fapi_l1_srs_pdu srs_pdu_data = {
        .Handle = 42,
        .Size = 0,
        .RNTI = 51,
        .SRS_Bandwidth  = 0,
        .FrequencyDomainPosition = 0,
        .SRS_HoppingBandwidth = 0,
        .TransmissionComb = 0,
        .ISRS_SRS_ConfigIndex = 0,
        .SoundingReferenceCyclicShift = 0,
    };

    
    struct fapi_l1_ul_config_request * ul_conf_srs =
        of_ul_conf(0 << 4 | 0,
                   ul_conf_add_pdu((uint8_t*) &ulsch_pdu_data, ULSCH_PDU_TYPE_SIZE,
                   ul_conf_add_pdu((uint8_t*) &srs_pdu_data,
                                   SRS_PDU_TYPE_SIZE,
                                   format_ul_conf (msg_buff, &ul_config_request_data))));


    // Configure UE
    struct fapi_l1_ue_config_request* request
        = (struct fapi_l1_ue_config_request*) ue_msg_buff;
    struct fapi_l1_ue_config_response* response
        = (struct fapi_l1_ue_config_response*) rsp_msg_buff;

    l1_alloc_ue_config_request (request, UE_CONFIG_TLVS, UE_CONFIG_TLVS_num);
    cl_ue_config_req (request, response);

    TEST_ASSERT_EQUAL_INT(MSG_OK, response->errorCode);

    test_UL_CONFIG_SRS(ul_conf_srs,
                       out_srs_0004_PHY_RXSTART_REQ_bin,
                       out_srs_0004_PHY_RXSTART_REQ_bin_len);

}


/*embunit:impl=- */
TestRef test_UL_CONFIG_UCI_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
            new_TestFixture("testPUCCH",testPUCCH),
            new_TestFixture("testSRS",testSRS),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_UL_CONFIG_UCI,"test_UL_CONFIG_UCI",setUp,tearDown,fixtures);
	return (TestRef)&test_UL_CONFIG_UCI;
};

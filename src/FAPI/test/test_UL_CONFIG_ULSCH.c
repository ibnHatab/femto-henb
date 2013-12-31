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

extern unsigned char out_siso_0033_PHY_RXSTART_REQ_bin[];
extern unsigned int out_siso_0033_PHY_RXSTART_REQ_bin_len;

extern unsigned char out_siso_0378_PHY_RXSTART_REQ_bin[];
extern unsigned int out_siso_0378_PHY_RXSTART_REQ_bin_len;

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

static void test_UL_CONFIG_ULSCH(struct fapi_l1_ul_config_request * fapi,
                                 unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PULSUBFRDESC apiRX_v = (PULSUBFRDESC) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);
    size_t msgSize = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_ulSubframeDescriptor_from_UL_CONFIG_request (
                              fapi, apiRX_v, &msgSize));

    save_api_fragment (PHY_RXSTART_REQ, storage, "/tmp/ulsch.api", msgSize);


    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    PULSUBFRDESC apiRX_v_Ref = (PULSUBFRDESC) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_RXSTART_REQ, genhdr->msgType);

    /* // FIXUP HEADER */
    apiRX_v_Ref->offsetRachCtrlStruct = apiRX_v->offsetRachCtrlStruct = 0; 
    apiRX_v_Ref->offsetULCtrlChannels = apiRX_v->offsetULCtrlChannels = 0; 
    apiRX_v_Ref->offsetsrsInfo = apiRX_v->offsetsrsInfo = 0; 
    apiRX_v_Ref->offsetCustomFeatures = apiRX_v->offsetCustomFeatures = 0; 

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, offsetof(ULSUBFRDESC, ulSfrCtrl));
    TEST_ASSERT_EQUAL_BIN(&apiRX_v_Ref->ulSfrCtrl,
                          &apiRX_v->ulSfrCtrl, sizeof(ULSUBFRCMNCTRL));

    PULCHANDESC ChRef = &apiRX_v_Ref->ulCh[0];
    PULCHANDESC Ch = &apiRX_v->ulCh[0];

    ChRef->repeatCycle = 0;
    *((uint16_t*)((uint8_t*)(&ChRef->ulSubChInfo.harqInfo) - 2)) = 0;
    *((uint8_t*)(&ChRef->ulSubChInfo.crcInfo) - 3) = 0;

    *((uint8_t*)(&ChRef->ulSubChInfo.scrInfo) - 3) = 0;
    *((uint8_t*)(&ChRef->ulSubChInfo.scrInfo) - 2) = 0;
    *((uint8_t*)(&ChRef->ulSubChInfo.scrInfo) - 1) = 0;

    *((uint8_t*)(&ChRef->ulSubChInfo.scrInfo) + 1) = 0;
    *((uint32_t*)&ChRef->ulSubChInfo.reserved) = 0;
    *((uint32_t*)&ChRef->ulSubChInfo.reserved) = 0;

    *(((uint32_t*)(&ChRef->ulSubChInfo.mapInfo) - 1)) = 0;

    ChRef->ulSubChInfo.puschDed.reserved = 0;


    memset(&ChRef->ulSubChInfo.mapInfo.reselmInfo[1],
           0, (MAXMAPCNT - 1) * sizeof(RESELEMINFO));

    ChRef->ulSubChInfo.mapInfo.reselmInfo [MAXMAPCNT].startRes = 0;
    ChRef->ulSubChInfo.mapInfo.reselmInfo [MAXMAPCNT].numRes = 0;
        
    TEST_ASSERT_EQUAL_BIN(ChRef, Ch, sizeof(ULCHANDESC));
}

/*
ul_PUSCH1_2empty20data.bin
Contains 2 empty channels and next 20 SFs of data below.

API trace matching: API (RxVector) #33, RxSDU - #38

IQ samples matching
12-13 ms - RRC Connection Request - PUSCH - 7 bytes (QPSK MCS=0) -

0000:63 74 00 06 A6 02 FF
 */

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


static void testRRC_CON_REQ(void)
{
    uint8_t msg_buff[BUFSIZ];
    uint8_t ue_msg_buff[BUFSIZ];
    uint8_t rsp_msg_buff[BUFSIZ];

    // (A) Configure PHY
    lte_cphy_set_state (PHY_STATE_IDLE);

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

    struct fapi_l1_ul_config_request * ul_conf_ulsch =
        of_ul_conf(887 << 4 | 7,
                   ul_conf_add_pdu((uint8_t*) &ulsch_pdu_data, ULSCH_PDU_TYPE_SIZE,
                                   format_ul_conf (msg_buff, &ul_config_request_data)));

    // Configure UE
    struct fapi_l1_ue_config_request* request
        = (struct fapi_l1_ue_config_request*) ue_msg_buff;
    struct fapi_l1_ue_config_response* response
        = (struct fapi_l1_ue_config_response*) rsp_msg_buff;

    l1_alloc_ue_config_request (request, UE_CONFIG_TLVS, UE_CONFIG_TLVS_num);
    cl_ue_config_req (request, response);

    TEST_ASSERT_EQUAL_INT(MSG_OK, response->errorCode);

    test_UL_CONFIG_ULSCH(ul_conf_ulsch,
                         out_siso_0033_PHY_RXSTART_REQ_bin,
                         out_siso_0033_PHY_RXSTART_REQ_bin_len);

}

/*
ul_PUSCH2_2empty20data.bin.
Contains 2 empty channels and next 20 SFs of data below.

API trace matching: API (RxVector) #378, RxSDU - #383

IQ samples matching
133-134 ms - RRC Connection Setup Complete - PUSCH - 87 bytes (QPSK MCS=4) -

0000:FF FF FF FF FF FF FF FF FF FF FF FF 02 FF FF FF : ÿÿÿÿÿÿÿÿÿÿÿÿ.ÿÿÿ
0010:FF FF FF FF FF FF FF FF FF FF FF FF FF 81 90 40 : ÿÿÿÿÿÿÿÿÿÿÿÿÿ@
0020:18 4D E0 46 00 C0 CA 1C 00 33 EB 3C 6F 02 FF FF : .MàF.ÀÊ..3ë<o.ÿÿ
0030:FF FF FF FF FF FF FF FF FF FF FF FF FF FF 02 FF : ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ.ÿ
0040:FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 02 : ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ.
0050:FF FF FF FF FF FF FF
 */
static void testRRC_SET_COMPL(void)
{
      //	TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef test_UL_CONFIG_ULSCH_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testRRC_CON_REQ",testRRC_CON_REQ),
		new_TestFixture("testRRC_SET_COMPL",testRRC_SET_COMPL),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_UL_CONFIG_ULSCH,"test_UL_CONFIG_ULSCH",setUp,tearDown,fixtures);
	return (TestRef)&test_UL_CONFIG_ULSCH;
};

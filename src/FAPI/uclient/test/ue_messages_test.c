#include <embUnit/embUnit.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <fapi_cl.h>
#include <lte_cphy_sap.h>
#include <fapi_cl_ue_db.h>
#include <fapi_cl_conv.h>

#include <LtePhyL2Api.h>
#include <fapi/l1_l2_iface.h>
#include <fapi/l1_tlv.h>
#include <fapi/l1_msg_alloc.h>

#include <lte_cphy_sap.h>
#include <fapi_cl_conv.h>

#include "api_storage.h"
#include "phy_messages_data.h"

/*embunit:include=+ */
extern void chpy_init_db ();
extern void cphy_sap_user_init (void);

extern int cphy_sap_send_mesg (uint8_t * data);
extern uint8_t * cphy_sap_receive_mesg ();

extern int l1_msg_alloc_count;

/*embunit:include=- */

static void setUp(void)
{
    cl_init ();
    lte_chpy_sap_init ();
    cl_ue_db_init ();

    /* initialize */
    lte_chpy_set_state (PHY_STATE_IDLE);
    cphy_sap_user_init ();
}

static void tearDown(void)
{
    /* terminate */
    if (l1_msg_alloc_count) {
        printf(">> cphy_sap_test_tests alloc count: %d\n", l1_msg_alloc_count);
    }
}


l1_tlv_word_t UE_CONFIG_TLVS [] = {
    {FAPI_L1_Handle                          , 2, {42}},
    {FAPI_L1_RNTI                            , 2, {.word = 1023}},
    /* CQI Config All TLVs in this grouping */
    {FAPI_L1_CQI_PUCCH_ResourceIndex         , 2, {1}},
    {FAPI_L1_CQI_PMI_ConfigIndex             , 2, {1}},
    {FAPI_L1_CQI_RI_ConfigIndex              , 2, {1}},
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
size_t UE_CONFIG_TLVS_num = sizeof(UE_CONFIG_TLVS) / sizeof(l1_tlv_word_t);


/*embunit:impl=+ */
static void testL1_UE_CONFIG(void)
{
    struct fapi_l1_ue_config_request* request = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_ue_config_request (&request,
                                                      UE_CONFIG_TLVS, UE_CONFIG_TLVS_num));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) request));

    struct fapi_l1_ue_config_response* rsp =
        (struct fapi_l1_ue_config_response*) cphy_sap_receive_mesg ();
    
    TEST_ASSERT_NOT_NULL(rsp);
    TEST_ASSERT_EQUAL_INT(FAPI_L1_UE_CONFIG_RESP, rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *) rsp);
}

static void testL1_UE_CONFIG_PARAMETERS(void)
{
    struct fapi_l1_ue_config_request* request = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_ue_config_request (&request, UE_CONFIG_TLVS+1,
                                                      UE_CONFIG_TLVS_num-1));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) request));

    struct fapi_l1_ue_config_response* rsp =
        (struct fapi_l1_ue_config_response*) cphy_sap_receive_mesg ();
    
    TEST_ASSERT_NOT_NULL(rsp);
    TEST_ASSERT_EQUAL_INT(FAPI_L1_UE_CONFIG_RESP, rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_INVALID_CONFIG, rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *) rsp);
}

static void testL1_UE_RELEASE(void)
{
    cl_ue_db_init ();
    // release not allocated    
    struct fapi_l1_ue_release_request* release_request = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_ue_release_request (&release_request,
                                                       UE_CONFIG_TLVS, 2 /* UE_CONFIG_TLVS_num */));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) release_request));

    struct fapi_l1_ue_release_response* release_rsp =
        (struct fapi_l1_ue_release_response*) cphy_sap_receive_mesg ();
    
    TEST_ASSERT_NOT_NULL(release_rsp);
    TEST_ASSERT_EQUAL_INT(FAPI_L1_UE_RELEASE_RESP, release_rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_INVALID_STATE, release_rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *) release_rsp);

    // allocate 
    struct fapi_l1_ue_config_request* config_request = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_ue_config_request (&config_request, UE_CONFIG_TLVS, UE_CONFIG_TLVS_num));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) config_request));

    struct fapi_l1_ue_config_response* config_rsp =
        (struct fapi_l1_ue_config_response*) cphy_sap_receive_mesg ();
    
    TEST_ASSERT_NOT_NULL(config_rsp);
    TEST_ASSERT_EQUAL_INT(FAPI_L1_UE_CONFIG_RESP, config_rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, config_rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *) config_rsp);

    // release
    // release not allocated
    release_request = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_ue_release_request (&release_request,
                                                       UE_CONFIG_TLVS, 2 /* UE_CONFIG_TLVS_num */));
    
    cphy_sap_send_mesg((uint8_t*) release_request);

    release_rsp =
        (struct fapi_l1_ue_release_response*) cphy_sap_receive_mesg ();
    
    TEST_ASSERT_NOT_NULL(release_rsp);
    TEST_ASSERT_EQUAL_INT(FAPI_L1_UE_RELEASE_RESP, release_rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, release_rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *) release_rsp);

}

/*embunit:impl=- */
TestRef ue_messages_test_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testL1_UE_CONFIG",testL1_UE_CONFIG),
		new_TestFixture("testL1_UE_CONFIG_PARAMETERS",testL1_UE_CONFIG_PARAMETERS),
		new_TestFixture("testL1_UE_RELEASE",testL1_UE_RELEASE),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(ue_messages_test,"ue_messages_test",setUp,tearDown,fixtures);
	return (TestRef)&ue_messages_test;
};

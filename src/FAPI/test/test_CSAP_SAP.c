#include <embUnit/embUnit.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

#include <fapi_cl.h>
#include <fapi_cl_conv.h>
#include <fapi_cl_cphy_sap.h>

#include <api_storage.h>

static void setUp(void)
{
    /* initialize */
    cl_init ();
    lte_cphy_sap_init ();    
}

static void tearDown(void)
{
	/* terminate */
}

/*embunit:impl=+ */
static char msg_buff_request[0x1000];
static char msg_buff_response[0x1000];

static void test_PARAM_REQ(void)
{
    fapi_l1_msg_hdr_t * request = (fapi_l1_msg_hdr_t *) msg_buff_request;
    fapi_l1_msg_hdr_t * response = (fapi_l1_msg_hdr_t *) msg_buff_response;
    
    l1_alloc_param_request((struct fapi_l1_param_request*)request);

    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));
    
    TEST_ASSERT_EQUAL_INT(FAPI_L1_PARAM_RESP, response->msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, ((struct fapi_l1_param_response*)response)->errorCode);

    lte_cphy_set_state (PHY_STATE_RUNNING);

    TEST_ASSERT_EQUAL_INT(0,
                          lte_cphy_request((fapi_l1_msg_hdr_t *)request,
                                           (fapi_l1_msg_hdr_t *)response));

    // check state
    TEST_ASSERT_EQUAL_INT(FAPI_L1_PARAM_RESP, response->msgType);
    TEST_ASSERT_EQUAL_INT(MSG_INVALID_STATE,
                          ((struct fapi_l1_param_response*)response)->errorCode);

}

static void testSAP_STATE(void)
{
    fapi_l1_msg_hdr_t * request = (fapi_l1_msg_hdr_t *) msg_buff_request;
    fapi_l1_msg_hdr_t * response = (fapi_l1_msg_hdr_t *) msg_buff_response;
    
    // <IDLE>
    TEST_ASSERT_EQUAL_INT(PHY_STATE_IDLE, lte_cphy_get_state());

    // get config
    l1_alloc_param_request((struct fapi_l1_param_request*)request);    
    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));

    l1_tlv_word_t * rx_antenas =
        find_tlv (FAPI_L1_RxAntennaPorts,
                  ((struct fapi_l1_param_response*)response)->tlvs,
                  ((struct fapi_l1_param_response*)response)->numberOfTLVs);
    TEST_ASSERT_NOT_NULL (rx_antenas);
    
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    
    TEST_ASSERT_EQUAL_INT(phy_config->RxAntennaPorts,
                          rx_antenas->val.word);

    // reconfigure
    uint16_t new_rx_antena = phy_config->RxAntennaPorts == 1 ? 2 : 1;
    rx_antenas->val.word = new_rx_antena;
    l1_alloc_config_request((struct fapi_l1_config_request*)request,
                            ((struct fapi_l1_param_response*)response)->tlvs,
                            ((struct fapi_l1_param_response*)response)->numberOfTLVs);

    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));
    
    TEST_ASSERT_EQUAL_INT(MSG_OK,
                          ((struct fapi_l1_config_response*)response)->errorCode);

    // <CONFIGURED>
    TEST_ASSERT_EQUAL_INT(PHY_STATE_CONFIGURED, lte_cphy_get_state());

    // check configuration
    l1_alloc_param_request((struct fapi_l1_param_request*)request);    
    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));

    rx_antenas = find_tlv (FAPI_L1_RxAntennaPorts,
                           ((struct fapi_l1_param_response*)response)->tlvs,
                           ((struct fapi_l1_param_response*)response)->numberOfTLVs);
    TEST_ASSERT_NOT_NULL (rx_antenas);
    TEST_ASSERT_EQUAL_INT(new_rx_antena, rx_antenas->val.word);

    // start phy
    response->msgType = FAPI_L1_MSG_INVALID;
    l1_alloc_start_request((struct fapi_l1_start_request*)request);    
    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));

    TEST_ASSERT_EQUAL_INT(FAPI_L1_MSG_INVALID, response->msgType);
    lte_cphy_set_state (PHY_STATE_RUNNING); // fixup for asyn message 
    // <RUNNING>
    TEST_ASSERT_EQUAL_INT(PHY_STATE_RUNNING, lte_cphy_get_state());

    l1_sfn_sf_t sfn_sf; sfn_sf.sfn_sf = 40 << 4 | 2; // need to be normalized
    l1_tlv_word_t CONFIG_REQUEST_TLVS [] = {
        {FAPI_L1_SFN_SF, 2, {40 << 4 | 2}}
    };

    l1_alloc_config_request((struct fapi_l1_config_request*)request,
                            CONFIG_REQUEST_TLVS, 1);

    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));
    
    TEST_ASSERT_EQUAL_INT(MSG_OK,
                          ((struct fapi_l1_config_response*)response)->errorCode);


    l1_sfn_sf_t new_sfn_sf;
    new_sfn_sf.sfn_sf = cl_get_sfnsf();

    CLDBG(">> sfn: %d|%d", sfn_sf.val.sf, sfn_sf.val.sfn);

    TEST_ASSERT_EQUAL_INT(sfn_sf.sfn_sf, new_sfn_sf.sfn_sf);

    
    // stop phy
    l1_alloc_stop_request((struct fapi_l1_stop_request*)request);    
    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request(request, response));
    
    TEST_ASSERT_EQUAL_INT(FAPI_L1_STOP_IND, response->msgType);

    TEST_ASSERT_EQUAL_INT(PHY_STATE_CONFIGURED, lte_cphy_get_state());
    
}

static void test_CL_SFN_SF(void)
{
    l1_sfn_sf_t sfn_sf, new_sfn_sf;
    sfn_sf.sfn_sf = 10 << 3 | 3;
    
    cl_set_sfnsf(sfn_sf.sfn_sf);    
    new_sfn_sf.sfn_sf = cl_increment_sfnsf(0);
    
    cl_increment_sfnsf(0);
    
    TEST_ASSERT_EQUAL_INT(new_sfn_sf.sfn_sf, sfn_sf.sfn_sf);
}   


/*embunit:impl=- */
TestRef test_CSAP_SAP_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("test_CL_SFN_SF",test_CL_SFN_SF),
                    new_TestFixture("test_PARAM_REQ",test_PARAM_REQ), 
                    new_TestFixture("testSAP_STATE",testSAP_STATE),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_CSAP_SAP,"test_CSAP_SAP",setUp,tearDown,fixtures);
	return (TestRef)&test_CSAP_SAP;
};

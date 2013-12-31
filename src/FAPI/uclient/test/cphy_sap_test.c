
#include <embUnit/embUnit.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <fapi_cl.h>
#include <fapi_cl_cphy_sap.h>
#include <fapi_cl_ue_db.h>
#include <fapi_cl_conv.h>

#include <LtePhyL2Api.h>
#include <fapi/l1_l2_iface.h>
#include <fapi/l1_tlv.h>
#include <fapi/l1_msg_alloc.h>

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

/*embunit:impl=+ */
extern const size_t phy_capabilities_tlv_number;
static void testInitializationIDLE(void)
{
    //The PARAM message exchange procedure
    struct fapi_l1_param_request* request = 0;
    struct fapi_l1_param_response* rsp = 0;

    // IDLE | The PHY indicates which capabilities it support
    TEST_ASSERT_EQUAL_INT(PHY_STATE_IDLE, lte_cphy_get_state ());
    TEST_ASSERT_EQUAL_INT(0, l1_alloc_param_request (request));

    TEST_ASSERT_EQUAL_INT(0, cphy_sap_send_mesg((uint8_t*) request));

    uint8_t *data = cphy_sap_receive_mesg ();
    TEST_ASSERT_NOT_NULL(data);

    rsp = (struct fapi_l1_param_response*) data;
    TEST_ASSERT_EQUAL_INT(FAPI_L1_PARAM_RESP, rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(phy_capabilities_tlv_number, rsp->numberOfTLVs);

    int i = find_tlv(FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS,
                     rsp->tlvs, rsp->numberOfTLVs);
    TEST_ASSERT(i > 0);
    // power was from default oconfig
    TEST_ASSERT_EQUAL_INT(/* DB_TO_FAPI(1) */0, rsp->tlvs[i].val.word);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *)rsp);
}

static void testInitializationCONFIGURED(void)
{
    //The Configure procedure
    struct fapi_l1_config_request* fapi_conf = 0;
    struct fapi_l1_config_response* conf_rsp = 0;
    {
        l1_alloc_config_request(fapi_conf, CONFIG_REQUEST_TLVS, CONFIG_REQUEST_TLVS_num);

        TEST_ASSERT_EQUAL_INT(0,
                              cphy_sap_send_mesg((uint8_t*) fapi_conf));

        conf_rsp = (struct fapi_l1_config_response*)cphy_sap_receive_mesg ();
        TEST_ASSERT_NOT_NULL(conf_rsp);
        
        TEST_ASSERT_EQUAL_INT(FAPI_L1_CONFIG_RESP, conf_rsp->hdr.msgType);
        
        TEST_ASSERT_EQUAL_INT(MSG_OK, conf_rsp->errorCode);
        l1_msg_free_generic ((fapi_l1_msg_hdr_t *)conf_rsp);
    }

    //The PARAM message exchange procedure
    {
        struct fapi_l1_param_request* param_request = 0;
        struct fapi_l1_param_response* param_rsp = 0;
        
        // CONFIGURED The PHY returns its current configuration
        TEST_ASSERT_EQUAL_INT(PHY_STATE_CONFIGURED, lte_cphy_get_state ());
        TEST_ASSERT_EQUAL_INT(0, l1_alloc_param_request (param_request));

        TEST_ASSERT_EQUAL_INT(0,
                              cphy_sap_send_mesg((uint8_t*) param_request));
        
        param_rsp = (struct fapi_l1_param_response*) cphy_sap_receive_mesg ();;
        TEST_ASSERT_NOT_NULL(param_rsp);
        TEST_ASSERT_EQUAL_INT(FAPI_L1_PARAM_RESP, param_rsp->hdr.msgType);
        
        int i_conf = find_tlv(FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS,
                              CONFIG_REQUEST_TLVS, CONFIG_REQUEST_TLVS_num);
        
        int i_param  = find_tlv(FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS,
                                param_rsp->tlvs, param_rsp->numberOfTLVs);
        
        TEST_ASSERT(i_conf > 0 && i_param > 0);
        // power was from default oconfig
        TEST_ASSERT_EQUAL_INT(CONFIG_REQUEST_TLVS[i_conf].val.word, param_rsp->tlvs[i_param].val.word);
        
        l1_msg_free_generic ((fapi_l1_msg_hdr_t *)param_rsp);
    }
    /*
      in the CONFIGURED state the CONFIG.request message, sent by the L2/L3
      software, may include only the TLVs that are required to change the PHY to a
      new configuration
    */
    {
        fapi_conf = 0;
        l1_alloc_config_request(fapi_conf, CONFIG_REQUEST_TLVS + 3, 3);
        
        TEST_ASSERT_EQUAL_INT(0,
                              cphy_sap_send_mesg((uint8_t*) fapi_conf));

        conf_rsp = (struct fapi_l1_config_response*) cphy_sap_receive_mesg ();
        TEST_ASSERT_NOT_NULL(conf_rsp);
        TEST_ASSERT_EQUAL_INT(FAPI_L1_CONFIG_RESP, conf_rsp->hdr.msgType);
        TEST_ASSERT_EQUAL_INT(MSG_OK, conf_rsp->errorCode);
        l1_msg_free_generic ((fapi_l1_msg_hdr_t *)conf_rsp);
    }
}

static void testInitializationRUNNING(void)
{
    //The PARAM message exchange procedure
    lte_chpy_set_state (PHY_STATE_RUNNING);
    struct fapi_l1_param_request* request = 0;
    struct fapi_l1_param_response* rsp = 0;

    // IDLE | The PHY indicates which capabilities it support
    TEST_ASSERT_EQUAL_INT(PHY_STATE_RUNNING, lte_cphy_get_state ());
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_param_request (request));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) request));

    uint8_t *data = cphy_sap_receive_mesg ();
    TEST_ASSERT_NOT_NULL(data);

    rsp = (struct fapi_l1_param_response*) data;
    TEST_ASSERT_EQUAL_INT(FAPI_L1_PARAM_RESP, rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(rsp->errorCode, MSG_INVALID_STATE);

    // The Configure procedure
    // TLVs permitted in CONFIG.request in the RUNNING state
    struct fapi_l1_config_request* fapi_conf;
    uint16_t fapi_conf_length = sizeof(struct fapi_l1_config_request)
        + 1 * sizeof(l1_tlv_word_t);

    fapi_conf = malloc(fapi_conf_length);

    // gen hdr
    fapi_conf->hdr.msgType = FAPI_L1_CONFIG_REQ;
    fapi_conf->hdr.lengthVendorSpecific = 0;
    fapi_conf->hdr.length = fapi_conf_length;

    fapi_conf->numberOfTLVs = 1;
    fapi_conf->tlvs [0].tag = FAPI_L1_SFN_SF;
    fapi_conf->tlvs [0].len = 2;
    fapi_conf->tlvs [0].val.word = 42;

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) fapi_conf));

    data = cphy_sap_receive_mesg ();
    TEST_ASSERT_NOT_NULL(data);

    struct fapi_l1_config_response* conf_rsp
        = (struct fapi_l1_config_response*) data;
    TEST_ASSERT_EQUAL_INT(FAPI_L1_CONFIG_RESP, conf_rsp->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, conf_rsp->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *)conf_rsp);
}


static void testTermination(void)
{
    /**
       If START.request is successful no response is returned by the PHY. When
       the PHY has started it will send a SUBFRAME indication to the L2/L3
       software
    */
    lte_chpy_set_state (PHY_STATE_CONFIGURED);
    struct fapi_l1_start_request* start_req = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_start_request (start_req));

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) start_req));

    fapi_l1_msg_hdr_t * hdr = (fapi_l1_msg_hdr_t *) cphy_sap_receive_mesg ();
    TEST_ASSERT_NULL(hdr);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *)hdr);

    TEST_ASSERT_EQUAL_INT(PHY_STATE_RUNNING, lte_cphy_get_state ());

    // repeat in running state
    start_req = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_start_request (start_req));
    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) start_req));

    struct fapi_l1_error_indication* error_ind
        = (struct fapi_l1_error_indication*) cphy_sap_receive_mesg ();
    TEST_ASSERT_NOT_NULL(error_ind);

    TEST_ASSERT_EQUAL_INT(FAPI_L1_ERROR_IND, error_ind->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_INVALID_STATE, error_ind->errorCode);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *)error_ind);
}

static void testRestart(void)
{
    struct fapi_l1_stop_request* stop_req = 0;
    TEST_ASSERT_EQUAL_INT(0,
                          l1_alloc_stop_request (stop_req));
    lte_chpy_set_state (PHY_STATE_RUNNING);

    TEST_ASSERT_EQUAL_INT(0,
                          cphy_sap_send_mesg((uint8_t*) stop_req));

    TEST_ASSERT_EQUAL_INT(PHY_STATE_CONFIGURED, lte_cphy_get_state ());

    uint8_t *data = cphy_sap_receive_mesg ();
    TEST_ASSERT_NOT_NULL(data);

    struct fapi_l1_stop_indication* error_ind
        = (struct fapi_l1_stop_indication*) data;
    TEST_ASSERT_EQUAL_INT(FAPI_L1_STOP_IND, error_ind->hdr.msgType);
    l1_msg_free_generic ((fapi_l1_msg_hdr_t *)error_ind);
}

static void testReset(void)
{
      //    TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef cphy_sap_test_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /*embunit:fixtures=+ */
        new_TestFixture("testInitializationIDLE",testInitializationIDLE),
            new_TestFixture("testInitializationCONFIGURED",testInitializationCONFIGURED),
            new_TestFixture("testInitializationRUNNING",testInitializationRUNNING),
            new_TestFixture("testTermination",testTermination),
            new_TestFixture("testRestart",testRestart),
            new_TestFixture("testReset",testReset),
//            /*embunit:fixtures=- */
            };
    EMB_UNIT_TESTCALLER(cphy_sap_test,"cphy_sap_test",setUp,tearDown,fixtures);
    return (TestRef)&cphy_sap_test;
};

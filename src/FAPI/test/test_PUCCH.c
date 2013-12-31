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

extern unsigned char out_siso_0065_PHY_RXSDU_IND_bin[];
extern unsigned int out_siso_0065_PHY_RXSDU_IND_bin_len;


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
    l1_sfn_sf_t sfn_sf = { .val.sfn = 888, .val.sf = 6};
    uint16_t channelId = 0;
    uint32_t Handle = 42;
    uint16_t RNTI = 1024;

    void *cur = go_next_rx_subframe_layer (sfn_sf);

    link_rx_request_info(cur, channelId, Handle, RNTI, UCI_CQI_SR_PDU_TYPE);

    fapi_l1_msg_hdr_t *fapi
        = (fapi_l1_msg_hdr_t *) msgBuff[0];
    struct fapi_l1_rx_cqi_indication *cqi_ind
        = (struct fapi_l1_rx_cqi_indication *) msgBuff[1];
    struct fapi_l1_crc_indication *crc_ind
        = (struct fapi_l1_crc_indication *) msgBuff[2];

    // Read reference trace
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_siso_0065_PHY_RXSDU_IND_bin;

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
    TEST_ASSERT_EQUAL_INT(PUCCH, api->chanType);
        
    TEST_ASSERT_EQUAL_INT(crc_ind_size, crc_ind->hdr.length);
    // SR not detected TEST_ASSERT_EQUAL_INT(fapi_msg_size, fapi->length);

    TEST_ASSERT_EQUAL_INT(cqi_ind_size, cqi_ind->hdr.length);

    struct fapi_l1_rx_cqi_pdu * PDU = cqi_ind->PDUs;

    TEST_ASSERT_EQUAL_INT(Handle, PDU->Handle);
    TEST_ASSERT_EQUAL_INT(RNTI, PDU->RNTI);
}

/*embunit:impl=- */
TestRef test_PUCCH_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
            new_TestFixture("testConvert",testConvert),
                //           new_TestFixture("testSendReceive",testSendReceive),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_PUCCH,"test_PUCCH",setUp,tearDown,fixtures);
	return (TestRef)&test_PUCCH;
};

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

static void setUp(void)
{
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
static void testRxInfo(void)
{
    l1_sfn_sf_t sfn_sf = { .val.sfn = 377, .val.sf = 7};

    // Initialized
    TEST_ASSERT_NOT_NULL(get_current_rx_subframe_layer());

    // move sf 7
    void * next = go_next_rx_subframe_layer (sfn_sf);

    // check if moved
    void * sf7 = get_current_rx_subframe_layer();
    TEST_ASSERT(next == sf7);

    // find sf 7 while staying on it
    void * prev = get_prev_rx_subframe_layer (sfn_sf);
    TEST_ASSERT(next == prev);

    // move sf 8
    l1_sfn_sf_t sfn_sf_prime = {.val.sfn = 377, .val.sf = 8};
    next = go_next_rx_subframe_layer (sfn_sf_prime);
    TEST_ASSERT(next == get_current_rx_subframe_layer());

    // find sf 7, cur is still 7
    prev = get_prev_rx_subframe_layer (sfn_sf);
    TEST_ASSERT(sf7 == prev);

}

uint16_t inc_sfn_sf (l1_sfn_sf_t in, uint16_t incr)
{
    l1_sfn_sf_t ret;
    int8_t k;
    /* positive adjustment
     * requires sf to be wrapped at 10 and sfn to be wrapped at 1024
     */
    ret.sfn_sf = in.sfn_sf;

    k   = (int8_t) ret.val.sf;
    k   += incr;
    if( k > 9 ) {
        ret.val.sf  = k % 10;
        ret.val.sfn = (ret.val.sfn + 1) % 1024;
    } else {
        ret.val.sf = k;
    }

    return ret.sfn_sf;
}

static void testFind(void)
{
    l1_sfn_sf_t sfn_sf = { .val.sfn = 377, .val.sf = 7};
    l1_sfn_sf_t target_sfn_sf;
    int i;

    // Fill a round
    for (i=0; i < 20; i++) {
        sfn_sf.sfn_sf = inc_sfn_sf(sfn_sf, 1);
        void *cur = go_next_rx_subframe_layer (sfn_sf);
        printf("+ %d >> 0x%xh, sfn/sf = %d/%d\n", i, (unsigned)cur, sfn_sf.val.sfn, sfn_sf.val.sf);
    }

    target_sfn_sf.sfn_sf = sfn_sf.sfn_sf;
    void * target_rx_info = get_current_rx_subframe_layer();
    TEST_ASSERT(target_rx_info == get_prev_rx_subframe_layer (target_sfn_sf));

    printf("->> 0x%xh, sfn/sf = %d/%d\n", (unsigned)target_rx_info,
           target_sfn_sf.val.sfn, target_sfn_sf.val.sf);

    for (i = 0; i < 10; i++) {
        sfn_sf.sfn_sf = inc_sfn_sf(sfn_sf, 1);
        void * next = go_next_rx_subframe_layer (sfn_sf);
        if (next == target_rx_info) {
            TEST_ASSERT_EQUAL_INT(i, 8); // 9 - number of infos
            break;
        }

        void * prev = get_prev_rx_subframe_layer (target_sfn_sf);

        printf("%d>> p:0x%xh -> n:0x%xh, sfn/sf = %d/%d\n", i,
               (unsigned)prev, (unsigned)next,
               sfn_sf.val.sfn, sfn_sf.val.sf);

        TEST_ASSERT (target_rx_info == prev);
    }

}

static void testMapUnmap(void)
{
    l1_sfn_sf_t sfn_sf = { .val.sfn = 377, .val.sf = 7};
    uint16_t channelId = 12;
    
    void *cur = go_next_rx_subframe_layer (sfn_sf);

    uint32_t Handle = 42, HandleOut;
    uint16_t RNTI = 1024, RNTI_Out;

    // register
    link_rx_request_info(cur, channelId, Handle, RNTI + ULSCH_PDU_TYPE, ULSCH_PDU_TYPE);
    link_rx_request_info(cur, channelId, Handle, RNTI + UCI_CQI_SR_PDU_TYPE, UCI_CQI_SR_PDU_TYPE);
    link_rx_request_info(cur, channelId, Handle, RNTI + SRS_PDU_TYPE, SRS_PDU_TYPE);


    void * prev = get_prev_rx_subframe_layer (sfn_sf);

    // unlink first 
    unlink_rx_request_info (prev, channelId,
                            &HandleOut, &RNTI_Out, ULSCH_PDU_TYPE);
    TEST_ASSERT_EQUAL_INT(Handle, HandleOut);
    TEST_ASSERT_EQUAL_INT(RNTI + ULSCH_PDU_TYPE, RNTI_Out);

    
    // unlink last 
    unlink_rx_request_info (prev, channelId,
                            &HandleOut, &RNTI_Out, SRS_PDU_TYPE);
    TEST_ASSERT_EQUAL_INT(Handle, HandleOut);
    TEST_ASSERT_EQUAL_INT(RNTI + SRS_PDU_TYPE, RNTI_Out);

}

/*embunit:impl=- */
TestRef test_HandleMap_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testFind",testFind),
		new_TestFixture("testRxInfo",testRxInfo),
		new_TestFixture("testMapUnmap",testMapUnmap),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_HandleMap,"test_HandleMap",setUp,tearDown,fixtures);
	return (TestRef)&test_HandleMap;
};

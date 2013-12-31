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

#include "api_storage.h"

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
    lte_cphy_sap_init ();
    cl_ue_db_init ();

    /* initialize */
    lte_cphy_set_state (PHY_STATE_IDLE);

}

static void tearDown(void)
{
    /* terminate */
}

/*embunit:impl=+ */
static void testFAPI_L1_RX_ULSCH_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_HARQ_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_CRC_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_RX_SR_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_RX_CQI_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_RACH_IND(void)
{
	TEST_FAIL("no implementation");
}

static void testFAPI_L1_SRS_IND(void)
{
	TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef uplink_data_test_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testFAPI_L1_RX_ULSCH_IND",testFAPI_L1_RX_ULSCH_IND),
		new_TestFixture("testFAPI_L1_HARQ_IND",testFAPI_L1_HARQ_IND),
		new_TestFixture("testFAPI_L1_CRC_IND",testFAPI_L1_CRC_IND),
		new_TestFixture("testFAPI_L1_RX_SR_IND",testFAPI_L1_RX_SR_IND),
		new_TestFixture("testFAPI_L1_RX_CQI_IND",testFAPI_L1_RX_CQI_IND),
		new_TestFixture("testFAPI_L1_RACH_IND",testFAPI_L1_RACH_IND),
		new_TestFixture("testFAPI_L1_SRS_IND",testFAPI_L1_SRS_IND),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(uplink_data_test,"uplink_data_test",setUp,tearDown,fixtures);
	return (TestRef)&uplink_data_test;
};

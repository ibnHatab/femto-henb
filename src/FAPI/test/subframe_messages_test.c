#include <embUnit/embUnit.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <fapi_cl.h>
#include <lte_cphy_sap.h>
#include <fapi_cl_ue_db.h>

#include <LtePhyL2Api.h>
#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

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
    lte_cphy_sap_init ();
    cl_ue_db_init ();

    
    /* initialize */
    lte_cphy_set_state (PHY_STATE_IDLE);
    cphy_sap_user_init ();
}

static void tearDown(void)
{
    /* terminate */
    if (l1_msg_alloc_count) {
        printf(">>phy_state_testset_tests alloc count: %d\n", l1_msg_alloc_count);
    }

}

#if EXAMPLE
pPhyConfig->PucchConfig.DeltaPucchShift = pUlCxt->ulSfrCtrl.deltaPUCCHShift;
pPhyConfig->PucchConfig.nCqiRb = pUlCxt->ulSfrCtrl.nRBCQI  ;
pPhyConfig->PucchConfig.nAnCs = pUlCxt->ulSfrCtrl.nCSAn  ;
pPhyConfig->PucchConfig.n1PucchAn = pUlCxt->ulSfrCtrl.n1PucchAN  ;

pPhyConfig->SrsInit.srsBandwidthConfig = pUlCxt->ulSfrCtrl.srsBandwitdhConfig  ;
pPhyConfig->SrsInit.srsSubframeConfig = pUlCxt->ulSfrCtrl.srsSubframeConfig  ;
pPhyConfig->SrsInit.srsSimultaneousANandSRS = pUlCxt->ulSfrCtrl.ackNackSRSSimultaneousTransmission ;

pPhyConfig->PuschConfig.NumberOfSubBands = pUlCxt->ulSfrCtrl.nSB;
pPhyConfig->PuschConfig.HoppingMode = pUlCxt->ulSfrCtrl.hoppingMode;
pPhyConfig->PuschConfig.HoppingOffset = pUlCxt->ulSfrCtrl.puschhoppingOffset;

pPhyConfig->UlRefSignalConfig.UlRsHopping = pUlCxt->ulSfrCtrl.groupHoppingEnabled;
pPhyConfig->UlRefSignalConfig.GroupAssignment = pUlCxt->ulSfrCtrl.groupAssignmentPUSCH;
pPhyConfig->UlRefSignalConfig.UlRsHopping = pUlCxt->ulSfrCtrl.sequenceHoppingEnabled;
pPhyConfig->UlRefSignalConfig.CyclicShiftFroDmrs = pUlCxt->ulSfrCtrl.cyclicShift;
#endif

/*embunit:impl=+ */
static void testFAPI_L1_SUBFRAME_IND(void)
{
      //    TEST_FAIL("no implementation");
} 

static void testFAPI_L1_DL_CONFIG_REQ(void)
{
    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) out_NN_0018_PHY_TXSTART_REQ_bin;
    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, apiLogInfo->Message);

    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, genhdr->msgType);
    
    PDLSUBFRDESC dlSubframeDescriptor = (PDLSUBFRDESC) &genhdr[1];

    //    TEST_ASSERT_EQUAL_BIN(apiInitRef, &apiInit, sizeof(INITPARM));

}

static void testFAPI_L1_UL_CONFIG_REQ(void)
{
      //	TEST_FAIL("no implementation");
}

static void testFAPI_L1_HI_DCI0_REQ(void)
{
    //TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef subframe_messages_test_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testFAPI_L1_SUBFRAME_IND",testFAPI_L1_SUBFRAME_IND),
		new_TestFixture("testFAPI_L1_DL_CONFIG_REQ",testFAPI_L1_DL_CONFIG_REQ),
		new_TestFixture("testFAPI_L1_UL_CONFIG_REQ",testFAPI_L1_UL_CONFIG_REQ),
		new_TestFixture("testFAPI_L1_HI_DCI0_REQ",testFAPI_L1_HI_DCI0_REQ),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(subframe_messages_test,"subframe_messages_test",setUp,tearDown,fixtures);
	return (TestRef)&subframe_messages_test;
};

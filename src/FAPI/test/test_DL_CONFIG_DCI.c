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

extern unsigned char out_mimo_0032_PHY_TXSTART_REQ_bin[];
extern unsigned int out_mimo_0032_PHY_TXSTART_REQ_bin_len;


static void setUp(void)
{
    /* initialize */
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

/*embunit:impl=+ */
static void test_DL_CONFIG_DCI_DL(struct fapi_l1_dl_config_request * fapi,
                                  unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PDLSUBFRDESC apiTX_v = (PDLSUBFRDESC) alloc_api_storage (sizeof(DLSUBFRDESC), &storage);;
    size_t msgSize = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_dlSubframeDescriptor_from_DL_CONFIG_request (
                              fapi, apiTX_v, &msgSize));

    save_api_fragment (PHY_TXSTART_REQ, storage, "/tmp/dci.api", msgSize);


    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    PDLSUBFRDESC apiTX_v_Ref = (PDLSUBFRDESC) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, genhdr->msgType);

    
    PDCICHANNELDESC dci_DL_Ref = (PDCICHANNELDESC)
        ((U8*)apiTX_v_Ref + apiTX_v_Ref->offsetDCIChannels);
    PDCICHANNELDESC dci_DL = (PDCICHANNELDESC)
        ((U8*)apiTX_v + apiTX_v->offsetDCIChannels);

    // FIXUP MIMO DCI channel
    dci_DL_Ref->offsetNextCh = 4;
    dci_DL->channelId = 0;
    dci_DL->txpowerControl = 0;
    

    TEST_ASSERT_EQUAL_BIN(dci_DL_Ref, dci_DL, sizeof(DCICHANNELDESC));

    // FIXUP HEADER
    apiTX_v_Ref->offsetPowerCtrl = apiTX_v->offsetPowerCtrl = 0;
    apiTX_v_Ref->offsetDCIChannels = apiTX_v->offsetDCIChannels = 0;
    apiTX_v_Ref->offsetCustomFeatures = apiTX_v->offsetCustomFeatures = 0;

    TEST_ASSERT_EQUAL_BIN(apiTX_v_Ref, apiTX_v, offsetof(DLSUBFRDESC, dlCh));

    

}


static void testDCI_MIMO(void)
{
    uint8_t msg_buff[BUFSIZ];

    // (A) Configure PHY
    send_CONFIG_REQUEST(MIMO_CONFIG_REQUEST_TLVS,
                        MIMO_CONFIG_REQUEST_TLVS_num);

   // (B) Check DL CH config
   struct fapi_l1_dl_config_request dl_config_data = {
       .SFN_SF = INVALID_SFNSF,
       .Length = 0,
       .NumberOf_PDCCH_OFDM_Symbols = 2,
       .NumberOf_DCIs = 0,
       .NumberOf_PDUs = 0,
       .NumberOf_PDSCH_RNTIs = 0,
       .TransmissionPowerFor_PCFICH = 0,
   };

   // 0: DCI DL PDU, see Section 3.3.1.2.1.
   struct fapi_l1_dci_dl_pdu dci_dl_pdu_data = {
       .DCI_Format = DCI_2A,
       .CCE_Index = 88,
       .AggregationLevel = 8,
       .RNTI = 0xFFFF,
       .ResourceAllocationType = 1,
       .VirtualResourceBlockAssignmentFlag = 1,
       .ResourceBlockCoding = 1,
       .MCS_1 = 31,
       .RedundancyVersion_1 = 3,
       .NewDataIndicator_1 = 1,
       .TransportBlockToCodewordSwapFlag = 1,
       .MCS_2 = 31,
       .RedundancyVersion_2 = 1,
       .NewDataIndicator_2 = 0,
       .HARQ_Process = 15,
       .TPMI = 3,
       .PMI = 1,
       .PrecodingInformation = 7,
       .TPC = 3,
       .DownlinkAssignmentIndex = 4,
       .N_GAP = N_GAP1,
       .TransportBlockSizeIndex = 31,
       .DownlinkPowerOffset = 1,
       .Allocate_PRACH_Flag = 1,
       .PreambleIndex  = 63,
       .PRACH_MaskIndex = 15,
       .RNTI_type = 1,
       .TransmissionPower = 10000
   };

   struct fapi_l1_bch_pdu bch_pdu_data = {
           .Length = 0,
           .PDU_Index = 1,
           .TransmissionPower = 0
   };

   struct fapi_l1_dl_config_request  *dl_conf_dci_dl =
       of_dl_conf(728 << 4 | 5,
                  dl_conf_add_pdu((uint8_t*) &bch_pdu_data, BCH_PDU_TYPE_SIZE, // fillin
                  dl_conf_add_pdu((uint8_t*) &dci_dl_pdu_data, DCI_DL_PDU_TYPE_SIZE,
                                  format_dl_conf (msg_buff, &dl_config_data))));

   test_DL_CONFIG_DCI_DL(dl_conf_dci_dl,
                         out_mimo_0032_PHY_TXSTART_REQ_bin,
                         out_mimo_0032_PHY_TXSTART_REQ_bin_len);


}

static void testDCI_SISO(void)
{
      //	TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef test_DL_CONFIG_DCI_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testDCI_MIMO",testDCI_MIMO),
		new_TestFixture("testDCI_SISO",testDCI_SISO),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_DL_CONFIG_DCI,"test_DL_CONFIG_DCI",setUp,tearDown,fixtures);
	return (TestRef)&test_DL_CONFIG_DCI;
};

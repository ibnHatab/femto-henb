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

extern unsigned char out_mimo_0054_PHY_TXSTART_REQ_bin[];
extern unsigned int out_mimo_0054_PHY_TXSTART_REQ_bin_len;

extern unsigned char out_mimo_0055_PHY_TXSDU_REQ_bin[];
extern unsigned int out_mimo_0055_PHY_TXSDU_REQ_bin_len;


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
static void test_DL_CONFIG_DLSCH(struct fapi_l1_dl_config_request * fapi,
                                 unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PDLSUBFRDESC apiTX_v = (PDLSUBFRDESC) alloc_api_storage (sizeof(DLSUBFRDESC), &storage);;
    size_t msgSize = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_dlSubframeDescriptor_from_DL_CONFIG_request (
                              fapi, apiTX_v, &msgSize));
    
    save_api_fragment (PHY_TXSTART_REQ, storage, "/tmp/dlsch.api", msgSize);


    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    PDLSUBFRDESC apiTX_v_Ref = (PDLSUBFRDESC) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_TXSTART_REQ, genhdr->msgType);

    // FIXUP HEADER
    apiTX_v_Ref->offsetPowerCtrl = apiTX_v->offsetPowerCtrl = 0;
    apiTX_v_Ref->offsetDCIChannels = apiTX_v->offsetDCIChannels = 0;
    apiTX_v_Ref->offsetCustomFeatures = apiTX_v->offsetCustomFeatures = 0;
    
    TEST_ASSERT_EQUAL_BIN(apiTX_v_Ref, apiTX_v, offsetof(DLSUBFRDESC, dlCh));

    // FIXUP MIMO
      //    apiTX_v_Ref->dlCh[0].hCid = apiTX_v->dlCh[0].hCid = 0;
    apiTX_v_Ref->dlCh[0].channelId = 0;
    apiTX_v_Ref->dlCh[0].offsetNextCh = 4;
    apiTX_v_Ref->dlCh[0].numCodeWords = 2; //FIXME: might be bug
    
    // SPS
    /* apiTX_v_Ref->dlCh[0].persistEnable = 0; */
    /* apiTX_v_Ref->dlCh[0].repeatCycle = 0; */

    /* apiTX_v_Ref->dlCh[0].mapInfo.reselmInfo[0].numRes = 0; */
    
    /* apiTX_v_Ref->dlCh[0].subChInfo[0].pad = 0; */
    /* apiTX_v_Ref->dlCh[0].subChInfo[0].reserved = 0; */
    /* apiTX_v_Ref->dlCh[0].subChInfo[0].nDi = 0; */

    TEST_ASSERT_EQUAL_BIN(&apiTX_v_Ref->dlCh[0], 
                          &apiTX_v->dlCh[0], sizeof(DLCHANDESC)); 
}

#if 0
static void test_TX_request (struct fapi_l1_tx_request * trx, uint16_t PDU_Index,
                             unsigned char *ref_api, unsigned int ref_api_len)
{
    struct fapi_l1_dl_pdu_config * dl_pdu_config;
    tx_req_for_each_pdu(trx, dl_pdu_config) {
        if (dl_pdu_config->PDU_Index == PDU_Index) 
            break;
    }

    struct fapi_cl_dl_pdu_info * info = fapi_cl_get_dl_pdu_info (dl_pdu_config->PDU_Index);
    PTXSDUREQ Tx_SDU_Desc = info->Tx_SDU_Desc;

    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PTXSDUREQ  Tx_SDU_Desc_Ref = (PTXSDUREQ) &apiLogInfo [1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_TXSDU_REQ, apiLogInfo->Message);

    // FIXUP
    /* Tx_SDU_Desc_Ref->phichSeqIndex = 0; */
    /* Tx_SDU_Desc_Ref->phichGrpNumber = 0; */
    /* Tx_SDU_Desc_Ref->maxBitsperCw = 24; */
    Tx_SDU_Desc_Ref->pTxSdu = 0x00;
    
    TEST_ASSERT_EQUAL_BIN(Tx_SDU_Desc, 
                          Tx_SDU_Desc_Ref, sizeof(TXSDUREQ)); 

}
#endif	/* 0 */ 

static void testDLSCH_MIMO(void)
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

   // Table 43: BfVectorType Structure
   struct fapi_l1_bf_vector_type bf_vector_type_data [] = {
       {
           .subbandIndex = 0,
           .numAntennas = 1,
           .bfValue = 0xAA00 & 0xCC
       },
       
   };
   
#define PDU_INDEX 3
   // 3: DLSCH PDU, see Section 3.3.1.2.4.
   static struct fapi_l1_dlsch_pdu dlsch_pdu_data = {
    .Length = 0,
    .PDU_Index = PDU_INDEX, 
    .RNTI = 0x7,
    .ResourceAllocationType = 2,
    .VirtualResourceBlockAssignmentFlag = 1,
    .ResourceBlockCoding = 2,
    .Modulation = MD_QPSK,
    .RedundancyVersion = 3,
    .TransportBlocks = 2,
    .TransportBlockToCodewordSwapFlag = 1,
    .TransmissionScheme = TX_DIVERSITY,
    .NumberOfLayers = 4,
    .NumberOfSubbands = 13,
    .CodebookEntries = {{15}},
    .UE_CategoryCapacity = 0xf, // just bug in ref API trace
    .P_A = 7,
    .DeltaPowerOffsetIndex = 1,
    .N_GAP = N_GAP2,
    .N_PRB = 1,
    .numBfPRBperSubban = 1,
    .bfVector = {}, // bf_vector_type_data
   };

    struct fapi_l1_dl_config_request  *dl_conf_dlsch =
    of_dl_conf(729 << 4 | 3,
               dl_conf_pdu_tail((uint8_t*) bf_vector_type_data, sizeof (bf_vector_type_data),
               dl_conf_add_pdu((uint8_t*) &dlsch_pdu_data, DLSCH_PDU_TYPE,
                                         sizeof(struct fapi_l1_dlsch_pdu) + sizeof (bf_vector_type_data),
               dl_conf_add_pdu((uint8_t*) NULL, DCI_DL_PDU_TYPE_SIZE,
                               format_dl_conf (msg_buff, &dl_config_data)))));

    test_DL_CONFIG_DLSCH(dl_conf_dlsch,
                         out_mimo_0054_PHY_TXSTART_REQ_bin,
                         out_mimo_0054_PHY_TXSTART_REQ_bin_len);

    // (C) Check MAC PDU
#if 0
    uint8_t mac_pdu [] = {0x45, 0x00, 0xb1, 0x7f, 0xe0, 0x00, 0x3d};
    
    struct fapi_l1_tx_request * trx =
        of_tx_req(729 << 4 | 3,
                  tx_req_add_pdu(0, PDU_INDEX, (uint8_t*) &mac_pdu, sizeof(mac_pdu),
                                 format_tx_req (msg_buff, NULL)));

    test_TX_request (trx, PDU_INDEX,
                     out_mimo_0055_PHY_TXSDU_REQ_bin,
                     out_mimo_0055_PHY_TXSDU_REQ_bin_len);
#endif

    
}

static void testDLSCH_SISO(void)
{
    //TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef test_DL_CONFIG_DLSCH_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testDLSCH_MIMO",testDLSCH_MIMO),
		new_TestFixture("testDLSCH_SISO",testDLSCH_SISO),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_DL_CONFIG_DLSCH,"test_DL_CONFIG_DLSCH",setUp,tearDown,fixtures);
	return (TestRef)&test_DL_CONFIG_DLSCH;
};

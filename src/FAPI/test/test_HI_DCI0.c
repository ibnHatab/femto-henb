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

extern unsigned char out_siso_0364_PHY_TXHIADCIUL_REQ_bin[];
extern unsigned int out_siso_0364_PHY_TXHIADCIUL_REQ_bin_len;

extern unsigned char out_siso_0389_PHY_TXHISDU_REQ_bin [];
extern unsigned int out_siso_0389_PHY_TXHISDU_REQ_bin_len;

extern unsigned char out_siso_0365_PHY_TXDCIULSDU_REQ_bin [];
extern unsigned int out_siso_0365_PHY_TXDCIULSDU_REQ_bin_len;


#if 0
#define PHY_TXHIADCIUL_REQ  22
#define PHY_TXHISDU_REQ     23
#define PHY_TXDCIULSDU_REQ  24
#endif

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


static uint8_t * readRefAPI(int msgType, unsigned char *ref_api, unsigned int ref_api_len)
{
    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;

    if (msgType != apiLogInfo->Message) return 0;

    uint8_t * apiRX_v_Ref = (uint8_t *) &apiLogInfo [1];

    return apiRX_v_Ref;
}

void testHI_AND_DCI (struct fapi_l1_hi_dci0_request * fapi,
                     unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PHIADCIULMSGDESC apiRX_v = (PHIADCIULMSGDESC) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);

    TEST_ASSERT_EQUAL_INT(0,
                          conv_HiandDciUlMessageDescriptor_from_HI_DCI0_request (
                              fapi, apiRX_v));

    PHIADCIULMSGDESC apiRX_v_Ref = (PHIADCIULMSGDESC)readRefAPI (PHY_TXHIADCIUL_REQ, ref_api, ref_api_len);
    apiRX_v_Ref->numHiSdus = apiRX_v_Ref->numDciUlSdus = 1;

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, sizeof(HIADCIULMSGDESC));
}

void testHI_PDU (struct DCI_HI_PDU_Configuration * pdu,
                 unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PHIINFOMSGDESC apiRX_v = (PHIINFOMSGDESC) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);
    //size_t msgSize = sizeof(HIADCIULMSGDESC) + 8;

    TEST_ASSERT_EQUAL_INT(0,
                          conv_HiInfoDescriptor_from_HI_PDU (pdu, apiRX_v));

    PHIINFOMSGDESC apiRX_v_Ref = (PHIINFOMSGDESC)readRefAPI (PHY_TXHISDU_REQ, ref_api, ref_api_len);
      //    apiRX_v_Ref->numHiSdus = apiRX_v_Ref->numDciUlSdus = 1;

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, sizeof(HIINFOMSGDESC));
}

void testDCI_PDU (struct DCI_HI_PDU_Configuration * pdu,
                  unsigned char *ref_api, unsigned int ref_api_len)
{
    static STORAGE_HEADER *storage = 0;

    PDCIULSDUMSG apiRX_v = (PDCIULSDUMSG) alloc_api_storage (sizeof(ULSUBFRDESC), &storage);
    U32 msgSize = sizeof(DCIULSDUMSG) + 8;
    
    TEST_ASSERT_EQUAL_INT(0,
                          conv_DciUlSduHeaderDescriptor_from_DCI_UL_PDU (pdu, apiRX_v, &msgSize));

    PDCIULSDUMSG apiRX_v_Ref = (PDCIULSDUMSG)readRefAPI (PHY_TXDCIULSDU_REQ, ref_api, ref_api_len);
      //    apiRX_v_Ref->numHiSdus = apiRX_v_Ref->numDciUlSdus = 1;

    TEST_ASSERT_EQUAL_BIN(apiRX_v_Ref, apiRX_v, sizeof(DCIULSDUMSG));
}


/*embunit:impl=+ */
static void testHI_DCI0(void)
{
    uint8_t msg_buff[BUFSIZ];

    // (A) Configure PHY
    lte_cphy_set_state (PHY_STATE_IDLE);

    send_CONFIG_REQUEST(SISO_CONFIG_REQUEST_TLVS,
                        SISO_CONFIG_REQUEST_TLVS_num);

    struct fapi_l1_hi_pdu fapi_l1_hi_pdu_data = {
        .ResourceBlockStart = 0,
        .CyclicShift2For_DMRS = 3,
        .HI_Value = HI_ACK,
        .I_PHICH = 0,
        .TransmissionPower = 0,
    };

    struct fapi_l1_dci_ul_pdu fapi_l1_dci_ul_pdu_data = {
        .DCI_Format = 2,
        .CCE_Index = 0x10,
        .AggregationLevel = 2,
        .RNTI = 0x33,
        .ResourceBlockStart = 100,
        .NumberOfResourceBlocks = 100,
        .MCS = 31,
        .CyclicShift2For_DMRS = 7,
        .FrequencyEnabledFlag = 1,
        .FrequencyHoppingBits = 3,
        .NewDataIndication = 1,
        .UE_TX_AntennaSelection = 2,
        .TPC = 0,
        .CQI_Request = 1,
        .UL_Index = 3,
        .DL_AssignmentIndex = 4,
        .TPC_Bitmap = 0,
    };

    static struct fapi_l1_hi_dci0_request fapi_l1_hi_dci0_request_data = {
        .SFN_SF = 0,
        .NumberOf_DCI = 0,
        .NumberOf_HI = 0,
        .DCI_HI_PDU_Configuration = {}
    };

    struct fapi_l1_hi_dci0_request * req =
        of_hi_dci0_req(899 << 4 | 4,
        hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_hi_pdu_data, HI_PDU_TYPE_SIZE,
        hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_dci_ul_pdu_data, DCI_UL_PDU_TYPE_SIZE,
                            format_hi_dci0_req (msg_buff, &fapi_l1_hi_dci0_request_data))));

    TEST_ASSERT_EQUAL_INT(1, req->NumberOf_HI);
    TEST_ASSERT_EQUAL_INT(1, req->NumberOf_DCI);

    testHI_AND_DCI(req, out_siso_0364_PHY_TXHIADCIUL_REQ_bin,
                   out_siso_0364_PHY_TXHIADCIUL_REQ_bin_len);

    struct DCI_HI_PDU_Configuration * ul_pdu;
    int i;
    hi_dci0_req_for_each_pdu (i, req, ul_pdu) {

        switch (ul_pdu->PDU_Type) {
        case HI_PDU_TYPE:
            testHI_PDU(ul_pdu, out_siso_0389_PHY_TXHISDU_REQ_bin,
                       out_siso_0389_PHY_TXHISDU_REQ_bin_len);
            break;
        case DCI_UL_PDU_TYPE:
            testDCI_PDU(ul_pdu, out_siso_0365_PHY_TXDCIULSDU_REQ_bin,
                        out_siso_0365_PHY_TXDCIULSDU_REQ_bin_len);
            break;
        default:
            TEST_ASSERT(0);
        }
    }

}


/*embunit:impl=- */
TestRef test_HI_DCI0_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testHI_DCI0",testHI_DCI0),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_HI_DCI0,"test_HI_DCI0",setUp,tearDown,fixtures);
	return (TestRef)&test_HI_DCI0;
};

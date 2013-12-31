/*embunit:include=+ */
#include <embUnit/embUnit.h>
/*embunit:include=- */


#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>

#include <fapi_cl.h>

#include <l1_sdu_combinator.h>

static void setUp(void)
{
    /* initialize */
}

static void tearDown(void)
{
    /* terminate */

}

/*embunit:impl=+ */
#include "sdu_combinator_data.hh"

static void testCheckSize(void)
{
    uint8_t msg_buff [BUFSIZ];

    struct fapi_l1_dl_config_request * cfr =
        format_dl_conf (msg_buff, &dl_bch_config);

    cfr = dl_conf_add_pdu((uint8_t*) &bch_pdu, BCH_PDU_TYPE_SIZE, cfr);

    cfr = of_dl_conf (728 << 4, cfr);

    int expected = offsetof (struct fapi_l1_dl_config_request, DL_PDU_Configuration)
        +  offsetof (struct DL_PDU_Configuration, DL_PDU)
        + sizeof(struct fapi_l1_bch_pdu);
    TEST_ASSERT_EQUAL_INT (expected, cfr->hdr.length);

    cfr = of_dl_conf(728 << 4,
                     dl_conf_add_pdu((uint8_t*) &bch_pdu, BCH_PDU_TYPE_SIZE,
                     dl_conf_add_pdu((uint8_t*) &bch_pdu, BCH_PDU_TYPE_SIZE,
                                                     format_dl_conf (msg_buff, &dl_bch_config))));
    
    expected = offsetof (struct fapi_l1_dl_config_request, DL_PDU_Configuration)
        +  2 * (offsetof (struct DL_PDU_Configuration, DL_PDU)
                + sizeof(struct fapi_l1_bch_pdu));
    TEST_ASSERT_EQUAL_INT (expected, cfr->hdr.length);
    TEST_ASSERT_EQUAL_INT (2, cfr->NumberOf_PDUs);
    TEST_ASSERT_EQUAL_INT (expected - offsetof (struct fapi_l1_dl_config_request, DL_PDU_Configuration),
                           cfr->Length);

}


static void testCheckOffset(void)
{
    uint8_t msg_buff [BUFSIZ];

    struct fapi_l1_dl_config_request * cfr =
        cfr = of_dl_conf(728 << 4,
                         dl_conf_add_pdu((uint8_t*) &bch_pdu_data, BCH_PDU_TYPE_SIZE,
                         dl_conf_add_pdu((uint8_t*) &mch_pdu_data, MCH_PDU_TYPE_SIZE,

                         dl_conf_pdu_tail((uint8_t*) bf_vector_type_data, sizeof (bf_vector_type_data),
                         dl_conf_add_pdu((uint8_t*) &dlsch_pdu_data, DLSCH_PDU_TYPE,
                                         sizeof(struct fapi_l1_dlsch_pdu) + sizeof (bf_vector_type_data),

                         dl_conf_add_pdu((uint8_t*) &dci_dl_pdu_data, DCI_DL_PDU_TYPE_SIZE,
                         dl_conf_add_pdu((uint8_t*) &pch_pdu_data, PCH_PDU_TYPE_SIZE,
                                         format_dl_conf (msg_buff, &dl_bch_config))))))));

    TEST_ASSERT_EQUAL_INT(4, cfr->NumberOf_PDUs);
    TEST_ASSERT_EQUAL_INT(1, cfr->NumberOf_DCIs);

    TEST_ASSERT_EQUAL_INT (cfr->hdr.length - offsetof (struct fapi_l1_dl_config_request, DL_PDU_Configuration),
                           cfr->Length);

    struct DL_PDU_Configuration * dl_pdu;
#if 1
      //    cfr->Length--;
    dl_pdu_for_each_entry (cfr, dl_pdu) {
#else	/* !( 0 ) */ 

        int i; for (i = 0, dl_pdu = &cfr->DL_PDU_Configuration[0]; 
     se               i < cfr->NumberOf_DCIs + cfr->NumberOf_PDUs;     
                    i++,
        dl_pdu = (struct DL_PDU_Configuration *)((uint8_t*)dl_pdu + dl_pdu->PDU_Size))
    {
#endif

        /* CLDBG(">> 0x%x - 0x%x i: %d diff %d, size %d, limit %d", dl_pdu, cfr->DL_PDU_Configuration, i,  */
        /*       ((char*)dl_pdu) - ((char*)cfr->DL_PDU_Configuration),  */
        /*       dl_pdu->PDU_Size , cfr->Length);  */

        dl_pdu_assert_bounds(cfr, dl_pdu);
        
        switch (dl_pdu->PDU_Type) {
        case DCI_DL_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (31, ((struct fapi_l1_dci_dl_pdu*) dl_pdu->DL_PDU)->MCS_2);
            break;
        case BCH_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (10000, ((struct fapi_l1_bch_pdu*) dl_pdu->DL_PDU)->TransmissionPower);
            break;
        case MCH_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (MD_64QAM, ((struct fapi_l1_mch_pdu*) dl_pdu->DL_PDU)->Modulation);
            break;
        case DLSCH_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (SINGLE_ANTENNA_PORT_5,
                 ((struct fapi_l1_dlsch_pdu*) dl_pdu->DL_PDU)->TransmissionScheme);
            TEST_ASSERT_EQUAL_INT
                ((0xAA00 & 0xCC),
                 ((struct fapi_l1_dlsch_pdu*) dl_pdu->DL_PDU)->bfVector[0].bfValue);
            break;
        case PCH_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (7, ((struct fapi_l1_pch_pdu*) dl_pdu->DL_PDU)->P_A);
            break;
        }
    }

}


#include "sdu_combinator_ul_data.hh"

static void testULCheckSize(void)
{
    uint8_t msg_buff [BUFSIZ];
    struct fapi_l1_ul_config_request * cfr =
        of_ul_conf(728 << 4,
                     ul_conf_add_pdu((uint8_t*) &ulsch_cqi_ri_pdu_data, ULSCH_CQI_RI_PDU_TYPE_SIZE,
                     ul_conf_add_pdu((uint8_t*) &ulsch_cqi_ri_pdu_data, ULSCH_CQI_RI_PDU_TYPE_SIZE,
                                     format_ul_conf (msg_buff, &ul_config_request_data))));

    TEST_ASSERT_EQUAL_INT (
        offsetof (struct fapi_l1_ul_config_request, UL_PDU_Configuration)
        +  2 * (offsetof (struct UL_PDU_Configuration, UL_PDU)
                + sizeof(struct fapi_l1_ulsch_cqi_ri_pdu))
        , cfr->hdr.length);

    TEST_ASSERT_EQUAL_INT (2, cfr->NumberOf_PDUs);
}


static void testULCheckOffset(void)
{
    uint8_t msg_buff [BUFSIZ];

    struct fapi_l1_ul_config_request * cfr =
        cfr = of_ul_conf(728 << 4,
                         ul_conf_add_pdu((uint8_t*) &ulsch_pdu_data, ULSCH_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &ulsch_cqi_ri_pdu_data, ULSCH_CQI_RI_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &ulsch_harq_pdu_data, ULSCH_HARQ_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &ulsch_cqi_harq_ri_pdu_data, ULSCH_CQI_HARQ_RI_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_cqi_pdu_data, UCI_CQI_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_sr_pdu_data, UCI_SR_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_harq_pdu_data, UCI_HARQ_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_sr_harq_pdu_data, UCI_SR_HARQ_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_cqi_harq_pdu_data, UCI_CQI_HARQ_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_cqi_sr_pdu_data, UCI_CQI_SR_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &uci_cqi_sr_harq_pdu_data, UCI_CQI_SR_HARQ_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &srs_pdu_data, SRS_PDU_TYPE_SIZE,
                         ul_conf_add_pdu((uint8_t*) &harq_buffer_pdu_data, HARQ_BUFFER_PDU_TYPE_SIZE,
                                         format_ul_conf (msg_buff, &ul_bch_config)))))))))))))));

    TEST_ASSERT_EQUAL_INT(13, cfr->NumberOf_PDUs);

    struct UL_PDU_Configuration * ul_pdu;
    ul_pdu_for_each_entry (cfr, ul_pdu) {
        ul_pdu_assert_bounds(cfr, ul_pdu);
        
        switch (ul_pdu->PDU_Type) {
        case ULSCH_PDU_TYPE:             break;
        case ULSCH_CQI_RI_PDU_TYPE:      break;
        case ULSCH_HARQ_PDU_TYPE:        break;
        case ULSCH_CQI_HARQ_RI_PDU_TYPE: break;
        case UCI_CQI_PDU_TYPE:           break;
        case UCI_SR_PDU_TYPE:            break;
        case UCI_HARQ_PDU_TYPE:          break;
        case UCI_SR_HARQ_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (42, ((struct fapi_l1_uci_sr_harq_pdu*)
                      ul_pdu->UL_PDU)->SR_Information.PUCCH_Index);
            break;
        case UCI_CQI_HARQ_PDU_TYPE:      break;
        case UCI_CQI_SR_PDU_TYPE:        break;
        case UCI_CQI_SR_HARQ_PDU_TYPE:   break;
        case SRS_PDU_TYPE:               break;
        case HARQ_BUFFER_PDU_TYPE:       break;
        default:
            TEST_ASSERT(0);
        }
    }
}




static void testTxCheckSize(void)
{
    return;
    uint8_t msg_buff [BUFSIZ];
    uint8_t mac_pdu [] = {0x62, 0xD8, 0x00};

    struct fapi_l1_tx_request * trx =
        of_tx_req(728 << 4,
                  tx_req_add_pdu(0, 42, (uint8_t*) &mac_pdu, sizeof(mac_pdu),
                                 format_tx_req (msg_buff, NULL)));

    TEST_ASSERT_EQUAL_INT (sizeof(struct fapi_l1_tx_request)
                           + sizeof(struct fapi_l1_dl_pdu_config)
                           - sizeof(uint32_t)
                           + roundup(sizeof(mac_pdu), 4),
                           trx->hdr.length);

}


static void testTxCheckOffset(void)
{
    uint8_t msg_buff [BUFSIZ];
    uint8_t mac_pdu [] = {0x62, 0xD8, 0x00};
    uint8_t mac_pdu_1 [] = {0x42};
    uint8_t mac_pdu_2 [] = {0x62, 0xD8, 0x00, 0x62, 0xD8, 0x00};

    struct fapi_l1_tx_request * trx =
        of_tx_req(728 << 4,
                  tx_req_add_pdu(0, 43, (uint8_t*) &mac_pdu_2, sizeof(mac_pdu_2),
                  tx_req_add_pdu(1, 42, (uint8_t*) &mac_pdu_1 + 0x0C000000 , sizeof(mac_pdu_1),
                  tx_req_add_pdu(0, 41, (uint8_t*) &mac_pdu, sizeof(mac_pdu),
                                 format_tx_req (msg_buff, NULL)))));

    TEST_ASSERT_EQUAL_INT (3, trx->NumberOf_PDUs);

    struct fapi_l1_dl_pdu_config * dl_pdu;
    tx_req_for_each_pdu(trx, dl_pdu) {
          //        printf(">> 0x%x i: %d, l: %d v: %x\n", dl_pdu, dl_pdu->PDU_Index, dl_pdu->PDU_Length, dl_pdu->tlvs[0].value);
 
        switch (dl_pdu->PDU_Index) {
        case 41:
            TEST_ASSERT_EQUAL_INT (mac_pdu[1],
                                   ((uint8_t*)(uint32_t*)&dl_pdu->tlvs[0].value)[1]);
            break;
        case 42:
            TEST_ASSERT_EQUAL_INT ((uint32_t)&mac_pdu_1 + 0x0C000000, dl_pdu->tlvs[0].value);
            break;
        case 43:
            TEST_ASSERT_EQUAL_INT (sizeof(mac_pdu_2), dl_pdu->tlvs[0].length); 
            TEST_ASSERT_EQUAL_INT (roundup(sizeof(mac_pdu_2), 4) 
                                    + sizeof(struct fapi_l1_dl_pdu_config) 
                                    - sizeof(uint32_t) 
                                    , dl_pdu->PDU_Length); 
            break;
        default:
            TEST_ASSERT(false);
        }
    }
}



static void test_HI_DCI0_CheckOffset(void)
{
    uint8_t msg_buff [BUFSIZ];

    struct fapi_l1_hi_dci0_request * req =
        of_hi_dci0_req(728 << 4,
                       // 2 x HI
                       hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_hi_pdu_data, HI_PDU_TYPE_SIZE,
                       hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_hi_pdu_data, HI_PDU_TYPE_SIZE,
                                             
                       // 3 x DCI0
                       hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_dci_ul_pdu_data, DCI_UL_PDU_TYPE_SIZE,
                       hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_dci_ul_pdu_data, DCI_UL_PDU_TYPE_SIZE,
                       hi_dci0_req_add_pdu((uint8_t*) &fapi_l1_dci_ul_pdu_data, DCI_UL_PDU_TYPE_SIZE,
                                           format_hi_dci0_req (msg_buff, &fapi_l1_hi_dci0_request_data)))))));

    TEST_ASSERT_EQUAL_INT(2, req->NumberOf_HI);
    TEST_ASSERT_EQUAL_INT(3, req->NumberOf_DCI);

    struct DCI_HI_PDU_Configuration * ul_pdu;
    int i;
    hi_dci0_req_for_each_pdu (i, req, ul_pdu) {
        
        switch (ul_pdu->PDU_Type) {
        case HI_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (7, ((struct fapi_l1_hi_pdu*)
                     ul_pdu->DCI_HI_PDU)->CyclicShift2For_DMRS);
            break;
        case DCI_UL_PDU_TYPE:
            TEST_ASSERT_EQUAL_INT
                (88, ((struct fapi_l1_dci_ul_pdu*)
                      ul_pdu->DCI_HI_PDU)->CCE_Index);
            break;
        default:
            TEST_ASSERT(0);
        }
    }
}



/*embunit:impl=- */
TestRef sdu_combinator_test_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
        new_TestFixture("testCheckSize",testCheckSize),
            new_TestFixture("testCheckOffset",testCheckOffset),
        new_TestFixture("testULCheckSize",testULCheckSize),
            new_TestFixture("testULCheckOffset",testULCheckOffset),
        new_TestFixture("testTxCheckSize",testTxCheckSize),
            new_TestFixture("testTxCheckOffset",testTxCheckOffset),
            new_TestFixture("test_HI_DCI0_CheckOffset",test_HI_DCI0_CheckOffset),
            /*embunit:fixtures=- */
            };
    EMB_UNIT_TESTCALLER(sdu_combinator_test,"sdu_combinator_test",setUp,tearDown,fixtures);
    return (TestRef)&sdu_combinator_test;
};

#if 0
  ul_conf_add_pdu((uint8_t*) &\1_pdu_data, \1_PDU_TYPE_SIZE,



#endif





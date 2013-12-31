  /*embunit:include=+ */
#include <embUnit/embUnit.h>
  /*embunit:include=- */

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


l1_tlv_word_t MIMO_CONFIG_REQUEST_TLVS [] = {
    /* Duplexing Mode                 1 */
    {FAPI_L1_DuplexingMode                              , 2, {FAPI_L1_DM_FDD}},
    /* PCFICH Power Offset            2 */
    {FAPI_L1_PCFICH_PowerOffset                         , 2, {0x1b42}},
/* P-B                            3 */
    {FAPI_L1_P_B                                        , 2, {0}},
    /* DL Cyclic Prefix Type          4 */
    {FAPI_L1_DL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* UL Cyclic Prefix Type          5 */
    {FAPI_L1_UL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* +RF Config                      All TLVs in this grouping */
    {FAPI_L1_DownlinkChannelBandwidth                   , 2, {50}},
    {FAPI_L1_UplinkChannelBandwidth                     , 2, {15}},
    {FAPI_L1_ReferenceSignalPower                       , 2, {.word=0x1b42}},
    {FAPI_L1_TxAntennaPorts                             , 2, {2}},
    {FAPI_L1_RxAntennaPorts                             , 2, {2}},
    /* PHICH Config                   All TLVs in this grouping */
    {FAPI_L1_PHICH_Resource                             , 2, {PHICH_R_ONE_SIXTH}},
    {FAPI_L1_PHICH_Duration                             , 2, {PHICH_D_NORMAL}},
    {FAPI_L1_PHICH_PowerOffset                          , 2, {0}},
    /* SCH Config                     All TLVs in this grouping */
    {FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS   , 2, {.word=6255}},
    {FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS , 2, {.word=6255}},
    {FAPI_L1_PhysicalCell_ID                            , 2, {0}},
    /* PRACH Config                   All TLVs in this grouping */
    {FAPI_L1_ConfigurationIndex                         , 2, {0x0C}},
    {FAPI_L1_RootSequenceIndex                          , 2, {0x16}},
    {FAPI_L1_ZeroCorrelationZoneConfiguration           , 2, {1}}, // Cyclic shift
    {FAPI_L1_HighSpeedFlag                              , 2, {HS_UNRESTRICTED_SET}},
    {FAPI_L1_FrequencyOffset                            , 2, {0}},

    /* PUSCH Config                   All TLVs in this grouping */
    {FAPI_L1_HoppingMode                                , 2, {HM_INTER_SF}},
    {FAPI_L1_HoppingOffset                              , 2, {1}},
    {FAPI_L1_NumberOfSubBands                           , 2, {2}},

    /* PUCCH Config                   All TLVs in this grouping */
    {FAPI_L1_Delta_PUCCH_Shift                          , 2, {1}}, //??
    {FAPI_L1_N_CQI_RB                                   , 2, {0}}, //??
    {FAPI_L1_N_AN_CS                                    , 2, {0}}, // 2b
    {FAPI_L1_N1Pucch_AN                                 , 2, {0}}, //??
    /* SRS Config                     All TLVs in this grouping */
    {FAPI_L1_SRS_BandwidthConfiguration                     , 2, {0}},
#if 0
    {FAPI_L1_MaxUpPTS                                   , 2, {MAX_UP_PTS_DISABLED}}, // TDD only
#endif
    {FAPI_L1_SRS_SubframeConfiguration                  , 2, {15}},
    {FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission   , 2, {SRS_NO_SIMULTANEOUS_TRANSMISSION}},
    /* Uplink Reference Signal Config All TLVs in this grouping */
    {FAPI_L1_Uplink_RS_Hopping                          , 2, {RS_NO_HOPPING}},
    {FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern  , 2, {0}},
    {FAPI_L1_CyclicShift1For_DMRS                       , 2, {0}},
    /* Data Report Mode               50 */
    {FAPI_L1_DataReportMode                             , 2, {1}},

    {FAPI_L1_SFN_SF                                     , 2, {0}}
    };

size_t MIMO_CONFIG_REQUEST_TLVS_num = sizeof(MIMO_CONFIG_REQUEST_TLVS) / sizeof(l1_tlv_word_t);

l1_tlv_word_t SISO_CONFIG_REQUEST_TLVS [] = {
    /* Duplexing Mode                 1 */
    {FAPI_L1_DuplexingMode                              , 2, {FAPI_L1_DM_FDD}},
    /* PCFICH Power Offset            2 */
    {FAPI_L1_PCFICH_PowerOffset                         , 2, {0x1b42}},
/* P-B                            3 */
    {FAPI_L1_P_B                                        , 2, {0}},
    /* DL Cyclic Prefix Type          4 */
    {FAPI_L1_DL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* UL Cyclic Prefix Type          5 */
    {FAPI_L1_UL_CyclicPrefixType                        , 2, {CP_NORMAL}},
    /* +RF Config                      All TLVs in this grouping */
    {FAPI_L1_DownlinkChannelBandwidth                   , 2, {50}},
    {FAPI_L1_UplinkChannelBandwidth                     , 2, {15}},
    {FAPI_L1_ReferenceSignalPower                       , 2, {.word=0x00}},
    {FAPI_L1_TxAntennaPorts                             , 2, {1}},
    {FAPI_L1_RxAntennaPorts                             , 2, {1}},
    /* PHICH Config                   All TLVs in this grouping */
    {FAPI_L1_PHICH_Resource                             , 2, {PHICH_R_ONE_SIXTH}},
    {FAPI_L1_PHICH_Duration                             , 2, {PHICH_D_NORMAL}},
    {FAPI_L1_PHICH_PowerOffset                          , 2, {0}},
    /* SCH Config                     All TLVs in this grouping */
    {FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS   , 2, {.word=0}},
    {FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS , 2, {.word=0}},
    {FAPI_L1_PhysicalCell_ID                            , 2, {1}},
    /* PRACH Config                   All TLVs in this grouping */
    {FAPI_L1_ConfigurationIndex                         , 2, {0x0C}},
    {FAPI_L1_RootSequenceIndex                          , 2, {0x16}},
    {FAPI_L1_ZeroCorrelationZoneConfiguration           , 2, {1}}, // Cyclic shift
    {FAPI_L1_HighSpeedFlag                              , 2, {HS_UNRESTRICTED_SET}},
    {FAPI_L1_FrequencyOffset                            , 2, {0}},
    /* PUSCH Config                   All TLVs in this grouping */
    {FAPI_L1_HoppingMode                                , 2, {HM_INTER_SF}},
    {FAPI_L1_HoppingOffset                              , 2, {1}},
    {FAPI_L1_NumberOfSubBands                           , 2, {2}},

    /* PUCCH Config                   All TLVs in this grouping */
    {FAPI_L1_Delta_PUCCH_Shift                          , 2, {2}}, //??
    {FAPI_L1_N_CQI_RB                                   , 2, {4}}, //??
    {FAPI_L1_N_AN_CS                                    , 2, {0}}, // 2b
    {FAPI_L1_N1Pucch_AN                                 , 2, {0}}, //??
    /* SRS Config                     All TLVs in this grouping */
    {FAPI_L1_SRS_BandwidthConfiguration                     , 2, {0}},
#if 0
    {FAPI_L1_MaxUpPTS                                   , 2, {MAX_UP_PTS_DISABLED}}, // TDD only
#endif
    {FAPI_L1_SRS_SubframeConfiguration                  , 2, {15}},
    {FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission   , 2, {SRS_NO_SIMULTANEOUS_TRANSMISSION}},
    /* Uplink Reference Signal Config All TLVs in this grouping */
    {FAPI_L1_Uplink_RS_Hopping                          , 2, {RS_NO_HOPPING}},
    {FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern  , 2, {0}},
    {FAPI_L1_CyclicShift1For_DMRS                       , 2, {0}},
    /* Data Report Mode               50 */
    {FAPI_L1_DataReportMode                             , 2, {1}},

    {FAPI_L1_SFN_SF                                     , 2, {0}}
};

size_t SISO_CONFIG_REQUEST_TLVS_num = sizeof(SISO_CONFIG_REQUEST_TLVS) / sizeof(l1_tlv_word_t);


void send_CONFIG_REQUEST(l1_tlv_word_t tlvs[], size_t num)
{
    static char msg_buff[BUFSIZ];
    static char reply_buff[BUFSIZ];
    struct fapi_l1_config_request* fapi_conf = (struct fapi_l1_config_request*) msg_buff;
    l1_alloc_config_request(fapi_conf, tlvs,  num);

    struct fapi_l1_config_response* fapi_conf_response = (struct fapi_l1_config_response*) reply_buff;
    TEST_ASSERT_EQUAL_INT(0, lte_cphy_request((fapi_l1_msg_hdr_t*)fapi_conf,
                                              (fapi_l1_msg_hdr_t*)fapi_conf_response));
    
    TEST_ASSERT_EQUAL_INT(FAPI_L1_CONFIG_RESP, fapi_conf_response->hdr.msgType);
    TEST_ASSERT_EQUAL_INT(MSG_OK, fapi_conf_response->errorCode);
}

extern unsigned char out_mimo_0000_PHY_INIT_REQ_bin[]; 
extern unsigned int out_mimo_0000_PHY_INIT_REQ_bin_len; 

extern unsigned char out_ref_0000_PHY_INIT_REQ_bin[];
extern unsigned int out_ref_0000_PHY_INIT_REQ_bin_len;


static void setUp(void)
{
    /* initialize */
}

static void tearDown(void)
{
    /* terminate */
}


static void test_CONFIG_REQUEST (l1_tlv_word_t *config, size_t config_size,
                                 unsigned char *ref_api, unsigned int ref_api_len)
{
    // SETUP
    char msg_buff[BUFSIZ];
    struct fapi_l1_config_request* fapi_conf = (struct fapi_l1_config_request*) msg_buff;

    l1_alloc_config_request(fapi_conf, config, config_size);

    // RUN
    INITPARM apiInit;
    uint8_t tlv_errors [FAPI_L1_TLV_MAX] = { FAPI_TLV_NOERROR };
    uint8_t tlv_errors_noerr [FAPI_L1_TLV_MAX] = { FAPI_TLV_NOERROR };

    int ret = conv_apiInit_from_CONFIG_request (fapi_conf, &apiInit, tlv_errors);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_BIN(tlv_errors, tlv_errors_noerr, FAPI_L1_TLV_MAX);

    // CHECK
    API_LOG_INFO *apiLogInfo = (API_LOG_INFO *) ref_api;
    PGENMSGDESC  genhdr = (PGENMSGDESC) &apiLogInfo [1];
    PINITPARM    apiInitRef = (PINITPARM) &genhdr[1];

    TEST_ASSERT_NOT_NULL(apiLogInfo);
    TEST_ASSERT_EQUAL_INT(PHY_INIT_REQ, apiLogInfo->Message);
    TEST_ASSERT_EQUAL_INT(PHY_INIT_REQ, genhdr->msgType);

    apiInitRef->phyCfg = (&apiInit)->phyCfg = 7;

    apiInitRef->referenceSignalPower = (&apiInit)->referenceSignalPower = 0;
    apiInitRef->primarySyncSignalPower = (&apiInit)->primarySyncSignalPower = 0;
    apiInitRef->secondarySyncSignalPower = (&apiInit)->secondarySyncSignalPower = 0;

    TEST_ASSERT_EQUAL_BIN(apiInitRef, &apiInit, sizeof(INITPARM));

}


/*embunit:impl=+
 */
static void testConfigMIMO(void)
{
    test_CONFIG_REQUEST (MIMO_CONFIG_REQUEST_TLVS, MIMO_CONFIG_REQUEST_TLVS_num,
                         out_mimo_0000_PHY_INIT_REQ_bin, out_mimo_0000_PHY_INIT_REQ_bin_len);

}

static void testConfigSISO(void)
{

    test_CONFIG_REQUEST (SISO_CONFIG_REQUEST_TLVS, SISO_CONFIG_REQUEST_TLVS_num,
                         out_ref_0000_PHY_INIT_REQ_bin, out_ref_0000_PHY_INIT_REQ_bin_len);

}


  /*embunit:impl=- */
TestRef test_CONFIG_REQ_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
        new_TestFixture("testConfigMIMO",testConfigMIMO),
            new_TestFixture("testConfigSISO",testConfigSISO),
            /*embunit:fixtures=- */
            };
    EMB_UNIT_TESTCALLER(test_CONFIG_REQ,"test_CONFIG_REQ",setUp,tearDown,fixtures);
    return (TestRef)&test_CONFIG_REQ;
};

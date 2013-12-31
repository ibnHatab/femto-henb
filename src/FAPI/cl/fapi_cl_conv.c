
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifdef MX_ARM_ENABLED
#include "appprintf.h"
#endif

#include <LtePhyL2Api.h>

#include <l1_types.h>
#include <l1_tlv.h>
#include <l1_l2_iface.h>
#include <l1_msg_alloc.h>
#include <l1_sdu_combinator.h>

#include "fapi_cl.h"
#include "fapi_cl_cphy_sap.h"
#include "fapi_cl_ue_db.h"

#include "fapi_cl_conv.h"


int conv_apiInit_from_CONFIG_request (struct fapi_l1_config_request* fapi,
                                      PINITPARM api,
                                      uint8_t *tlv_errors)
{
    int ret = 0, i;
    for (i = 0; i < fapi->numberOfTLVs; i++) {
        uint8_t tlv_tag =fapi->tlvs[i].tag;
        uint16_t tlv_value = fapi->tlvs[i].val.word;        // only tlv.val.word in here
        switch (tlv_tag) {
        case FAPI_L1_DownlinkChannelBandwidth: {
            uint16_t DlchannelBandwidth = tlv_value;
            api->nMaxDlRb = DlchannelBandwidth;
            api->nMaxUlRb = DlchannelBandwidth;
            switch (DlchannelBandwidth) {
            case RB_SLOT_6:
            {
                api->channelBandwidth = BW_1_4_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_6;
                api->fftSize = FFTSZ_128;
                api->numberUsedSubcarriers = NUSC_72;
            }
            break;
            case RB_SLOT_15:
            {
                api->channelBandwidth = BW_3_0_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_15;
                api->fftSize = FFTSZ_256;
                api->numberUsedSubcarriers = NUSC_180;
            }
            break;
            case RB_SLOT_25:
            {
                api->channelBandwidth = BW_5_0_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_25;
                api->fftSize = FFTSZ_512;
                api->numberUsedSubcarriers = NUSC_300;
            }
            break;
            case RB_SLOT_50:
            {
                api->channelBandwidth = BW_10_0_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_50;
                api->fftSize = FFTSZ_1024;
                api->numberUsedSubcarriers = NUSC_600;
            }
            break;
            case RB_SLOT_75:
            {
                api->channelBandwidth = BW_15_0_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_75;
                api->fftSize = FFTSZ_1536;
                api->numberUsedSubcarriers = NUSC_900;
            }
            break;
            case RB_SLOT_100:
            {
                api->channelBandwidth = BW_20_0_MHZ;
                api->numberResourceBlocksperSlot = RB_SLOT_100;
                api->fftSize = FFTSZ_2048;
                api->numberUsedSubcarriers = NUSC_1200;
            }
            break;
            default:
                tlv_errors [FAPI_L1_DownlinkChannelBandwidth] |= FAPI_TLV_WRONG_VALUE;
            }
        } break;

        case FAPI_L1_DuplexingMode:
            api->frtypeDuplexmode = tlv_value == FAPI_L1_DM_FDD ? FDD : TDD;
            break;
        case FAPI_L1_UL_CyclicPrefixType:
            break;
        case FAPI_L1_DL_CyclicPrefixType: {
            api->cyclicPrefixType = tlv_value;

            switch (tlv_value) {
            case CP_NORMAL:
                api->physicalResourceBandwidth = PRB_12;
                break;
            case CP_EXTENDED:
                api->physicalResourceBandwidth = PRB_24;
                break;
            default:
                tlv_errors [FAPI_L1_DL_CyclicPrefixType] |= FAPI_TLV_WRONG_VALUE;
            }
        } break;
        case FAPI_L1_PhysicalCell_ID:
            api->phyLayerCellIdGroup = CELL_GRP(tlv_value);
            api->phyLayerId = CELL_LAYER(tlv_value);
            break;
        case FAPI_L1_TxAntennaPorts:
            api->txAntennaPortCount = tlv_value;
            break;
        case FAPI_L1_ReferenceSignalPower:
            api->referenceSignalPower = POW_FROM_FAPI(tlv_value);
            break;
        case FAPI_L1_PrimarySynchronizationSignal_EPRE_EPRERS:
            api->primarySyncSignalPower = POW_FROM_FAPI(tlv_value);
            break;
        case FAPI_L1_SecondarySynchronizationSignal_EPRE_EPRERS:
            api->secondarySyncSignalPower = POW_FROM_FAPI(tlv_value);
            break;

        case FAPI_L1_RxAntennaPorts: {
            api->rxAntennaPortCount = tlv_value;

            switch (tlv_value) {
            case 1:
                api->numDataRePerPRB             = 80;
                break;
            case 2:
                api->numDataRePerPRB             = 76;
                break;
            case 4:
                api->numDataRePerPRB             = 72;
                break;
            default:
                tlv_errors [FAPI_L1_RxAntennaPorts] |= FAPI_TLV_WRONG_VALUE;
            }
        } break;

        case FAPI_L1_P_B:
            api->pb = tlv_value;
            break;

        case FAPI_L1_SFN_SF: {
            l1_sfn_sf_t sfn_sf;
            sfn_sf.sfn_sf = tlv_value;
            api->radioFrameNumber            = sfn_sf.val.sfn;
            api->subframeNumber              = sfn_sf.val.sf;
            CLDBG("conf SFN/SF MAC %d/%d", sfn_sf.val.sfn, sfn_sf.val.sf);
        } break;

            /* SRS Config                     All TLVs in this grouping */
        case FAPI_L1_SRS_BandwidthConfiguration:
            api->srsBandwidthConfig = tlv_value;
            break;
        case FAPI_L1_SRS_SubframeConfiguration:
            api->srsSubframeConfig = tlv_value;
            break;
        case FAPI_L1_SRS_AckNack_SRS_SimultaneousTransmission:
            api->srsSimultaneousANandSRS = tlv_value;
            break;
            /* PRACH Config                   All TLVs in this grouping */
        case FAPI_L1_ConfigurationIndex:
            api->prachConfigurationIndex = tlv_value;
            break;
        case FAPI_L1_FrequencyOffset:
            api->prachFrequencyOffset = tlv_value;
            break;
        case FAPI_L1_HighSpeedFlag:
            api->prachHighSpeedFlag = tlv_value;
            break;
        case FAPI_L1_ZeroCorrelationZoneConfiguration:
            api->prachCyclicShiftConfig = tlv_value;
            break;
        case FAPI_L1_RootSequenceIndex:
            api->prachRootSequenceIndex = tlv_value;
            break;
        case FAPI_L1_PHICH_Resource:
            api->phichResource = tlv_value;
            break;
        case FAPI_L1_PHICH_Duration:
            api->phichDuration = tlv_value;
            break;


            /**
               Stored in PHY config
             */
        case FAPI_L1_Delta_PUCCH_Shift:
        case FAPI_L1_N_CQI_RB:
        case FAPI_L1_N_AN_CS:
        case FAPI_L1_N1Pucch_AN:
            break;
            /* PUSCH Config                   All TLVs in this grouping */
        case FAPI_L1_HoppingMode:
        case FAPI_L1_HoppingOffset:
        case FAPI_L1_NumberOfSubBands:
            break;

            /* Uplink Reference Signal Config All TLVs in this grouping */
        case FAPI_L1_Uplink_RS_Hopping:
        case FAPI_L1_GroupAssignmentDeltaSequence_ShiftPattern:
        case FAPI_L1_CyclicShift1For_DMRS:
            break;

            //??
        case FAPI_L1_UplinkChannelBandwidth:
        case FAPI_L1_PHICH_PowerOffset:
        case FAPI_L1_PCFICH_PowerOffset:
        case FAPI_L1_DataReportMode:
            break;

            /**
               Unsupported TLV's
             */
        case FAPI_L1_MaxUpPTS:
            break;
        default:
            BUG_ON(tlv_tag > FAPI_L1_TLV_MAX);
            ret |= FAPI_TLV_UNSUPPORTED;
            tlv_errors [tlv_tag] |= FAPI_TLV_UNSUPPORTED;
        }
    }

    // Hardcoded part


    api->phyCfg                      = CL_PHY_PHYINIT_CONFIG;
    api->reserved                    = 0;

    api->rxSduLineMonitorInfoenable  = DISABLED;
    api->txStartConfDisable          = ENABLED;
    api->rxStartConfDisable          = ENABLED;
    api->customExtensionReportEn     = DISABLED;
    api->rachReportMode              = 0 /* PHY_RXSDU */;
    api->txSduConfEnable             = DISABLED;
    api->sduConfig                   = 0 /* FIXME: contimuos data */;

    api->fastfwdorPreclockingNumber  = 1600;
    api->radioAccessMode             = OFDMA;
    api->slotNumber                  = 0; //GET_TLV (FAPI_L1_TLV_MAX);; //??

    return ret;
}

//// DL //// DL //// DL //// DL //// DL //// DL //// DL //// DL //// DL ////
// 0: DCI DL PDU, see Section 3.3.1.2.1.
static __inline int
conv_dlSubframeDescriptor_add_DCI (struct fapi_l1_dci_dl_pdu * dci,
                                   int dciIdx,
                                   PDLSUBFRDESC api)
{
    int res = 0;
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    PDCICHANNELDESC dciCh = (PDCICHANNELDESC)((U8*)api + api->offsetDCIChannels);
    PDCICHANNELDESC pDci = &dciCh[dciIdx];

    CLDBG("DCI sf: %d, idx %d format 0x%x", api->subframeNumber, dciIdx, dci->DCI_Format);

    pDci->offsetNextCh = sizeof(DCICHANNELDESC);
    pDci->channelId = dciIdx;
    pDci->txpowerControl = dci->TransmissionPower;
    pDci->crcLength = SIXTEEN;
    pDci->crcScrambling = ENABLED;
    pDci->channelType = PDCCH;
    ; pDci->reserved = 0;

    pDci->numCodeWords = 1;
    pDci->nLayers = phy_config->TxAntennaPorts;
    pDci->transmissionMode = phy_config->TxAntennaPorts;

    ; pDci->reserved1 = 0;
    pDci->scrmblerInitValue = dci->RNTI;
    pDci->numberofEntries = 1; // ??
    pDci->startRes = 0; //??
    pDci->numRes = 4; //??

    return res;
}
// 1: BCH PDU, see Section 3.3.1.2.2.
static __inline int
conv_dlSubframeDescriptor_add_BCH (struct fapi_l1_bch_pdu * bch,
                                   int dlChIdx,
                                   PDLSUBFRDESC api)
{
    int ret = 0, i;
    PDLCHANDESC pCh = &api->dlCh[dlChIdx];

    // (A) Make assotiation to the MAC SDU
    PTXSDUREQ sdu = fapi_cl_add_dl_sdu (api->subframeNumber,
                                        bch->Length, bch->PDU_Index);

    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    CLDBG("alloc BCHs sf%d, idx %d ptr 0x%x",
          api->subframeNumber, bch->PDU_Index, (uint32_t)sdu);


    sdu->chanId = dlChIdx;
    sdu->msgType = PHY_TXSDU_REQ;
    sdu->phyEntityId = 0;
    sdu->nackAck = 0;
    sdu->uciFormat = 0;
    sdu->channelType = PBCH;
    sdu->phichSeqIndex = 0;
    sdu->cwId = 0;
    sdu->phichGrpNumber = 0;
    sdu->maxBitsperCw = 24; // 8 * bch->Length ??
    sdu->msgLen = (sdu->maxBitsperCw + 7) >> 3;
    sdu->pTxSdu = 0x00;

    // (B) Fill in DL CH
    pCh->offsetNextCh = sizeof(DLCHANDESC);
    pCh->channelId =  api->numCtrlChannels + dlChIdx;
    pCh->txpowerControl = bch->TransmissionPower;
    pCh->persistEnable = DISABLED;
    pCh->repeatCycle = 0x00;
    pCh->channelType = PBCH;
    pCh->hCid = 0;
    pCh->numCodeWords = 1;
    pCh->nLayers = phy_config->TxAntennaPorts;
    pCh->transmissionMode = phy_config->TxAntennaPorts;
    pCh->ueCategory = 1;

    // DLSUBCHINFO
    pCh->subChInfo[0].codingDescriptor = TBVITCC;
    pCh->subChInfo[0].blockCodeConcatenation = DISABLED;
    pCh->subChInfo[0].modulationType = QPSK;
    pCh->subChInfo[0].mcsType = 4;
    pCh->subChInfo[0].reserved = 0;
    pCh->subChInfo[0].nDi = 0;
    pCh->subChInfo[0].rV = 0;

    pCh->subChInfo[0].reserved = NOT_SPECIFIED_VALUE;
    pCh->subChInfo[0].flushReq = 0;
    pCh->subChInfo[0].pad = 0;


    pCh->subChInfo[1].codingDescriptor = 0;
    pCh->subChInfo[1].blockCodeConcatenation = 0;
    pCh->subChInfo[1].modulationType = 0;
    pCh->subChInfo[1].mcsType = 0;
    pCh->subChInfo[1].reserved = 0;
    pCh->subChInfo[1].nDi = 0;
    pCh->subChInfo[1].rV = 0;

    pCh->subChInfo[1].reserved = NOT_SPECIFIED_VALUE;
    pCh->subChInfo[1].flushReq = 0;
    pCh->subChInfo[1].pad = 0;

    // CRCINFO
    pCh->crcInfo.crcLength = SIXTEEN;
    pCh->crcInfo.crcScrambling = ENABLED;

    // SCRAMBLERINFO
    pCh->scrInfo.scramblerType = DLSCR;
    pCh->scrInfo.scrinitValueinput = phy_config->PhysicalCell_ID;

    // DLDEDICATEDPRECODERINFO
    pCh->dlPrecoderInfo.cddType = 0;
    pCh->dlPrecoderInfo.codeBookIdx = 0;
    pCh->dlPrecoderInfo.reserved = NOT_SPECIFIED_VALUE;
    for (i = 0; i < sizeof(pCh->dlPrecoderInfo.codebookSubsetRestriction) /
             sizeof(pCh->dlPrecoderInfo.codebookSubsetRestriction[0]); i++) {
        pCh->dlPrecoderInfo.codebookSubsetRestriction[i] = 0;
    }

    // MAPPINGINFO
    memset(&pCh->mapInfo, 0, sizeof(MAPPINGINFO));

    return ret;
}
// 2: MCH PDU, see Section 3.3.1.2.3.
static __inline int
conv_dlSubframeDescriptor_add_MCH (struct fapi_l1_mch_pdu * mch,
                                   int dlChIdx,
                                   PDLSUBFRDESC api)
{
    int ret = 0;
    return ret;
}
// 3: DLSCH PDU, see Section 3.3.1.2.4.
static int aModulationMap [] = {
    BPSK,  // 0-
    QPSK,  // 2: QPSK
    QAM16, // 4: 16QAM
    QAM64  // 6: 64QAM
};

static __inline int
conv_dlSubframeDescriptor_add_DLSCH (struct fapi_l1_dlsch_pdu * dlsch,
                                     int dlChIdx,
                                     PDLSUBFRDESC api)
{
    int ret = 0, i;
    PDLCHANDESC pCh = &api->dlCh[dlChIdx];

    // (A) Make assotiation to the MAC SDU
    PTXSDUREQ sdu = fapi_cl_add_dl_sdu (api->subframeNumber,
                                        dlsch->Length, dlsch->PDU_Index);

    static int aTransmissionModeMap [] = {
        1, // 0: SINGLE_ANTENNA_PORT_0,
        2, // 1: TX_DIVERSITY,
        3,  /*Open-loop spatial multiplexing */ // 2: LARGE_DELAY_CDD,
        4, // 3: CLOSED_LOOP_SPATIAL_MULTIPLEXING,
        5, // 4: MULTI_USER_MIMO,
        6, // 5: CLOSED_LOOP_RANK_1_PRECODING,
        7, // 6: SINGLE_ANTENNA_PORT_5.
    };


      //    struct fapi_cl_phy_config * phy_config = cl_get_phy_config ();
    CLDBG("alloc DLSCH sf%d, idx %d ptr 0x%x", api->subframeNumber,
          dlsch->PDU_Index, (uint32_t)sdu);


    sdu->chanId = dlChIdx;
    sdu->msgType = PHY_TXSDU_REQ;
    sdu->phyEntityId = 0;
    sdu->nackAck = 0;
    sdu->uciFormat = 0;
    sdu->channelType = PDSCH;
    sdu->phichSeqIndex = 0;
    sdu->cwId = 0;
    sdu->phichGrpNumber = 0;
    sdu->maxBitsperCw = 24; // ?? OGL
    sdu->msgLen = dlsch->Length;
    sdu->pTxSdu = 0x0;

    // (B) Fill in DL CH
    pCh->offsetNextCh = sizeof(DLCHANDESC);
    pCh->channelId = dlChIdx;
    pCh->txpowerControl = 0; // ??
    pCh->persistEnable = ENABLED; //??
    pCh->repeatCycle = 1; //For persistent channel ??
    pCh->channelType = PDSCH;
    pCh->hCid = 0;
    pCh->numCodeWords = aTransmissionModeMap [dlsch->TransmissionScheme] < 2 ? 1 : 2;
    pCh->nLayers = dlsch->NumberOfLayers == 1 ? 1 : 2;
    pCh->transmissionMode = aTransmissionModeMap [dlsch->TransmissionScheme];
    pCh->ueCategory = dlsch->UE_CategoryCapacity;

    // DLSUBCHINFO
    pCh->subChInfo[0].codingDescriptor = TURBOCDR;
    pCh->subChInfo[0].blockCodeConcatenation = ENABLED;
    pCh->subChInfo[0].modulationType = aModulationMap [dlsch->Modulation / 2];
    pCh->subChInfo[0].mcsType = 1; //??
    pCh->subChInfo[0].reserved = 0;
    pCh->subChInfo[0].nDi = 0;
    pCh->subChInfo[0].rV = 0;

    pCh->subChInfo[0].reserved = NOT_SPECIFIED_VALUE;
    pCh->subChInfo[0].flushReq = 0;
    pCh->subChInfo[0].pad = 0;


    pCh->subChInfo[1].codingDescriptor = 0; // ??
    pCh->subChInfo[1].blockCodeConcatenation = 0;
    pCh->subChInfo[1].modulationType = 0;
    pCh->subChInfo[1].mcsType = 0;
    pCh->subChInfo[1].reserved = 0;
    pCh->subChInfo[1].nDi = 0;
    pCh->subChInfo[1].rV = 0;

    pCh->subChInfo[1].reserved = NOT_SPECIFIED_VALUE;
    pCh->subChInfo[1].flushReq = 0;
    pCh->subChInfo[1].pad = 0;

    // CRCINFO
    pCh->crcInfo.crcLength =  TWTYFOUR;
    pCh->crcInfo.crcScrambling = ENABLED;

    // SCRAMBLERINFO
    pCh->scrInfo.scramblerType = DLSCR;
    pCh->scrInfo.scrinitValueinput =  dlsch->RNTI;

    // DLDEDICATEDPRECODERINFO
    pCh->dlPrecoderInfo.cddType = 0;
    pCh->dlPrecoderInfo.codeBookIdx = 0;
    pCh->dlPrecoderInfo.reserved = NOT_SPECIFIED_VALUE;
    for (i = 0; i < sizeof(pCh->dlPrecoderInfo.codebookSubsetRestriction) /
             sizeof(pCh->dlPrecoderInfo.codebookSubsetRestriction[0]); i++) {
        if (dlsch->TransmissionScheme == CLOSED_LOOP_SPATIAL_MULTIPLEXING
            || dlsch->TransmissionScheme == MULTI_USER_MIMO
            || dlsch->TransmissionScheme == CLOSED_LOOP_RANK_1_PRECODING) {
            pCh->dlPrecoderInfo.codebookSubsetRestriction[i] =
                dlsch->CodebookEntries[i].CodebookIndex;
        } else {
            pCh->dlPrecoderInfo.codebookSubsetRestriction[i] = 0;
        }
    }

    // MAPPINGINFO
    memset(&pCh->mapInfo, 0, sizeof(MAPPINGINFO));
    pCh->mapInfo.numberofEntries = 1;
    pCh->mapInfo.reselmInfo[0].startRes = 0; //??
    pCh->mapInfo.reselmInfo[0].numRes = 1;

    return ret;
}
// 4: PCH PDU, see Section 3.3.1.2.5.
static __inline int
conv_dlSubframeDescriptor_add_PCH (struct fapi_l1_pch_pdu * pch,
                                   int dlChIdx,
                                   PDLSUBFRDESC api)
{
    int ret = 0;
    return ret;
}

int
conv_dlSubframeDescriptor_from_DL_CONFIG_request(struct fapi_l1_dl_config_request * fapi,
                                                 PDLSUBFRDESC api, size_t *msgSize)
{
    int ret = 0, idx_DLCHANDESC, idx_DCICHANNELDESC;
    PDLCMNTXPWRCTL dlCmnPwrCtl;
    l1_sfn_sf_t sfn_sf;
    struct DL_PDU_Configuration * dl_pdu_config;

      // (A) DlSubframeDescriptor
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    U32 struOffset = 0;

    sfn_sf.sfn_sf = fapi->SFN_SF;
    api->frameNumber = sfn_sf.val.sfn;
    api->subframeNumber = sfn_sf.val.sf;

    CLDBG("SFN/SF TX.v %d/%d", api->frameNumber, api->subframeNumber);

    api->subframeType = DLTX;
    api->antennaPortcount = phy_config->TxAntennaPorts;

    // (A.1) allocate offsets to the DCI and PWR
    struOffset = offsetof(DLSUBFRDESC, dlCh) + fapi->NumberOf_PDUs * sizeof(DLCHANDESC);
    /// past fixed part

    api->offsetDCIChannels = fapi->NumberOf_DCIs == 0 ? 0 : struOffset;
    struOffset += fapi->NumberOf_DCIs * sizeof(DCICHANNELDESC);

    api->offsetPowerCtrl = struOffset;
    api->offsetCustomFeatures = 0;

    //
    api->numCtrlSymbols = fapi->NumberOf_PDCCH_OFDM_Symbols; //?? OLG: 2-3 const
    api->phichResource = phy_config->PHICH_Resource;
    api->phichDuration = phy_config->PHICH_Duration;
    api->numberOfPhichChannels = 0; //??? OLG HARQ ACK / UL HARQ
    //?? phy_conf->PHICH_PowerOffset;

    api->numberofChannelDescriptors = fapi->NumberOf_PDUs + fapi->NumberOf_DCIs;
    api->numCtrlChannels = fapi->NumberOf_DCIs;

    // (B) Fill chennel descriptors
    idx_DLCHANDESC = idx_DCICHANNELDESC = 0;
    {// FIXME: add i to macro params, made scope declaration
        dl_pdu_for_each_entry (fapi, dl_pdu_config) {
            uint8_t *dl_pdu = &dl_pdu_config->DL_PDU[0];
            dl_pdu_assert_bounds (fapi, dl_pdu_config);

            switch (dl_pdu_config->PDU_Type) {
                // B.1 DLCHANDESC
            case BCH_PDU_TYPE: // FIXME: channel naming not dlSubframeDescriptor
                ret |= conv_dlSubframeDescriptor_add_BCH
                    ((struct fapi_l1_bch_pdu *) dl_pdu, idx_DLCHANDESC ++, api);
                break;
            case MCH_PDU_TYPE:
                ret |= conv_dlSubframeDescriptor_add_MCH
                    ((struct fapi_l1_mch_pdu *) dl_pdu, idx_DLCHANDESC ++, api);
                break;

            case DLSCH_PDU_TYPE:
                ret |= conv_dlSubframeDescriptor_add_DLSCH
                    ((struct fapi_l1_dlsch_pdu *) dl_pdu, idx_DLCHANDESC ++, api);
                break;

            case PCH_PDU_TYPE:
                ret |= conv_dlSubframeDescriptor_add_PCH
                    ((struct fapi_l1_pch_pdu *) dl_pdu, idx_DLCHANDESC ++, api);
                break;

                // B.2 DCICHANNELDESC
            case DCI_DL_PDU_TYPE:
                ret |= conv_dlSubframeDescriptor_add_DCI
                    ((struct fapi_l1_dci_dl_pdu *) dl_pdu, idx_DCICHANNELDESC ++, api);
                break;
            default:
                BUG();
            }
        }
    }

    // Marking last channel description with magic 4
    if (idx_DLCHANDESC > 0){
        api->dlCh[idx_DLCHANDESC -1].offsetNextCh = 4;
    }

    if (idx_DCICHANNELDESC > 0){
        PDCICHANNELDESC dciCh = (PDCICHANNELDESC)((U8*)api + api->offsetDCIChannels);
        dciCh[idx_DCICHANNELDESC -1].offsetNextCh = 4;
    }


    // (C) DLCMNTXPWRCTL
    dlCmnPwrCtl = (PDLCMNTXPWRCTL)((U8*)(api) + api->offsetPowerCtrl);
    dlCmnPwrCtl->pilotPower = fapi->TransmissionPowerFor_PCFICH; // ??
    dlCmnPwrCtl->psyncPower = 0;
    dlCmnPwrCtl->ssyncPower = 0;
    dlCmnPwrCtl->ciPower    = 0;
    dlCmnPwrCtl->paprControl   =  DISABLED;
    dlCmnPwrCtl->paprThreshold =  0x0000;

    *msgSize = api->offsetPowerCtrl + sizeof (DLCMNTXPWRCTL);

    return ret;
}

//// RX_VEX //// RX_VEX //// RX_VEX //// RX_VEX //// RX_VEX //// RX_VEX ////

// Table 48: CQI_RI Information
static struct fapi_l1_cqi_ri_information CQI_RI_INFORMATION = {
    0, // .DL_CQI_PMI_SizeRank_Eq_1
    0, // .DL_CQI_PMI_SizeRank_Gt_1
    0, // .RI_Size
    0, // .DeltaOffset_CQI
    0, //.DeltaOffset_RI
};

static struct fapi_l1_harq_information HARQ_INFORMATION = {
    0, // .HARQ_Size
    0, // .DeltaOffset_HARQ
    0, // .ACK_NACK_Mode
};

static struct fapi_l1_initial_transmission_parameters  INITIAL_TRANSMISSION_PARAMETERS = {
    0, // .N_SrsInitial = 1,
    0, // .InitialNumberOfResourceBlocks = 100,
};

static __inline int
conv_UlChannelDescriptor_add_ULSCH (struct UL_PDU_Configuration * ul_pdu_config,
                                    int ulChIdx,
                                    PULSUBFRDESC api)
{
    int ret = 0;
    struct fapi_l1_ulsch_pdu                       * from_ulsch_pdu = NULL;
    struct fapi_l1_cqi_ri_information              * from_cqi_ri_info = &CQI_RI_INFORMATION;
    struct fapi_l1_harq_information                * from_harq_info = &HARQ_INFORMATION;
    struct fapi_l1_initial_transmission_parameters * from_initial_trans_param = &INITIAL_TRANSMISSION_PARAMETERS;
    struct cl_ue_conf *ue_conf = NULL;
    PULCHANDESC pCh = &api->ulCh[ulChIdx];

    // (A) Desassemble FAPI
    switch (ul_pdu_config->PDU_Type) {
    case ULSCH_PDU_TYPE: {
        from_ulsch_pdu = (struct fapi_l1_ulsch_pdu *) &ul_pdu_config->UL_PDU[0];
    } break;

    case ULSCH_CQI_RI_PDU_TYPE: {
        struct fapi_l1_ulsch_cqi_ri_pdu * pdu =
            (struct fapi_l1_ulsch_cqi_ri_pdu *)  &ul_pdu_config->UL_PDU[0];
        from_ulsch_pdu = &pdu->ULSCH_PDU;
        from_cqi_ri_info = &pdu->CQI_RI_Information;
        from_initial_trans_param = &pdu->InitialTransmissionParameters;
    } break;

    case ULSCH_HARQ_PDU_TYPE:{
        struct fapi_l1_ulsch_harq_pdu * pdu =
            (struct fapi_l1_ulsch_harq_pdu *)  &ul_pdu_config->UL_PDU[0];
        from_ulsch_pdu = &pdu->ULSCH_PDU;
        from_harq_info = &pdu->HARQ_Information;
        from_initial_trans_param = &pdu->InitialTransmissionParameters;
    } break;
    case ULSCH_CQI_HARQ_RI_PDU_TYPE:{
        struct fapi_l1_ulsch_cqi_harq_ri_pdu * pdu =
            (struct fapi_l1_ulsch_cqi_harq_ri_pdu *)  &ul_pdu_config->UL_PDU[0];
        from_ulsch_pdu = &pdu->ULSCH_PDU;
        from_cqi_ri_info = &pdu->CQI_RI_Information;
        from_harq_info = &pdu->HARQ_Information;
        from_initial_trans_param = &pdu->InitialTransmissionParameters;
    } break;
    default:
        BUG();
    }

    // (B) Build API UlChannelDescriptor
    pCh->offsetNextCh = sizeof(ULCHANDESC);
    pCh->channelId = api->numberOfCtrlChannelDescriptors + ulChIdx;
    pCh->txpowerControl = 0; //??
    pCh->persistEnable = DISABLED; // ??
    pCh->repeatCycle = 0; // ??
    pCh->channelType = PUSCH; // ??? UCI 5 - PUSCH, 7 - PUCCH


    ue_conf = cl_find_ue_conf(from_ulsch_pdu->Handle); // FIXME: Handle -> RNTI
    // (B.1) Save channle to Handler mapping
    link_rx_request_info(get_current_rx_subframe_layer (),
                         pCh->channelId,
                         from_ulsch_pdu->Handle,
                         from_ulsch_pdu->RNTI,
                         ULSCH_PDU_TYPE);

#ifdef AC_MSPD_TDD
    pCh->freqHoppingEnable = from_ulsch_pdu->FrequencyHoppingEnabledFlag;
    pCh->ulHoppingBits = from_ulsch_pdu->FrequencyHoppingBits;
    ; pCh->padding = 0;
#endif	/* defined AC_MSPD_TDD */
    pCh->halfIterations = 0x10; //??

    // (C) TIMADVERRINF
    // Information managed by PHY and only referenced MAC
    pCh->timAdvErrInfo.mErrAvgCh[0] = 0;
    pCh->timAdvErrInfo.mErrAvgCh[1] = 0;
    pCh->timAdvErrInfo.mErrExpo = 0;    
    pCh->timAdvErrInfo.mSetDefaults = 1;

    // (B.1) UlSubChannelDescriptor
    {
        PMCINFO mcinfo           = &pCh->ulSubChInfo.mcinfo;
        PHARQINFO harqInfo       = &pCh->ulSubChInfo.harqInfo;
        PCRCINFO crcInfo         = &pCh->ulSubChInfo.crcInfo;
        PSCRAMBLERINFO scrInfo   = &pCh->ulSubChInfo.scrInfo;
        PPUSCHDED puschDed       = &pCh->ulSubChInfo.puschDed;
        PCQIPMIRIRPT cqiPmiRiRpt = &pCh->ulSubChInfo.cqiPmiRiRpt;
        PMAPPINGINFO mapInfo     = &pCh->ulSubChInfo.mapInfo;

        pCh->ulSubChInfo.reserved = 0;

        mcinfo->codingDescriptor = TURBOCDR; //??
        mcinfo->blockCodeConcatenation = DISABLED; //??
        mcinfo->modulationType = aModulationMap [from_ulsch_pdu->ModulationType / 2];
        mcinfo->mcsType = 0; //?? OLG: need to calculate in UL SCH DCI

        ; harqInfo->reserved = 0;
        harqInfo->nDi = from_ulsch_pdu->NewDataIndication;
        harqInfo->rV = from_ulsch_pdu->RedundancyVersion;
        
        harqInfo->flushReq = 0; // Fix latter in HARQ block

        crcInfo->crcLength = TWTYFOUR;
        crcInfo->crcScrambling = DISABLED; //??

        scrInfo->scramblerType = ULSCR;
        scrInfo->scrinitValueinput = from_ulsch_pdu->RNTI;

        puschDed->betaOffsetACKIndex = from_harq_info->DeltaOffset_HARQ;
        puschDed->betaOffsetRIIndex = from_cqi_ri_info->DeltaOffset_RI;
        puschDed->betaOffsetCQIIndex = from_cqi_ri_info->DeltaOffset_CQI;
        puschDed->nDMRS2 = from_ulsch_pdu->CyclicShift2For_DMRS;

        puschDed->nACK = from_harq_info->HARQ_Size;
        puschDed->nrbi = from_initial_trans_param->InitialNumberOfResourceBlocks;
        puschDed->nRI = from_cqi_ri_info->RI_Size;
        puschDed->nr1CQI = from_cqi_ri_info->DL_CQI_PMI_SizeRank_Eq_1;
        puschDed->nrg1CQI = from_cqi_ri_info->DL_CQI_PMI_SizeRank_Gt_1;
        puschDed->nsymi = 0xC; // OLG OK; 2*Nsym -1 - SRS ? 1:0
#ifndef AC_MSPD_TDD
        ; puschDed->reserved = 0;
#else	/* !( AC_MSPD_TDD ) */
        puschDed->ackNackReportMode :1;
        puschDed->Nbundled :8;
#endif

        cqiPmiRiRpt->cqiReportModeAperiodic = 0; //??
        cqiPmiRiRpt->nomPDSCHRSEPREOffset = 0; //?? offset -1
        cqiPmiRiRpt->cqiReportPeriodicEnable = DISABLED; //??
        cqiPmiRiRpt->cqiPUCCHResourceIndex = ue_conf->CQI_PUCCH_ResourceIndex;
        cqiPmiRiRpt->cqipmiConfigIndex = ue_conf->CQI_PMI_ConfigIndex;
        cqiPmiRiRpt->cqiFormatIndicatorPeriodic = 0; //??
        cqiPmiRiRpt->K = 0; //??
        cqiPmiRiRpt->riConfigIndex = ue_conf->CQI_RI_ConfigIndex;
        cqiPmiRiRpt->simultaneousAckNackAndCQI = ue_conf->CQI_SimultaneousAckNackAnd_CQI;
        ; cqiPmiRiRpt->padding = 0;

        memset(mapInfo, 0, sizeof(MAPPINGINFO)); 
        mapInfo->numberofEntries = 1; // FIXME: 
        mapInfo->reselmInfo[0].startRes = 1;
        mapInfo->reselmInfo[0].numRes = 3;
        // NB: MCS and Mapping are important
    }

    return ret;
}


static struct fapi_l1_cqi_information CQI_INFORMATION = {
    0, // .PUCCH_Index = 1185,
    0, // .DL_CQI_PMI_Size = 255,
};

static struct fapi_l1_sr_information SR_INFORMATION = {
    0, // .PUCCH_Index = 42,
};

static struct fapi_l1_harq_information_fdd HARQ_INFORMATION_FDD = {
    0, // .PUCCH_Index = 0,
    1, // .HARQ_Size  = 1,
};

static __inline int
conv_UlCtrlChannelDescriptor_add_UCI (struct UL_PDU_Configuration * ul_pdu_config,
                                      int ulCtrlChIdx,
                                      PULSUBFRDESC api)
{
    int ret = 0;
    uint32_t Handle = CL_UE_HND_POISON;
    uint16_t RNTI = 0xFFFF;
    struct fapi_l1_cqi_information *from_cqi_info = &CQI_INFORMATION;
    struct fapi_l1_sr_information  *from_sr_info   = &SR_INFORMATION;
    struct fapi_l1_harq_information_fdd /*FIXME: tdd*/ *from_harq_info = &HARQ_INFORMATION_FDD;
    struct cl_ue_conf *ue_conf = NULL;
    uint8_t sr_and_harq_present = FALSE;

    PULCTRLCHDESC pChArray = (PULCTRLCHDESC) (((uint8_t*)api) + api->offsetULCtrlChannels);

    PULCTRLCHDESC pCh = &pChArray[ulCtrlChIdx];

    // (A) Desassemble FAPI
    switch (ul_pdu_config->PDU_Type) {
    case UCI_CQI_PDU_TYPE: {
        struct fapi_l1_uci_cqi_pdu * pdu =
            (struct fapi_l1_uci_cqi_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_cqi_info = &pdu->CQI_Information;
    } break;
    case UCI_SR_PDU_TYPE:{
        struct fapi_l1_uci_sr_pdu * pdu =
            (struct fapi_l1_uci_sr_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_sr_info = &pdu->SR_Information;
    } break;
    case UCI_HARQ_PDU_TYPE:{
        struct fapi_l1_uci_harq_pdu * pdu =
            (struct fapi_l1_uci_harq_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_harq_info = &pdu->HARQ_Information.fdd;
    } break;
    case UCI_SR_HARQ_PDU_TYPE:{
        struct fapi_l1_uci_sr_harq_pdu * pdu =
            (struct fapi_l1_uci_sr_harq_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_sr_info = &pdu->SR_Information;
        from_harq_info = &pdu->HARQ_Information.fdd;
        sr_and_harq_present = TRUE;
    } break;
    case UCI_CQI_HARQ_PDU_TYPE:{
        struct fapi_l1_uci_cqi_harq_pdu * pdu =
            (struct fapi_l1_uci_cqi_harq_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_cqi_info = &pdu->CQI_Information;
        from_harq_info = &pdu->HARQ_Information.fdd;
    } break;
    case UCI_CQI_SR_PDU_TYPE:{
        struct fapi_l1_uci_cqi_sr_pdu * pdu =
            (struct fapi_l1_uci_cqi_sr_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_cqi_info = &pdu->CQI_Information;
        from_sr_info = &pdu->SR_Information;
    } break;
    case UCI_CQI_SR_HARQ_PDU_TYPE:{
        struct fapi_l1_uci_cqi_sr_harq_pdu * pdu =
            (struct fapi_l1_uci_cqi_sr_harq_pdu *) &ul_pdu_config->UL_PDU[0];
        Handle = pdu->Handle; RNTI = pdu->RNTI;
        from_cqi_info = &pdu->CQI_Information;
        from_sr_info = &pdu->SR_Information;
        from_harq_info = &pdu->HARQ_Information.fdd;
        sr_and_harq_present = TRUE;
    } break;
    default:
        BUG();
    }

    if ((ue_conf = cl_find_ue_conf(Handle)) != 0)
        return 1; // FIXME: allow ULSCH programming without UE

    // (B) Build API UlCtrlChannelDescriptor
    pCh->offsetNextCh = sizeof(ULCTRLCHDESC);
    pCh->channelId = ulCtrlChIdx;
    pCh->txpowerControl = 0;//??
    pCh->crcLength = 0; // NONE
    pCh->channelType = PUCCH;
    ; pCh->reserved = 0;
    pCh->scrmblerInitValue = RNTI; //?? 36.211 5.4.1
    pCh->codingDescriptor = CODEUCI; //??
    pCh->blockCodeConcatenation = DISABLED;
    pCh->modulationType = BPSK;
    pCh->mcsType = 0; //??

    // (D) Resource allocation
    pCh->numberofEntries = 1;
    pCh->startRes = 2; //??
    pCh->numRes = 1; //??

    // (C) TIMADVERRINF
    // FIXME: connect to RxSDU
    pCh->timAdvErrInfo.mErrAvgCh[0] = 0;
    pCh->timAdvErrInfo.mErrAvgCh[1] = 0;
    pCh->timAdvErrInfo.mErrExpo = 0;
    pCh->timAdvErrInfo.mSetDefaults = 1;

    // (B.1) PUCCHDEDCTL
    {
        PPUCCHDEDCTL pucchDedCtl = &pCh->pucchDedCtl;
        pucchDedCtl->formatType = 0; //?? Format 0
        pucchDedCtl->ackNackRepetition = DISABLED; // ??
	pucchDedCtl->simSRHarq = sr_and_harq_present;
        pucchDedCtl->repetitionFactor = ue_conf->AN_RepetitionFactor; //??
        pucchDedCtl->msPucchSrSel = 0; //?? no docs
        pucchDedCtl->n1PucchANRep = ue_conf->AN_n1PUCCH_ANRep;
        pucchDedCtl->cqiPUCCHResourceIndex = from_cqi_info->PUCCH_Index;
        pucchDedCtl->srPUCCHResourceIndex = from_sr_info->PUCCH_Index;
        pucchDedCtl->dlCqiPmiSizeBits = from_cqi_info->DL_CQI_PMI_Size;
        pucchDedCtl->harqPucchIndex = from_harq_info->PUCCH_Index;
        pucchDedCtl->harqSizebits = from_harq_info->HARQ_Size;
        pucchDedCtl->n1PUCCHResourceIndex = 0; //?? what is N1?
        ; pucchDedCtl->reserved = 0;
        // FIXME: TDD
    }

    // (B.1) CQIPMIRIRPT
    {
        PCQIPMIRIRPT cqiPmiRiRpt = &pCh->cqiPmiRiRpt;
        cqiPmiRiRpt->cqiReportModeAperiodic = 0; //??
        cqiPmiRiRpt->nomPDSCHRSEPREOffset = 0; //?? offset -1
        cqiPmiRiRpt->cqiReportPeriodicEnable = DISABLED; //??
        cqiPmiRiRpt->cqiPUCCHResourceIndex = ue_conf->CQI_PUCCH_ResourceIndex;
        cqiPmiRiRpt->cqipmiConfigIndex = ue_conf->CQI_PMI_ConfigIndex;
        cqiPmiRiRpt->cqiFormatIndicatorPeriodic = 0; //??
        cqiPmiRiRpt->K = 1; //??
        cqiPmiRiRpt->riConfigIndex = ue_conf->CQI_RI_ConfigIndex;
        cqiPmiRiRpt->simultaneousAckNackAndCQI = ue_conf->CQI_SimultaneousAckNackAnd_CQI;
        ; cqiPmiRiRpt->padding = 0;
    }
    return ret;
}


static __inline int
conv_SoundingRefDedicated_add_SRS (struct fapi_l1_srs_pdu * srs_pdu,
                                   int ulSrsIdx,
                                   PULSUBFRDESC api)
{
    int ret = 0;

    PSRSDED pSRSArray = (PSRSDED) (((uint8_t*)api) + api->offsetsrsInfo);
    PSRSDED pCh = &pSRSArray[ulSrsIdx];

    /* FIXME: link to UL indication
    uint32_t Handle;
    // An opaque handling returned in the SRS.indication
    uint16_t Size;
    // The size of the PDU in bytes.
    uint16_t RNTI;
    */

    pCh->enableDisable       = ENABLED;
    pCh->srsBandwidth        = srs_pdu->SRS_Bandwidth;
    pCh->srsHoppingBandwidth = srs_pdu->SRS_HoppingBandwidth;
    pCh->transmissionComb    = srs_pdu->TransmissionComb;
    pCh->cyclicShift         = srs_pdu->SoundingReferenceCyclicShift;
    pCh->freqDomainPosition  = srs_pdu->FrequencyDomainPosition;
    pCh->duration            = FALSE;
    pCh->srsConfigIndex      = srs_pdu->ISRS_SRS_ConfigIndex;


    return ret;
}

static __inline int
conv_ulSubframeDescriptor_release_HARQ
                    (struct fapi_l1_harq_buffer_pdu * harq_pdu, PULSUBFRDESC api)
{
    int ret = 0;
    return ret;
}

static __inline int
enumerate_UL_CONFIG_channels (struct fapi_l1_ul_config_request * fapi,
                              U8* numberofDataChannelDescriptors,
                              U8* numberOfCtrlChannelDescriptors,
                              U8* numberSrsinSf)
{
    int i, numberHarq = 0;
    *numberofDataChannelDescriptors = 0;
    *numberOfCtrlChannelDescriptors = 0;
    *numberSrsinSf = 0;

    for (i = 0; i < fapi->NumberOf_PDUs; i++) {
        switch (fapi->UL_PDU_Configuration[i].PDU_Type) {
        case ULSCH_PDU_TYPE:
        case ULSCH_CQI_RI_PDU_TYPE:
        case ULSCH_HARQ_PDU_TYPE:
        case ULSCH_CQI_HARQ_RI_PDU_TYPE:
            (*numberofDataChannelDescriptors) ++;
            break;

        case UCI_CQI_PDU_TYPE:
        case UCI_SR_PDU_TYPE:
        case UCI_HARQ_PDU_TYPE:
        case UCI_SR_HARQ_PDU_TYPE:
        case UCI_CQI_HARQ_PDU_TYPE:
        case UCI_CQI_SR_PDU_TYPE:
        case UCI_CQI_SR_HARQ_PDU_TYPE:
            (*numberOfCtrlChannelDescriptors) ++;
            break;

        case SRS_PDU_TYPE:
            (*numberSrsinSf) ++;
            break;

        case HARQ_BUFFER_PDU_TYPE:
            numberHarq ++;
            break;

        }
    }

    BUG_ON(fapi->NumberOf_PDUs != numberHarq
           + (*numberSrsinSf)
           + (*numberOfCtrlChannelDescriptors)
           + (*numberofDataChannelDescriptors));

    return numberHarq;
}

int
conv_ulSubframeDescriptor_from_UL_CONFIG_request(struct fapi_l1_ul_config_request * fapi,
                                                 PULSUBFRDESC api, size_t *msgSize)
{
    int ret = 0, /* numberHarq, */ idx_ULCHANDESC, idx_ULCTRLCHDESC, idx_SRSDED;
    l1_sfn_sf_t sfn_sf;
    struct UL_PDU_Configuration * ul_pdu_config;
    PRACHCTRL rachCtrl;
    U32 struOffset = 0;
    U8 numberofDataChannelDescriptors = 0;

    // (A) UlSubframeDescriptor
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();

    sfn_sf.sfn_sf = fapi->SFN_SF;

    api->frameNumber = sfn_sf.val.sfn;
    api->subframeNumber = sfn_sf.val.sf;
    api->subframeType = ULRX;

    CLDBG("SFN/SF RX.v %d/%d", api->frameNumber, api->subframeNumber);

    api->antennaPortcount = phy_config->RxAntennaPorts;

    /* numberHarq = */ enumerate_UL_CONFIG_channels (fapi,
                                                     &numberofDataChannelDescriptors,
                                                     &api->numberOfCtrlChannelDescriptors,
                                                     &api->numberSrsinSf);
    api->numberofChannelDescriptors =
        numberofDataChannelDescriptors + api->numberOfCtrlChannelDescriptors;

    // (A.1) allocate offsets to the CTRL, SRS and RACH
    struOffset = offsetof(ULSUBFRDESC, ulCh);
    struOffset += numberofDataChannelDescriptors * sizeof(ULCHANDESC);
    /// past fixed part

    api->offsetULCtrlChannels = api->numberOfCtrlChannelDescriptors == 0 ? 0 : struOffset;
    struOffset += api->numberOfCtrlChannelDescriptors * sizeof(ULCTRLCHDESC);

    api->offsetsrsInfo = api->numberSrsinSf == 0 ? 0 : struOffset;
    struOffset += api->numberSrsinSf * sizeof(SRSDED);

    api->offsetRachCtrlStruct = struOffset;
    api->offsetCustomFeatures = 0;

    // (B) ULSubframeCommonControl - ULSUBFRCMNCTRL
    // (B.1) PUCCH Config
    api->ulSfrCtrl.deltaPUCCHShift = phy_config->Delta_PUCCH_Shift;
    api->ulSfrCtrl.nRBCQI          = phy_config->N_CQI_RB;
    api->ulSfrCtrl.nCSAn           = phy_config->N_AN_CS;
    api->ulSfrCtrl.n1PucchAN       = phy_config->N1Pucch_AN;
    api->ulSfrCtrl.pad             = 0;
    // (B.2) SRS Config
    api->ulSfrCtrl.srsBandwitdhConfig                 = phy_config->SRS_BandwidthConfiguration;
    api->ulSfrCtrl.srsSubframeConfig                  = phy_config->SRS_SubframeConfiguration;
    api->ulSfrCtrl.ackNackSRSSimultaneousTransmission = phy_config->SRS_AckNack_SRS_SimultaneousTransmission;
    // (B.3) PUSCH Config
    api->ulSfrCtrl.nSB                = phy_config->NumberOfSubBands;
    api->ulSfrCtrl.hoppingMode        = phy_config->HoppingMode;
    api->ulSfrCtrl.puschhoppingOffset = phy_config->HoppingOffset;


    api->ulSfrCtrl.enable64QAM = 0; // ??

    // (B.4) Uplink Reference Signal Config
    api->ulSfrCtrl.groupHoppingEnabled    =
        phy_config->Uplink_RS_Hopping     == RS_GROUP_HOPPING ? 1 : 0;
    api->ulSfrCtrl.groupAssignmentPUSCH   = phy_config->GroupAssignmentDeltaSequence_ShiftPattern;
    api->ulSfrCtrl.sequenceHoppingEnabled =
        phy_config->Uplink_RS_Hopping     == RS_SEQUENCE_HOPPING ? 1 : 0;
    api->ulSfrCtrl.cyclicShift            = phy_config->CyclicShift1For_DMRS;
    ; api->ulSfrCtrl.padding              = 0;

    idx_ULCHANDESC = idx_ULCTRLCHDESC = idx_SRSDED = 0;
    {
        ul_pdu_for_each_entry(fapi, ul_pdu_config) {
            uint8_t *ul_pdu = &ul_pdu_config->UL_PDU[0];
            ul_pdu_assert_bounds (fapi, ul_pdu_config);

            switch (ul_pdu_config->PDU_Type) {

                // (C) ULCHANDESC
            case ULSCH_PDU_TYPE:
            case ULSCH_CQI_RI_PDU_TYPE:
            case ULSCH_HARQ_PDU_TYPE:
            case ULSCH_CQI_HARQ_RI_PDU_TYPE:
                ret |= conv_UlChannelDescriptor_add_ULSCH (ul_pdu_config, idx_ULCHANDESC ++, api);
                break;
                // (D) ULCTRLCHDESC
            case UCI_CQI_PDU_TYPE:
            case UCI_SR_PDU_TYPE:
            case UCI_HARQ_PDU_TYPE:
            case UCI_SR_HARQ_PDU_TYPE:
            case UCI_CQI_HARQ_PDU_TYPE:
            case UCI_CQI_SR_PDU_TYPE:
            case UCI_CQI_SR_HARQ_PDU_TYPE:
                ret |= conv_UlCtrlChannelDescriptor_add_UCI
                    (ul_pdu_config, idx_ULCTRLCHDESC ++, api);
                break;

                // (E) SRSDED
            case SRS_PDU_TYPE:
                //     = fapi->SRS_Present; // FIXME:
                ret |= conv_SoundingRefDedicated_add_SRS
                    ((struct fapi_l1_srs_pdu *) ul_pdu, idx_SRSDED ++, api);
                break;

            case HARQ_BUFFER_PDU_TYPE:
                  // FIXME: might need API populated
                ret |= conv_ulSubframeDescriptor_release_HARQ
                    ((struct fapi_l1_harq_buffer_pdu *) ul_pdu, api);
                break;

            default:
                BUG();
            }
        }
    }

    // Marking last channel description with magic 4
    if (idx_ULCHANDESC > 0)
        api->ulCh[idx_ULCHANDESC -1].offsetNextCh = 4;

    if (idx_ULCTRLCHDESC > 0)
        ((PULCTRLCHDESC)
         ((U8*)api + api->offsetULCtrlChannels))[idx_ULCTRLCHDESC -1].offsetNextCh = 4;

    /* IGNORE  if (idx_SRSDED > 0) */
    /*     ((PSRSDED) */
    /*      ((U8*)api + api->offsetsrsInfo))[idx_SRSDED -1].offsetNextCh = 4; */

    // (F) RACHCTRL
    if (fapi->RACH_PRACH) {        
        rachCtrl = (PRACHCTRL)((U8*)(api) + api->offsetRachCtrlStruct);
        rachCtrl->prachEnable = fapi->RACH_PRACH;
        rachCtrl->rootSequenceIndex = 0;
        rachCtrl->prachConfigIndex = 0;
        rachCtrl->highSpeedFlag = 0;
        rachCtrl->zeroCorrelationZoneConfig = 0;
        rachCtrl->prachFreqOffset = 0;
        rachCtrl->prachPreambleNumber = 0;
        rachCtrl->pad = 0;
        *msgSize = api->offsetRachCtrlStruct + sizeof (RACHCTRL);
    }
    // fixme: msg size + 4
    return ret;
}


int
conv_HiandDciUlMessageDescriptor_from_HI_DCI0_request(struct fapi_l1_hi_dci0_request * fapi,
                                                      PHIADCIULMSGDESC hiadci)
{
    int ret = 0;
    l1_sfn_sf_t sfn_sf;
    sfn_sf.sfn_sf = fapi->SFN_SF;

    hiadci->msgLength = sizeof(HIADCIULMSGDESC);
    hiadci->msgType = PHY_TXHIADCIUL_REQ;
    hiadci->phyEntityId = 0;
    hiadci->numDciUlSdus = fapi->NumberOf_DCI;
    hiadci->numHiSdus = fapi->NumberOf_HI ;

    hiadci->frameNumber = sfn_sf.val.sfn;
    hiadci->subFrameNumber = sfn_sf.val.sf;

    hiadci->reserved = 0;

    return ret;
}

int
conv_HiInfoDescriptor_from_HI_PDU(struct DCI_HI_PDU_Configuration *ul_pdu,
                                  PHIINFOMSGDESC hi_info)
{
    int ret = 0;
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    struct fapi_l1_hi_pdu * fapi = (struct fapi_l1_hi_pdu *)ul_pdu->DCI_HI_PDU;

    hi_info->msgLength = sizeof(HIINFOMSGDESC);
    hi_info->msgType = PHY_TXHISDU_REQ;
    hi_info->phyEntityId = 0;

    hi_info->nackAck = fapi->HI_Value;
    hi_info->transmissionMode = phy_config->TxAntennaPorts == 1 ? 0 : 1;

    hi_info->phichSequenceIndex = fapi->CyclicShift2For_DMRS; //??
    hi_info->phichGroupNumber = fapi->ResourceBlockStart; //??
    hi_info->channelId = 0; //??

    hi_info->reserved = 0;

    hi_info->txPowerControl = fapi->TransmissionPower;

    return ret;
}

int
conv_DciUlSduHeaderDescriptor_from_DCI_UL_PDU(struct DCI_HI_PDU_Configuration *ul_pdu,
                                              PDCIULSDUMSG dci_info, U32 *msgSize)
{
    int ret = 0;
    struct lte_cphy_config * phy_config = lte_cphy_get_config ();
    struct fapi_l1_dci_ul_pdu * fapi = (struct fapi_l1_dci_ul_pdu *)ul_pdu->DCI_HI_PDU;

    dci_info->msgLength = sizeof(DCIULSDUMSG);
    dci_info->msgType = PHY_TXDCIULSDU_REQ;
    dci_info->phyEntityId = 0;

    dci_info->channelId = 0; //?

    dci_info->numPayloadBytes = 0; //??
    dci_info->numBitsDciUL = 0; //?/

    dci_info->rnti = fapi->RNTI;
    dci_info->numCCE = fapi->AggregationLevel;
    dci_info->startCCE = fapi->CCE_Index;
    dci_info->transmissionMode = phy_config->TxAntennaPorts == 1 ? 0 : 1;
    dci_info->txPowerControl = fapi->TPC;
    dci_info->pTxSdu = 0; //?? make sdu here

    return ret;
}


//// UPLINK //// UPLINK //// UPLINK //// UPLINK //// UPLINK ////

// ?? FAPI_L1_RX_SR_IND
int
conv_RX_XX_indication_from_RxStatusInd (PRXSTATUSIND api,
                                        fapi_l1_msg_hdr_t *fapi)
{
    int ret = 0;
    uint16_t msgLength = 0;

    switch (api->statusType) {
    case PRACH_RESULT: {
        int ev_idx;
        PRX_PRACH_STATUS_EVENT rx_prachi_ev = (PRX_PRACH_STATUS_EVENT) &api->pEventList;

        // (A) PhyRxStatusIndDescriptor
        struct fapi_l1_rach_indication * rach_ind = (struct fapi_l1_rach_indication *) fapi;
        rach_ind->SFN_SF.val.sfn = api->frameNum;
        rach_ind->SFN_SF.val.sf  = api->subFrameNum;
        rach_ind->NumberOfPreambles = api->eventNumber;

        // (B) PhyPrachStatusEventDescriptor
        for (ev_idx = 0; ev_idx < api->eventNumber; ev_idx++) {
            PRX_PRACH_STATUS_EVENT prach = &rx_prachi_ev [ev_idx];
            struct fapi_l1_rach_preamble * preamble = &rach_ind->preambles[ev_idx];

            preamble->RNTI = 0; // ??
            preamble->Preamble = prach->prachPreambleId;
            preamble->TimingAdvance = prach->timingOffset;
        }

        l1_msg_alloc_generic (FAPI_L1_RACH_IND, fapi, sizeof(struct fapi_l1_rach_indication)
                              + rach_ind->NumberOfPreambles * sizeof(struct fapi_l1_rach_preamble));

    } break;
    case CQIRIHI_RESULT: {
        int ev_idx;
        PRX_CQIRIHI_STATUS_EVENT rx_cqi_ev = (PRX_CQIRIHI_STATUS_EVENT) &api->pEventList;

        // (A) PhyRxStatusIndDescriptor
        struct fapi_l1_rx_cqi_indication * rx_cqi_ind = (struct fapi_l1_rx_cqi_indication *) fapi;
        rx_cqi_ind->SFN_SF.val.sfn = api->frameNum;
        rx_cqi_ind->SFN_SF.val.sf  = api->subFrameNum;
        rx_cqi_ind->NumberOf_PDUs = api->eventNumber;

        // (B) PhyCqiRiHiIndDescriptor
        for (ev_idx = 0; ev_idx < api->eventNumber; ev_idx++) {
            PRX_CQIRIHI_STATUS_EVENT rx_cqirihi_ev = &rx_cqi_ev [ev_idx];
            struct fapi_l1_rx_cqi_pdu * rx_cqi_pdu = &rx_cqi_ind->PDUs[ev_idx];

            rx_cqi_pdu->Handle = 0; //FIXME: ??
            rx_cqi_pdu->RNTI = 0;  //??

            rx_cqi_pdu->Length = 0; //??
            // Length of PDU in bytes.
            rx_cqi_pdu->DataOffset = 0; //??
            // Gives the PDU#i data address offset from the beginning of the
            // 'Number of PDUs' field.
            // An offset of 0 indicates a CRC or decoding error, or only RI
            // received on PUSCH.

            rx_cqi_pdu->UL_CQI = rx_cqirihi_ev->cqi_pmi_pt;
            rx_cqi_pdu->RI = rx_cqirihi_ev->ri_pt;
            rx_cqi_pdu->TimingAdvance = 0; //See RXSDU??

            // ?? BIT_FIELD  harq_pt:4; // FIXME: OLG HARQ RANK HI
            //    BIT_FIELD  status_len:8;
            //    BIT_FIELD  chanId:16;

            // FIXME: add payload
        }

        l1_msg_alloc_generic (FAPI_L1_RX_CQI_IND, fapi,
                              sizeof(struct fapi_l1_rx_cqi_indication)
                              + rx_cqi_ind->NumberOf_PDUs * sizeof(struct fapi_l1_rx_cqi_pdu));
    } break;
    case SRS_RESULT: {
        int ev_idx;
        PRX_SRS_STATUS_EVENT rx_sts = (PRX_SRS_STATUS_EVENT) &api->pEventList;
        // (A) PhyRxStatusIndDescriptor
        struct fapi_l1_srs_indication * srs_ind = (struct fapi_l1_srs_indication*) fapi;
        msgLength = sizeof(struct fapi_l1_srs_indication);
        srs_ind->SFN_SF.val.sfn = api->frameNum;
        srs_ind->SFN_SF.val.sf  = api->subFrameNum;
        srs_ind->NumberOf_UEs = api->eventNumber;

        // (B) PhySrsStatusEventDescriptor
        for (ev_idx = 0; ev_idx < api->eventNumber; ev_idx++) {
            U8 rb4;
            struct fapi_l1_ue_srs_config * ue_srs_conf = &srs_ind->UEs[ev_idx];

            ue_srs_conf->Handle = rx_sts->srsIndex; // FIXME: create SRS context
            ue_srs_conf->RNTI = 0; // FIXM: rnti poison
            
            ue_srs_conf->DopplerEstimation = 0; // ?? // FFS. Values: 0  255,
            ue_srs_conf->TimingAdvance = rx_sts->timingAdvance;

            ue_srs_conf->NumberOfResourceBlocks = rx_sts->numberRb;
            ue_srs_conf->RB_Start = rx_sts->rbStart;

            // (C) PhySrsStatusEventDescriptor
            for (rb4 = 0; rb4 < rx_sts->numberRb / 4; rb4 += 4) {
                U8  *psnr4 = &rx_sts->psnr[rb4];
                struct fapi_l1_rb_snr_config * RB4 = &ue_srs_conf->RBs[rb4];
                RB4[0].SNR = psnr4[3];
                RB4[1].SNR = psnr4[2];
                RB4[2].SNR = psnr4[1];
                RB4[3].SNR = psnr4[0];
            }

            msgLength += sizeof(struct fapi_l1_ue_srs_config) +
                ue_srs_conf->NumberOfResourceBlocks * sizeof(struct fapi_l1_rb_snr_config);
            // (B.1) go next status
            rx_sts = (PRX_SRS_STATUS_EVENT) ((uint8_t*)rx_sts
                                             + offsetof(RX_SRS_STATUS_EVENT, psnr)
                                             + rx_sts->numberRb * sizeof(U8));
        }

        l1_msg_alloc_generic (FAPI_L1_SRS_IND, (fapi_l1_msg_hdr_t *) srs_ind, msgLength);
    } break;
    default:
        BUG();
    }

    return ret;
}


/**
 * Construct RX.indication from RxSduInd
 *
 * @param api PHY RX SDU
 * @param fapi SR indication or PUSCH
 * @param cqi_ind CQI indication (not NULL in PUCCH)
 * @param crc_ind CRC indication if not NULL (conditional on configured DataReportnigMode)
 *
 * @return errors
 */
void estimateSize_RX_ULSCH_indication_from_RxSduInd (PRXSDUIND api,
                                                     uint16_t *fapi_msg_size,
                                                     uint16_t *cqi_ind_size,
                                                     uint16_t *crc_ind_size) {
    uint16_t DataLength = (api->numBitsRx + 7) >> 3;
    
    *fapi_msg_size = *cqi_ind_size = *crc_ind_size = 0;
    
    switch (api->chanType) {
    case PUCCH: {
        *cqi_ind_size = sizeof(struct fapi_l1_rx_cqi_indication)
            + sizeof(struct fapi_l1_rx_cqi_pdu) + DataLength;
        
        *fapi_msg_size = sizeof(struct fapi_l1_rx_sr_indication) + sizeof(struct fapi_l1_SR_config);
    } break;
    case PUSCH: {
        *fapi_msg_size = sizeof(struct fapi_l1_rx_ulsch_indication)
            + sizeof(struct fapi_l1_ulsch_pdu_config) + DataLength;
    } break;
    default:
        BUG();
    }

    *crc_ind_size = sizeof(struct fapi_l1_crc_indication) + sizeof(struct fapi_l1_crc_config);    
}

int conv_RX_ULSCH_indication_from_RxSduInd (PRXSDUIND api,
                                            fapi_l1_msg_hdr_t *fapi,
                                            struct fapi_l1_rx_cqi_indication *cqi_ind,
                                            struct fapi_l1_crc_indication *crc_ind)
{
    int ret = 0;
    l1_sfn_sf_t SFN_SF;

    SFN_SF.val.sfn = api->frameNum;
    SFN_SF.val.sf = api->subFrameNum;

    // (A) Send CRC.indication
    if (crc_ind) {
        struct fapi_l1_crc_config * crc_config = crc_ind->CRCs;
        crc_ind->SFN_SF = SFN_SF;
        crc_ind->NumberOf_CRCs = 1;

        // FIXME
        crc_config->Handle = 0; // The handle received in the ULSCH PDU.
        crc_config->RNTI = 0;

        switch (api->status) {
        case 0:
            crc_config->CRC_Flag = CRC_CORRECT;
            break;
        case 9:
            crc_config->CRC_Flag = CRC_ERROR;
            break;
        default:
            BUG();
        }

        l1_msg_alloc_generic (FAPI_L1_CRC_IND, (fapi_l1_msg_hdr_t *) crc_ind,
                              sizeof(struct fapi_l1_crc_indication) + sizeof(struct fapi_l1_crc_config));
    }

    switch (api->chanType) {
    case PUCCH: {
        // (B) Handle CQI information
        struct fapi_l1_rx_cqi_pdu *PDU = cqi_ind->PDUs;
        void * rx_subframe_info = get_prev_rx_subframe_layer (SFN_SF);
        uint32_t Handle = 0;
        uint16_t RNTI = 0;

        cqi_ind->NumberOf_PDUs = 1;
        cqi_ind->SFN_SF = SFN_SF;

        BUG_ON(cqi_ind == NULL);
        if (! rx_subframe_info) {
            CLDBG("PUSCH SFN/SF: %d/%d", SFN_SF.val.sfn, SFN_SF.val.sf);
            BUG();
            return 1;
        }
        unlink_rx_request_info (rx_subframe_info, api->channelId,
                                &Handle, &RNTI, UCI_CQI_SR_PDU_TYPE);

        PDU->Handle = Handle;
        PDU->RNTI = RNTI;
        PDU->UL_CQI = api->ul_CQI;
        PDU->RI = 0; // ??
        PDU->TimingAdvance = api->timingAdv;

        switch (api->status) {
        case 0:
            PDU->Length = (api->numBitsRx + 7) >> 3;
            PDU->DataOffset = (uint8_t*)&PDU[1] - (uint8_t*)&cqi_ind->NumberOf_PDUs;
            memcpy((uint8_t*)&cqi_ind->NumberOf_PDUs + PDU->DataOffset, &api->pRxSdu, PDU->Length);
            break;
        case 9: // CRC ERROR
            PDU->Length = 0;
            PDU->DataOffset = 0;
            break;
        default:
            BUG();
        }
            

        l1_msg_alloc_generic (FAPI_L1_RX_CQI_IND, (fapi_l1_msg_hdr_t *) cqi_ind,
                              sizeof(struct fapi_l1_rx_cqi_indication)
                              + sizeof(struct fapi_l1_rx_cqi_pdu)
                              + PDU->Length);

        // (B) If PUCCH detected
        if(api->pucchDetected){ // it's DEAD_ZONE pucch, trash it for now

            if (api->srdetected) {
                struct fapi_l1_rx_sr_indication * rx_sr_ind = (struct fapi_l1_rx_sr_indication *) fapi;
                struct fapi_l1_SR_config * SR = rx_sr_ind->SRs;
                    
                rx_sr_ind->SFN_SF = SFN_SF;
                rx_sr_ind->NumberOf_SRs = 1;

                SR->Handle = Handle;
                SR->RNTI = RNTI;

                l1_msg_alloc_generic (FAPI_L1_RX_SR_IND, (fapi_l1_msg_hdr_t *) rx_sr_ind,
                                      sizeof(struct fapi_l1_rx_sr_indication)
                                      + sizeof(struct fapi_l1_SR_config));
            }
        }

    } break;
    case PUSCH: {
        // (C) Handle PUSCH
        struct fapi_l1_rx_ulsch_indication * ulsch_ind = (struct fapi_l1_rx_ulsch_indication *) fapi;
        struct fapi_l1_ulsch_pdu_config * PDU = ulsch_ind->RX_Config;
        void * rx_subframe_info = get_prev_rx_subframe_layer (SFN_SF);
        uint32_t Handle;
        uint16_t RNTI;
        
        // (C.1) PhyRxSduIndDescriptor
        ulsch_ind->SFN_SF = SFN_SF.sfn_sf;
        ulsch_ind->NumberOf_PDUs = 1;
        
        // (C.2) Unmap channelId -> Handle
        if (! rx_subframe_info) {
            CLDBG("PUSCH SFN/SF: %d/%d", SFN_SF.val.sfn, SFN_SF.val.sf);
            BUG();
            return 1;
        }
        unlink_rx_request_info (rx_subframe_info, api->channelId,
                                &Handle, &RNTI, ULSCH_PDU_TYPE);

        PDU->Handle = Handle;
        PDU->RNTI = RNTI;
        PDU->UL_CQI = api->ul_CQI;
        PDU->TimingAdvance = api->timingAdv;

        // (C.3) Gives the PDU#i data address offset from the beginning of the'Number of PDUs' field.
        // An offset of 0 indicates a CRC or decoding error.
        //
        switch (api->status) {
        case 0:
            PDU->Length = (api->numBitsRx + 7) >> 3;
            PDU->DataOffset = (uint8_t*)&PDU[1] - (uint8_t*)&ulsch_ind->NumberOf_PDUs;
            memcpy((uint8_t*)&ulsch_ind->NumberOf_PDUs + PDU->DataOffset, &api->pRxSdu, PDU->Length);
            /// We put data at the end of first PDU
            break;
        case 9: // CRC ERROR
            PDU->Length = 0;
            PDU->DataOffset = 0;
            break;
        default:
            BUG();
        }

        PDU->DataOffset = (uint8_t*)&PDU[1] - (uint8_t*)&ulsch_ind->NumberOf_PDUs;

        memcpy((uint8_t*)&ulsch_ind->NumberOf_PDUs + PDU->DataOffset, &api->pRxSdu, PDU->Length);
        
          /* IGNORE: Idx = pRxSduInd->chanType - PUSCH; */
          /*     testConfConfig.timAdvErrInfo[Idx].mErrAvgCh[0] = pRxSduInd->mErrAvgCh[0]; */
          /*     testConfConfig.timAdvErrInfo[Idx].mErrAvgCh[1] = pRxSduInd->mErrAvgCh[1]; */

        l1_msg_alloc_generic (FAPI_L1_RX_ULSCH_IND, (fapi_l1_msg_hdr_t *) ulsch_ind,
                              sizeof(struct fapi_l1_rx_ulsch_indication)
                              +sizeof(struct fapi_l1_ulsch_pdu_config)
                              +PDU->Length);

    } break;
    default:
        BUG();
    }

    return ret;
}

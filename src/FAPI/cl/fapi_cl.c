
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "resultcodes.h"
#include "appprintf.h"
#include "supervisor.h"
#include "mlog.h"
#include "tcb.h"

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>
#include <l1_msg_alloc.h>
#include <l1_sdu_combinator.h>

#include "fapi_cl_cphy_sap.h"
#include "fapi_cl_conv.h"
#include "fapi_cl_phy_iface.h"
#include "fapi_cl_mac_iface.h"

#include "fapi_cl.h"

#ifdef PHY_FAPI_TESTING

const char * PHY_ID_STR [] =
{
    "PHY_TXSTART_REQ    ", /* 1 */
    "PHY_TXSTART_CONF   ", /* 2 */
    "PHY_TXSTART_IND    ", /* 3 */
    "PHY_TXSDU_REQ      ", /* 4 */
    "PHY_TXSDU_CONF     ", /* 5 */
    "PHY_TXEND_IND      ", /* 6 */
    "PHY_RXSTART_REQ    ", /* 7 */
    "PHY_RXSTART_CONF   ", /* 8 */
    "PHY_RXSTART_IND    ", /* 9 */
    "PHY_RXSDU_IND      ", /* 10 */
    "PHY_RXEND_IND      ", /* 11 */
    "PHY_INIT_REQ       ", /* 12 */
    "PHY_INIT_IND       ", /* 13 */
    "PHY_RXSTATUS_IND   ", /* 14 */
    "PHY_RECONFIG_REQ   ", /* 15 */
    "PHY_RECONFIG_CNF   ", /* 16 */
    "PHY_START_REQ      ", /* 17 */
    "PHY_START_CONF     ", /* 18 */
    "PHY_STOP_REQ       ", /* 19 */
    "PHY_STOP_CONF      ", /* 20 */
    "PHY_STOP_IND       ", /* 21 */
    "PHY_TXHIADCIUL_REQ ", /* 22 */
    "PHY_TXHISDU_REQ    ", /* 23 */
    "PHY_TXDCIULSDU_REQ ", /* 24 */
};

#endif

#define TX_SUBFRAME_LAYERS_NUMBER 2
#define RX_SUBFRAME_LAYERS_NUMBER 9
#define MAX_RX_CHANNEL_MAP 3

struct fapi_cl_info {
    l1_sfn_sf_t cl_sfn_sf;
    // Curent subframe number in CL
    // Must be synchronized with MAC whos SF master and PHY - SF slave
    // *** Downlink programming context ***
    struct fapi_cl_tx_request_info {
        struct fapi_cl_dl_pdu_info dl_pdu_info[NUMBER_OF_PDUS_MAX];
        uint16_t PRACH_expected;
        // Flagged if PRACH was programmed
        uint16_t RXSDU_expected;
        // Flagged if SDU reception programmed
    } tx_subframe_layers [TX_SUBFRAME_LAYERS_NUMBER];
    // Data layer per SF which holds PDU programming fro DL_CONFIG.request
    // to the TX.request.
     // New layer used for each SF. Data layer freed after PHY done with MAC PDU's.

    int current_tx_layer_index;
    // Data layer index

    // *** Uplink programming context ***
    struct fapi_cl_rx_request_info {
        struct fapi_cl_rx_request_info * prev;
        struct fapi_cl_rx_request_info * next;

        l1_sfn_sf_t sfn_sf;

        // Channel map for ULSCH, UCI_CQI_SR, SRS
        struct channel_map {
            uint32_t handleOfChannel [MAXCHSUBFRAME];
            uint16_t rntiOfHandle [MAXCHSUBFRAME];
        } channel_map [MAX_RX_CHANNEL_MAP];

    } rx_subframe_layers [RX_SUBFRAME_LAYERS_NUMBER];

    struct fapi_cl_rx_request_info * current_rx_layer;

    uint32_t msgBuf[16 * 1024 / sizeof(uint32_t)];
    // API message buffer for MAC indications
};

/**
 *   This structured holds CL global context
 */
struct fapi_cl_info cl_info;

int cl_init (void)
{
    int i;
    cl_info.cl_sfn_sf.sfn_sf = 0;

    CLDBG("START");

    // TX
    memset(&cl_info.tx_subframe_layers[0], 0, sizeof(cl_info.tx_subframe_layers));
    cl_info.current_tx_layer_index = 0;

    // RX
    for (i = 0; i < RX_SUBFRAME_LAYERS_NUMBER; ++i)
    {
        struct fapi_cl_rx_request_info * sf_info = &cl_info.rx_subframe_layers [i];

        sf_info->sfn_sf.sfn_sf = 0;
        memset(sf_info->channel_map, 0, sizeof(sf_info->channel_map));

        if (i == 0) {
            sf_info->prev = &cl_info.rx_subframe_layers [RX_SUBFRAME_LAYERS_NUMBER -1];
        } else {
            sf_info->prev = &cl_info.rx_subframe_layers [i-1];
        }

        if (i == RX_SUBFRAME_LAYERS_NUMBER -1) {
            sf_info->next = &cl_info.rx_subframe_layers [0];
        } else {
            sf_info->next = &cl_info.rx_subframe_layers [i+1];
        }
    }
    cl_info.current_rx_layer = &cl_info.rx_subframe_layers[0];

    return 0;
}

void * get_current_rx_subframe_layer () {
    return cl_info.current_rx_layer;
}

void * go_next_rx_subframe_layer (l1_sfn_sf_t sfn_sf) {

    cl_info.current_rx_layer = cl_info.current_rx_layer->next;
    cl_info.current_rx_layer->sfn_sf.sfn_sf = sfn_sf.sfn_sf;

    return cl_info.current_rx_layer;
}

void * get_prev_rx_subframe_layer (l1_sfn_sf_t sfn_sf) {
    struct fapi_cl_rx_request_info * p = cl_info.current_rx_layer;

    do {
        if (p->sfn_sf.sfn_sf == sfn_sf.sfn_sf)
            return p;
        p = p->prev;
    } while (p != cl_info.current_rx_layer);

    return NULL;
}

__inline struct channel_map * rx_channel_map (struct fapi_cl_rx_request_info * rx_info,
                                            UL_PDU_Type_e pdu_type) {
    switch (pdu_type) {
    case ULSCH_PDU_TYPE:      return &rx_info->channel_map[0];
    case UCI_CQI_SR_PDU_TYPE: return &rx_info->channel_map[1];
    case SRS_PDU_TYPE:        return &rx_info->channel_map[2];
    default: BUG();
    }
    return NULL;
}

void link_rx_request_info(void * rx_info, uint16_t channelId,
                          uint32_t Handle, uint16_t RNTI, UL_PDU_Type_e pdu_type)
{
    struct channel_map * map = rx_channel_map (rx_info, pdu_type);

    map->handleOfChannel[channelId] = Handle;
    map->rntiOfHandle[Handle] = RNTI;
}

void unlink_rx_request_info (void * rx_info, uint16_t channelId, uint32_t *Handle,
                             uint16_t *RNTI, UL_PDU_Type_e pdu_type)
{

    struct channel_map * map = rx_channel_map (rx_info, pdu_type);

    *Handle = map->handleOfChannel[channelId];
    map->handleOfChannel[channelId] = 0;

    *RNTI = map->rntiOfHandle[*Handle];
    map->rntiOfHandle[*Handle] = 0;
}

/// Advance to next, fresh data layer
static void flip_subframe_layer ()
{
    cl_info.current_tx_layer_index ++;
    if (cl_info.current_tx_layer_index == TX_SUBFRAME_LAYERS_NUMBER)
        cl_info.current_tx_layer_index = 0;
}

/// Return current data layer
static struct fapi_cl_tx_request_info * get_current_subframe_layer () {
    return &cl_info.tx_subframe_layers[cl_info.current_tx_layer_index];
}

/// Return previous data layer
 static struct fapi_cl_tx_request_info * get_prev_subframe_layer ()
{
    int prev_idx = cl_info.current_tx_layer_index -1;

    if (prev_idx < 0) prev_idx = TX_SUBFRAME_LAYERS_NUMBER -1;

    return &cl_info.tx_subframe_layers[prev_idx];
}


/**
 * Allocate TXSDU incurrent data layer.  Uses PDU index as array index assuming
 * that it will reset to 0 each subframe on MAC side.
 */
PTXSDUREQ fapi_cl_add_dl_sdu (uint8_t subframeNumber,
                              uint16_t Length, uint16_t PDU_Index)
{
    struct fapi_cl_dl_pdu_info * pdu;
    struct fapi_cl_tx_request_info * current_layer = get_current_subframe_layer();
    BUG_ON(!(PDU_Index < NUMBER_OF_PDUS_MAX));

    pdu = &current_layer->dl_pdu_info[PDU_Index];
    pdu->Length = Length;
    pdu->PDU_Index = PDU_Index;


    pdu->Tx_SDU_Desc = (PTXSDUREQ) SvsrAllocMsg();
    return
        pdu->Tx_SDU_Desc;
}

struct fapi_cl_dl_pdu_info *
fapi_cl_get_dl_pdu_info (uint16_t PDU_Index)
{
    struct fapi_cl_tx_request_info * current_layer = get_current_subframe_layer();
    BUG_ON(!(PDU_Index < NUMBER_OF_PDUS_MAX));

    return
        &current_layer->dl_pdu_info[PDU_Index];
}


void data_layer_cleanup (struct fapi_cl_tx_request_info * layer)
{
    int i;
    for (i = 0; i < NUMBER_OF_PDUS_MAX; i++) {
        struct fapi_cl_dl_pdu_info *pdu_info = &layer->dl_pdu_info[i];

        if (pdu_info->PDU_Index) {
            // FIXME: Linear reuse of TX blocks from upper
            BUG_ON(pdu_info->PDU_Index == i);
            CLDBG("[%d] idx %d PDU 0x%x of %d", i, pdu_info->PDU_Index,
                  pdu_info->MAC_PDU, pdu_info->Length);
        }

        pdu_info->PDU_Index = 0;
        pdu_info->Length = 0;
        if (pdu_info->MAC_PDU) {
            SvsrFreeIMem(pdu_info->MAC_PDU);
            pdu_info->MAC_PDU = 0;
        }
        pdu_info->Tx_SDU_Desc = 0;
    }
}

uint16_t cl_set_sfnsf (uint16_t sfn_sf)
{
    cl_info.cl_sfn_sf.sfn_sf = sfn_sf;

    return // and normalize
        cl_increment_sfnsf(0);
}

uint16_t cl_get_sfnsf (void)
{
    return cl_info.cl_sfn_sf.sfn_sf;
}

uint16_t cl_increment_sfnsf (uint16_t incr)
{
    l1_sfn_sf_t ret;
    int8_t k;
    /* positive adjustment
     * requires sf to be wrapped at 10 and sfn to be wrapped at 1024
     */
    ret.sfn_sf = cl_info.cl_sfn_sf.sfn_sf;

    k   = (int8_t) ret.val.sf;
    k   += incr;
    if( k > 9 ) {
        ret.val.sf  = k % 10;
        ret.val.sfn = (ret.val.sfn + 1) % 1024;
    } else {
        ret.val.sf = k;
    }

    return (cl_info.cl_sfn_sf.sfn_sf = ret.sfn_sf);
}

//// UTILS //// UTILS //// UTILS //// UTILS //// UTILS //// UTILS //// UTILS ////
void* fapi_cl_phy_alloc_msg (void) {
    return
        SvsrAllocMsg();
}

int fapi_cl_phy_send_msg (uint8_t message_type, void* data, uint32_t msg_size) {
    return
        MX_OK == FapiSendMsgToPhy (message_type, data, msg_size) ? 0 : -1;
}

//// TX REQ //// TX REQ //// TX REQ //// TX REQ //// TX REQ //// TX REQ ////

static __inline int handle_DL_CONFIG_REQ (ApiParam * pParam, PMAC2PHY_QUEUE_EL pListElem)
{
    int ret = 0;
    struct fapi_l1_dl_config_request * fapi;
    size_t msgSize;
    PDLSUBFRDESC api;
    PGENMSGDESC genHdr;

    CLDBG("ApiParam * pParam 0x%x", pParam);

    fapi = (struct fapi_l1_dl_config_request *) &pParam->ParamData[0];

    BUG_ON (fapi->hdr.msgType != FAPI_L1_DL_CONFIG_REQ);

    pListElem->MessagePtr = (U8*) SvsrAllocMsg();
    if (!pListElem->MessagePtr)
        return -ENOMEM;

    genHdr = (PGENMSGDESC) pListElem->MessagePtr;
    api = (PDLSUBFRDESC) &genHdr[1];

    ret = conv_dlSubframeDescriptor_from_DL_CONFIG_request(fapi, api, &msgSize);

    genHdr->msgSpecific = msgSize;
    genHdr->msgType = PHY_TXSTART_REQ;
    genHdr->phyEntityId = 0;

    pListElem->frameNumber = cl_info.cl_sfn_sf.val.sfn;
    pListElem->MessageType = PHY_TXSTART_REQ;
    pListElem->subframeNumber = cl_info.cl_sfn_sf.val.sf;
    pListElem->MessageLen = msgSize + sizeof (GENMSGDESC);

    return ret;
}

static __inline int handle_UL_CONFIG_REQ (ApiParam * pParam, PMAC2PHY_QUEUE_EL pListElem)
{
    int ret = 0;
    struct fapi_l1_ul_config_request * fapi;
    size_t msgSize;
    PULSUBFRDESC api;
    PGENMSGDESC genHdr;

    CLDBG("ApiParam * pParam 0x%x", pParam);

    fapi = (struct fapi_l1_ul_config_request *) &pParam->ParamData[0];

    BUG_ON (fapi->hdr.msgType != FAPI_L1_UL_CONFIG_REQ);

    pListElem->MessagePtr = (U8*) SvsrAllocMsg();
    if (!pListElem->MessagePtr)
        return -ENOMEM;

    genHdr = (PGENMSGDESC) pListElem->MessagePtr;
    api = (PULSUBFRDESC) &genHdr[1];

    ret |= conv_ulSubframeDescriptor_from_UL_CONFIG_request(fapi, api, &msgSize);

    genHdr->msgSpecific = msgSize;
    genHdr->msgType = PHY_RXSTART_REQ;
    genHdr->phyEntityId = 0;

    pListElem->frameNumber = cl_info.cl_sfn_sf.val.sfn;
    pListElem->MessageType = PHY_RXSTART_REQ;
    pListElem->subframeNumber = cl_info.cl_sfn_sf.val.sf;
    pListElem->MessageLen = msgSize + sizeof (GENMSGDESC);

    return ret;
}

static __inline int handle_TX_REQ (ApiParam * pParam, PMAC2PHY_QUEUE_EL *pList)
{
    int rc = 0, i;
    struct fapi_l1_tx_request * fapi;
    PMAC2PHY_QUEUE_EL current;
    struct fapi_cl_tx_request_info * current_layer = get_current_subframe_layer();

    CLDBG("ApiParam * pParam 0x%x", pParam);

    current = *pList;

    fapi = (struct fapi_l1_tx_request *) &pParam->ParamData[0];


    BUG_ON (fapi->hdr.msgType != FAPI_L1_TX_REQ);

    // (A) For each dl_pdu_config in TX.request
    for (i = 0; i < fapi->NumberOf_PDUs; i++) {

        struct fapi_l1_dl_pdu_config * pdu_conf = &fapi->TX_Config[i];
        struct fapi_cl_dl_pdu_info * pdu_info = &current_layer->dl_pdu_info[pdu_conf->PDU_Index];


        PTXSDUREQ pTxSduReq = pdu_info->Tx_SDU_Desc;
        _ASSERT_PTR(pTxSduReq);

        CLDBG("SDU place sf%d, idx %d ptr 0x%x",
               cl_info.cl_sfn_sf.val.sf, pdu_conf->PDU_Index, pTxSduReq);

        // (A.0) Allocate list element but first
        if (current != *pList) {
            current->Next = (PMAC2PHY_QUEUE_EL) SvsrAllocMsg(/* sizeof(MAC2PHY_QUEUE_EL) */);
            current = current->Next;
            current->Next = NULL;
            current->MessagePtr = NULL;
        }

        // (A.1) Relink MAC PDU to the PTXSDUREQ
        pdu_info->MAC_PDU = (uint8_t *) pdu_conf->tlvs[0].value;
        pTxSduReq->pTxSdu = (U32 *) pdu_info->MAC_PDU;
        pTxSduReq->msgLen = pdu_conf->tlvs[0].length;

        // (A.2) Allocate list element
        current->MessagePtr = (U8*) pTxSduReq;
        current->frameNumber = cl_info.cl_sfn_sf.val.sfn;
        current->MessageType = PHY_TXSDU_REQ;
        current->subframeNumber = cl_info.cl_sfn_sf.val.sf;
        current->MessageLen = sizeof (TXSDUREQ);

        {
            // FIXME: we will try zero-copy minding that we are on I-CORE transport
            // if it doesn't kicks off, allocate MAC SDU from I-CORE
            memcpy(&pTxSduReq[1], (void*)pdu_conf->tlvs[0].value, pdu_conf->tlvs[0].length);
#if 0
            if (pdu_info->MAC_PDU) {
                SvsrFreeIMem(pdu_info->MAC_PDU);
                pdu_info->MAC_PDU = 0;
            }
#endif
            current->MessageLen += pdu_conf->tlvs[0].length;
        }
    }

    *pList = current;
    return rc;
}

static __inline int handle_HI_DCI0_REQ (ApiParam * pParam, PMAC2PHY_QUEUE_EL *pList)
{
    int ret = 0, i;
    struct DCI_HI_PDU_Configuration * ul_pdu;
    struct fapi_l1_hi_dci0_request * fapi
        = (struct fapi_l1_hi_dci0_request *) &pParam->ParamData[0];
    PMAC2PHY_QUEUE_EL current;

    BUG_ON (fapi == 0 || fapi->hdr.msgType != FAPI_L1_HI_DCI0_REQ);

    CLDBG("ApiParam * pParam 0x%x", pParam);

    current = *pList;

    // (A) Create HiandDciUlMessageDescriptor

    {
        PHIADCIULMSGDESC hiadci = (PHIADCIULMSGDESC)current->MessagePtr;
        current->frameNumber = cl_info.cl_sfn_sf.val.sfn;
        current->subframeNumber = cl_info.cl_sfn_sf.val.sf;
        current->MessageType = PHY_TXHIADCIUL_REQ;
        current->MessageLen = sizeof (HIADCIULMSGDESC);

        ret |= conv_HiandDciUlMessageDescriptor_from_HI_DCI0_request(fapi, hiadci);
    }

    // (B) Iterate HI & DCI SDU's; add them to list
    hi_dci0_req_for_each_pdu (i, fapi, ul_pdu) {

        // (B.0) Allocate list element for SDU's
        current->Next = (PMAC2PHY_QUEUE_EL) SvsrAllocMsg(/* sizeof(MAC2PHY_QUEUE_EL) */);
        current = current->Next;
        current->Next = NULL;
        current->MessagePtr = (U8*) SvsrAllocMsg();

        current->frameNumber = cl_info.cl_sfn_sf.val.sfn;
        current->subframeNumber = cl_info.cl_sfn_sf.val.sf;

        switch (ul_pdu->PDU_Type) {
        case HI_PDU_TYPE: {
            current->MessageType = PHY_TXHISDU_REQ;
            current->MessageLen = sizeof(HIINFOMSGDESC);
            ret |= conv_HiInfoDescriptor_from_HI_PDU(ul_pdu, (PHIINFOMSGDESC)current->MessagePtr);
        } break;
        case DCI_UL_PDU_TYPE: {
            U32 msgSize = 0;
            current->MessageType = PHY_TXDCIULSDU_REQ;
            current->MessageLen = sizeof(HIINFOMSGDESC);
            ret |= conv_DciUlSduHeaderDescriptor_from_DCI_UL_PDU(ul_pdu, (PDCIULSDUMSG)current->MessagePtr, &msgSize);
        } break;
        default:
            BUG_ON(0);
        }

    }

    *pList = current;
    return ret;
}

/**
 * Process incomming API messages
 *
 * @param pClientCtx - CL application context
 * @param pApi - API message
 *
 * @return
 */
int fapi_cl_parse_fapi_tx_data (LPVOID pClientCtx, ApiHeader *pApi)
{
    ApiParam * pParam = NULL;
    int err = 0;
    l1_sfn_sf_t new_sfn_sf;
    PMAC2PHY_QUEUE_EL list_head = NULL, current = NULL, tmp;

    UINT32 start_tick = MxGetTicks();

    CLDBG("ApiHeader *pApi 0x%x", pApi);

    // (A) Use fresh subframe data layer for this SF request
    flip_subframe_layer ();
    // cl_increment_sfnsf (1);

    // (B) Scan all API param and extracts FAPI structures
    // (B.1) Form linked list for PHY iface

    while ((pParam = ApiGetNextParam(pApi, pParam)) != NULL)
    {
        if ((tmp = (PMAC2PHY_QUEUE_EL) SvsrAllocMsg(/* sizeof(MAC2PHY_QUEUE_EL) */)) == NULL) {
            uart_printf("CL, cpu=%d, cannot allocate I-CORE memory\r\n", MxGetCpuID());
            err = -1; break;
        }
          //        CLDBG("alloc list 0x%x", tmp);

        if (current) {
            current->Next = tmp;
            current = current->Next;
        } else {
            list_head = current = tmp;
        }

        current->Next = NULL;
        current->MessagePtr = NULL;

        _ASSERT_PTR(pParam);

        // value after gen header
        new_sfn_sf.sfn_sf = *((uint16_t*)((U8*)&pParam->ParamData[0]
                                          + sizeof(fapi_l1_msg_hdr_t)));
        if (cl_info.cl_sfn_sf.sfn_sf != new_sfn_sf.sfn_sf) {
            CLDBG("cl takes action 'align SFN/SF %d/%d -> %d/%d'",
                  cl_info.cl_sfn_sf.val.sfn, cl_info.cl_sfn_sf.val.sf,
                  new_sfn_sf.val.sfn, new_sfn_sf.val.sf
                );

            cl_set_sfnsf(new_sfn_sf.sfn_sf);
        }


        switch (pParam->ParamID) {
        case FAPI_L1_DL_CONFIG_REQ:{
            err = handle_DL_CONFIG_REQ (pParam, current);
        } break;
        case FAPI_L1_UL_CONFIG_REQ:{
            err = handle_UL_CONFIG_REQ (pParam, current);
        } break;
        case FAPI_L1_TX_REQ:{
            err = handle_TX_REQ (pParam, &current);
        } break;
        case FAPI_L1_HI_DCI0_REQ:{
            err = handle_HI_DCI0_REQ (pParam, &current);
        } break;

        default:
            CLDBG("param msg 0x%x unknown", pParam->ParamID);
            err = -1;
        }

        if (err) break;
    }

    if ( err ) {
        current = list_head;

        while (current) {
            tmp = current;
            current = current->Next;

            if (tmp->MessagePtr) SvsrFreeIMem(tmp->MessagePtr);

            SvsrFreeIMem(tmp);
        }

        data_layer_cleanup(get_current_subframe_layer());

    } else {

        if (FAILED(SvsrSendMsg(0, IID_LTE_EX, list_head, SVSR_MSG_OPT_DEFAULT)))
            uart_printf("Failed to send TX/RX\r\n");

        CLDBG("list () : cl -> PHY");
    }

    data_layer_cleanup(get_prev_subframe_layer());
    MLogTask(PID_CL_PARSE_FAPI_TX_DATA, RESOURCE_LARM, start_tick, MxGetTicks());

    return 0;
}


//// PHY MSG //// PHY MSG //// PHY MSG //// PHY MSG //// PHY MSG ////
int fapi_cl_handle_phy_command(PGENMSGDESC pMsgHdr)
{
    MXRC rc = SUCCESS;
    ApiHeader *pApi = (ApiHeader *) cl_info.msgBuf;
    ApiParam *rspParam = FapiAllocMacMsg(pApi, IID_WIMAX);
    UINT32 start_tick = MxGetTicks();
    fapi_l1_msg_hdr_t *fapi_msg = (fapi_l1_msg_hdr_t *)  &rspParam->ParamData[0];

    CLDBG("PGENMSGDESC pMsgHdr 0x%x", pMsgHdr);

    fapi_msg->msgType = FAPI_L1_MSG_INVALID;
    fapi_msg->length = 0;

    CLDBG("cl handle %s", PHY_MSG(pMsgHdr->msgType));

    switch (pMsgHdr->msgType)
    {
    case PHY_INIT_IND: {
        CLDBG("cl change state PHY_STATE_CONFIGURED");
        lte_cphy_set_state (PHY_STATE_CONFIGURED);
        // FIXME: clea
        SvsrLoggerSetMask (0xffffffff);
    } break;

    case PHY_START_CONF: {
        CLDBG("cl change state PHY_STATE_RUNNING");
        lte_cphy_set_state (PHY_STATE_RUNNING);
    } break;

    case PHY_STOP_IND: {
        CLDBG("cl change state PHY_STATE_RUNNING");
        l1_alloc_stop_indication ((struct fapi_l1_stop_indication *)fapi_msg);
        lte_cphy_set_state (PHY_STATE_CONFIGURED);
    } break;

        // (A.1) SUBFRAME.indication: CL -> MAC
    case PHY_TXSTART_IND: {
        PMSGIND msgInd = (PMSGIND) pMsgHdr;
        uint16_t sfnSf = (msgInd->frameNumber << 4) | msgInd->subFrameNum;

        l1_alloc_subframe_indication (sfnSf, (struct fapi_l1_subframe_indication*) fapi_msg);
    } break;

    default:
        ; // pass
    }

    if (fapi_msg->msgType != FAPI_L1_MSG_INVALID) {
        rc = FapiSendMsgToMac (pApi, fapi_msg->msgType, fapi_msg->length);
    }

    if (SvsrFreeIMem(pMsgHdr) != MX_OK)
        uart_printf("CL ERROR: can't free the RxSdu [%x]\n", rc);

    MLogTask(PID_CL_HANDLE_PHY_COMMAND, RESOURCE_LARM, start_tick, MxGetTicks());

    return rc;
}

int fapi_cl_handle_phy_msg(PGENMSGDESC pMsgHdr, PHY_MSG_Priority_e priority)
{
    MXRC rc = SUCCESS;
    ApiHeader *pApi = (ApiHeader *)cl_info.msgBuf;
    ApiParam *rspParam;
    UINT32 start_tick = MxGetTicks();
    fapi_l1_msg_hdr_t *fapi_msg = NULL;

    CLDBG("PGENMSGDESC pMsgHdr 0x%x", pMsgHdr);
    CLDBG("handle %s", PHY_MSG(pMsgHdr->msgType));

    // (A) Handle PHY UP message
    FapiAllocMacMsg(pApi, IID_LTE_MAC);
    switch (pMsgHdr->msgType)
    {
        // (A.2) This is from PHY indicating end of reception
    case PHY_RXEND_IND: {
        priority = PHY_MSG_DISPATCH;
    } break;
        // (A.3) ULSCH_CQI_HARQ_RI | UCI_CQI_SR_HARQ | SRS | HARQ_BUFFER : CL -> MAC
    case PHY_RXSTATUS_IND: {
        PRXSTATUSIND pRxStatusInd = (PRXSTATUSIND)pMsgHdr;
        rspParam = ApiAddParam(pApi, NULL, PHY_RXSTATUS_IND, NULL, 0);
        fapi_msg = (fapi_l1_msg_hdr_t *)  &rspParam->ParamData[0];

        if (! (conv_RX_XX_indication_from_RxStatusInd (pRxStatusInd, fapi_msg) < 0)) {
            rc = FapiSendMsgToMac (pApi, fapi_msg->msgType, fapi_msg->length);
        }
    } break;
        // (A.4) ULSCH : CL -> MAC
    case PHY_RXSDU_IND: {
        PRXSDUIND pRxSduInd = (PRXSDUIND) pMsgHdr;
        struct fapi_l1_rx_cqi_indication *cqi_ind = NULL;
        struct fapi_l1_crc_indication *crc_ind = NULL;
        uint16_t fapi_msg_size, cqi_ind_size, crc_ind_size;
        //U32 outMsgSize = 0;

        // (A.4.1) Allocate list for PUSCH, PUCCH CQI and CRC indications
        estimateSize_RX_ULSCH_indication_from_RxSduInd (pRxSduInd,
                                                        &fapi_msg_size, &cqi_ind_size, &crc_ind_size);

        rspParam = ApiAddParam(pApi, NULL, PHY_RXSDU_IND, NULL, 0);
        rspParam->ParamLength = fapi_msg_size;

        fapi_msg = (fapi_l1_msg_hdr_t *) rspParam->ParamData;
        pApi->Length += GetParamTotalSize(rspParam);

        if (cqi_ind_size) {
            rspParam = ApiGetNextParam(pApi, rspParam/* , FAPI_L1_RX_CQI_IND, NULL, 0 */);
            rspParam->ParamLength = cqi_ind_size;

            cqi_ind = (struct fapi_l1_rx_cqi_indication *) rspParam->ParamData;
            pApi->Length += GetParamTotalSize(rspParam);
        }

        if (lte_cphy_get_config()->DataReportMode) {
            rspParam = ApiGetNextParam(pApi, rspParam/* , PHY_RXSDU_IND, NULL, 0 */);
            rspParam->ParamLength = crc_ind_size;

            crc_ind = (struct fapi_l1_crc_indication *)rspParam->ParamData;
            pApi->Length += GetParamTotalSize(rspParam);
        }

        // (A.4.2) Parse RXSDU and send list to MAC
        if (! (conv_RX_ULSCH_indication_from_RxSduInd (pRxSduInd, fapi_msg, cqi_ind, crc_ind) < 0)) {
            rc = FapiSendListToMac (pApi);
        }
    } break;

    default:
        ; // pass
    };

    // (B) Exgress messages optimization opportunity
    if (priority == PHY_MSG_DISPATCH) {
        // rc = send_all_to_mac();
    }

    // (C) Free memory of linked list leaf
    if ((rc = SvsrFreeIMem(pMsgHdr)) != MX_OK)
        uart_printf("CL ERROR: cann't free the RxSdu [%x]\n", rc);

    MLogTask(PID_CL_HANDLE_PHY_MSG, RESOURCE_LARM, start_tick, MxGetTicks());

    return rc;
}

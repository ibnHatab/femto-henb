#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_tlv.h>

#include "fapi_cl.h"


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

inline struct channel_map * rx_channel_map (struct fapi_cl_rx_request_info * rx_info,
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


#if 0
/// Advance to next, fresh data layer
static void flip_subframe_layer ()
{
    cl_info.current_tx_layer_index ++;
    if (cl_info.current_layer_index == SUBFRAME_LAYERS_NUMBER)
        cl_info.current_layer_index = 0;
}
#endif

/// Return current data layer
static struct fapi_cl_tx_request_info * get_current_tx_subframe_layer () {
    return &cl_info.tx_subframe_layers[cl_info.current_tx_layer_index];
}


/**
 * Allocate TXSDU incurrent data layer.  Uses PDU index as array index assuming
 * that it will reset to 0 each subframe on MAC side.
 */
PTXSDUREQ fapi_cl_add_dl_sdu (uint8_t subframeNumber,
                              uint16_t Length, uint16_t PDU_Index)
{
    struct fapi_cl_dl_pdu_info * pdu;
    struct fapi_cl_tx_request_info * current_layer = get_current_tx_subframe_layer();
    BUG_ON(!(PDU_Index < NUMBER_OF_PDUS_MAX));

    pdu = &current_layer->dl_pdu_info[PDU_Index];
    pdu->Length = Length;
    pdu->PDU_Index = PDU_Index;

    pdu->Tx_SDU_Desc = (PTXSDUREQ) calloc(1, sizeof(TXSDUREQ));
    return
        pdu->Tx_SDU_Desc;
}
struct fapi_cl_dl_pdu_info *
fapi_cl_get_dl_pdu_info (uint16_t PDU_Index)
{
    struct fapi_cl_tx_request_info * current_layer = get_current_tx_subframe_layer();
    BUG_ON(!(PDU_Index < NUMBER_OF_PDUS_MAX));

    return
        &current_layer->dl_pdu_info[PDU_Index];
}

void data_layer_cleanup (struct fapi_cl_tx_request_info * layer)
{
    int i;
    for (i = 0; i < NUMBER_OF_PDUS_MAX; i++) {
        struct fapi_cl_dl_pdu_info *pdu_info = &layer->dl_pdu_info[i];
        if (pdu_info->MAC_PDU) {
            free(pdu_info->MAC_PDU);
            pdu_info->MAC_PDU = 0;
        }
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

void* fapi_cl_phy_alloc_msg (void) {
    return
        malloc(BUFSIZ);
}

int fapi_cl_phy_send_msg (uint8_t message_type, void* data, uint32_t msg_size) {
    CLDBG("msg: %d of %d bytes", message_type, msg_size);
    return 0;
}

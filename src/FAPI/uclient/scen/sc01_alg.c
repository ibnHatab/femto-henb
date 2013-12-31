#include <sys/time.h>
#include <sys/param.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>

#include <msgroute.h>
#include <icpu.h>
#include <ctrlmsg.h>

#include <l1_l2_iface.h>
#include <l1_msg_alloc.h>
#include <l1_sdu_combinator.h>
#include <l1_types.h>
#include <l1_tlv.h>

#include <fapi_cl.h>
#include <fapi_iface.h>

#include "sc_alg.h"

#include "sc01_data.hh"


int sc01_phy_init_siso ()
{

    int ret = sc_phy_init (CONFIG_REQUEST_TLVS, CONFIG_REQUEST_TLVS_num);
    return ret;
}


int sc01_generate_bch (uint16_t sfn_sf)
{
    sc_sfn_sf.sfn_sf = sc_inc_sfnsf (sc_sfn_sf.sfn_sf, 1);

    CLDBG("!! handle tti: MAC <> PHY %d/%d <> %d/%d\n",
          sc_sfn_sf.val.sfn, sc_sfn_sf.val.sf,
          sfn_sf >> 4, sfn_sf & 0x0F
        );
        PMSGHEADER pHdr;
    if ((pHdr = MsgAllocBuffer(MSG_FAST)) == 0)
        return -ENOMEM;

    PPARAMHEADER param = fapi_alloc_msg (pHdr, MSGS_LTEMAC, MSGT_DATA);

    // (A) Create DL.config
    struct fapi_l1_dl_config_request *dl_conf =
        of_dl_conf(sc_sfn_sf.sfn_sf,
                   dl_conf_add_pdu((uint8_t*) &bch_pdu_data, BCH_PDU_TYPE_SIZE,
                                   format_dl_conf ((uint8_t *)param->data, &dl_bch_config)));

    param = fapi_format_param(pHdr, param, dl_conf->hdr.msgType, dl_conf->hdr.length);

    // (B) UL.config
    struct fapi_l1_ul_config_request *ul_conf =
        of_ul_conf(sc_sfn_sf.sfn_sf,
                   format_ul_conf ((uint8_t *)param->data, &ul_bch_config));

    param = fapi_format_param(pHdr, param, ul_conf->hdr.msgType, ul_conf->hdr.length);

    // (C) MAC SDU
    U8 *macSdu;
    if ((macSdu = (U8*) MsgAllocBuffer(MSG_FAST)) == 0)
        return -ENOMEM;

    memcpy(macSdu, &mac_bch_pdu, sizeof(mac_bch_pdu));
                   
    // (D) Tx.request
    struct fapi_l1_tx_request *tx_req =
        of_tx_req (sc_sfn_sf.sfn_sf,
                   tx_req_add_pdu(1, 1, (uint8_t*) IcpuGetPhys(macSdu), sizeof(mac_bch_pdu),
                                  format_tx_req((uint8_t *)param->data, &tx_bch_request)));
    
    param = fapi_format_param(pHdr, param, tx_req->hdr.msgType, tx_req->hdr.length);

    CLDBG("send data request of %d", pHdr->length);
    if (fapi_send_mesg (pHdr) < 0) {
        CLDBG("error send data request\n");
        return -1; // stop reactor
    }

    return 0;
}

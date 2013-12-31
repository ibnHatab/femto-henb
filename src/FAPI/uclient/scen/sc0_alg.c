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

#include "sc0_data.hh"


int sc0_phy_init_siso ()
{
    int ret = sc_phy_init (SISO_CONFIG_REQUEST_TLVS, SISO_CONFIG_REQUEST_TLVS_num);
    sc_sfn_sf.sfn_sf = 0; //300 << 4;
    return ret;
}


int sc0_sent_empty_vectors (uint16_t SfnSf)
{
    l1_sfn_sf_t dl_sfn_sf, ul_sfn_sf, phy_sfn_sf;;

    // Increment scenario SFN/SF
    sc_sfn_sf.sfn_sf = sc_inc_sfnsf (sc_sfn_sf.sfn_sf, 1);

    dl_sfn_sf.sfn_sf = sc_sfn_sf.sfn_sf;
    ul_sfn_sf.sfn_sf = sfnsf_adjust (sc_sfn_sf.sfn_sf, -2);

    phy_sfn_sf.sfn_sf = SfnSf;


    PMSGHEADER pHdr;
    if ((pHdr = MsgAllocBuffer(MSG_FAST)) == 0)
        return -ENOMEM;

    PPARAMHEADER param = fapi_alloc_msg (pHdr, MSGS_LTEMAC, MSGT_DATA);

    // (A) Create DL.config
    struct fapi_l1_dl_config_request *dl_conf =
        of_dl_conf(dl_sfn_sf.sfn_sf,
                   format_dl_conf ((uint8_t *)param->data, &dl_empty_config));

    param = fapi_format_param(pHdr, param, dl_conf->hdr.msgType, dl_conf->hdr.length);

    // (B) UL.config
    struct fapi_l1_ul_config_request *ul_conf =
        of_ul_conf(ul_sfn_sf.sfn_sf,
                   format_ul_conf ((uint8_t *)param->data, &ul_empty_config));

    param = fapi_format_param(pHdr, param, ul_conf->hdr.msgType, ul_conf->hdr.length);

    // (C) MAC SDU

    // (D) Tx.request
    struct fapi_l1_tx_request *tx_req =
        of_tx_req (sc_sfn_sf.sfn_sf,
                   format_tx_req((uint8_t *)param->data, &tx_empty_request));

    param = fapi_format_param(pHdr, param, tx_req->hdr.msgType, tx_req->hdr.length);

    CLDBG("data(PHY=[%d/%d], UL=[%d/%d], DL=[%d/%d]) : test -> cl\n",
          phy_sfn_sf.val.sfn, phy_sfn_sf.val.sf,
          ul_sfn_sf.val.sfn, ul_sfn_sf.val.sf,
          dl_sfn_sf.val.sfn, dl_sfn_sf.val.sf
        );

    if (fapi_send_mesg (pHdr) < 0) {
        CLDBG("error send data request\n");
        return -1; // stop reactor
    }

    return 0;
}






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
#include <l1_types.h>
#include <l1_tlv.h>

#include <fapi_cl.h>

#include "fapi_iface.h"

/// Global SFN/SF value used in a scenario
l1_sfn_sf_t sc_sfn_sf;

uint16_t sc_inc_sfnsf (uint16_t sfn_sf, uint16_t inc)
{
    l1_sfn_sf_t ret;
    int8_t k;
    /* positive adjustment
     * requires sf to be wrapped at 10 and sfn to be wrapped at 1024
     */
    ret.sfn_sf = sfn_sf;

    k   = (int8_t) ret.val.sf;
    k   += inc;
    if( k > 9 ) {
        ret.val.sf  = k % 10;
        ret.val.sfn = (ret.val.sfn + 1) % 1024;
    } else {
        ret.val.sf = k;
    }

    return ret.sfn_sf;
}

uint16_t sfnsf_adjust( uint16_t sfnsf, int adj )
{
  l1_sfn_sf_t ret;

  /* positive adjustment
   * requires sf to be wrapped at 10 and sfn to be wrapped at 1024
   */
  ret.sfn_sf = sfnsf;

  if( adj >= 0 )
  {

    ret.val.sf   += adj;
    if( ret.val.sf > 9 )
    {
      ret.val.sf  = ret.val.sf % 10;
      ret.val.sfn = (ret.val.sfn + 1) % 1024;
    }
  }
  else
  /* negative adjustment, careful handling of wrapping at zero required */
  {
    if( ret.val.sf < (-adj) )
    {
      ret.val.sf = (ret.val.sf+10) + adj;
      if( ret.val.sfn == 0 )
      {
        ret.val.sfn = 1023;
      }
      else
      {
        ret.val.sfn--;
      }
    }
    else
    {
      ret.val.sf += adj;
    }
  }

  return( ret.sfn_sf );
}


int sc_phy_init (l1_tlv_word_t* conf, size_t conf_size)
{
    int ret = 0;
    struct fapi_l1_config_request* request = 0;
    struct fapi_l1_config_response* rsp = 0;

    // (A) Configure PHY
    PMSGHEADER pHdr;
    if ((pHdr = MsgAllocBuffer (MSG_FAST)) == 0)
        return -ENOMEM;

    PPARAMHEADER param = fapi_alloc_msg (pHdr, MSGS_WIMAXPHY, MSGT_CONFIG);

    request = (struct fapi_l1_config_request*) param->data;
    l1_alloc_config_request (request, conf, conf_size);

    param = fapi_format_param(pHdr, param, request->hdr.msgType, request->hdr.length);

    if ((ret = fapi_send_mesg (pHdr))!= 0) {
        CLDBG(" error send request\n");
        return -1;
    }
    CLDBG("fapi_l1_config_request : test -> cl\n");

    // (B) Wait for reply
    sleep (T_CL_PHY_CONF_TIMEOUT_LONG / 1000000);

    rsp = (struct fapi_l1_config_response*) fapi_receive_mesg ();
    CLDBG("fapi_l1_config_response : cl -> test\n");

    if (! rsp || rsp->hdr.msgType != FAPI_L1_CONFIG_RESP) {
        CLDBG(" SC 1: RCV NO CONFIG_RESP MSG \n");
        return -1;
    }

    if (rsp->errorCode == MSG_INVALID_CONFIG) {
        int i;
        CLDBG(" SC 1: MSG is NOK with error code %d\n", rsp->errorCode);
        CLDBG(" SC 1: invalid config TLVs unsuported (%d)/missing(%d)\n",
               rsp->numberOfInvalidOrUnsupportedTLVs,
               rsp->numberOfMissingTLVs);
        for (i = 0;
             i < rsp->numberOfInvalidOrUnsupportedTLVs + rsp->numberOfMissingTLVs;
             i++) {
            CLDBG(" SC 1: TLV.tag (%d)\n", rsp->tlvs[i].tag);
        }
        return -2;
    }

    CLDBG("PHY Config RSP MSG OK\n");



    // (C) Set initial SFN/SF
    sc_sfn_sf.sfn_sf = 728 << 4;
    
    return 0;
}


int sc_handle_fapi(fapi_l1_msg_hdr_t *genHdr)
{
    int ret = 0;
    switch (genHdr->msgType) {
    case FAPI_L1_STOP_IND     : { printf("recv: %s\n", "STOP_IND"); } break;
    case FAPI_L1_ERROR_IND    : { printf("recv: %s\n", "ERROR_IND"); } break;
    case FAPI_L1_HARQ_IND     : { printf("recv: %s\n", "HARQ_IND"); } break;
    case FAPI_L1_CRC_IND      : { printf("recv: %s\n", "CRC_IND"); } break;
    case FAPI_L1_RX_ULSCH_IND : { printf("recv: %s\n", "RX_ULSCH_IND"); } break;
    case FAPI_L1_SRS_IND      : { printf("recv: %s\n", "SRS_IND"); } break;
    case FAPI_L1_RX_SR_IND    : { printf("recv: %s\n", "RX_SR_IND"); } break;
    case FAPI_L1_RX_CQI_IND   : { printf("recv: %s\n", "RX_CQI_IND"); } break;
    default:
        printf("unknown recv: %d\n", genHdr->msgType);
        ret = -1;
    }

    return ret;
}



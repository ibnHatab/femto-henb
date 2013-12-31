
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/param.h>

#include <time.h>
#include <unistd.h>
#include <poll.h>

#include <msgroute.h>
#include <icpu.h>
#include <ctrlmsg.h>


#include <l1_l2_iface.h>
#include <l1_msg_alloc.h>
#include <l1_types.h>
#include <l1_tlv.h>

#include <fapi_cl.h>

#include "scen/sc_alg.h"

#include "fapi_iface.h"

extern int IcpuHandle;
static uint8_t msg_buff[MSG_MAXSIZE];

extern volatile U32 *ApbTimerReg;

RESULTCODE init_adi (uint32_t configBand, uint32_t configTxAnt, uint32_t configRxAnt,
                     uint32_t configLoopback, uint32_t configDejitter_init)
{
    uint16_t ConSrcID = 0x00ff;
    printf("ADI: Band=%d, TxAnt=%d, RxAnt=%d, Loopback=%d, Dejitter_init=%d\n",
           configBand, configTxAnt, configRxAnt, configLoopback, configDejitter_init);

    return MsgCommand(ConSrcID, MSGS_DI | MSGS_LARM, ADI_BAND, 5, // 5 parameters send to larm
                      1, TYPE_U32, configBand,              // band
                      3, TYPE_U32, configTxAnt,             // txant
                      4, TYPE_U32, configRxAnt,             // rxant
                      5, TYPE_U32, configLoopback,          // loopback
                      2, TYPE_U32, configDejitter_init);    // init dejitter
}

static __inline void api_create (PMSGHEADER pHdr, U16 dstID, U16 msgType, U16 msgID)
{
    pHdr->control = CTRL_DEFAULT;
    pHdr->type = msgType;
    pHdr->length = 0;
    pHdr->srcID = MSGS_MAC;
    pHdr->dstID = dstID | MSGS_LARM;
    pHdr->msgID = msgID;
}

PPARAMHEADER fapi_alloc_msg (PMSGHEADER api, U16 dstID, U16 msgType)
{
    api_create(api, dstID, msgType, PHY_LTE_MSG_EX);

    return (PPARAMHEADER) &api[1];
}

PPARAMHEADER fapi_format_param (PMSGHEADER api, PPARAMHEADER pParam, U16 paramID, U32 paramLength)
{
    pParam->paramID = paramID;
    pParam->length = paramLength;

    api->length += (offsetof(PARAMHEADER, data) + roundup(pParam->length, 4));

    return (PPARAMHEADER) (((char*)pParam->data) + roundup(pParam->length, 4));
}

int fapi_send_mesg (PMSGHEADER pHdr)
{
    return
        MsgDispatch(pHdr->srcID, sizeof(MSGHEADER) + pHdr->length, pHdr);
}

ssize_t read_phy_message(uint8_t * buff, int timeout)
{
    struct pollfd pollFD, *pfd;
    nfds_t nfds = 0;

    pfd = &pollFD;

    pollFD.fd = IcpuHandle;
    pollFD.events = POLLIN;
    pollFD.revents = 0;
    nfds = IcpuHandle + 1;

    if (poll(pfd, nfds, timeout) < 0)
        return -1;

    if ((pollFD.revents & POLLIN) == POLLIN)
        return
            IcpuRxDirect((void *) buff);

    return 0;
}

#define REG_IN_US ((*ApbTimerReg) / 150)

char * fapi_receive_mesg ()
{
    PMSGHEADER pHdr;
    ssize_t len;
    int timeout = 200;
    int start = REG_IN_US;

    while ((REG_IN_US - start) < timeout) {
        if ((len = read_phy_message(msg_buff, 20)) < 0) {
            printf("Error: read from lower\n");
            return 0;
        }

        if (len == 0) continue;

        if (len < sizeof(MSGHEADER)) return 0;

        pHdr = (PMSGHEADER) msg_buff;
        pHdr->srcID &= ~MSGS_LARM;
 
          //        CLDBG(" + %d\n", pHdr->length);

        if (pHdr->srcID == MSGS_WIMAXPHY
            || pHdr->srcID == MSGS_LTEMAC) {
            PPARAMHEADER pParam;
            param_for_each_entry (pHdr, pParam) {
                CLDBG("msg (paramID='0x%x', length='%d'): cl -> test\n", pParam->paramID, pParam->length);

                if (pParam->paramID == FAILURE) {
                    printf("Error: read error (%d) from lower\n", pParam->data[0]);
                    return 0;
                } else {
                    // return first one
                    return  (char*) &pParam->data[0];
                }
            }
        } else {
            printf("Warning: unknown source ID 0x%04X\n", pHdr->srcID);
            return 0;
        }
    }

    return 0;
}

void clean_icpu()
{
    ssize_t len;
    int done = 3;
    while (done--) {
        if ((len = read_phy_message(msg_buff, 20)) < 0) {
            CLDBG("error read from lower\n");
        }
    }
}

/// @param start_request
int send_phy_start_request()
{
    struct fapi_l1_start_request * start_request;
    PMSGHEADER pHdr;
    PPARAMHEADER param;
    // START
    if ((pHdr = MsgAllocBuffer(MSG_FAST)) == 0)
        return -ENOMEM;

    /* PPARAMHEADER */
    param = fapi_alloc_msg (pHdr, MSGS_WIMAXPHY, MSGT_CONFIG);

    start_request = (struct fapi_l1_start_request*) param->data;

    l1_alloc_start_request (start_request);

    fapi_format_param(pHdr, param, start_request->hdr.msgType, start_request->hdr.length);

    if (fapi_send_mesg (pHdr) < 0) {
        CLDBG(" error send start request\n");
        return -3;
    }
    CLDBG("fapi_l1_start_request: test -> cl\n");
    
    return 0;
}

int scenarion_reactor (int tti_number,
                       int (*sc_handle_tti)(uint16_t sfn_sf),
                       int (*sc_handle_api)(fapi_l1_msg_hdr_t *genHdr))
{
    int done = 0;
    fd_set readfds;
    signed int highestFD = 0, retval = 0;
    // struct timeval tv;
    int total_tti = 0;

    send_phy_start_request();

    if (sc_handle_tti)
        if (sc_handle_tti(sc_sfn_sf.sfn_sf) < 0)
            return 1;

    while ( ! done )
    {
        FD_ZERO (&readfds);
        FD_SET (IcpuHandle, &readfds);

        /* tv.tv_sec = 0; */
        /* tv.tv_usec =2000; */

        if (IcpuHandle > highestFD)
            highestFD = IcpuHandle;

        retval = select(highestFD + 1, &readfds, NULL, NULL, NULL/* &tv */);

        if (retval == -1) {
            printf("Error: select() < 0");
            break;
        } else if (retval == 0) {
            usleep (50);
              //            printf(".");
            continue;
        }

        if (! FD_ISSET(IcpuHandle, &readfds))
            continue;

        CLDBG("Data is available now.\n");

        int nBytes = IcpuRxDirect(msg_buff);

        if (nBytes < sizeof(MSGHEADER)) {
            printf("Error: recv insuficient bytes %d", nBytes);
            break;
        }

        PMSGHEADER pHdr = (PMSGHEADER) msg_buff;

        pHdr->srcID &= ~MSGS_LARM;

        CLDBG("msg(msgIs=0x%x, len=%d) : cl -> test", pHdr->msgID, pHdr->length);

        if (pHdr->srcID == MSGS_WIMAXPHY
            || pHdr->srcID == MSGS_LTEMAC) {
            PPARAMHEADER pParam;

            param_for_each_entry (pHdr, pParam) {
                CLDBG("handle paramID=0x%x, len=%d\n", pParam->paramID, pParam->length);

                if (pParam->paramID == FAILURE) {
                    printf("Error: read API error (%d) from lower\n", pParam->data[0]);
                } else {
                    fapi_l1_msg_hdr_t *genHdr = (fapi_l1_msg_hdr_t *) pParam->data;

                    switch (genHdr->msgType)
                    {
                    case FAPI_L1_SUBFRAME_IND: {
                        struct fapi_l1_subframe_indication * si
                            = (struct fapi_l1_subframe_indication *) genHdr;
                        l1_sfn_sf_t sfn_sf;
                        sfn_sf.sfn_sf = si->SFN_SF;

                        CLDBG("Subframe indication: %d/%d\n", sfn_sf.val.sfn, sfn_sf.val.sf);

                        total_tti ++;
                        
                        if (sc_handle_tti)
                            if (sc_handle_tti(si->SFN_SF) < 0)
                                done = 1;
                        
                        if (tti_number) tti_number --;
                        if (tti_number == 1) done = 1;
                    } break;
                    case FAPI_L1_STOP_IND: {
                        printf("Error: STOP IND from lower\n");
                        done = 1;
                    } break;

                    default:
                        if (sc_handle_api)
                            if (sc_handle_api(genHdr) < 0)
                                done = 1;
                    }

                }
            }



        } else {
            printf("Warning: unknown source ID 0x%04X\n", pHdr->srcID);
        }
    }
    
    return total_tti;
}

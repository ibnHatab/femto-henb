#ifndef _FAPI_IFACE_H_
#define _FAPI_IFACE_H_

/**
 * Iterate over parameter starting from message header
 *
 * @param pHdr - message header
 * @param pParam - the parameter * to use as a loop counter
 */
#define param_for_each_entry(pHdr, pParam)                                      \
    for (pParam = (PPARAMHEADER) &pHdr[1];                                      \
        ((char*)pParam - ((char*)pHdr + sizeof (MSGHEADER))) < pHdr->length;    \
        pParam = (PPARAMHEADER)(((char*)pParam)                                 \
                                + offsetof(PARAMHEADER, data)                   \
                                + roundup (pParam->length, 4)))


/** 
 * Initialize ADI chipset
 * 
 * @param configBand           - the LTE band
 * @param configTxAnt          - number of TX antenas
 * @param configRxAnt          - number of RX antenas
 * @param configLoopback       - enable loopback
 * @param configDejitter_init  - initialize radio jitter
 * 
 * @return resultcode
 */
RESULTCODE init_adi (U32 configBand, U32 configTxAnt, U32 configRxAnt,
                     U32 configLoopback, U32 configDejitter_init);

/** 
 * Read out I-CPU interface. Used on startup to clean bus
 * 
 */
void clean_icpu();


/** 
 * Format API header with L-ARM destination
 * 
 * @param api     - * to the msg header
 * @param dstID   - destination instance ID i.e. MSGS_WIMAXPHY for CPHY-SAP or MSGS_LTEMAC for FAPI CL
 * @param msgType - type of message. Popular ones: MSGT_CONFIG, MSGT_DATA
 * 
 * @return one past location for data or first parameter 
 */
PPARAMHEADER fapi_alloc_msg (PMSGHEADER api, U16 dstID, U16 msgType);

/** 
 * Fill in parameter and update message heaser with it size
 * 
 * @param api          - * to the msg header
 * @param pPar         - * to the parameter
 * @param paramID      - parameter ID
 * @param paramLength  - len of data in parameter
 * 
 * @return one past * for next parameter
 */
PPARAMHEADER fapi_format_param (PMSGHEADER api, PPARAMHEADER pPar,
                                U16 paramID, U32 paramLength);

/** 
 * Send message over I-CPU to L-ARM
 * 
 * @param pHdr - * to the message
 * 
 * @return OK if 0
 */
int fapi_send_mesg (PMSGHEADER pHdr);

/** 
 * Read one I-CPU block, look for non error message in parameters
 * 
 * @return message if any ot NULL
 */
char * fapi_receive_mesg ();

/** 
 * Scenario reactor handling input from I-CPU
 * 
 * @param ttis - number of SUBFRAME.indication to run
 * @param sc_handle_tti - callback to DL generator function 
 * @param sc_handle_api - callback to UL processing function 
 * 
 * @return 
 */
int scenarion_reactor (int ttis,
                       int (*sc_handle_tti)(uint16_t sfn_sf),
                       int (*sc_handle_api)(fapi_l1_msg_hdr_t *genHdr));

#endif /* _FAPI_IFACE_H_ */

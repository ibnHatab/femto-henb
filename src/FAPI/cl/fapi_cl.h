#ifndef _FAPI_CL_H_
#define _FAPI_CL_H_

#include <LtePhyL2Api.h>

/**
   Convergency layer connect P4, P5 and P7 interfaces to the MSPD PHY
   
   P4 - the Network Listen Results interface
   P5 - the PHY mode control interface
   P7 - the main datapath interface
   
*/

#define FAPI_CL_PERSIST_UE_INFO_MAX 192
// FIXME: must be LTE user max

#define CL_PHY_TIMER_RESOLUTION 1000  /* conf AUTO_TIMER if non zero otherwise AUTO_PCIE*/
#define CL_PHY_TIMER_MODE_COUNT 50   /* 290 for mlog full */
#define CL_PHY_PHYINIT_CONFIG (PHYINIT_NEW_MAC_TO_PHY_INTERFACE    \
                                | PHYINIT_ROBUST_CONTROL       \
                                | PHYINIT_PHY_MAINTAINS_PBCH   \
        )

/**
  Reconfiguration timeout. Short one user in RUNNING stat, long is for anything
  else.
*/
#define T_CL_PHY_CONF_TIMEOUT_SHORT 20
#define T_CL_PHY_CONF_TIMEOUT_LONG  3000000

#  if MX_ARM_ENABLED
#define BUG() do {                                                      \
	uart_printf("BUG: failure at %s:%d/%s()!\n\r", __FILE__, __LINE__, __func__); \
    } while (0)
#  else	/* !( MX_ARM_ENABLED ) */
#define BUG() do {                                                      \
	printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
    } while (0)
#endif

#define BUG_ON(condition) do { if (condition) BUG(); } while (0)

#ifdef PHY_FAPI_TESTING

#  if MX_ARM_ENABLED
#define CLDBG(spec, args...) do {                                    \
        uart_printf("%08u:CL:%s:%03d\t"spec"\n\r", MxGetTicks(), __func__, __LINE__, ##args ); \
} while (0)

extern const char * PHY_ID_STR [];
#define PHY_MSG(ID) ID [PHY_ID_STR]


#  else	/* !( MX_ARM_ENABLED ) */

extern volatile U32 *ApbTimerReg;
#define GET_TICK *ApbTimerReg

#define CLDBG(spec, args...) do {                             \
        printf("%08u:UH:%s:%03d\t"spec"\n", GET_TICK, __func__,  __LINE__, ##args );  \
} while (0)



#  endif /* MX_ARM_ENABLED */

#else	/* !( defined PHY_FAPI_TESTING ) */

#define CLDBG(spec, args...) do {} while (0)

#endif	/* defined PHY_FAPI_TESTING */ 


#define PID_CL_TTI_IND               30300
#define PID_CL_LTE_API_REQ           30301
#define PID_CL_CPHY_SAP_API_REQ      30302

#define PID_CL_PHY_MSG_SND           30303
#define PID_CL_PHY_MSG_HND           30304
#define PID_CL_PHY_MSG_LIST_HND      30305

#define PID_CL_MAC_MSG_SND           30306

#define PID_CL_PARSE_FAPI_TX_DATA    30307
#define PID_CL_HANDLE_PHY_COMMAND    30308
#define PID_CL_HANDLE_PHY_MSG        30309


/**
 * This structure maps DL config descriptors to the MAC PDU
 * that corresponds it trough uniq index
 */ 
struct fapi_cl_dl_pdu_info {
    uint16_t Length;
    // The length (in bytes) of the associated MAC PDU, delivered in
    // TX.request. This should be the actual length of the MAC
    // PDU, which may not be a multiple of 32-bits.

    uint16_t  PDU_Index;
    // This is a count value which is incremented every time a BCH,
    // MCH, PCH or DLSCH PDU is included in the
    // DL_CONFIG.request message.
    // This value is repeated in TX.request and associates the
    // control information to the data.
    // It is reset to 0 for every subframe
    // Range 0  65535

    uint8_t *MAC_PDU;
    // Pointer to the MAC PDU delivered with TX.request.
    // If not inlined into PHU Tx_SDU_Desc, will be freeed on
    // following TTI.
    
    PTXSDUREQ Tx_SDU_Desc;
    // PHY TxSduReqDescriptor allocated by DL_CONFIG.request for each of BCH,
    // MCH, PCH or DLSCH PDU. 
    // Payload will be attached when DL_PDU_CONFIG with matching
    // PDU_Index fount in TX.request
};

#define NUMBER_OF_PDUS_MAX 514
#define NUMBER_OF_DCIS_MAX 255

#define NOT_SPECIFIED_VALUE (0)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/** 
 * Allocate DL PDU map element
 * 
 * @param Length     - len of actual PDU
 * @param PDU_Index  - association index
 * 
 * @return - newlly allocated PDU
 */
PTXSDUREQ fapi_cl_add_dl_sdu (uint8_t subframeNumber, uint16_t Length, uint16_t PDU_Index);
struct fapi_cl_dl_pdu_info * fapi_cl_get_dl_pdu_info (uint16_t PDU_Index);

/** 
 * Module initialization
 * 
 * 
 * @return - OK if 0
 */
int cl_init (void);


/** 
 * Hard addjust SFN/SF in CL
 * 
 * @param sfn_sf - new SFN/SF
 * 
 * @return new sfn_sf
 */
uint16_t cl_set_sfnsf (uint16_t sfn_sf);
uint16_t cl_get_sfnsf (void);
uint16_t cl_increment_sfnsf (uint16_t inc);



typedef enum {
    PHY_MSG_DELAY = 0,
    PHY_MSG_DISPATCH = 1 
} PHY_MSG_Priority_e;

/**
 * Process API commands to FAPI ones
 * @param pMsgHdr - API message
 * 
 * @return OK if 0
 */
int fapi_cl_handle_phy_command (PGENMSGDESC pMsgHdr);

/**
 * Process PHY UL messages
 * 
 * @param pMsgHdr - API message
 * @param priority - message priority
 * 
 * @return OK if 0
 */
int fapi_cl_handle_phy_msg (PGENMSGDESC pMsgHdr, PHY_MSG_Priority_e priority);

/**
 * Page allocator wraper for system agnostic modules
 * 
 * @return void*
 */
void * fapi_cl_phy_alloc_msg (void);

/**
 * Forward message to the PHY interface
 * 
 * @param message_type - message type
 * @param data         - data
 * @param msg_size     - size
 * 
 * @return int
 */
int fapi_cl_phy_send_msg (uint8_t message_type, void* data, uint32_t msg_size);



/** 
 * Hashing routines that maps UL channel programming and detection 
 * on PHY level with Handlers used by FAPI PHY interface.
 *
 * @param rx_info SF/SFN privete RX information
 * @param channelId API channelId allocated to PUSCH, control chanel or SRS
 * @param Handle Opaque handle used on PHY interface
 * @param RNTI UE id
 * @param pdu_type There are 3 channel to handle maps for different type of channels:
 *                 ULSCH_PDU_TYPE, UCI_CQI_SR_PDU_TYPE, SRS_PDU_TYPE.
 */
void link_rx_request_info(void * rx_info, uint16_t channelId,
                          uint32_t Handle, uint16_t RNTI, UL_PDU_Type_e pdu_type);

void unlink_rx_request_info (void * rx_info, uint16_t channelId, uint32_t *Handle,
                             uint16_t *RNTI, UL_PDU_Type_e pdu_type);

void * get_current_rx_subframe_layer (void);
void * go_next_rx_subframe_layer (l1_sfn_sf_t sfn_sf);
void * get_prev_rx_subframe_layer (l1_sfn_sf_t sfn_sf);


#endif /* _FAPI_CL_H_ */


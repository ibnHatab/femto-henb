#ifndef _FAPI_CL_UE_DB_H_
#define _FAPI_CL_UE_DB_H_

/* 3.2.6.4  UE Configuration TLVs
   The configuration TLVs that are used in the UE_CONFIG message exchanges.
   Type      Description  Table 25: TLV format
   -------------------------------------------
   uint8_t   Tag
   uint8_t   Length (in bytes)
   variable  Value  (bytes % 4 == 0)

   XXX: Note that Value is variable in UE message.
   XXX: Threre is oportunity to optimize encoder using this destinction.
*/

#define CL_UE_PERSIST_INFO_MAX 30

struct cl_ue_conf {
    uint16_t Handle;
    uint16_t RNTI;
    /* CQI Config All TLVs in this grouping */
    uint16_t CQI_PUCCH_ResourceIndex;
    uint16_t CQI_PMI_ConfigIndex;
    uint16_t CQI_RI_ConfigIndex;
    uint8_t  CQI_SimultaneousAckNackAnd_CQI;
    /* ACK/NACK Config All TLVs in this grouping */
    uint8_t  AN_RepetitionFactor;
    uint16_t AN_n1PUCCH_ANRep;
#if 0
    uint8_t  TDD_AckNackFeedbackMode;
#endif
    /* SRS Config All TLVs in this grouping */
    uint8_t  SRS_Bandwidth;
    uint8_t  SRS_Hopping_Bandwidth;
    uint8_t  FrequencyDomainPosition;
    uint8_t  SRS_Duration;
    uint16_t ISRS_SRS_ConfigIndex;
    uint8_t  TransmissionComb;
    uint8_t  SoundingReferenceCyclicShift;
    /* SR Config All TLVs in this grouping */
    uint16_t SR_PUCCH_ResourceIndex;
    uint8_t  SR_ConfigIndex;
    /* SPS Config All TLVs in this grouping */
    uint16_t SPS_DL_ConfigSchedulingInterval;
    uint16_t SPS_DL_n1PUCCH_AN_Persistent;
};

#define CL_UE_HND_POISON 0xFFFF

/** 
 * Initialize module database
 */
int cl_ue_db_init (void);

/** 
 * Process UE configuration request,
 * stores persistent parameters in database.
 * 
 * @param req - request
 * 
 * @return OK if 0, otherwise check for -ENOMEM
 */
int cl_ue_config_req (struct fapi_l1_ue_config_request* req,
                      struct fapi_l1_ue_config_response* response);

/** 
 * Process UE releasr request
 * clean persistent parameters in database.
 * 
 * @param req - request
 * 
 * @return OK if 0, otherwise check for -ENOMEM
 */
int cl_ue_release_req (struct fapi_l1_ue_release_request* req,
    struct fapi_l1_ue_release_response* response);

/** 
 * Find UE configuratio fia opaque handle
 * 
 * @param Handle - handle to find
 * 
 * @return UE config or NULL if invalid handle
 */
struct cl_ue_conf * cl_find_ue_conf (uint16_t Handle);

#endif /* _FAPI_CL_UE_DB_H_ */

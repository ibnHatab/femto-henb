#ifndef _SC_ALG_H_
#define _SC_ALG_H_

/** 
 * SFN/SF management function
 * 
 * @param sfn_sf - in val
 * @param inc - sf intrement
 * 
 * @return out val with increment
 */
uint16_t sc_inc_sfnsf (uint16_t sfn_sf, uint16_t inc);
uint16_t sfnsf_adjust (uint16_t sfnsf, int adj);

extern l1_sfn_sf_t sc_sfn_sf;

/**
 * Generic init
 */
int sc_phy_init (l1_tlv_word_t* conf, size_t conf_size);

/**
 * Generic UPLINK handler
 */
int sc_handle_fapi(fapi_l1_msg_hdr_t *genHdr);

#endif /* _SC_ALG_H_ */

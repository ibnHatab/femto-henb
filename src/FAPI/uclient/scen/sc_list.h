#ifndef _SC_LIST_H_
#define _SC_LIST_H_

struct scenario_defs {
    const char * sc_name;                             /**<  display name */
    int sc_ttis;                                      /**<  number of subframes to run */
    int (*sc_init)(void);                             /**<  Init function*/
    int (*sc_handle_tti)(uint16_t sfn_sf);            /**<  Subframe handling function */
    int (*sc_handle_api)(fapi_l1_msg_hdr_t *genHdr);  /**<  UPLINK handling function */
};

extern struct scenario_defs sc_list[];
extern const int sc_list_size;

int list_scenarios (void);


#endif /* _SC_LIST_H_ */


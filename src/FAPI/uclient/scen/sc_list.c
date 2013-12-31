#include <stdio.h>
#include <stdint.h>

#include <l1_l2_iface.h>

#include "sc_alg.h"

#include "sc_list.h"

// Init
extern int sc0_phy_init_siso (void);
extern int sc01_phy_init_siso ();

// TTI DL handlers
extern int sc0_sent_empty_vectors (uint16_t sfn_sf);
extern int sc01_generate_bch (uint16_t sfn_sf);

// FAPI UL handlers
extern int scenario_handle_fapi(fapi_l1_msg_hdr_t *genHdr);


struct scenario_defs sc_list [] = {
    {"Empty vectors 100 tti",
     60000,
     sc0_phy_init_siso,
     sc0_sent_empty_vectors,
     sc_handle_fapi
    },
    {"BCH generator",
     50,
     sc01_phy_init_siso,
     sc01_generate_bch,
     sc_handle_fapi
    },
};

const int sc_list_size = sizeof(sc_list) / sizeof(struct scenario_defs);

int list_scenarios (void)
{
    int i;
    printf("> Scenario List:\n");
    for (i = 0; i < sc_list_size; i++) {
        printf("\t%02d: '%s' runs %d tti's\n", i, sc_list[i].sc_name, sc_list[i].sc_ttis);
    }
    return i;
}



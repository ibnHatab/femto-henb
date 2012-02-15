#ifndef _TIMERUNIT_H_
#define _TIMERUNIT_H_

#define TTI jiffies

typedef enum {
    FAPI_L2_ARQ_RETRANSMIT = 0,
    FAPI_L2_HARQ_TIMEOUT,    
} fapi_l2_timer_e;

struct kt_data {
    fapi_l2_timer_e type;
	unsigned long period;
	unsigned long start_time;	/* jiffies value when we first started the timer */
};

void timerunit_cb(void);

#endif /* _TIMERUNIT_H_ */

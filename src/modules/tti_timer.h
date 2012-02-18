#ifndef _TTI_TIMER_H_
#define _TTI_TIMER_H_

#include <linux/timer.h>

/**
 *  Simplified timers with TTI resolution.
 *     - supports single group (tvec)
 *     - no lockdep
 *     - no per cpu tvec
 *     - ?
 */

extern int init_tti_timers (void);

#endif /* _TTI_TIMER_H_ */

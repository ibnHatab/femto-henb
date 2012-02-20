#ifndef _LTE_DEBUG_H_
#define _LTE_DEBUG_H_

#ifdef LTE_DEBUG

#define CDBG(msg, args...) do {                                       \
        printk(KERN_DEBUG "CDBG> [%s] " msg , __func__ , ##args );    \
} while (0)

#else
#define CDBG(msg, args...) do {} while (0)
#endif

#endif /* _LTE_DEBUG_H_ */

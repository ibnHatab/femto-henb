#ifndef _TTI_TIMER_H_
#define _TTI_TIMER_H_

#include <linux/list.h>

struct tti_tvec_base;

struct tti_timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	struct list_head entry;
	unsigned long expires;
	struct tti_tvec_base *base;

	void (*function)(unsigned long);
	unsigned long data;
};

#define TTI 1

#define TTI_TIMER_INITIALIZER(_function, _expires, _data) {		\
		.entry = { .prev = TIMER_ENTRY_STATIC },                \
		.function = (_function),			\
		.expires = (_expires),				\
        .data = (_data),				    \
	}

extern int  init_tti_timers_array (unsigned long base_tti);
extern void run_timer_task(unsigned long current_tti);

/**
 * tti_timer_pending - is a timer pending?
 * @timer: the timer in question
 *
 * timer_pending will tell whether a given timer is currently pending,
 * or not.
 
 * return value: 1 if the timer is pending, 0 if not.
 */
static inline int tti_timer_pending(const struct tti_timer_list * timer)
{
	return timer->entry.next != NULL;
}

extern void tti_init_timer(struct tti_timer_list *timer);
extern void tti_add_timer(struct tti_timer_list *timer);
extern int tti_mod_timer(struct tti_timer_list *timer, unsigned long expires);
extern int tti_mod_timer_pending(struct tti_timer_list *timer, unsigned long expires);
extern int tti_del_timer(struct tti_timer_list * timer);


#endif /* _TTI_TIMER_H_ */

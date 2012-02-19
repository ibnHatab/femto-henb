#undef TRACE_SYSTEM
#define TRACE_SYSTEM tti_timer

#define TRACE_INCLUDE_FILE tti_timer_trace
#define TRACE_INCLUDE_PATH .

#ifndef _TTI_TIMER_TRACE_H_
#define _TTI_TIMER_TRACE_H_

#include <linux/module.h>
#include <tti_timer.h>

DECLARE_EVENT_CLASS(tti_timer_class,

	TP_PROTO(struct tti_timer_list *timer),

	TP_ARGS(timer),

	TP_STRUCT__entry(
		__field( void *,	timer	)
	),

	TP_fast_assign(
		__entry->timer	= timer;
	),

	TP_printk("timer=%p", __entry->timer)
);

/**
 * tti_timer_init - called when the timer is initialized
 * @timer:	pointer to struct tti_timer_list
 */
DEFINE_EVENT(tti_timer_class, tti_timer_init,

	TP_PROTO(struct tti_timer_list *timer),

	TP_ARGS(timer)
);

/**
 * tti_timer_start - called when the timer is started
 * @timer:	pointer to struct tti_timer_list
 * @expires:	the timers expiry time
 */
TRACE_EVENT(tti_timer_start,

	TP_PROTO(struct tti_timer_list *timer, unsigned long expires),

	TP_ARGS(timer, expires),

	TP_STRUCT__entry(
		__field( void *,	timer		)
		__field( void *,	function	)
		__field( unsigned long,	expires		)
		__field( unsigned long,	now		)
	),

	TP_fast_assign(
		__entry->timer		= timer;
		__entry->function	= timer->function;
		__entry->expires	= expires;
		__entry->now		= jiffies;
	),

	TP_printk("timer=%p function=%pf expires=%lu [timeout=%ld]",
		  __entry->timer, __entry->function, __entry->expires,
		  (long)__entry->expires - __entry->now)
);

/**
 * tti_timer_expire_entry - called immediately before the timer callback
 * @timer:	pointer to struct tti_timer_list
 *
 * Allows to determine the timer latency.
 */
TRACE_EVENT(tti_timer_expire_entry,

	TP_PROTO(struct tti_timer_list *timer),

	TP_ARGS(timer),

	TP_STRUCT__entry(
		__field( void *,	timer	)
		__field( unsigned long,	now	)
		__field( void *,	function)
	),

	TP_fast_assign(
		__entry->timer		= timer;
		__entry->now		= jiffies;
		__entry->function	= timer->function;
	),

	TP_printk("timer=%p function=%pf now=%lu", __entry->timer, __entry->function,__entry->now)
);

/**
 * tti_timer_expire_exit - called immediately after the timer callback returns
 * @timer:	pointer to struct tti_timer_list
 *
 * When used in combination with the tti_timer_expire_entry tracepoint we can
 * determine the runtime of the timer callback function.
 *
 * NOTE: Do NOT derefernce timer in TP_fast_assign. The pointer might
 * be invalid. We solely track the pointer.
 */
DEFINE_EVENT(tti_timer_class, tti_timer_expire_exit,

	TP_PROTO(struct tti_timer_list *timer),

	TP_ARGS(timer)
);

/**
 * tti_timer_cancel - called when the timer is canceled
 * @timer:	pointer to struct tti_timer_list
 */
DEFINE_EVENT(tti_timer_class, tti_timer_cancel,

	TP_PROTO(struct tti_timer_list *timer),

	TP_ARGS(timer)
);


#endif /* _TTI_TIMER_TRACE_H_ */

/* This part must be outside protection */
#include <trace/define_trace.h>

/*
 *  tti_timer.c
 *
 *  MAC/RLC internal timers
 *
 *  Design based on Kernel internal timers, basic process system calls
 *  : linux/kernel/timer.c
 */

#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/cpu.h>
#include <linux/slab.h>

#include <lte_debug.h>

#include <tti_timer.h>

#define CREATE_TRACE_POINTS
#include <tti_timer_trace.h>

#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVR_SIZE (1 << TVR_BITS)
#define TVR_MASK (TVR_SIZE - 1)

struct tti_tvec_root {
	struct list_head vec[TVR_SIZE];
};

struct tti_tvec_base {
	struct tti_timer_list *running_timer;
	unsigned long timer_jiffies;
	unsigned long next_timer;
	struct tti_tvec_root tv;
} ____cacheline_aligned;

struct tti_tvec_base *_tti_tvec_base = 0;

#undef CONFIG_DEBUG_TTI_TIMERS

#ifdef CONFIG_DEBUG_TTI_TIMERS
static struct debug_obj_descr timer_debug_descr;

static void *timer_debug_hint(void *addr)
{
	return ((struct tti_timer_list *) addr)->function;
}

/*
 * fixup_init is called when:
 * - an active object is initialized
 */
static int timer_fixup_init(void *addr, enum debug_obj_state state)
{
	struct tti_timer_list *timer = addr;

	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		tti_del_timer(timer);
		debug_object_init(timer, &timer_debug_descr);
		return 1;
	default:
		return 0;
	}
}

/*
 * fixup_activate is called when:
 * - an active object is activated
 * - an unknown object is activated (might be a statically initialized object)
 */
static int timer_fixup_activate(void *addr, enum debug_obj_state state)
{
	struct tti_timer_list *timer = addr;

	switch (state) {

	case ODEBUG_STATE_NOTAVAILABLE:
		/*
		 * This is not really a fixup. The timer was
		 * statically initialized. We just make sure that it
		 * is tracked in the object tracker.
		 */
		if (timer->entry.next == NULL &&
		    timer->entry.prev == TIMER_ENTRY_STATIC) {
			debug_object_init(timer, &timer_debug_descr);
			debug_object_activate(timer, &timer_debug_descr);
			return 0;
		} else {
			WARN_ON_ONCE(1);
		}
		return 0;

	case ODEBUG_STATE_ACTIVE:
		WARN_ON(1);

	default:
		return 0;
	}
}

/*
 * fixup_free is called when:
 * - an active object is freed
 */
static int timer_fixup_free(void *addr, enum debug_obj_state state)
{
	struct tti_timer_list *timer = addr;

	switch (state) {
	case ODEBUG_STATE_ACTIVE:
		tti_del_timer(timer);
		debug_object_free(timer, &timer_debug_descr);
		return 1;
	default:
		return 0;
	}
}


static struct debug_obj_descr timer_debug_descr = {
	.name		= "tti_timer_list",
	.debug_hint	= timer_debug_hint,
	.fixup_init	= timer_fixup_init,
	.fixup_activate	= timer_fixup_activate,
	.fixup_free	= timer_fixup_free,
};

static inline void debug_timer_init(struct tti_timer_list *timer)
{
	debug_object_init(timer, &timer_debug_descr);
}

static inline void debug_timer_activate(struct tti_timer_list *timer)
{
	debug_object_activate(timer, &timer_debug_descr);
}

static inline void debug_timer_deactivate(struct tti_timer_list *timer)
{
	debug_object_deactivate(timer, &timer_debug_descr);
}

static inline void debug_timer_free(struct tti_timer_list *timer)
{
	debug_object_free(timer, &timer_debug_descr);
}

#else
static inline void debug_timer_init(struct tti_timer_list *timer) { }
static inline void debug_timer_activate(struct tti_timer_list *timer) { }
static inline void debug_timer_deactivate(struct tti_timer_list *timer) { }
#endif

#ifdef CONFIG_TRACE_TTI_TIMERS
#endif	/* defined CONFIG_TRACE_TTI_TIMERS */

static inline void
debug_init(struct tti_timer_list *timer)
{
	debug_timer_init(timer);
	trace_tti_timer_init(timer);
}

static inline void
debug_activate(struct tti_timer_list *timer, unsigned long expires)
{
	debug_timer_activate(timer);
	trace_tti_timer_start(timer, expires);
}

static inline void
debug_deactivate(struct tti_timer_list *timer)
{
	debug_timer_deactivate(timer);
	trace_tti_timer_cancel(timer);
}

/**
 * init_tti_timers - initialize a timer array
 * @base_tti: the TTI to be initialized
 *
 * init_tti_timers() must be done to a timer prior calling *any* of the
 * other timer array functions. 
 */
int init_tti_timers_array (unsigned long base_tti)
{
    int j;
    long cpu = (long)smp_processor_id();
    struct tti_tvec_base *base;
    //BUG_ON(_tti_tvec_base != NULL);
    
    base = kmalloc_node(sizeof(*base),
                        GFP_KERNEL | __GFP_ZERO,
                        cpu_to_node(cpu));
    if (!base)
        return -ENOMEM;

    for (j = 0; j < TVR_SIZE; j++)
		INIT_LIST_HEAD(base->tv.vec + j);

    base->timer_jiffies = base_tti;
    base->next_timer = base->timer_jiffies;

    _tti_tvec_base = base;
    CDBG("_tti_tvec_base %x\n", _tti_tvec_base);
    
    return 0;
}
EXPORT_SYMBOL_GPL (init_tti_timers_array);

static inline void __init_timer(struct tti_timer_list *timer)
{
	timer->entry.next = NULL;
	timer->base = _tti_tvec_base;
}

static inline void detach_timer(struct tti_timer_list *timer,
				int clear_pending)
{
	struct list_head *entry = &timer->entry;

    debug_deactivate(timer);

    __list_del(entry->prev, entry->next);
	if (clear_pending)
		entry->next = NULL;
	entry->prev = LIST_POISON2;
}

static void tti_internal_add_timer(struct tti_tvec_base *base, struct tti_timer_list *timer)
{
	unsigned long expires = timer->expires;
	unsigned long idx = expires - base->timer_jiffies;
	struct list_head *vec;

	if (idx < TVR_SIZE) {
		int i = expires & TVR_MASK;
		vec = base->tv.vec + i;
	} else if ((signed long) idx < 0) {
		/*
		 * Can happen if you add a timer with expires == jiffies,
		 * or you set a timer to go off in the past
		 */
		vec = base->tv.vec + (base->timer_jiffies & TVR_MASK);
	} else {
		int i;
		/* If the timeout is larger than available in array
		 * then we use the maximum timeout:
		 */
		if (idx > TVR_SIZE) {
			idx = TVR_SIZE;
			expires = idx + base->timer_jiffies;
		}
		i = expires & TVR_MASK;
		vec = base->tv.vec + i;
	}
	/*
	 * Timers are FIFO:
	 */
	list_add_tail(&timer->entry, vec);
}

static inline int
__tti_mod_timer(struct tti_timer_list *timer, unsigned long expires, bool pending_only)
{
	struct tti_tvec_base *base = _tti_tvec_base;
	int ret = 0;

	BUG_ON(!timer->function);

    if (tti_timer_pending(timer)) {
		detach_timer(timer, 0);
		if (timer->expires == base->next_timer)
			base->next_timer = base->timer_jiffies;
		ret = 1;
	} else {
		if (pending_only)
			goto out;
	}

    debug_activate(timer, expires);

    timer->expires = expires;
	if (time_before(timer->expires, base->next_timer))
		base->next_timer = timer->expires;
	tti_internal_add_timer(base, timer);

out:
    return ret;
}

/**
 * tti_init_timer - initialize a timer
 * @timer: the timer to be initialized
 *
 * tti_init_timer() must be done to a timer prior calling *any* of the
 * other timer functions.
 */
void tti_init_timer(struct tti_timer_list *timer)
{
    debug_init(timer);
    
    __init_timer(timer);
}
EXPORT_SYMBOL_GPL(tti_init_timer);

/**
 * tti_add_timer - start a timer
 * @timer: the timer to be added
 *
 * The kernel will do a ->function(->data) callback from the
 * timer handlr at the ->expires point in the future. The
 * current time is 'TTI'.
 *
 * The timer's ->expires, ->function (and if the handler uses it, ->data)
 * fields must be set prior calling this function.
 *
 * Timers with an ->expires field in the past will be executed in the next
 * timer tick.
 */
void tti_add_timer(struct tti_timer_list *timer)
{
	tti_mod_timer(timer, timer->expires);
}
EXPORT_SYMBOL_GPL(tti_add_timer);

/**
 * tti_mod_timer_pending - modify a pending timer's timeout
 * @timer: the pending timer to be modified
 * @expires: new timeout in jiffies
 *
 * tti_mod_timer_pending() is the same for pending timers as mod_timer(),
 * but will not re-activate and modify already deleted timers.
 *
 * It is useful for unserialized use of timers.
 */
int tti_mod_timer_pending(struct tti_timer_list *timer, unsigned long expires)
{
	return __tti_mod_timer(timer, expires, true);
}
EXPORT_SYMBOL_GPL(tti_mod_timer_pending);

/**
 * tti_mod_timer - modify a timer's timeout
 * @timer: the timer to be modified
 * @expires: new timeout in jiffies
 *
 * tti_mod_timer() is a more efficient way to update the expire field of an
 * active timer (if the timer is inactive it will be activated)
 *
 * tti_mod_timer(timer, expires) is equivalent to:
 *
 *     tti_del_timer(timer); timer->expires = expires; add_timer(timer);
 *
 * Note that if there are multiple unserialized concurrent users of the
 * same timer, then tti_mod_timer() is the only safe way to modify the timeout,
 * since add_timer() cannot modify an already running timer.
 *
 * The function returns whether it has modified a pending timer or not.
 * (ie. tti_mod_timer() of an inactive timer returns 0, tti_mod_timer() of an
 * active timer returns 1.)
 */
int tti_mod_timer(struct tti_timer_list *timer, unsigned long expires)
{
	/*
	 * This is a common optimization triggered by the
	 * networking code - if the timer is re-modified
	 * to be the same thing then just return:
	 */
	if (tti_timer_pending(timer) && timer->expires == expires)
		return 1;

	return __tti_mod_timer(timer, expires, false);
}
EXPORT_SYMBOL_GPL(tti_mod_timer);

/**
 * tti_del_timer - deactive a timer.
 * @timer: the timer to be deactivated
 *
 * tti_del_timer() deactivates a timer - this works on both active and inactive
 * timers.
 *
 * The function returns whether it has deactivated a pending timer or not.
 * (ie. tti_del_timer() of an inactive timer returns 0, tti_del_timer() of an
 * active timer returns 1.)
 */
int tti_del_timer(struct tti_timer_list *timer)
{
	struct tti_tvec_base *base = _tti_tvec_base;
	int ret = 0;

	if (tti_timer_pending(timer)) {
        detach_timer(timer, 1);
        if (timer->expires == base->next_timer)
            base->next_timer = base->timer_jiffies;
        ret = 1;
    }

	return ret;
}
EXPORT_SYMBOL_GPL(tti_del_timer);

static void call_timer_fn(struct tti_timer_list *timer, void (*fn)(unsigned long),
                          unsigned long data)
{
    trace_tti_timer_expire_entry(timer);
	fn(data);
	trace_tti_timer_expire_exit(timer);
}

/**
 * __run_timers - run all expired timers (if any) on this CPU.
 * @base: the timer vector to be processed.
 *
 * This function executes all expired timer vectors.
 */
static inline void __run_timers(struct tti_tvec_base *base, unsigned long internal_tti)
{
	struct tti_timer_list *timer;

	while (time_after_eq(internal_tti, base->timer_jiffies)) {
		struct list_head work_list;
		struct list_head *head = &work_list;
		int index = base->timer_jiffies & TVR_MASK;

		++base->timer_jiffies;
		list_replace_init(base->tv.vec + index, &work_list);
		while (!list_empty(head)) {
			void (*fn)(unsigned long);
			unsigned long data;

			timer = list_first_entry(head, struct tti_timer_list, entry);
			fn = timer->function;
			data = timer->data;

            base->running_timer = timer;
			detach_timer(timer, 1);

			call_timer_fn(timer, fn, data);
		}
	}
	base->running_timer = NULL;
}

/*
 * This function runs timers and the timer-tq in task context.
 */
void run_timer_task(unsigned long current_tti)
{
	struct tti_tvec_base *base = _tti_tvec_base;

    if (time_after_eq(current_tti, base->timer_jiffies))
		__run_timers(base, current_tti);
}
EXPORT_SYMBOL_GPL(run_timer_task);

MODULE_AUTHOR("ibnHatab");
MODULE_DESCRIPTION("TTI Timer Array");
MODULE_LICENSE("GPL v2");

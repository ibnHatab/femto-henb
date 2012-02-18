
#include <linux/slab.h>
#include <linux/module.h>


#include <tti_timer.h>

#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVR_SIZE (1 << TVR_BITS)

struct tvec_root {
	struct list_head vec[TVR_SIZE];
};

struct tti_tvec_base {
	struct timer_list *running_timer;
	unsigned long timer_jiffies;
	unsigned long next_timer;
	struct tvec_root tv;
} ____cacheline_aligned;

struct tti_tvec_base *tti_tvec;

int init_tti_timers(void)
{
    long cpu = (long)smp_processor_id();
    
    struct tti_tvec_base 
        *base = kmalloc_node(sizeof(*base),
                             GFP_KERNEL | __GFP_ZERO,
                             cpu_to_node(cpu));
    if (!base)
        return -ENOMEM;
    

    base->timer_jiffies = jiffies;
    base->next_timer = base->timer_jiffies;

    tti_tvec = base;
    
    return 0;
}

EXPORT_SYMBOL_GPL (init_tti_timers);


MODULE_AUTHOR("VKI");
MODULE_DESCRIPTION("Timer attay");
MODULE_LICENSE("GPL v2");

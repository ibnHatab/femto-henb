#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/bug.h>
#include <linux/jiffies.h>
#include <linux/timer.h>


#include <embUnit/embUnit.h>

/*embunit:include=+ */
#include "timerunit.h"
/*embunit:include=- */


static void setUp(void)
{
	/* initialize */
}

static void tearDown(void)
{
	/* terminate */
}

/*embunit:impl=+ */

/********************************************************************************/
static struct timer_list my_timer;
static void my_timer_function(unsigned long ptr)
{
	printk(KERN_INFO "my_timer_fun, jiffies = %ld\n", jiffies);
    printk(KERN_INFO "current task pid is %d\n", (int)current->pid);

	*((int*) ptr) += 100;

    printk(KERN_INFO "Deleted time,r rc = %d\n", del_timer_sync(&my_timer));
}

static void testJiffies(void)
{
    //    timerunit_cb();
	static int len = 100;
    /* initialize */
	init_timer(&my_timer);	

    my_timer.function = my_timer_function;
	my_timer.expires = jiffies + HZ;	/* one second delay */
	my_timer.data = &len;
	printk(KERN_INFO "Adding timer at jiffies = %ld +1s %ld\n", jiffies, my_timer.expires);
	add_timer(&my_timer);
    
    TEST_ASSERT_EQUAL_INT(200, len);
}

/********************************************************************************/
static void testTimeStamp(void)
{
    u32 tsc_lo, tsc_hi;		/* TSC when timer is executed */
    int distinguished_condition = 0;
    int fatal_condition = 0;

    BUG_ON(distinguished_condition);

    if (fatal_condition)
    {
        panic("I'm giving up because task %d\n", current->pid);
    }

    //    rdtsc(tsc_hi, tsc_lo);
	TEST_FAIL("no implementation on ARM");
    
}

/********************************************************************************/
static void testDelay(void)    
{
	TEST_FAIL("no implementation");
}

/********************************************************************************/

static struct timer_list timer_a, timer_b;

static struct kt_data *data_a, *data_b;

static void ktfun_a(unsigned long var)
{
    struct kt_data *tdata = (struct kt_data *)var;
	printk(KERN_INFO "A: [%d] period = %ld  start = %ld TTI = %d\n",
           tdata->type, tdata->period, tdata->start_time, TTI);
}

static void testDynamicTimers(void)
{
	init_timer(&timer_a);
	init_timer(&timer_b);

	data_a = kmalloc(sizeof(*data_a), GFP_KERNEL);
	data_b = kmalloc(sizeof(*data_b), GFP_KERNEL);

    data_a->type = FAPI_L2_ARQ_RETRANSMIT;
	data_b->type = FAPI_L2_HARQ_TIMEOUT;
    
    data_a->period = 1 * HZ;	/* short period, 1 second  */
	data_b->period = 2 * HZ;	/* longer period, 10 seconds */

	data_a->start_time = jiffies;
	data_b->start_time = jiffies;

    timer_a.function = ktfun_a;
	timer_b.function = ktfun_a;

    timer_a.data = (unsigned long)data_a;
	timer_b.data = (unsigned long)data_b;

    timer_a.expires = jiffies + data_a->period;
	timer_b.expires = jiffies + data_b->period;

    add_timer(&timer_a);
	add_timer(&timer_b);
}

/********************************************************************************/
static void testHighResTimers(void)
{
	TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef TimerTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testJiffies",testJiffies),
            new_TestFixture("testTimeStamp",testTimeStamp), 
            new_TestFixture("testDelay",testDelay), 
            new_TestFixture("testDynamicTimers",testDynamicTimers), 
		/* new_TestFixture("testHighResTimers",testHighResTimers), */
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(timerTest,"timerTest",setUp,tearDown,fixtures);
	return (TestRef)&timerTest;
};

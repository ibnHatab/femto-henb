
#include <linux/module.h>

#include <embUnit/embUnit.h>

/*embunit:include=+ */
#include "tti_timer.h"
/*embunit:include=- */

unsigned long current_tti;

static void setUp(void)
{
	/* initialize */
    current_tti = 0;    
}

static void tearDown(void)
{
	/* terminate */
	printk(KERN_INFO "TTI = %ld\n", current_tti);
}

/*embunit:impl=+ */

static struct tti_timer_list my_timer;
static void my_timer_function(unsigned long ptr)
{
	printk(KERN_INFO "my_timer_fun, TTI = %ld\n", current_tti);
    printk(KERN_INFO "Deleted time,r rc = %d\n", tti_del_timer(&my_timer));
}

static void testInitialize(void)
{
	TEST_FAIL("no implementation");
}

static void testAddRemove(void)
{
	TEST_FAIL("no implementation");
}

static void testModify(void)
{
	TEST_FAIL("no implementation");
}

static struct tti_timer_list timer_a;
static unsigned long data_a;

static void ktfun_a(unsigned long var)
{
	printk(KERN_INFO "A: [%ld]\n", var);
}

static void testDynamicTimers(void)
{
    init_tti_timers_array(current_tti);
    
	tti_init_timer(&timer_a);
    data_a = 42;

    timer_a.function = ktfun_a;
    timer_a.data = (unsigned long)data_a;
    timer_a.expires = current_tti + 1 * TTI;

    tti_add_timer(&timer_a);

    current_tti += 2;
    run_timer_task(current_tti);
}

/*embunit:impl=- */
TestRef tti_timerTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testInitialize",testInitialize),
		new_TestFixture("testAddRemove",testAddRemove),
		new_TestFixture("testModify",testModify),
		new_TestFixture("testDynamicTimers",testDynamicTimers),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(tti_timerTest,"tti_timerTest",setUp,tearDown,fixtures);
	return (TestRef)&tti_timerTest;
};

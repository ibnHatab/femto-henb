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
static void testJiffies(void)
{
    timerunit_cb();
}

static void testTimeStamp(void)
{
    int distinguished_condition = 1;
    BUG_ON(distinguished_condition);
    
	TEST_FAIL("no implementation");
}

static void testDelay(void)    
{
    int fatal_condition = 0;
    if (fatal_condition)
    {
        panic("I'm giving up because task %d\n", current->pid);
    }
	TEST_FAIL("no implementation");
}

static void testDynamicTimers(void)
{
	TEST_FAIL("no implementation");
}

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
		new_TestFixture("testHighResTimers",testHighResTimers),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(timerTest,"timerTest",setUp,tearDown,fixtures);
	return (TestRef)&timerTest;
};

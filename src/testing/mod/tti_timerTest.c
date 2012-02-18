#include <embUnit/embUnit.h>

/*embunit:include=+ */
#include "tti_timer.h"
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

/*embunit:impl=- */
TestRef tti_timerTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testInitialize",testInitialize),
		new_TestFixture("testAddRemove",testAddRemove),
		new_TestFixture("testModify",testModify),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(tti_timerTest,"tti_timerTest",setUp,tearDown,fixtures);
	return (TestRef)&tti_timerTest;
};

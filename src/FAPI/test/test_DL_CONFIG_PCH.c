#include <embUnit/embUnit.h>

/*embunit:include=+ */
#include "fapi/l1_l2_iface.h"
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
static void testPCH_MIMO(void)
{
	TEST_FAIL("no implementation");
}

static void testPCH_SISO(void)
{
	TEST_FAIL("no implementation");
}

/*embunit:impl=- */
TestRef test_DL_CONFIG_PCH_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
	/*embunit:fixtures=+ */
		new_TestFixture("testPCH_MIMO",testPCH_MIMO),
		new_TestFixture("testPCH_SISO",testPCH_SISO),
	/*embunit:fixtures=- */
	};
	EMB_UNIT_TESTCALLER(test_DL_CONFIG_PCH,"test_DL_CONFIG_PCH",setUp,tearDown,fixtures);
	return (TestRef)&test_DL_CONFIG_PCH;
};

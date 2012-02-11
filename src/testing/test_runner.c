#include <linux/module.h>
#include <embUnit/embUnit.h>

MODULE_DESCRIPTION("embunit runner for linux kernel");
MODULE_LICENSE("GPL");

/*embunit:extern=+ */
extern TestRef CounterTest_tests(TestCaller *test);
extern TestRef PersonTest_tests(TestCaller *test);
/*embunit:extern=- */

void stdimpl_print(const char *string)
{
	printk(string);
}

static int __init test_runner_init(void)
{
	static TestCaller test;

	TestRunner_start();
	/*embunit:run=+ */
	TestRunner_runTest(CounterTest_tests(&test));
	TestRunner_runTest(PersonTest_tests(&test));
	/*embunit:run=- */
	TestRunner_end();

	return 0;
}

static void __exit test_runner_exit(void)
{
	return;
}

module_init(test_runner_init);
module_exit(test_runner_exit);

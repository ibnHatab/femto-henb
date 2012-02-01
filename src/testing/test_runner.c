#include <linux/module.h>
#include <embUnit/embUnit.h>

MODULE_DESCRIPTION("embunit sample for linux kernel");
MODULE_AUTHOR("Mikhail Gruzdev <michail.gruzdev@gmail.com>");
MODULE_LICENSE("GPL");

extern TestRef CounterTest_tests(TestCaller *test);
extern TestRef PersonTest_tests(TestCaller *test);

void stdimpl_print(const char *string)
{
	printk(string);
}

static int __init test_runner_init(void)
{
	static TestCaller test;

	TestRunner_start();
	TestRunner_runTest(CounterTest_tests(&test));
	TestRunner_runTest(PersonTest_tests(&test));
	TestRunner_end();

	return 0;
}

static void __exit test_runner_exit(void)
{
	return;
}

module_init(test_runner_init);
module_exit(test_runner_exit);

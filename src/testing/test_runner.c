#include <linux/module.h>
#include <embUnit/embUnit.h>

#define DRV_NAME	"embunit_runner"
#define DRV_VERSION	"0.1.7"
#define ABC_SIZE 'z'-'0'+1

static char test_selector[ABC_SIZE];
module_param_string(abc, test_selector, ABC_SIZE, S_IRUGO | S_IWUSR);

// static TestCaller test_caller;

/*embunit:extern=+ */
extern TestRef CounterTest_tests (void);
extern TestRef PersonTest_tests (void);
extern TestRef TimerTest_tests (void);

/*embunit:extern=- */

void test_runner_execute (char* selector)
{
    while (*selector) {
        switch (*selector) {
            /*embunit:run=+ */
        case '0':
            TestRunner_runTest (CounterTest_tests ());
            TestRunner_runTest (PersonTest_tests ());
            break;
        case '1':   break;
        case '2':  TestRunner_runTest (TimerTest_tests ()); break;
        case '3': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '4': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '5': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '6': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '7': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '8': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case '9': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'A': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'B': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'C': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'D': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'E': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'F': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'G': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'H': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'I': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'J': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'K': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'L': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'M': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'N': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'O': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'P': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'Q': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'R': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'S': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'T': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'U': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'V': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'W': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'X': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'Y': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'Z': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'a': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'b': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'c': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'd': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'e': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'f': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'g': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'h': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'i': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'j': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'k': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'l': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'm': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'n': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'o': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'p': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'q': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'r': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 's': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 't': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'u': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'v': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'w': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'x': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'y': /* TestRunner_runTest (*_tests (&test_caller)); */  break;
        case 'z': /* TestRunner_runTest (*_tests (&test_caller)); */  break;            
            /*embunit:run=- */
        }
        ++ selector;
    }
}

void stdimpl_print(const char *string)
{
	printk(string);
}


static int __init test_runner_init(void)
{
	TestRunner_start();
    test_runner_execute (test_selector);
	TestRunner_end();

	return 0;
}

static void __exit test_runner_exit(void)
{
	return;
}

module_init(test_runner_init);
module_exit(test_runner_exit);

MODULE_DESCRIPTION("embunit runner for linux kernel");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

#include <stdio.h>

#include <embUnit/embUnit.h>

extern TestRef ue_messages_test_tests(void);

int main (int argc, const char* argv[])
{
	TestRunner_start();
        /* TestRunner_runTest (cphy_sap_test_tests()); */
        /* TestRunner_runTest (ue_messages_test_tests());  */
	TestRunner_end();
	return 0;
}


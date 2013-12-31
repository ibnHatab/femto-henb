#include <stdio.h>

#include <embUnit/embUnit.h>

extern TestRef test_CONFIG_REQ_tests(void);
extern TestRef test_CSAP_SAP_tests(void);
extern TestRef test_DL_CONFIG_PBCH_tests(void);
extern TestRef test_DL_CONFIG_DCI_tests(void);
extern TestRef test_DL_CONFIG_DLSCH_tests(void);
extern TestRef test_UL_CONFIG_ULSCH_tests(void);
extern TestRef test_UL_CONFIG_UCI_tests(void);
extern TestRef test_HI_DCI0_tests(void);
extern TestRef sdu_combinator_test_tests(void);
extern TestRef test_SRS_INDICATION_tests(void);    
extern TestRef test_PUSH_tests(void);
extern TestRef test_HandleMap_tests(void);
extern TestRef test_PUCCH_tests(void);
    
int main (int argc, const char* argv[])
{
	TestRunner_start();
        /* Call tests here */
        TestRunner_runTest (sdu_combinator_test_tests()); 
        TestRunner_runTest (test_CONFIG_REQ_tests());
        TestRunner_runTest (test_CSAP_SAP_tests());
        TestRunner_runTest (test_DL_CONFIG_PBCH_tests());
        
        TestRunner_runTest (test_DL_CONFIG_DLSCH_tests());
        TestRunner_runTest (test_DL_CONFIG_DCI_tests());
        TestRunner_runTest (test_UL_CONFIG_ULSCH_tests());
        TestRunner_runTest (test_UL_CONFIG_UCI_tests());
        TestRunner_runTest (test_HI_DCI0_tests());
#if RUN_REGRESSION_TESTS
        TestRunner_runTest (test_SRS_INDICATION_tests()); // core
#endif
        TestRunner_runTest (test_HandleMap_tests());
        TestRunner_runTest (test_PUSH_tests());
        TestRunner_runTest (test_PUCCH_tests());

        
#if RUN_REGRESSION_TESTS
#endif
        
	TestRunner_end();
	return 0;
}


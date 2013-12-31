
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

#include <icpu.h>
#include <ctrlmsg.h>

#include <l1_l2_iface.h>

#include "fapi_iface.h"
#include "scen/sc_list.h"

extern RESULTCODE InitSettings(int argc, char *argv[]);
extern RESULTCODE MsgDispatcherInit(void);
extern RESULTCODE InitAdi (U32 configBand, U32 configTxAnt, U32 configRxAnt,
                           U32 configLoopback, U32 configDejitter_init);

extern int IcpuHandle;

int verbose;
int timerModeTTI = 1;
int scenario = -1;
int scenarioList = 0;
int adiInitNeeded = 0;

void usage(const char *prog)
{
    printf("Usage: %s [OPTION] [scenario]\n"
           "Start FAPI client tests.\n"
           "\n"
           "\t-T [msec] , timer mode\n"
           "\t-a , ADI init\n"
           "\t-l , scenarios list\n"
           "\t-v , verbose\n"
           "\t-h , this help\n"
           , prog);
}

int parse_args(int argc, char *argv[])
{
    int flags, opt;
    flags = 0;
    while ((opt = getopt(argc, argv, "T:s:alv")) != -1) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'T':
            timerModeTTI = atoi(optarg);
            break;
        case 'l':
            scenarioList = 1;
            break;
        case 'a':
            adiInitNeeded = 1;
            break;
        default: /* '?' */
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (! scenarioList) {
        if (optind >= argc) {
            fprintf(stderr, "Error: Expected scenario number\n");
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }

        scenario = atoi(argv[optind]);
    }

    return argc;
}

RESULTCODE set_realtime()
{
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &sp) >= 0)
        return TRUE;
    return FALSE;
}

RESULTCODE init_mlog()
{
    return SUCCESS;
}

int main (int argc, char* argv[], char* envp[])
{
    struct sched_param sp;
    unsigned char msgBuf[BUFSIZ];

    parse_args(argc, argv);

    InitSettings(0/* argc */, argv); // read ini from c4k.ini

    if (scenarioList) {
        list_scenarios();
        return 0;
    }

    if (!(scenario < sc_list_size)) {
        printf("Error: scenario <%02d> not listed. Use -l to list.\n", scenario);
        exit(EXIT_FAILURE);
    }

    if (verbose) {
        printf("Processing scenario <%02d>\n", scenario);
    }

    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &sp) < 0) {
        printf("Warning: Set FIFO RT fail\n");
    }

    if (SUCCESS != MsgDispatcherInit()) {
        printf("MsgDispatcherInit Failed!!!\n");
        exit(EXIT_FAILURE);
    }

    IcpuInit(UARM);
    if (IcpuHandle < 0) {
        printf("ERROR: ICPU Init FD=%d\n", IcpuHandle);
        exit(EXIT_FAILURE);        
    }
    
    if (verbose) {
        printf("MAC:ICPU FD=%d\n", IcpuHandle);
    }

    IcpuRxDirect((void *) msgBuf); // clean Q

    if (adiInitNeeded){
        printf("Programming ADI\n");
        if (SUCCESS != init_adi(7,1,1,0,0)) {
            printf("ERROR: ADI Init Failed!!!\n");
            exit(1);
        } else {
            while (! IcpuRxDirect((void *) msgBuf)) {
                sleep(1);
                printf(".");
            }
            printf(" ADI Init OK!!!\n");

            sleep(1);
        }
    }

    /// SCENARIO /// SCENARIO /// SCENARIO /// SCENARIO /// SCENARIO ///

    if (sc_list[scenario].sc_init()) {
        printf("ERROR: Scenario <%02d> Initialization fail\n", scenario);
        exit(2);
    }

    if (sc_list[scenario].sc_handle_tti == NULL
        && sc_list[scenario].sc_handle_api == NULL) {
        printf("Warning: Scenario %02d: '%s', finished without running reactor\n",
               scenario, sc_list[scenario].sc_name);
        
    }
    
    if (verbose) 
        printf("Running reactor: %d tti's\n", sc_list[scenario].sc_ttis);
    
    int total_tti = scenarion_reactor(sc_list[scenario].sc_ttis,
                                       sc_list[scenario].sc_handle_tti,
                                       sc_list[scenario].sc_handle_api);

    printf("Scenario %02d: '%s', finished with total tti's %d.\n",
           scenario,  sc_list[scenario].sc_name, total_tti);

    return 0;

}

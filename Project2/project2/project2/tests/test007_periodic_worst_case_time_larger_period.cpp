/*
 * test007_periodic_worst_case_time_larger_period.cpp
 *
 * Created: 3/8/2015 2:29:22 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_007

/************************************************************************/
/* Expected: ERROR ERR_RUN_3_INVALID_CONFIGURATION                      */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void periodic_task()
{
    int i;
    for (i = 0; i < 3; i++)
    {
        add_to_trace(Now());
        print_trace();
        Task_Next();
    }

    add_to_trace(0);
    print_trace();
}

int r_main()
{
    set_trace_test(6);
    Task_Create_Periodic(periodic_task, 0, 10, 20, 1);
    return 0;
}

#endif
/*
 * test_006_create_periodic_task.cpp
 *
 * Created: 3/8/2015 2:23:45 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_006

/************************************************************************/
/* Expected: T006;0;10;20;0;                                              */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"
#include "../profiler.h"

void periodic_task()
{
    int i;
    for (i = 0; i < 3; i++)
    {
        add_to_trace(Now());

        Task_Next();
    }

    add_to_trace(0);
    print_trace();
    CORRECT_ON;
}

int r_main()
{
    set_trace_test(6);
    INCORRECT_ON;
    EnableProfileSample3();
    Task_Create_Periodic(periodic_task, 0, 20, 10, 1);
    DisableProfileSample3();
    return 0;
}

#endif
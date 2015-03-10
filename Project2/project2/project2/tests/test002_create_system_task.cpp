/*
 * test002_create_system_task.cpp
 *
 * Created: 3/8/2015 1:35:02 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_002

/************************************************************************/
/* Expected: T002;1;                                                   */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"
#include "../profiler.h"

void system_task()
{
    add_to_trace(1);
    print_trace();
    CORRECT_ON;
}

int r_main()
{
    set_trace_test(2);
    INCORRECT_ON;
    EnableProfileSample2();
    Task_Create_System(system_task, 0);
    DisableProfileSample2();
    return 0;
}

#endif
/*
 * test004_system_task_preemption.cpp
 *
 * Created: 3/8/2015 1:37:02 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_004

/************************************************************************/
/* Expected: T004;1;2;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void empty_system_task()
{
    add_to_trace(2);
    print_trace();
    CORRECT_ON;
}

void create_system_task()
{
    Task_Create_System(empty_system_task, 1);
    add_to_trace(1);
}

int r_main()
{
    set_trace_test(4);
    INCORRECT_ON;
    Task_Create_System(create_system_task, 0);   
    return 0;
}

#endif
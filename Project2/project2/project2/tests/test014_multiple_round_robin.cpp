/*
 * test014_multiple_round_robin.cpp
 *
 * Created: 3/8/2015 4:22:54 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_014

/************************************************************************/
/* Expected: T014;1;2;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void rr_task_2()
{
    add_to_trace(2);
    print_trace();
    CORRECT_ON;
}

void rr_task_1()
{
    Task_Create_RoundRobin(rr_task_2, 0);
    add_to_trace(1);
}

int r_main()
{
    set_trace_test(14);
    INCORRECT_ON;
    Task_Create_RoundRobin(rr_task_1, 0);
    return 0;
}

#endif

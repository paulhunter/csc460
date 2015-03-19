/*
 * test_022_publish_preempt_system.cpp
 *
 * Created: 3/8/2015 4:26:16 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_022

/************************************************************************/
/* Expected: T021;1;2;3;4;5;101;6;102;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

SERVICE * s;

void rr_task_2()
{
    add_to_trace(3);
    Service_Publish(s, 100);
    add_to_trace(5);
}

void rr_task_1()
{
    int16_t v;
    add_to_trace(2);
    Service_Subscribe(s, &v);
    add_to_trace(6);
    add_to_trace(v + 2);
    print_trace();
    CORRECT_ON;
}

void system_task_1()
{
    int16_t v;
    add_to_trace(1);
    Service_Subscribe(s, &v);
    add_to_trace(4);
    add_to_trace(v + 1);
}

int r_main()
{
    set_trace_test(22);
    INCORRECT_ON;
    s = Service_Init();
    Task_Create_RoundRobin(rr_task_1, 0);
    Task_Create_System(system_task_1, 0);
    Task_Create_RoundRobin(rr_task_2, 0);
    return 0;
}

#endif
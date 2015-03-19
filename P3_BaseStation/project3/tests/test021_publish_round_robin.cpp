/*
 * test021_publish_round_robin.cpp
 *
 * Created: 3/8/2015 4:25:55 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_021

/************************************************************************/
/* Expected: T021;1;2;3;4;5;101;6;102;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"
#include "../profiler.h"

SERVICE * s;

void rr_task_3()
{
    add_to_trace(3);
    EnableProfileSample7();
    Service_Publish(s, 100);
    DisableProfileSample7();
    add_to_trace(4);
}

void rr_task_2()
{
    int16_t v;
    add_to_trace(2);
    Service_Subscribe(s, &v);
    add_to_trace(6);
    add_to_trace(v + 2);
    print_trace();
    CORRECT_ON;
}

void rr_task_1()
{
    int16_t v;
    add_to_trace(1);
    Service_Subscribe(s, &v);
    add_to_trace(5);
    add_to_trace(v + 1);
}

int r_main()
{
    set_trace_test(21);
    INCORRECT_ON;
    s = Service_Init();
    Task_Create_RoundRobin(rr_task_1, 0);
    Task_Create_RoundRobin(rr_task_2, 0);
    Task_Create_RoundRobin(rr_task_3, 0);
    return 0;
}

#endif
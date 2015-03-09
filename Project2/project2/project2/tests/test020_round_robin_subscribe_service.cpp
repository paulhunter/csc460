/*
 * test020_round_robin_subscribe_service.cpp
 *
 * Created: 3/8/2015 4:25:17 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_020

/************************************************************************/
/* Expected: T020;1;2;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

SERVICE * s;

void rr_task_3()
{
    print_trace();
    CORRECT_ON;
}

void rr_task_2()
{
    int16_t v;
    add_to_trace(2);
    Service_Subscribe(s, &v);
}

void rr_task_1()
{
    int16_t v;
    add_to_trace(1);
    Service_Subscribe(s, &v);
}

int r_main()
{
    set_trace_test(20);
    INCORRECT_ON;
    s = Service_Init();
    Task_Create_RoundRobin(rr_task_1, 0);
    Task_Create_RoundRobin(rr_task_2, 0);
    Task_Create_RoundRobin(rr_task_3, 0);
    return 0;
}

#endif
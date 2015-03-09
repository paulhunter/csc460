/*
 * test009_multiple_periodic_tests.cpp
 *
 * Created: 3/8/2015 2:30:20 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_009

/************************************************************************/
/* Expected: T009;0;3;1;4;2;5;50;10;                                           */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void periodic_task_1()
{
    int i;
    for (i = 0; i < 3; i++)
    {
        add_to_trace(i);
        Task_Next();
    }

    add_to_trace(50);
    print_trace();

}

void periodic_task_2()
{
    int i;
    for (i = 0; i < 3; i++)
    {
        add_to_trace(i + 3);
        Task_Next();
    }

    add_to_trace(20);

}

int r_main()
{
    set_trace_test(9);

    Task_Create_Periodic(periodic_task_1, 0, 50, 5, 1);
    Task_Create_Periodic(periodic_task_2, 0, 20, 5, 10);

    return 0;
}

#endif
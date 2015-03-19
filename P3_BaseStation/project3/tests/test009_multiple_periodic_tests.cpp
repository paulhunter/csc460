/*
 * test009_multiple_periodic_tests.cpp
 *
 * Created: 3/8/2015 2:30:20 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_009

/************************************************************************/
/* Expected: T009;0;1;3;15;4;35;1;51;5;55;20;2;101;50;                  */
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
        add_to_trace(Now() / TICK);
        Task_Next();
    }

    add_to_trace(50);
    print_trace();
    CORRECT_ON;

}

void periodic_task_2()
{
    int i;
    for (i = 0; i < 3; i++)
    {
        add_to_trace(i + 3);
        add_to_trace(Now() / TICK);
        Task_Next();
    }

    add_to_trace(20);
}

int r_main()
{
    set_trace_test(9);
    INCORRECT_ON;

    Task_Create_Periodic(periodic_task_1, 0, 50, 5, 1);
    Task_Create_Periodic(periodic_task_2, 0, 20, 5, 15);

    return 0;
}

#endif
/*
 * test000_sanity.cpp
 *
 * Created: 3/8/2015 1:14:04 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"
#include "../test.h"

void system_sanity_task()
{
    add_to_trace(1);
    print_trace();
    CORRECT_ON;
} 

int r_main()
{
    set_trace_test(0);
    INCORRECT_ON;
    Task_Create_System(system_sanity_task, 0);
    return 0;
}

 #endif
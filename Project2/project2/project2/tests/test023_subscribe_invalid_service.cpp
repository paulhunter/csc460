/*
 * test023_subscribe_invalid_service.cpp
 *
 * Created: 3/8/2015 5:01:38 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_023

/************************************************************************/
/* Expected: ERROR. Invalid Service                                     */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void rr_task()
{
    int16_t v;
    Service_Subscribe(NULL, &v);
}

int r_main()
{
    set_trace_test(23);
    Task_Create_RoundRobin(rr_task, 0);

    return 0;
}

#endif
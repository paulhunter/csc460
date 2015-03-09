/*
 * test023_subscribe_invalid_service.cpp
 *
 * Created: 3/8/2015 5:01:38 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_023

/************************************************************************/
/* Expected: ERR_RUN_9_INVALID_SERVICE                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void rr_task()
{
    int16_t v;
    Service_Subscribe(NULL, &v);
    CORRECT_ON;
}

int r_main()
{
    set_trace_test(23);
    INCORRECT_ON;
    Task_Create_RoundRobin(rr_task, 0);

    return 0;
}

#endif
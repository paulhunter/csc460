/*
 * test019_periodic_subscrie_service.cpp
 *
 * Created: 3/8/2015 4:24:57 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_019

/************************************************************************/
/* Expected: ERR_RUN_10_PERIODIC_SUBSCRIBE. Periodic tasks can't subscribe to services          */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

void periodic_task()
{
    SERVICE * s = Service_Init();
    int16_t v;
    Service_Subscribe(s, &v);
    CORRECT_ON;
}

int r_main()
{
    set_trace_test(19);
    INCORRECT_ON;
    Task_Create_Periodic(periodic_task, 0, 20, 10, 1);

    return 0;
}

#endif
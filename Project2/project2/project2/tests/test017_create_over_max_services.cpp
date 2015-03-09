/*
 * test017_create_over_max_services.cpp
 *
 * Created: 3/8/2015 4:24:16 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_017

/************************************************************************/
/* Expected: ERR_RUN_8_SERVICE_CAPACITY_REACHED                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

int r_main()
{
    set_trace_test(17);
    int i;
    for (i = 0; i < MAXSERVICES + 1; i++)
    {
        Service_Init();
    }

    return 0;
}

#endif
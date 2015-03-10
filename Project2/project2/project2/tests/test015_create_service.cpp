/*
 * test015_create_service.cpp
 *
 * Created: 3/8/2015 4:23:27 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_015

/************************************************************************/
/* Expected: T015;1;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"
#include "../profiler.h"

int r_main()
{
    set_trace_test(15);
    INCORRECT_ON;
    EnableProfileSample5();
    SERVICE * s = Service_Init();
    DisableProfileSample5();
    add_to_trace(1);
    print_trace();
    CORRECT_ON;
    return 0;
}

#endif

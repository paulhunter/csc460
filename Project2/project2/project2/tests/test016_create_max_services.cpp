/*
 * test016_create_max_services.cpp
 *
 * Created: 3/8/2015 4:23:50 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_016

/************************************************************************/
/* Expected: T016;1;2;3;4;...;MAXSERVICES - 1;                          */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

int r_main()
{
    set_trace_test(16);
    INCORRECT_ON;
    int i;
    for (i = 0; i < MAXSERVICES; i++)
    {
        Service_Init();
        add_to_trace(i);
    }

    print_trace();
    CORRECT_ON;
    return 0;
}

#endif

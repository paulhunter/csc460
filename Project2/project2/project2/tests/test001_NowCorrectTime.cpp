/*
 * test001_NowCorrectTime.cpp
 *
 * Created: 3/8/2015 1:34:05 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_001

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 /************************************************************************/
 /* Expected: T001;0;10;20;...                                           */
 /************************************************************************/

 int r_main()
 {
    int i;

    set_trace_test(1);
    INCORRECT_ON;
    for (i = 0; i < 20; i++)
    {
        add_to_trace(Now());
        _delay_ms(100);
    }
    print_trace();
    CORRECT_ON;
    return 0;
 }

 #endif
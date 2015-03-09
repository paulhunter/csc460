/*
 * test011_periodic_task_not_completed_on_time.cpp
 *
 * Created: 3/8/2015 2:32:59 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_011

 /************************************************************************/
 /* Expected: ERROR                                                      */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 void periodic_task()
 {
     _delay_ms(50);
 }

 int r_main()
 {
     set_trace_test(11);
     Task_Create_Periodic(periodic_task_1, 0, 2, 1, 1);
     return 0;
 }

 #endif
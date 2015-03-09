/*
 * test013_create_round_robin.cpp
 *
 * Created: 3/8/2015 4:22:35 PM
 *  Author: jaguz_000
 */ 
 #ifdef USE_TEST_013

 /************************************************************************/
 /* Expected: T013;1;                                                  */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 void rr_task()
 {
     add_to_trace(1);
     print_trace();
 }

 int r_main()
 {
     set_trace_test(13);
     Task_Create_RoundRobin(rr_task, 0);
     return 0;
 }

 #endif

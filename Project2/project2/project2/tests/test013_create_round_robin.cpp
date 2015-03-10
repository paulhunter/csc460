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
 #include "../profiler.h"

 void rr_task()
 {
     add_to_trace(1);
     print_trace();
     CORRECT_ON;
 }

 int r_main()
 {
     set_trace_test(13);
     INCORRECT_ON;
     EnableProfileSample4();
     Task_Create_RoundRobin(rr_task, 0);
     DisableProfileSample4();
     return 0;
 }

 #endif

/*
 * test018_system_subscribe_service.cpp
 *
 * Created: 3/8/2015 4:24:41 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_018

 /************************************************************************/
 /* Expected: T018;1;     2 should not print if task goes to sleep       */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"
 #include "../profiler.h"

 SERVICE * s;

 void rr_task()
 {
    DisableProfileSample6();
 }

 int r_main()
 {
     set_trace_test(18);
     Task_Create_RoundRobin(rr_task, 0);
     INCORRECT_ON;
     int16_t v;
     s = Service_Init();
     add_to_trace(1);
     print_trace();
     EnableProfileSample6();
     Service_Subscribe(s, &v);
 
     add_to_trace(2);
     print_trace();
     CORRECT_ON;
     return 0;
 }

 #endif

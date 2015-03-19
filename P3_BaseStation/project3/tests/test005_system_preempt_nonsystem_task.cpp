/*
 * test005_system_preempt_nonsystem_task.cpp
 *
 * Created: 3/8/2015 1:53:00 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_005

 /************************************************************************/
 /* Expected: T005;1;2;                                                  */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 void system_task()
 {
     add_to_trace(1);
 }

 void rr_task()
 {
     Task_Create_System(system_task, 1);
     add_to_trace(2);
     print_trace();
     CORRECT_ON;
 }

 int r_main()
 {
     set_trace_test(5);
     INCORRECT_ON;
     Task_Create_RoundRobin(rr_task, 0);
     return 0;
 }

 #endif
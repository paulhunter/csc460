/*
 * test012_perdioc_system_preemption.cpp
 *
 * Created: 3/8/2015 2:33:30 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_012

 /************************************************************************/
 /* Expected: T012;1;2;                                                  */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 void system_task()
 {
     add_to_trace(1);
 }

 void periodic_task()
 {
     Task_Create_System(system_task, 0);
     add_to_trace(2);
     print_trace();
 }

 int r_main()
 {
     set_trace_test(12);
     Task_Create_Periodic(periodic_task, 0, 20, 10, 1);
     return 0;
 }

 #endif
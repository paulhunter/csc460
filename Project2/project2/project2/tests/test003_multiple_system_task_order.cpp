/*
 * test003_multiple_system_task_order.cpp
 *
 * Created: 3/8/2015 1:36:46 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_TEST_003

 /************************************************************************/
 /* Expected: T003;1;2;                                                  */
 /************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

 void system_task_2()
 {
     add_to_trace(2);
     print_trace();
 }

 void system_task_1()
 {
     add_to_trace(1);
 }

 int r_main()
 {
     set_trace_test(3);
     Task_Create_System(system_task_1, 0);
     Task_Create_System(system_task_2, 1);
     return 0;
 }

 #endif
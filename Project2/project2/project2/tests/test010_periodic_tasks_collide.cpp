/*
 * test010_periodic_tasks_collide.cpp
 *
 * Created: 3/8/2015 2:32:10 PM
 *  Author: jaguz_000
 */ 
 #ifdef USE_TEST_010

 /************************************************************************/
 /* Expected: ERR_RUN_5_PERIODIC_TASKS_SCHEDULED_AST                                                      */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"

 void periodic_task_1()
 {
     int i;
     for (i = 0; i < 3; i++)
     {
         add_to_trace(i);
         Task_Next();
     }

     add_to_trace(50);
     print_trace();
 }

 void periodic_task_2()
 {
     int i;
     for (i = 0; i < 3; i++)
     {
         add_to_trace(i + 3);
         Task_Next();
     }

     add_to_trace(10);
     print_trace();
 }

 int r_main()
 {
     set_trace_test(10);
     Task_Create_Periodic(periodic_task_1, 0, 50, 10, 1);
     Task_Create_Periodic(periodic_task_2, 0, 50, 10, 1);
     return 0;
 }

 #endif
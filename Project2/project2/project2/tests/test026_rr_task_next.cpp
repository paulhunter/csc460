/*
 * test026_rr_task_next.cpp
 *
 * Created: 3/9/2015 3:47:05 PM
 *  Author: jaguz_000
 */ 

 #ifdef USE_TEST_026

 /************************************************************************/
 /* Expected: T026;1;2;3                                                 */
 /************************************************************************/

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "../trace/trace.h"
 #include "../profiler.h"

 void rr_task_2()
 {
     DisableProfileSample7();
     add_to_trace(2);
 }

void rr_task_1()
{
    add_to_trace(1);
    EnableProfileSample7();
    Task_Next();
    add_to_trace(3);
    print_trace();
    CORRECT_ON;
}

 int r_main()
 {
     set_trace_test(26);
     INCORRECT_ON;
     Task_Create_RoundRobin(rr_task_1, 0);
     Task_Create_RoundRobin(rr_task_2, 0);
     return 0;
 }

#endif
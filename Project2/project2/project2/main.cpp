#include "main.h"
#include "os.h"

#define USE_TEST_001

// Tests
#include "tests/test000_sanity.cpp"
#include "tests/test001_NowCorrectTime.cpp"
#include "tests/test002_create_system_task.cpp"
#include "tests/test003_multiple_system_task_order.cpp"
#include "tests/test004_system_task_preemption.cpp"
#include "tests/test005_system_preempt_nonsystem_task.cpp"
#include "tests/test006_create_periodic_task.cpp"
#include "tests/test007_periodic_worst_case_time_larger_period.cpp"
#include "tests/test008_periodic_period_negative.cpp"
#include "tests/test009_multiple_periodic_tests.cpp"
#include "tests/test010_periodic_tasks_collide.cpp"
#include "tests/test011_periodic_task_not_completed_on_time.cpp"
#include "tests/test012_periodic_system_preemption.cpp"

#ifdef USE_MAIN

int r_main()
{	
    //test_1_correct_led_on();
	test_2_incorrect_led_on();
    return 0;
}

#endif
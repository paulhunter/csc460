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

#ifdef USE_MAIN

int r_main()
{	
    //test_1_correct_led_on();
	test_2_incorrect_led_on();
    return 0;
}

#endif
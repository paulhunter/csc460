/*
 * CPPFile1.cpp
 *
 * Created: 3/7/2015 7:29:04 PM
 *  Author: Justin Guze
 */ 

#include "test.h"

void init()
{
    INIT_TEST_LEDS;
}

void test_1_correct_led_on()
{
    init();
    CORRECT_ON;
}

void test_2_incorrect_led_on()
{
    init();
    INCORRECT_ON;
}
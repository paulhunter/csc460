#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CORRECT_LED (uint8_t)(_BV(PB2))
#define INCORRECT_LED (uint8_t)(_BV(PB3))

#define INIT_TEST_LEDS (DDRB = CORRECT_LED | INCORRECT_LED)

#define CORRECT_ON PORTB = (uint8_t)(CORRECT_LED)
#define CORRECT_OFF PORTB = (uint8_t)(0)
#define INCORRECT_ON PORTB = (uint8_t)(INCORRECT_LED)
#define INCORRECT_OFF PORTB = (uint8_t)(0)

void test_1_correct_led_on();
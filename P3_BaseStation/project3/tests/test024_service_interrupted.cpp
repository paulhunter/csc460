/*
 * test024_service_interrupted.cpp
 *
 * Created: 3/8/2015 8:31:56 PM
 *  Author: jaguz_000
 */ 



#ifdef USE_TEST_024

/************************************************************************/
/* Expected: T024;1;2;                                                  */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../trace/trace.h"

SERVICE * services[3];
uint16_t static volatile s_index = 0;

void setup() {

    //Clear timer configs
    TCCR3A = 0;
    TCCR3B = 0;

    //Set to CTC (mode 4)
    TCCR3B |= (1<<WGM32);
    
    //Set prescaler to 256
    TCCR3B |= (1<<CS32);
    
    //Set TOP value (0.5 seconds)
    OCR3A = 31250;
    
    //Enable interrupt A for timer 3.
    TIMSK3 |= (1<<OCIE3A);
    
    //Set timer to 0 (optional here).
    TCNT3 = 0;
}

ISR(TIMER3_COMPA_vect)
{
    Service_Publish(services[s_index], 3);
    s_index = (s_index + 1) % 3;
    add_to_trace(2);
    print_trace();
}

int r_main()
{
    set_trace_test(24);
    INCORRECT_ON;
    setup();
    int i;

    for (i = 0; i < 3; i++)
    {
        services[i] = Service_Init();
    }

    int v;
    add_to_trace(1);
    Service_Subscribe(services[0], &v);
    add_to_trace(v);

    CORRECT_ON;
    // Disable further interrupts
    TIMSK3 = 0;
    return 0;
}

#endif
/*
 * basestation.cpp
 *
 * Created: 3/19/2015 3:40:01 PM
 *  Author: jaguz_000
 */ 

#ifdef BASESTATION

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../radio/cops_and_robbers.h"
#include "../radio/nRF24L01.h"
#include "../radio/packet.h"
#include "../radio/radio.h"
#include "../radio/sensor_struct.h"
#include "../radio/spi.h"
#include "../trace/trace.h"

#define RADIO_VCC_PIN (uint8_t) (_BV(PB4))
#define INIT_RADIO_PIN (DDRB = RADIO_VCC_PIN)
#define RADIO_LOW (PORTB = (uint8_t)0)
#define RADIO_HIGH (PORTB = (uint8_t)RADIO_VCC_PIN)

#define RADIO_SEND_DEBUG_PIN (uint8_t) (_BV(PA0))
#define RED_DEBUG_PIN (uint8_t) (_BV(PA1))
#define INIT_DEBUG_LEDS (DDRA = RADIO_SEND_DEBUG_PIN) // | RED_DEBUG_PIN)
#define RADIO_SEND_DEBUG_OFF PORTA = (uint8_t)(0)
#define RADIO_SEND_DEBUG_ON PORTA = (uint8_t)(RADIO_SEND_DEBUG_PIN)
#define RED_DEBUG_OFF PORTA = (uint8_t)(0)
#define RED_DEBUG_ON PORTA = (uint8_t)(RED_DEBUG_PIN)

#define HIGH_BYTE(x) x>>8
#define LOW_BYTE(x) x & 0x00FF

#define ROOMBA_DRIVE_OPCODE 137

volatile uint8_t rxflag = 0;
uint8_t radio_addr[5] = { 0x01, 0xFC, 0x96, 0x92, 0x00 };
uint8_t radio_target = 0;

radiopacket_t transmission_packet;

void setup_radio()
{
    INIT_RADIO_PIN;
    RADIO_LOW;
    RADIO_HIGH;

    rxflag = 0;

    Radio_Init();

    Radio_Configure_Rx(RADIO_PIPE_0, radio_addr, ENABLE);
    Radio_Configure(RADIO_2MBPS, RADIO_HIGHEST_POWER);
    Radio_Set_Tx_Addr(ROOMBA_ADDRESSES[radio_target]);
}

void setup_joysticks()
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

    ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
    ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

    // No MUX values needed to be changed to use ADC0

    ADCSRA |= (1 << 5);  // Set ADC to Free-Running Mode
    ADCSRA |= (1 << ADEN);  // Enable ADC
    ADCSRA |= (1 << ADSC);  // Start A2D Conversions
}

void set_sender_address(uint8_t * sender_address, uint8_t * address, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        sender_address[i] = address[i];
    }
}

void send_packet()
{
    while(1)
    {
        RADIO_SEND_DEBUG_ON;
        transmission_packet.type = COMMAND;
        pf_command_t * command = &(transmission_packet.payload.command);
        set_sender_address(command->sender_address, radio_addr, 5);

        command->command = ROOMBA_DRIVE_OPCODE;
        command->num_arg_bytes = 4;
        command->arguments[0] = HIGH_BYTE(500);
        command->arguments[1] = LOW_BYTE(500);
        command->arguments[2] = HIGH_BYTE(10);
        command->arguments[3] = LOW_BYTE(10);
        Radio_Transmit(&transmission_packet, RADIO_WAIT_FOR_TX);
        RADIO_SEND_DEBUG_OFF;
        Task_Next();
    }
}

void turn_on(int i)
{
    if (!i)
    {
      //  RADIO_SEND_DEBUG_ON;
    } else if (i)
    {
        //RED_DEBUG_ON;
    }
    RADIO_SEND_DEBUG_ON;
}

void read_adc(int i)
{
    if (ADCH < 100)
    {
        turn_on(i);
    } else if (ADCH > 156)
    {
        turn_on(i);
    } else {
        RADIO_SEND_DEBUG_OFF;
      //  RED_DEBUG_OFF;
    }
}

void poll_joysticks()
{
    int i = 0;
    while(1)
    {
       ADMUX = 0;
       ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
       ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading    
       for(i = 0; i < 1; i++)
       {
           // ADMUX &= (uint8_t) 240;
         //   ADMUX |= (uint8_t) i; 
         //   add_to_trace(ADMUX);
         read_adc(i);
         
         ADMUX = 0xC0 | (uint8_t)i;
        // Task_Next();
        // ADCSRA |= (1 << ADSC);
       }
       // print_trace();
       Task_Next();
    }
}

void turn_off()
{
    while(1)
    {
        Task_Next();
    }
}

void radio_rxhandler(uint8_t pipe_number)
{
    rxflag = 1;
}

int r_main()
{
    INIT_DEBUG_LEDS;
    RADIO_SEND_DEBUG_OFF;
    Task_Create_System(setup_radio, 0);
    Task_Create_System(setup_joysticks, 0);
   // Task_Create_Periodic(send_packet, 0, 100, 50, 5);
    Task_Create_Periodic(poll_joysticks, 0, 5, 1, 10);
   // Task_Create_Periodic(turn_off, 0, 100, 50, 60);

    return 0;
}

#endif
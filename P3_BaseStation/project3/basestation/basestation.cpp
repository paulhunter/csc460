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

/* Joystick Sampling Data */
typedef struct
{
    uint8_t leftJoyX;
    uint8_t leftJoyY;

    uint8_t rightJoyX;
    uint8_t rightJoyY;
    /* For the time being there is only one, but in the future
       one could expand this to contain eight buttons, using
       bit flags to indicate status, pressed = 1, unpressed = 0. */    
    uint8_t buttons;
} controllerState_t;

#define NUM_CONTROLLERS 4
controllerState_t controllers[NUM_CONTROLLERS];



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

/* 
   oooo                                    .    o8o            oooo                 
   `888                                  .o8    `"'            `888                 
    888  .ooooo.  oooo    ooo  .oooo.o .o888oo oooo   .ooooo.   888  oooo   .oooo.o 
    888 d88' `88b  `88.  .8'  d88(  "8   888   `888  d88' `"Y8  888 .8P'   d88(  "8 
    888 888   888   `88..8'   `"Y88b.    888    888  888        888888.    `"Y88b.  
    888 888   888    `888'    o.  )88b   888 .  888  888   .o8  888 `88b.  o.  )88b 
.o. 88P `Y8bod8P'     .8'     8""888P'   "888" o888o `Y8bod8P' o888o o888o 8""888P' 
`Y888P            .o..P'                                                            
                  `Y8P'                                                           
*/
void setup_joysticks()
{
    /* We use a single ADC onboard the package, using an
       onbaord multiplexer to select one of the 16 available
       analog inputs. Joystck channels are connected as follows
         0 -  3 Controller 1.  0/ 1 Left X/Y,  2/ 3 Right X/Y.  
         4 -  7 Controller 2.  4/ 5 Left X/Y,  6/ 7 Right X/Y.  
         8 - 11 Controller 3.  8/ 9 Left X/Y, 10/11 Right X/Y.  
        12 - 15 Controller 4. 12/13 Left X/Y, 14/15 Right X/Y.

       Each Controller also features a single push button connected
       to digital inputs as follows
        52 - Controller 1 : Button A
        53 - Controller 2 : Button A
        54 - Controller 3 : Button A
        55 - Controller 4 : Button A
    */
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

    ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
    ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

    ADCSRA |= (1 << ADEN);  // Enable ADC
}

/**
 * Read an analog value from a given channel. 
 * On the AT mega2560, there are 16 available channels. 
 */
uint8_t read_analog(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0 ) | (0x0F & channel); //Set the mux to the appropriate value.
    /* We now set the Start Conversion bit to trigger a fresh sample. */
    ADCSRA |= (1 << ADSC);
    /* We wait on the ADC to complete the operation, when it completes, the hardware
       will set the ADSC bit to 0. */
    while ((ADCSRA & (1 << ADSC)));
    /* We setup the ADC to shift input to left, so we simply return the High register. */
    return ADCH;
}

/**
 * A task function which is used to periodically poll the status of the controllers 
 */
void periodic_poll_controllers()
{
    int i, a;
    for(;;)
    {
        for(i = 0, a = 0; i < NUM_CONTROLLERS; i++, a += 4)
        {
            controllers[i].leftJoyX = read_analog(a);
            controllers[i].leftJoyY = read_analog(a + 1);
            controllers[i].rightJoyX = read_analog(a + 2);
            controllers[i].rightJoyY = read_analog(a + 3);
            //TODO: Add sample for Digital Buttons. 
        }

        Task_Next();
    }
}

/* END OF JOYSTICK METHODS */

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
    Task_Create_Periodic(periodic_poll_controllers, 0, 5, 1, 10);
   // Task_Create_Periodic(turn_off, 0, 100, 50, 60);

    return 0;
}

#endif
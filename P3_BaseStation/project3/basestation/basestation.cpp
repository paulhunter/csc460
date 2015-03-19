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
#define INIT_DEBUG_LEDS (DDRA = RADIO_SEND_DEBUG_PIN)
#define RADIO_SEND_DEBUG_OFF PORTA = (uint8_t)(0)
#define RADIO_SEND_DEBUG_ON PORTA = (uint8_t)(RADIO_SEND_DEBUG_PIN)

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

void radio_rxhandler(uint8_t pipe_number)
{
    rxflag = 1;
}

int r_main()
{
    INIT_DEBUG_LEDS;
    Task_Create_System(setup_radio, 0);
    Task_Create_Periodic(send_packet, 0, 100, 50, 5);
    return 0;
}

#endif
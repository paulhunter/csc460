/*
 * main.cpp
 *
 *  Created on: 18-Feb-2011
 *      Author: nrqm
 *
 * Example program for time-triggered scheduling on Arduino.
 *
 * This program pulses one pin every 500 ms and another pin every 300 ms
 *
 * There are two important parts in this file:
 * 		- at the end of the setup function I call Scheduler_Init and Scheduler_StartTask.  The latter is called once for
 * 		  each task to be started (note that technically they're not really tasks because they share the stack and don't
 * 		  have separate contexts, but I call them tasks anyway because whatever).
 * 		- in the loop function, I call the scheduler dispatch function, which checks to see if a task needs to run.  If
 * 		  there is a task to run, then it its callback function defined in the StartTask function is called.  Otherwise,
 * 		  the dispatch function returns the amount of time (in ms) before the next task will need to run.  The idle task
 * 		  can then do something useful during that idle period (in this case, it just hangs).
 *
 * To use the scheduler, you just need to define some task functions and call Scheduler_StartTask in the setup routine
 * for each of them.  Keep the loop function below the same.  The scheduler will call the tasks you've created, in
 * accordance with the creation parameters.
 *
 * See scheduler.h for more documentation on the scheduler functions.  It's worth your time to look over scheduler.cpp
 * too, it's not difficult to understand what's going on here.
 */
 
#include "Arduino.h"

#include "scheduler.h"

#include "cops_and_robbers.h"
#include "nRF24L01.h"
#include "packet.h"
#include "radio.h"
#include "sensor_struct.h"
#include "spi.h"

#define HIGH_BYTE(x) x>>8
#define LOW_BYTE(x) x & 0x00FF 

////// GLOBALS /////
 
uint8_t idle_pin = 7;

 /* Joystick */

 //Analog pins used for the joystick. 
#define JOY_X_APIN 0
#define JOY_Y_APIN 1
#define JOY_SW_DPIN 3

#define RADIO_VCC_PIN 10

#define MAX_JOY_X_VAL 2000
#define MIN_JOY_X_VAL -2000
#define LOW_JOY_X_DZ -20
#define HIGH_JOY_X_DZ 20

#define MAX_JOY_Y_VAL 360
#define MIN_JOY_Y_VAL -360
#define LOW_JOY_Y_DZ -20
#define HIGH_JOY_Y_DZ 20

// Roomba opcodes

#define ROOMBA_DRIVE_OPCODE 137

//Neutral Roomba Values
#define ROOMBA_NEUTRAL_DEGREE 0x8000
#define ROOMBA_LEFT_DEGREE 200
#define ROOMBA_RIGHT_DEGREE -200

//Global variables
int joy_x_value;
int joy_y_value;
int joy_sw_pushed;


/* Radio */
//Our defined address - hopefully not conflicting.
uint8_t radio_addr[5] = { 0x01, 0xFC, 0x96, 0x92, 0x00 };
volatile uint8_t rxflag = 0;
uint8_t transFlag = 0;
uint8_t radio_target = 0; 

radiopacket_t recvPacket;
radiopacket_t transPacket;



// idle task
void idle(uint32_t idle_period)
{
	// this function can perform some low-priority task while the scheduler has nothing to run.
	// It should return before the idle period (measured in ms) has expired.  For example, it
	// could sleep or respond to I/O.
 
	// example idle function that just pulses a pin.
	//digitalWrite(idle_pin, HIGH);
	delay(idle_period);
	//digitalWrite(idle_pin, LOW);
}

/* RAIDO METHODS */

//Configure our radio for use.
void radio_setup()
{
        pinMode(RADIO_VCC_PIN, OUTPUT);
        digitalWrite(RADIO_VCC_PIN, LOW);
        digitalWrite(RADIO_VCC_PIN, HIGH);
	
        rxflag = 0;

	Radio_Init();
	// configure the receive settings for radio pipe 0
	Radio_Configure_Rx(RADIO_PIPE_0, radio_addr, ENABLE);
	// configure radio transceiver settings.
	Radio_Configure(RADIO_2MBPS, RADIO_HIGHEST_POWER);
	Radio_Set_Tx_Addr(ROOMBA_ADDRESSES[radio_target]);

}

void radio_rxhandler(uint8_t pipe_number)
{
	rxflag = 1;
}

/* JOYSTICK METHODS */

//Init and set appropriate pin modes for the interfaces to the joystick.
void joystick_setup()
{
	//Configure digital pin used for Joystick Sw
	pinMode(JOY_SW_DPIN, INPUT);
	joy_x_value = 0;
	joy_y_value = 0;
	joy_sw_pushed = 0;
}

void task_poll_sensors()
{
	//Sample each of the two axes at the same time.
	joy_x_value = analogRead(JOY_X_APIN);
	joy_y_value = analogRead(JOY_Y_APIN);

	//Apply Deadzone and scaling to the X axis.
	int val = map(joy_y_value, 0, 1023, MIN_JOY_Y_VAL, MAX_JOY_Y_VAL);
	if (val <= HIGH_JOY_Y_DZ && val >= LOW_JOY_Y_DZ) 
	{
		joy_y_value = 0;
	}
	else 
	{
		joy_y_value = val;
	}

	//Apply deadzone and scaling to the Y axis. 
	val = map(joy_x_value, 0, 1023, MIN_JOY_X_VAL, MAX_JOY_X_VAL);
	if (val <= HIGH_JOY_X_DZ && val >= LOW_JOY_X_DZ) 
	{
		joy_x_value = ROOMBA_NEUTRAL_DEGREE;
	}
	else 
	{
                if (val < 0)
                {
                    joy_x_value = MIN_JOY_X_VAL - val;  
                } else {
                    joy_x_value = MAX_JOY_X_VAL - val; 
                }
	}

	//Sample and set the Switch flag.
	if(digitalRead(JOY_SW_DPIN) == LOW)
	{
		joy_sw_pushed = 1;
	}
	else
	{
		joy_sw_pushed = 0;
	}

        roombaMoveCommand(joy_y_value, joy_x_value);

}

void task_send_packet()
{
    // No packet to send
    if (!transFlag)
    {
       return; 
    }
    Radio_Transmit(&transPacket, RADIO_WAIT_FOR_TX);
    transFlag = 0;
}

void setSenderAddress(pf_command_t * command, uint8_t * address, int length)
{
       int i;
       for (i = 0; i < length; i++)
       {
          command->sender_address[i] = address[i];
       } 
}

void roombaMoveCommand(int16_t velocity, int16_t degree)
{
        transPacket.type = COMMAND;
        pf_command_t * command =  &(transPacket.payload.command);
        setSenderAddress(command, radio_addr, 5);
        
        command->command = ROOMBA_DRIVE_OPCODE;
        command->num_arg_bytes = 4;
        command->arguments[0] = HIGH_BYTE(velocity);
        command->arguments[1] = LOW_BYTE(velocity);
        command->arguments[2] = HIGH_BYTE(degree);
        command->arguments[3] = LOW_BYTE(degree);
        transFlag = 1;
}


void setup()
{
//        Serial.begin(9600);
	joystick_setup();
	radio_setup();
 
	Scheduler_Init();
 
	// Start task arguments are:
	//		start offset in ms, period in ms, function callback
	Scheduler_StartTask(0, 20, task_poll_sensors);
        Scheduler_StartTask(1, 200, task_send_packet);
}
 
void loop()
{
	uint32_t idle_period = Scheduler_Dispatch();
	if (idle_period)
	{
		idle(idle_period);
	}
}

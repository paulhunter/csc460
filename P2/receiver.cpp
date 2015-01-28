/*
#include 
#include "arduino/Arduino.h"
#include "radio/radio.h"
 
uint8_t station_addr[5] = { 0xE4, 0xE4, 0xE4, 0xE4, 0xE4 };
 
// this must be volatile because it's used in an ISR (radio_rxhandler).
volatile uint8_t rxflag = 0;
 
radiopacket_t packet;
 
int main()
{
	// string buffer for printing to UART
	char output[128];
	sei();
	init();
 
	// start the serial output module
	Serial.begin(100000);
	pinMode(13, OUTPUT);
	pinMode(10, OUTPUT);	// radio's Vcc pin is connected to digital pin 10.
 
	// reset the radio
	digitalWrite(10, LOW);
	delay(100);
	digitalWrite(10, HIGH);
	delay(100);
 
	// initialize the radio, including the SPI module
	Radio_Init();
 
	// configure the receive settings for radio pipe 0
	Radio_Configure_Rx(RADIO_PIPE_0, station_addr, ENABLE);
	// configure radio transciever settings.
	Radio_Configure(RADIO_2MBPS, RADIO_HIGHEST_POWER);
 
	// print a message to UART to indicate that the program has started up
	snprintf(output, 128, "STATION START\n\r");
	Serial.print(output);
 
	for (;;)
	{
		if (rxflag)
		{
			if (Radio_Receive(&amp;packet) != RADIO_RX_MORE_PACKETS)
			{
				// if there are no more packets on the radio, clear the receive flag;
				// otherwise, we want to handle the next packet on the next loop iteration.
				rxflag = 0;
			}
 
			// This station is only expecting to receive MESSAGE packets.
			if (packet.type != MESSAGE)
			{
				snprintf(output, 128, "Error: wrong packet type (type %d).\n\r", packet.type);
				Serial.print(output);
				continue;
			}
 
			// Set the transmit address to the one specified in the message packet.
			Radio_Set_Tx_Addr(packet.payload.message.address);
 
			// Print out the message, along with the message ID and sender address.
			snprintf(output, 128, "Message ID %d from 0x%.2X%.2X%.2X%.2X%.2X: '%s'\n\r",
					packet.payload.message.messageid,
					packet.payload.message.address[0],
					packet.payload.message.address[1],
					packet.payload.message.address[2],
					packet.payload.message.address[3],
					packet.payload.message.address[4],
					packet.payload.message.messagecontent);
			Serial.print(output);
 
			// Reply to the sender by sending an ACK packet.
			packet.type = ACK;
 
			if (Radio_Transmit(&amp;packet, RADIO_WAIT_FOR_TX) == RADIO_TX_MAX_RT)
			{
				snprintf(output, 128, "Could not reply to sender.\n\r");
				Serial.print(output);
			}
			else
			{
				snprintf(output, 128, "Replied to sender.\n\r");
				Serial.print(output);
			}
			// delay for a bit to give a visible flash on the LED.
			delay(50);
			digitalWrite(13, LOW);
		}
	}
 
	return 0;
}
 

// * This function is a hook into the radio's ISR.  It is called whenever the radio generates an RX_DR (received data ready) interrupt.
 
void radio_rxhandler(uint8_t pipenumber)
{
	// just set a flag and toggle an LED.  The flag is polled in the main function.
	rxflag = 1;
	digitalWrite(13, HIGH);
} */

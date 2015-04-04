/*
 * CPPFile1.cpp
 *
 * Created: 4/3/2015 4:35:56 PM
 *  Author: jaguz_000
 */ 

#ifdef USE_ROOMBA


#include <avr/io.h>
#include "../os.h"
#include "radio.h"
#include "roomba.h"
#include "roomba_sci.h"
#include "uart.h"
#include "ir.h"
#include "game.h"

#define RADIO_SEND_DEBUG_PIN (uint8_t) (_BV(PA0))
#define INIT_DEBUG_LEDS (DDRA = RADIO_SEND_DEBUG_PIN)
#define RADIO_SEND_DEBUG_OFF PORTA = (uint8_t)(0)
#define RADIO_SEND_DEBUG_ON PORTA = (uint8_t)(RADIO_SEND_DEBUG_PIN)

// Jordan's defines from main.c

#define RADIO_VCC_DDR DDRL
#define RADIO_VCC_PORT PORTL
#define RADIO_VCC_PIN PL2

//INDICATOR DEFINITIONS:

#define MODIFY_INDICATOR PC0 	//Digital I/O pin 37 on arduino mega, 	A8 on ATMega 2560 pinout.
#define HUMAN_INDICATOR PC1 	//Digital I/O pin 36 on arduino mega, 	A9 on ATMega 2560 pinout.
#define ZOMBIE_INDICATOR PC2 	//Digital I/O pin 35 on arduino mega, 	A10 on ATMega 2560 pinout.

#define RX_RADIO_PACKET PB4 	//Blinks when a radio packet is recieved Digital I/O pin 10 on arduino.
#define TX_RADIO_PACKET PB5		//Blinks when radio packet is transmitted. Digital I/O 11 on arduino.

#define IR_RX PB6				//Blinks when an IR packet is received.


#define MODIFY_INDICATOR_ON() PORTC |= (1 << MODIFY_INDICATOR)
#define MODIFY_INDICATOR_OFF() PORTC &= ~(1<<MODIFY_INDICATOR)

#define ZOMBIE_STUN_INDICATOR_OFF() MODIFY_INDICATOR_OFF()
#define ZOMBIE_STUN_INDICATOR_ON() MODIFY_INDICATOR_ON()
#define ZOMBIE_TEAM_INDICATOR_ON() PORTC |= (1 << PC2)
#define ZOMBIE_TEAM_INDICATOR_OFF() PORTC &= ~(1 << PC2)

#define HUMAN_SHIELD_INDICATOR_ON() MODIFY_INDICATOR_ON()
#define HUMAN_SHIELD_INDICATOR_OFF() MODIFY_INDICATOR_OFF()
#define HUMAN_TEAM_INDICATOR_ON() PORTC |= (1 << PC1)
#define HUMAN_TEAM_INDICATOR_OFF() PORTC &= ~(1 << PC1)

#define RADIO_PACKET_RX_TOGGLE() PORTB ^= (1 << RX_RADIO_PACKET)
#define RADIO_PACKET_TX_TOGGLE() PORTB ^= (1 << TX_RADIO_PACKET)

#define IR_RX_TOGGLE() PORTB ^= (1 << IR_RX)

SERVICE* radio_receive_service;
SERVICE* ir_receive_service;
uint8_t roomba_num = 0;
uint8_t ir_count = 0;

struct player_state {
    uint8_t player_id;
    uint8_t team;
    uint8_t state;
    uint8_t hit_flag;
    uint8_t last_ir_code;
};
struct player_state player;

void setup_roomba() {
    Roomba_Init();
    Radio_Set_Tx_Addr(base_station_address);

    // Initialize pins
    DDRB |= (1<<PB4);
    DDRB |= (1<<PB5);
    DDRB |= (1<<PB6);
    PORTB &= ~(1<<PB4);
    PORTB &= ~(1<<PB5);
    PORTB &= ~(1<<PB6);

    DDRC |= (1 << PC0); //stun/shield indicator (pin 37 on arduino mega)
    DDRC |= (1 << PC1); //Human Indicator (pin 36 on arduino mega)
    DDRC |= (1 << PC2); //Zombie indicator (pin 35 on arduino mega)

    PORTC &= ~(1<<PC0);
    PORTC &= ~(1<<PC1);
    PORTC &= ~(1<<PC2);


    // Servo timers
//    TCCR2A |= (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
//    TCCR2B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
//    ICR1 = 4999; // fPWM=50Hz 
//    DDRH |= (1<<PH3);
}

void radio_rxhandler(uint8_t pipenumber) {
    RADIO_PACKET_RX_TOGGLE();
    Service_Publish(radio_receive_service,0);
}

void ir_rxhandler() {
    IR_RX_TOGGLE();
    int16_t value = IR_getLast();
    int i = 0;
    for(i = 0; i< 4; ++i){
        if( value == PLAYER_IDS[i]){
            if( value != PLAYER_IDS[roomba_num]){
                Service_Publish(ir_receive_service,value);
            }
        }
    }

}

struct roomba_command {
    uint8_t opcode;
    uint8_t num_args;
    uint8_t args[32];
};

void SendCommandToRoomba(struct roomba_command* cmd){
    if (cmd->opcode == START ||
    cmd->opcode == BAUD ||
    cmd->opcode == SAFE ||
    cmd->opcode == FULL ||
    cmd->opcode == SENSORS)
    {
        return;
    }

    //Pass the command to the Roomba.
    Roomba_Send_Byte(cmd->opcode);
    int i = 0;
    for (i = 0; i < cmd->num_args; i++){
        Roomba_Send_Byte(cmd->args[i]);
    }
}

void Servo_Rotate(int16_t servo_vy)
{
    OCR1A=servo_vy;
}

void handleRoombaInput(pf_game_t* game)
{
    int16_t vx = (game->velocity_x/(255/9) - 4)*124;
    int16_t vy = (game->velocity_y/(255/9) - 4)*-124;

   // int16_t servo_vy = (game->servo_velocity_y / (255/9) - 4) * -124;

    if(vy == 0){
        if( vx > 0){
            vx = 1;
            vy = 300;
            } else if(vx < 0){
            vx = -1;
            vy = 300;
        }
    }

    // Trollolololololololol
    if(game->game_team == ZOMBIE && game->game_state == (uint8_t)STUNNED) {
        vx = 1;
        vy = 150;
    }

    Roomba_Drive(vy,-1*vx);

   // Servo_Rotate(servo_vy);

    // fire every 5th packet
    if( ir_count == 5){
        IR_transmit(player.player_id);
        ir_count = 0;
    }
    ir_count++;
}

void handleStateInput(pf_game_t* game){
    player.team = game->game_team;
    player.state = game->game_state;

    switch(player.team) {
        case ZOMBIE:
        ZOMBIE_TEAM_INDICATOR_ON();
        HUMAN_TEAM_INDICATOR_OFF();
        break;
        case HUMAN:
        HUMAN_TEAM_INDICATOR_ON();
        ZOMBIE_TEAM_INDICATOR_OFF();
        break;
        default:
        break;
    }

    if(player.state == SHIELDED || player.state == NORMAL) {
        MODIFY_INDICATOR_ON();
        } else if(player.state == SHIELDLESS || player.state == STUNNED) {
        MODIFY_INDICATOR_OFF();
    }
}

void send_back_packet()
{
    //Radio_Flush();
    radiopacket_t packet;

    packet.type = GAME;
    int i = 0;
    for(i = 0;i < 5; ++i){
        packet.payload.game.sender_address[i] = ROOMBA_ADDRESSES[roomba_num][i];
    }

    packet.payload.game.game_player_id = player.player_id;
    packet.payload.game.game_team = player.team;
    packet.payload.game.game_state = player.state;
    packet.payload.game.game_hit_flag = (player.last_ir_code != 0) ? 1: 0;
    packet.payload.game.game_enemy_id = player.last_ir_code;

    RADIO_PACKET_TX_TOGGLE();   

    // reset the stuff
    player.hit_flag = 0;
    player.last_ir_code = 0;

    // Radio_Transmit(&packet, RADIO_WAIT_FOR_TX);
    Radio_Transmit(&packet, RADIO_RETURN_ON_TX);
}

void update_ir_state()
{
    int16_t value;
    for(;;){
        Service_Subscribe(ir_receive_service,&value);
        player.last_ir_code = value;
    }
}

void power_cycle_radio()
{
    //Turn off radio power.
    RADIO_VCC_DDR |= (1 << RADIO_VCC_PIN);
    RADIO_VCC_PORT &= ~(1<<RADIO_VCC_PIN);
    _delay_ms(500);
    RADIO_VCC_PORT |= (1<<RADIO_VCC_PIN);
    _delay_ms(500);
}

void update_radio_state() {
    int16_t value;

    for(;;) {
        Service_Subscribe(radio_receive_service,&value);
        //Handle the packets

        RADIO_RX_STATUS result;
        radiopacket_t packet;
        do {
            result = Radio_Receive(&packet);

            if(result == RADIO_RX_SUCCESS || result == RADIO_RX_MORE_PACKETS) {
                if( packet.type == GAME)
                {
                    handleRoombaInput(&packet.payload.game);
                    handleStateInput(&packet.payload.game);
                }
            }

        } while (result == RADIO_RX_MORE_PACKETS);

        send_back_packet();
    }
}

int r_main(void)
{
    INIT_DEBUG_LEDS;

    power_cycle_radio();

    //Initialize radio.
    Radio_Init();
    IR_init();
    Radio_Configure_Rx(RADIO_PIPE_0, ROOMBA_ADDRESSES[roomba_num], ENABLE);
    Radio_Configure(RADIO_1MBPS, RADIO_HIGHEST_POWER);

    radio_receive_service = Service_Init();
    ir_receive_service = Service_Init();

    // System tasks
    Task_Create_System(setup_roomba, 0);
    Task_Create_RoundRobin(update_radio_state, 0);
    Task_Create_RoundRobin(update_ir_state, 0);

    player.player_id = PLAYER_IDS[roomba_num];
    player.team = 0;
    player.state = 0;
    player.hit_flag = 0;
    player.last_ir_code = 0;

    Task_Terminate();
    return 0 ;
}

#endif
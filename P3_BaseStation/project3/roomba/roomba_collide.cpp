/*
 * roomba_collide.cpp
 *
 * Created: 4/4/2015 5:13:19 PM
 *  Author: jaguz_000 
*/ 

#include "roomba_collide.h"

uint8_t get_bump_wheel_sensor()
{
    roomba_sensor_data_t sensor_packet;
    Roomba_UpdateSensorPacket(EXTERNAL, &sensor_packet);
    return sensor_packet.bumps_wheeldrops;
}

int is_bumped()
{
    uint8_t bump_wheel_sensor = get_bump_wheel_sensor();
    if (bump_wheel_sensor & BUMP_LEFT || bump_wheel_sensor & BUMP_RIGHT)
    {
        return 1;
    }
    return 0;
}


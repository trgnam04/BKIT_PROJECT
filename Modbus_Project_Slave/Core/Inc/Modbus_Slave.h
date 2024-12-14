/*
 * Modbus_Slave.h
 *
 *  Created on: Dec 5, 2024
 *      Author: ASUS
 */

#ifndef INC_MODBUS_SLAVE_H_
#define INC_MODBUS_SLAVE_H_

#include "main.h"
#include "lcd.h"
#include "modbus485_fsm.h"
#include "rs485.h"
#include <string.h>
#include "sensor.h"

#define POTENTIOMETER_REGISTER_ADDRESS 0
#define LED_REGISTER_ADDRESS 2
#define LIGHT_REGISTER_ADDRESS 4
#define TEMPERATURE_REGISTER_ADDRESS 6
#define CURRENT_REGISTER_ADDRESS 10
#define VOLTAGE_REGISTER_ADDRESS 14

#define SLAVE_ADDRESS 0x02

#define COIL_A	1
#define COIL_B	2
#define COIL_C	4
#define COIL_D	8
#define COIL_E	16
#define COIL_F	32


typedef enum{
	IDLE,
	WAITTING_FOR_CMD,
	COMMAND_PARSER,
	READ_MULTIPLE_HOLDING_REGISTER_HANDLER,
	READ_HOLDING_REGISTER_HANDLER,
	WRITE_HOLDING_REGISTER_HANDLER
} Slave_behavior;

typedef struct{
	uint8_t s;
} Slave_Subsignal;

typedef struct{
	Slave_Subsignal subsignal;
	Slave_behavior signal;
	uint8_t Register[20];
	uint8_t Address;

} Slave_Device;

extern Slave_Device BKIT01;


void InitSlave(Slave_Device* hDev);
void CommandParser_handler(Slave_Device* hDev);
void Read_multiple_holding_register_handler(Slave_Device* hDev);
void Read_holding_register_handler(Slave_Device* hDev);
void Write_holding_register_handler(Slave_Device* hDev);
void slave_behavior(Slave_Device* hDev);
void TurnOffAllLed(void);

#endif /* INC_MODBUS_SLAVE_H_ */


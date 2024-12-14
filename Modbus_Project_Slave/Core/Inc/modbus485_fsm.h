/*
 * modbus45_fsm.h
 *
 *  Created on: Dec 5, 2024
 *      Author: nguye
 */

#ifndef INC_MODBUS485_FSM_H_
#define INC_MODBUS485_FSM_H_

#include "usart.h"
#include "software_timer.h"
#include <stdio.h>
#include "crc16.h"

#define TI_RS485_T35_TIMER	3
#define TI_RS485_T15_TIMER	4

#define TI_RS485_T35_TIME	4
#define TI_RS485_T15_TIME	1


enum modbus485_state_t
{
	RS485_SUPER_INIT,
	RS485_INIT,
	RS485_IDLE,
	RS485_TRANSMIT,
	RS485_RECEIVE,
	RS485_WAITING_CONTROL

};

enum frame_status_t
{
	FRAME_OK,
	FRAME_NOT_OK
};

extern uint8_t receive_buffer[256];


void rs485_send_cmd(uint8_t* cmd_without_crc, uint8_t cmd_size_without_crc);
uint8_t is_frame_status();
uint8_t get_frame_size();

uint8_t is_character_received();
uint8_t is_transmit();
uint8_t crc_checking();

void rs485_fsm_init();
void rs485_fsm();

void init_rs485_super_init();
void rs485_super_init();
void init_rs485_init();
void rs485_init();
void init_rs485_idle();
void rs485_idle();
void init_rs485_transmit();
void rs485_transmit();
void init_rs485_receive();
void rs485_receive();
void init_rs485_waiting_control();
void rs485_waiting_control();
void RS485_UART_Callback(UART_HandleTypeDef *huart);

#endif /* INC_MODBUS485_FSM_H_ */

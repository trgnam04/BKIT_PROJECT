/*
 * modbus485_fsm.c
 *
 *  Created on: Dec 5, 2024
 *      Author: nguye
 */


#include "modbus485_fsm.h"
// DEBUG
// END DEBUG

uint8_t transmit_buffer[256] = {0x00};
uint8_t transmit_size = 0;

uint8_t receive_byte_buffer = 0;
uint8_t receive_buffer[256] = {0x00};
uint8_t receive_index = 0;

enum frame_status_t FRAME_STATUS = FRAME_NOT_OK;
uint8_t frame_size = 0;
uint8_t isCRCChecking = 0;

enum modbus485_state_t MODBUS485_STATE = RS485_SUPER_INIT;
uint8_t character_received_flag = 0;
uint8_t transmit_flag = 0;

void rs485_send_cmd(uint8_t* cmd_without_crc, uint8_t cmd_size_without_crc)
{
	if (cmd_size_without_crc > 254) return;

	for(int i = 0; i < cmd_size_without_crc; i++){
		transmit_buffer[i] = cmd_without_crc[i];
	}
//	sprintf((void*)transmit_buffer, "%s", cmd_without_crc);
	transmit_size = cmd_size_without_crc;

	uint16_t crc = crc16(transmit_buffer, transmit_size);
	transmit_buffer[transmit_size] = (uint8_t)(crc & 0x00FF); // CRC low
	transmit_buffer[transmit_size + 1] = (uint8_t)((crc >> 8) & 0x00FF); // CRC high

	transmit_size += 2;
	transmit_flag = 1;
}

uint8_t is_frame_status()
{
	if (FRAME_STATUS == FRAME_OK)
	{
		FRAME_STATUS = FRAME_NOT_OK;
		return 1;
	}
	return 0;
}

uint8_t get_frame_size()
{
	return frame_size;
}

uint8_t is_character_received()
{
	if (character_received_flag == 1)
	{
		character_received_flag = 0;
		return 1;
	}
	return 0;
}

uint8_t is_transmit()
{
	if (transmit_flag == 1)
	{
		transmit_flag = 0;
		return 1;
	}
	return 0;
}

uint8_t crc_checking()
{
	if (receive_index - 2 >= 0)
	{
		uint16_t crc = crc16(receive_buffer, receive_index - 2);
		uint8_t crc_high = (crc >> 8) & 0xFF;
		uint8_t crc_low = crc & 0xFF;
		if (crc_low == receive_buffer[receive_index - 2] && crc_high == receive_buffer[receive_index - 1])
		{
			// DEBUG
//			uart_Rs232SendString((uint8_t*)"CRC: 1\n\r");
			// END DEBUG
			return 1;
		}
	}

	// DEBUG
//	uart_Rs232SendString((uint8_t*)"CRC: 0\n\r");
	// END DEBUG
	return 0;
}

void rs485_fsm_init()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"FSM_INIT\n\r");
	// END DEBUG
	HAL_UART_Receive_IT(&huart3, &receive_byte_buffer, 1);
}

void rs485_fsm()
{
	switch (MODBUS485_STATE) {
		case RS485_SUPER_INIT:
			rs485_super_init();

			if (1) // CHANGE STATE -> RS485_INIT
			{
				init_rs485_init();
				MODBUS485_STATE = RS485_INIT;
			}
			break;
		case RS485_INIT:
			rs485_init();

			if (timer_flag[TI_RS485_T35_TIMER]) // CHANGE STATE -> RS485_IDLE
			{
				init_rs485_idle();
				MODBUS485_STATE = RS485_IDLE;
			}
			if (is_character_received()) // CHANGE STATE -> RS485_INIT
			{
				init_rs485_init();
				MODBUS485_STATE = RS485_INIT;
			}
			break;
		case RS485_IDLE:
			rs485_idle();

			if (is_transmit()) // CHANGE STATE -> RS485_TRANSMIT
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
				HAL_UART_Transmit(&huart3, transmit_buffer, transmit_size, HAL_MAX_DELAY);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET);
				init_rs485_transmit();
				MODBUS485_STATE = RS485_TRANSMIT;
			}
			if (is_character_received()) // CHANGE STATE -> RS485_RECEIVE
			{
				init_rs485_receive();
				MODBUS485_STATE = RS485_RECEIVE;
			}
			break;
		case RS485_TRANSMIT:
			rs485_transmit();

			if (timer_flag[TI_RS485_T35_TIMER]) // CHANGE STATE -> IDLE
			{
				init_rs485_idle();
				MODBUS485_STATE = RS485_IDLE;
			}
			break;
		case RS485_RECEIVE:
			rs485_receive();

			if (is_character_received()) // CHANGE STATE -> RS485_RECEIVE
			{
				init_rs485_receive();
				MODBUS485_STATE = RS485_RECEIVE;
			}
			if (timer_flag[TI_RS485_T15_TIMER]) // CHANGE STATE -> RS485_WAITING_CONTROL
			{
				init_rs485_waiting_control();
				MODBUS485_STATE = RS485_WAITING_CONTROL;
			}
			break;
		case RS485_WAITING_CONTROL:
			rs485_waiting_control();

			if (timer_flag[TI_RS485_T35_TIMER]) // CHANGE STATE -> RS485_IDLE
			{
				init_rs485_idle();
				MODBUS485_STATE = RS485_IDLE;
			}
			break;
		default:
			break;
	}
}

void init_rs485_super_init()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: SUPER_INIT\n\r");
	// END DEBUG
	// Do not thing
}
void rs485_super_init()
{
	// Do not thing
}
void init_rs485_init()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: INIT\n\r");
	// END DEBUG
	set_timer(TI_RS485_T35_TIMER, TI_RS485_T35_TIME);
}
void rs485_init()
{
	// Do not thing
}
void init_rs485_idle()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: IDLE\n\r");
	// END DEBUG
	transmit_flag = 0;
	receive_index = 0;
}
void rs485_idle()
{
	// Do not thing
}
void init_rs485_transmit()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: TRANSMIT\n\r");
	// END DEBUG

	set_timer(TI_RS485_T35_TIMER, TI_RS485_T35_TIME);
}
void rs485_transmit()
{
	// Do not thing
}
void init_rs485_receive()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: RECEIVE\n\r");
	// END DEBUG
	set_timer(TI_RS485_T35_TIMER, TI_RS485_T35_TIME);
	set_timer(TI_RS485_T15_TIMER, TI_RS485_T15_TIME);
}
void rs485_receive()
{
	// Do not thing
}
void init_rs485_waiting_control()
{
	// DEBUG
//	uart_Rs232SendString((uint8_t*)"S: WAITING_CONTROL\n\r");
	// END DEBUG
	isCRCChecking = 0;
}
void rs485_waiting_control()
{
	if (is_character_received()) // CHANGE STATE -> RS485_WAITING_CONTROL
	{
		FRAME_STATUS = FRAME_NOT_OK;
	}

	if (isCRCChecking == 0)
	{
		// CHECK SLAVE ADDRESS if needed
		if (crc_checking())
		{
			FRAME_STATUS = FRAME_OK;
		}
		else
		{
			FRAME_STATUS = FRAME_NOT_OK;
		}
		frame_size = receive_index;
		isCRCChecking = 1;
	}
}
void RS485_UART_Callback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		receive_buffer[receive_index++] = receive_byte_buffer;
		character_received_flag = 1;

		// DEBUG
//		HAL_UART_Transmit(&huart1, &receive_byte_buffer, 1, HAL_MAX_DELAY);
		// END DEBUG

		HAL_UART_Receive_IT(&huart3, &receive_byte_buffer, 1);
	}
}

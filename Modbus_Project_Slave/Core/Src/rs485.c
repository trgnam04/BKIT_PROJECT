/*
 * rs485.c
 *
 *  Created on: Nov 24, 2024
 *      Author: ADMIN
 */
#include "rs485.h"
unsigned char Receive_Flag = 0;

Modbus_HandleTypeDef master;
Modbus_HandleTypeDef slave;


HAL_StatusTypeDef Modbus_Send(Modbus_HandleTypeDef* hModbus){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,SET);
	if(HAL_UART_Transmit(hModbus->huart,hModbus->Tx_buf,hModbus->Tx_size,HAL_MAX_DELAY) != HAL_OK)
		return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
	return HAL_OK;
}

unsigned char Modbus_CheckData(Modbus_HandleTypeDef* hModbus){
	uint16_t rx_crc = crc16(hModbus->Rx_buf, hModbus->Rx_size - 2);
	uint16_t high_crc = hModbus->Rx_buf[hModbus->Rx_size - 1];
	uint16_t low_crc =  hModbus->Rx_buf[hModbus->Rx_size - 2];
	uint16_t receive_crc = (high_crc<<8)|(low_crc);
	return (receive_crc == rx_crc);
}

Modbus_Status Modbus_Transmit_Master(Modbus_HandleTypeDef* hModbus, uint8_t Address, uint8_t Function, uint8_t* Data, uint8_t Size, uint32_t Timeout){
	hModbus->Tx_buf[0] = Address;
	hModbus->Tx_buf[1] = Function;
	int i;
	uint16_t crc = 0;
	for(i=0; i<Size; i++){
		hModbus->Tx_buf[i+2] = *(Data + i);
	}
	crc = crc16(hModbus->Tx_buf, i+2);
	hModbus->Tx_buf[i+2] = crc & 0x00FF; // LOW
	hModbus->Tx_buf[i+3] = crc >> 8; //HIGH
	hModbus->Tx_size = Size + 4;
	if(Modbus_Send(hModbus) == HAL_OK){
		return HAL_ERROR;
	}
	HAL_Delay(5);
	uint16_t start = HAL_GetTick();
	while(!hModbus->RxFlag){
		if(HAL_GetTick() -  start == Timeout){
			return MODBUS_TIMEOUT;
		}
	}
	hModbus->RxFlag = 0;
	HAL_Delay(5);
	return MODBUS_OKE;
}

Modbus_Status Modbus_Transmit_Slave(Modbus_HandleTypeDef* hModbus, uint8_t Address, uint8_t Function, uint8_t* Data, uint8_t Size, uint32_t Timeout){
	hModbus->Tx_buf[0] = Address;
	hModbus->Tx_buf[1] = Function;
	int i;
	uint16_t crc = 0;
	for(i=0; i<Size; i++){
		hModbus->Tx_buf[i+2] = *(Data + i);
	}
	crc = crc16(hModbus->Tx_buf, i+2);
	hModbus->Tx_buf[i+2] = crc & 0x00FF; // LOW
	hModbus->Tx_buf[i+3] = crc >> 8; //HIGH
	hModbus->Tx_size = Size + 4;
	if(Modbus_Send(hModbus) == HAL_OK){
		return MODBUS_ERROR;
	}
	HAL_Delay(5);
	return MODBUS_OKE;
}

Modbus_Status Modbus_Receive(Modbus_HandleTypeDef* hModbus){ //Thieu timeout
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
	HAL_UARTEx_ReceiveToIdle_IT(hModbus->huart, hModbus->Rx_buf, MAX_SIZE);
	return MODBUS_OKE;
}

void Modbus_CallBack(Modbus_HandleTypeDef* hModbus, UART_HandleTypeDef* huart, uint16_t Size){
	if(huart->Instance == hModbus->huart->Instance){
		Receive_Flag = Modbus_CheckData(hModbus);
		hModbus->Rx_size = Size;
		hModbus->RxFlag = 1;
		HAL_UART_Transmit(&huart1, hModbus->Rx_buf, hModbus->Rx_size, HAL_MAX_DELAY);
		HAL_UARTEx_ReceiveToIdle_IT(hModbus->huart, hModbus->Rx_buf, MAX_SIZE);
	}

}

void Modbus_Init(Modbus_HandleTypeDef* hModbus, UART_HandleTypeDef* huart){
	hModbus->huart = huart;
	hModbus->Address = 0;
	memset(hModbus->Rx_buf,0,sizeof(hModbus->Rx_buf));
	hModbus->Rx_size = 256;
	memset(hModbus->Tx_buf,0,sizeof(hModbus->Tx_buf));
	hModbus->Tx_size = 0;
	hModbus->RxFlag = 0;
	Modbus_Receive(hModbus);
//	Modbus_CallBack(&slave, huart, Size);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	Modbus_CallBack(&slave, huart, Size);
}

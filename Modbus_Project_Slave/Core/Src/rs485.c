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
	HAL_UART_Transmit(&huart1,hModbus->Tx_buf,hModbus->Tx_size,HAL_MAX_DELAY);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,SET);
	if(HAL_UART_Transmit(hModbus->huart,hModbus->Tx_buf,hModbus->Tx_size,HAL_MAX_DELAY) != HAL_OK){
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
		return HAL_ERROR;
	};

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
	return HAL_OK;
}

unsigned char Modbus_CheckData(Modbus_HandleTypeDef* hModbus){
	uint16_t rx_crc = crc16(hModbus->Rx_buf, hModbus->Rx_size - 2);
	uint16_t high_crc = hModbus->Rx_buf[hModbus->Rx_size - 1];
	uint16_t low_crc =  hModbus->Rx_buf[hModbus->Rx_size - 2];
	uint16_t receive_crc = (high_crc<<8)|(low_crc);
//	uint8_t status = (receive_crc == rx_crc);
//	if(status){
//		HAL_UART_Transmit(&huart1, (uint8_t*)"oke", 3, 100);
//	}else{
//		HAL_UART_Transmit(&huart1, (uint8_t*)"NO", 2, 100);
//	}
	return (receive_crc == rx_crc) ? 1 : 0;
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
	hModbus->Tx_buf[2] = Size;
	int i;
	uint16_t crc = 0;
	for(i=0; i<Size; i++){
		hModbus->Tx_buf[i+3] = *(Data + i);
	}
	crc = crc16(hModbus->Tx_buf, i+3);
	hModbus->Tx_buf[i+3] = crc & 0x00FF; // LOW
	hModbus->Tx_buf[i+4] = crc >> 8; //HIGH
	hModbus->Tx_size = Size + 5;
	if(Modbus_Send(hModbus) == HAL_OK){
		return MODBUS_ERROR;
	}
	HAL_Delay(5);
	return MODBUS_OKE;
}

Modbus_Status Modbus_Receive(Modbus_HandleTypeDef* hModbus){ //Thieu timeout
	HAL_UARTEx_ReceiveToIdle_IT(hModbus->huart, hModbus->Rx_buf, MAX_SIZE);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,RESET);
	return MODBUS_OKE;
}

void Modbus_CallBack(Modbus_HandleTypeDef* hModbus, UART_HandleTypeDef* huart, uint16_t Size){
	hModbus->Rx_size = Size;
	hModbus->RxFlag = 1;
	Receive_Flag = Modbus_CheckData(hModbus);
//	HAL_UART_Transmit(&huart1, hModbus->Rx_buf, hModbus->Rx_size, HAL_MAX_DELAY);
	HAL_UARTEx_ReceiveToIdle_IT(hModbus->huart, hModbus->Rx_buf, MAX_SIZE);
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
}



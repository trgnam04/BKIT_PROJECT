/*
 * Modbus_Slave.c
 *
 *  Created on: Dec 5, 2024
 *      Author: ASUS
 */

#include "Modbus_Slave.h"


Slave_Device BKIT01;


void InitSlave(Slave_Device* hDev){
	hDev->signal = IDLE;
	hDev->subsignal.s = 0;
	hDev->Address = SLAVE_ADDRESS;
	memset(hDev->Register, 0, sizeof(hDev->Register));
	Modbus_Init(&slave, &huart3);
}

void CommandParser_handler(Slave_Device* hDev){
	switch(slave.Rx_buf[1]){
	case READ_COIL:{
		break;
	}
	case READ_DISCRETE_INPUT:{
		break;
	}
	case READ_HOLDING_REGISTER:{
		hDev->signal = READ_HOLDING_REGISTER_HANDLER;
		break;
	}
	case READ_INPUT_REGISTER:{
		break;
	}
	case WRITE_SINGLE_COIL:{
		break;
	}
	case WRITE_SINGLE_HOLDING_REGISTER:{
		hDev->signal = WRITE_HOLDING_REGISTER_HANDLER;
		break;
	}
	case WRITE_MULTIPLE_COIL:{
		break;
	}
	case WRITE_MULTIPLE_HOLDING_REGISTER:{
		hDev->signal = READ_MULTIPLE_HOLDING_REGISTER_HANDLER;
		break;
	}
	}
	return;
}

void splitFloat(float _input, uint8_t *data) {
	data[0] = *(((uint8_t*)&_input)+0);
	    data[1] = *(((uint8_t*)&_input)+1);
	    data[2] = *(((uint8_t*)&_input)+2);
	    data[3] = *(((uint8_t*)&_input)+3);
//	memcpy(data, &_input, sizeof(float));
};

void splitUint16(uint16_t input, uint8_t *higherByte, uint8_t *lowerByte) {
    *higherByte = (input >> 8) & 0xFF; // Lấy 8 bit cao
    *lowerByte = input & 0xFF;        // Lấy 8 bit thấp
}


void Read_multiple_holding_register_handler(Slave_Device* hDev){
	uint16_t startReg = (uint16_t)(slave.Rx_buf[2]) << 8 | (uint16_t)(slave.Rx_buf[3]);
	uint16_t numberOfReg = (uint16_t)(slave.Rx_buf[4]) << 8 | (uint16_t)(slave.Rx_buf[5]);
	slave.Tx_buf[0] = hDev->Address;
	slave.Tx_buf[1] = slave.Rx_buf[1];
	slave.Tx_buf[2] = numberOfReg * 2;
	int i;
	for(i = 0; i < numberOfReg * 2; i++){
		slave.Tx_buf[i + 3] = hDev->Register[startReg];
	}
	slave.Tx_size = i + 3;
	rs485_send_cmd(slave.Tx_buf, slave.Tx_size);

//	Modbus_Transmit_Slave(&slave, hDev->Address, slave.Rx_buf[1], &hDev->Register[startReg], numberOfReg * 2, 100);

	return;
}

void Read_holding_register_handler(Slave_Device* hDev){
//	uint8_t data[3];
//	data[0] = 3;
//	data[1] = hDev->Register[slave.Rx_buf[2]];
//	data[2] = hDev->Register[slave.Rx_buf[2] + 1];
//	slave.Tx_buf[0] = hDev->Address;
//	slave.Tx_buf[1] = slave.Rx_buf[1];
//	slave.Tx_buf[2] = 3;
//	int i;
//	for(i = 0; i < numberOfReg * 2; i++){
//		slave.Tx_buf[i + 3] = hDev->Register[startReg];
//	}
//	slave.Tx_size = i + 3;
//	Modbus_Transmit_Slave(&slave, hDev->Address, slave.Rx_buf[1], data, 3, 100);
//	return;
}

static void ReadData(Slave_Device* hDev){
	float tempCur = sensor_get_current();
	uint16_t tempLt = sensor_get_light();
	uint16_t tempPt =sensor_get_potentiometer();
	float tempT = sensor_get_temperature();
	float tempVol = sensor_get_voltage();

	// Current
	splitFloat(tempCur, &hDev->Register[CURRENT_REGISTER_ADDRESS]);
	// Temperature
	splitFloat(tempT, &hDev->Register[TEMPERATURE_REGISTER_ADDRESS]);
	// Voltage
	splitFloat(tempVol, &hDev->Register[VOLTAGE_REGISTER_ADDRESS]);
	// Light
	splitUint16(tempLt, &hDev->Register[LIGHT_REGISTER_ADDRESS], &hDev->Register[LIGHT_REGISTER_ADDRESS + 1]);
	// Potentiometer
	splitUint16(tempPt, &hDev->Register[POTENTIOMETER_REGISTER_ADDRESS], &hDev->Register[POTENTIOMETER_REGISTER_ADDRESS + 1]);

}

void Write_holding_register_handler(Slave_Device* hDev){

//	HAL_UART_Transmit(&huart3, slave.Rx_buf, slave.Rx_size, 1000);
	rs485_send_cmd(slave.Rx_buf, slave.Rx_size);
	hDev->Register[LED_REGISTER_ADDRESS] = slave.Rx_buf[4];
	hDev->Register[LED_REGISTER_ADDRESS + 1] = slave.Rx_buf[5];
	uint8_t CoilState = hDev->Register[LED_REGISTER_ADDRESS + 1];
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, CoilState & COIL_A);
	HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, CoilState & COIL_B);
	HAL_GPIO_WritePin(OUTPUT_X0_GPIO_Port, OUTPUT_X0_Pin, CoilState & COIL_C);
	HAL_GPIO_WritePin(OUTPUT_X1_GPIO_Port, OUTPUT_X1_Pin, CoilState & COIL_D);
	HAL_GPIO_WritePin(OUTPUT_X2_GPIO_Port, OUTPUT_X2_Pin, CoilState & COIL_E);
	HAL_GPIO_WritePin(OUTPUT_X3_GPIO_Port, OUTPUT_X3_Pin, CoilState & COIL_F);
}

void TurnOffAllLed(void){
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, RESET);
		HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, RESET);
		HAL_GPIO_WritePin(OUTPUT_X0_GPIO_Port, OUTPUT_X0_Pin, RESET);
		HAL_GPIO_WritePin(OUTPUT_X1_GPIO_Port, OUTPUT_X1_Pin, RESET);
		HAL_GPIO_WritePin(OUTPUT_X2_GPIO_Port, OUTPUT_X2_Pin, RESET);
		HAL_GPIO_WritePin(OUTPUT_X3_GPIO_Port, OUTPUT_X3_Pin, RESET);
};

void slave_behavior(Slave_Device* hDev){
	switch(hDev->signal){
	case IDLE:{
		hDev->signal = WAITTING_FOR_CMD;
		break;
	}
	case WAITTING_FOR_CMD:{
		ReadData(hDev);
		if(Receive_Flag){
			hDev->signal = COMMAND_PARSER;
			Receive_Flag = 0;
			break;
		}
		break;
	}
	case COMMAND_PARSER:{
		CommandParser_handler(hDev);
		break;
	}
	case READ_HOLDING_REGISTER_HANDLER:{
		Read_holding_register_handler(hDev);
		hDev->signal = IDLE;
		break;
	}
	case READ_MULTIPLE_HOLDING_REGISTER_HANDLER:{
		Read_multiple_holding_register_handler(hDev);
		hDev->signal = IDLE;
		break;
	}
	case WRITE_HOLDING_REGISTER_HANDLER:{
		Write_holding_register_handler(hDev);
		hDev->signal = IDLE;
		break;
	}
	}
}

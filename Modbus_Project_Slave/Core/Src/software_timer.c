/*
 * software_timer.c
 */

/* Includes */
#include "software_timer.h"
#include "tim.h"

#include "led_7seg.h"

/* Variables */
uint8_t timer_flag[__MAX_TIM];
uint32_t timer_counter[__MAX_TIM];

uint8_t timer1_flag = 0;
uint16_t timer1_counter = 0;
uint16_t timer1_mul = 0;


uint8_t timer2_flag = 0;
uint16_t timer2_counter = 0;
uint16_t timer2_mul = 0;

uint8_t timer4_flag = 0;
uint16_t timer4_counter = 0;
uint16_t timer4_mul = 0;

/**
 * @brief  	Init timer interrupt
 * @param  	None
 * @retval 	None
 */
void timer2_init(void) {
	HAL_TIM_Base_Start_IT(&htim2);
}

void timer4_init(void) {
	HAL_TIM_Base_Start_IT(&htim4);
}

/**
 * @brief	Set duration of software timer interrupt
 * @param	duration Duration of software timer interrupt
 * @retval 	None
 */
void timer1_set(int ms) {
	timer1_mul = ms / TIMER_CYCLE_2;
	timer1_counter = timer2_mul;
	timer1_flag = 0;
}

void timer2_set(int ms) {
	timer2_mul = ms / TIMER_CYCLE_2;
	timer2_counter = timer2_mul;
	timer2_flag = 0;
}

void timer4_set(int ms) {
	timer4_mul = ms / TIMER_CYCLE_4;
	timer4_counter = timer4_mul;
	timer4_flag = 0;
}

/**
 * @brief  	Timer interrupt routine
 * @param  	htim TIM Base handle
 * @note	This callback function is called by system
 * @retval 	None
 */
void InitTimer(void){
	for(int i = 0; i < __MAX_TIM; i++){
		timer_counter[i] = 0;
		timer_flag[i] = 0;
	}
}

void set_timer(uint32_t ms, uint8_t idx){
	timer_flag[idx] = 0;
	timer_counter[idx] = ms;
}

void TimerRun(void){
	for(int i = 0; i < __MAX_TIM; i++){
		if(timer_counter[i] > 0){
			timer_counter[i]--;
			if(timer_counter[i] == 0){
				timer_flag[i] = 1;
			}
		}
	}
}





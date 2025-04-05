#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

TIM_HandleTypeDef htim3;


void delay(uint32_t ms);
void RCC_Config(void);
void GPIO_Config(void);
void TIMER_Config(void);

int main(){
	RCC_Config();
	TIMER_Config();
	GPIO_Config();
	
	
	while(1){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		delay(1000);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		delay(1000);
	}
}


void delay(uint32_t ms){
	__HAL_TIM_SET_COUNTER(&htim3, 0);  // Reset counter v? 0
  while (__HAL_TIM_GET_COUNTER(&htim3) < ms * 10);
}

void RCC_Config(){
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
}

void TIMER_Config(void){
	htim3.Instance = TIM3;
	
	htim3.Init.Prescaler 	= 1600 - 1;
	htim3.Init.Period 		= 0xFFFF;
	htim3.Init.ClockDivision = 	TIM_CLOCKDIVISION_DIV1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_Base_Start(&htim3);
}

void GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin 	= GPIO_PIN_13;
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 	= GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

void RCC_Config(){
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main(){
	RCC_Config();
	GPIO_Config();
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	while(1){
		if(GPIOA->IDR & (1 << 0)){
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		}else{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
	}
		

}



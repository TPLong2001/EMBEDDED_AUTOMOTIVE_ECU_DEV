#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO

#define SPI_SCK_Pin			GPIO_Pin_0
#define SPI_MISO_Pin 		GPIO_Pin_1
#define SPI_MOSI_Pin 		GPIO_Pin_2
#define SPI_CS_Pin 			GPIO_Pin_3
#define SPI_GPIO 				GPIOA
#define SPI_RCC 				RCC_APB2Periph_GPIOA

#define SPI_MISO_HIGH		GPIO_SetBits(SPI_GPIO, SPI_MISO_Pin)
#define SPI_MISO_LOW		GPIO_ResetBits(SPI_GPIO, SPI_MISO_Pin)

void RCC_Config(void){
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	///GPIO SPI
	RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
}

void TIM_Config(void){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//72MHz
	TIM_InitStruct.TIM_Prescaler 		= 7200 - 1;				// 0.1ms count++
	TIM_InitStruct.TIM_Period 			= 0xFFFF;
	TIM_InitStruct.TIM_CounterMode 	=	TIM_CounterMode_Up;


	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);		// On Timer
	
}

void GPIO_Config(void){
	// SLAVE: SCK, MOSI, CS - INPUT; MISO - OUTPUT
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin 		= SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin 		= SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
}

void Delay_ms(uint32_t ms)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < ms * 10);
}

void SPI_Slave_Transmit(uint8_t data){
	uint8_t u8Mask = 0x80;		// 0b1000 0000
	uint8_t transData;
	
	while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	
	for(uint8_t i = 0; i<8; i++){
		while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin) == Bit_RESET);
		
		transData = data & u8Mask;
		if(transData){
			SPI_MISO_HIGH;
			Delay_ms(1);
		}
		else{
			SPI_MISO_LOW;
			Delay_ms(1);
		}
		
		while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin) == Bit_SET);
	}
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
}

uint8_t SPI_Slave_Receive(void){
	uint8_t u8Data = 0x00;
	uint8_t bitReceive = 0x00;
	
	// Cho chan CS keo xuong
	// Cho CLK keo len
	// Khi CLK = 1 => Doc data nhan duoc
	// Doi CLK = 0 => Tiep tuc doi CLK len 1 de doc bit tiep theo
	// khi doc du 8 bit => cho CS keo len de ket thuc.
	
	while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	
	for(uint8_t i = 0; i<8; i++){
		while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
		if(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
			while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
				bitReceive = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin);
			}
			u8Data = (uint8_t)(u8Data << 1);
			u8Data |= bitReceive;			
		}
	}
	
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	return u8Data;
}


int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();

	
	uint8_t data;
	while(1){
		data = SPI_Slave_Receive();
	}
}

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

#define SPI_MOSI_HIGH		GPIO_SetBits(SPI_GPIO, SPI_MOSI_Pin)
#define SPI_MOSI_LOW		GPIO_ResetBits(SPI_GPIO, SPI_MOSI_Pin)

#define SPI_CS_HIGH			GPIO_SetBits(SPI_GPIO, SPI_CS_Pin)
#define SPI_CS_LOW			GPIO_ResetBits(SPI_GPIO, SPI_CS_Pin)

#define SPI_SCK_HIGH		GPIO_SetBits(SPI_GPIO, SPI_SCK_Pin)
#define SPI_SCK_LOW			GPIO_ResetBits(SPI_GPIO, SPI_SCK_Pin)


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
	// MASTER: SCK, MOSI, CS - Output; MISO - Input
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin 		= SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin 		= SPI_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
}

void Delay_ms(uint32_t ms)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < ms * 10);
}

void SPI_Init(void){
	// Trang thai khoi tao ban dau khi chua truyen:
	// CPOL = 0 => SCK = 0
	// CS = 1 - CHUA CHON SLAVE
	// MISO = 0
	// MOSI = 0
	SPI_SCK_LOW;
	SPI_CS_HIGH;
	SPI_MOSI_LOW;
	SPI_MISO_LOW;
}


void clock(void){
	// Tao xung clock 1 chu ky
	SPI_SCK_HIGH;
	Delay_ms(4);
	SPI_SCK_LOW;
	Delay_ms(4);
}

void SPI_Master_Transmit(uint8_t u8Data){
	uint8_t u8Mask = 0x80;		// 0b1000 0000
	uint8_t transData;
	
	// Keo chan CS de bay dau truyen
	// Truyen lan luot tung bit tu cao den thap MSB -> LSB
	// Sau khi truyen moi bit - truyen kem 1 clock
	// Sau khi truyen xong 8 bit keo CS len ket thuc truyen.
	
	SPI_CS_LOW;
	Delay_ms(1);
	
	for(uint8_t i = 0; i < 8; i++){
		transData = u8Data & u8Mask;
		if(transData){
			SPI_MOSI_HIGH;
			Delay_ms(1);
		}
		else{
			SPI_MOSI_LOW;
			Delay_ms(1);
		}
		clock();
		u8Data = (uint8_t)(u8Data << 1);
	}
	
	SPI_CS_HIGH;
	Delay_ms(1);
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
	
	SPI_Init();
	
	uint8_t dataTrans[] = {1, 3, 5, 7, 9, 35, 56};
	while(1){
		for(uint8_t i = 0; i < 7; i++){
			SPI_Master_Transmit(dataTrans[i]);
			Delay_ms(1000);
		}
	
	}
}

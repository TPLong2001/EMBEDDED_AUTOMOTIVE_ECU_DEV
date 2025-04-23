#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI


#define SPI1_NSS 		GPIO_Pin_4
#define SPI1_SCK		GPIO_Pin_5
#define SPI1_MISO 	GPIO_Pin_6
#define SPI1_MOSI 	GPIO_Pin_7
#define SPI1_GPIO 	GPIOA

#define SPI_NSS_HIGH	GPIO_SetBits(SPI1_GPIO, SPI1_NSS)
#define SPI_NSS_LOW		GPIO_ResetBits(SPI1_GPIO, SPI1_NSS)


void RCC_Config(void){
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	///GPIO SPI
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
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
	
	GPIO_InitStruct.GPIO_Pin 		= SPI1_NSS | SPI1_SCK | SPI1_MISO | SPI1_MOSI;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO, &GPIO_InitStruct);
}

void SPI_Config(void){
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_16;
	SPI_InitStruct.SPI_CPHA								= SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL								= SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize						= SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction					= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit						= SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode								= SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS								= SPI_NSS_Soft;
	
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);
}

void Delay_ms(uint32_t ms)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < ms * 10);
}


void SPI_Send1Byte(uint8_t data){
	// Keo CS xuong 0
	// Kiem tra xem thanh ghi DATA TRUYEN co rong khong (SPI da truyen xong chua) -> FLAG_TXE = 1
	// Neu da truyen xong thi bat dau truyen du lieu
	// Sau khi truyen xong thi kiem tra xem co ban truyen hay khong -> FLAG_BSY = 0
	// Keo CS len 1
	
	// MASTER
	SPI_NSS_LOW;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	SPI_NSS_HIGH;
}

uint8_t SPI_Receive1Byte(void)
{	
	// SLAVE
	uint8_t data;
	// Cho thanh ghi DATA NHAN day ( nhan xong 1 bit du lieu) thi bat dau => FLAG_RXNE = 1 thi bat dau
	// LAY DU LIEU
	// Doi cho he thong lay du lieu xong va ranh			=> FLAG_BSY = 0 thi ket thuc
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	data = (uint8_t)SPI_I2S_ReceiveData(SPI1);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	return data;
}

uint8_t SPI_Master_Transmit_Receive_1Byte(uint8_t txData){
	// MASTER
	uint8_t rxData;
	
	SPI_NSS_LOW;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, txData);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	rxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	SPI_NSS_HIGH;
	return rxData;
}

uint8_t SPI_Slave_Transmit_Receive_1Byte(uint8_t txData){
	// SLAVE
	uint8_t rxData;
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, txData);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	rxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	return rxData;
}


int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	SPI_Config();
	
	uint8_t dataTrans[] = {1, 3, 5, 7, 9, 35, 56};
	while(1){
		for(uint8_t i = 0; i < 7; i++){
			SPI_Send1Byte(dataTrans[i]);
			Delay_ms(1000);
		}
	
	}
}

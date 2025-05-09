
# Bài 5 SPI Software & Hardware

## Tổng quan
SPI (Serial Peripheral Interface) là một chuẩn giao tiếp đồng bộ phổ biến trong các hệ thống nhúng.  
Trên mỗi dòng vi điều khiển khác nhau module SPI sẽ được tích hợp, điều khiển bởi các thanh ghi,phần cứng, IO khác nhau, đấy gọi là SPI cứng (hardware SPI).  

Có 1 cách lập trình giả lập SPI cứng đó là “SPI mềm”. Thực chất SPI mềm là cách “mô phỏng” bằng cách tạo ra một giao thức truyền thông giống SPI nhưng chỉ sử dụng các cổng vào ra của vi điều khiển.  
Như vậy chỉ với việc điều khiển GPIO của chip hoàn toàn có thể thực hiện giao thức SPI. 

## SPI Software

Ta sẽ sử dụng 4 chân GPIO để truyền nhận SPI gồm MISO, MOSI, CS và SCK.

__Định nghĩa__ các GPIO và PIN sử dụng cho 4 chân truyền nhận SPI:
```c
#define SPI_SCK_Pin			GPIO_Pin_0
#define SPI_MISO_Pin 		GPIO_Pin_1
#define SPI_MOSI_Pin 		GPIO_Pin_2
#define SPI_CS_Pin 			GPIO_Pin_3
#define SPI_GPIO 			GPIOA
#define SPI_RCC 			RCC_APB2Periph_GPIOA
```

__Hàm khởi tạo__ các trạng thái ban đầu của SPI:
```c
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
```

SPI sử dụng `clock` để __đồng bộ__ chuyền và nhận vì vậy cần phải tạo xung clock từ chân SPI_SCK.  
`Clock` sẽ được tạo bằng __hàm delay__ kết hợp với __set/reset__ tín hiệu trên đường `SCK`:

=> Hàm tạo xung clock:
```c
void clock(void){
	// Tao xung clock 1 chu ky
	SPI_SCK_HIGH;
	Delay_ms(4);
	SPI_SCK_LOW;
	Delay_ms(4);
}
```

__Triển khai hàm truyền data__ của master theo __MODE 0__ (CPOL = 0, CPHA = 0):
```c
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

```

__Triển khai hàm nhận data của slave__:  
Vì master truyền theo xung lên của clock lên slave cũng sẽ nhận data trong suốt thời gian từ lúc xung lên cho đến thi xung xuống.

```c
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
```
Ở slave sẽ dựa vào tín hiệu CS và SCK để xác định lệnh truyền nhận từ Master cũng như thời diểm đọc dữ liệu.

## SPI Hardware

STM32F1 có 2 module SPI:

- SPI1: Dùng bus APB2 (tốc độ cao hơn).

- SPI2: Dùng bus APB1.

### Cấu hình 

Ta sử dụng SPI1, các chân đã được thiết lập sẵn. Ta sẽ cấu hình cho các chân này. Đầu tiên define các chân với các tên riêng biệt:
```c
#define SPI1_NSS 	GPIO_Pin_4
#define SPI1_SCK	GPIO_Pin_5
#define SPI1_MISO 	GPIO_Pin_6
#define SPI1_MOSI 	GPIO_Pin_7
#define SPI1_GPIO 	GPIOA
```
Các chân này đã được __config__ sẵn chức năng của các chân SPI1.

Tiếp theo, ta __cấu hình GPIO__ cho các chân với mode AF như sau:
```c
void GPIO_Config(void){
	// MASTER: SCK, MOSI, CS - Output; MISO - Input
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin 		= SPI1_NSS | SPI1_SCK | SPI1_MISO | SPI1_MOSI;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO, &GPIO_InitStruct);
}
```

Tiếp theo, ta __cấu hình SPI phần cứng__ muốn sử dụng bằng struct của SPL:
```c
void SPI_Config(void){
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_16;
	SPI_InitStruct.SPI_CPHA					= SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL					= SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize				= SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit				= SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode					= SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS					= SPI_NSS_Soft;
	
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);
}
```

__Triển khai__ các hàm `transmit` và `receive`:

Trong thư viện SPL đã có sẵn cấu hình các hàm `SPI_I2S_SendData` và `SPI_I2S_ReceiveData` nhưng chân NSS ta cấu hình phần mềm (SPI_NSS_Soft) => Ta __chủ động kéo xuống__ trước khi truyền và __kiểm tra cờ__ sau khi truyền xong.

```c
void SPI_Send1Byte(uint8_t data){
	// Keo CS xuong 0
	// Kiem tra xem thanh ghi DATA TRUYEN co rong khong (SPI da truyen xong chua) -> FLAG_TXE = 1
	// Neu da truyen xong thi bat dau truyen du lieu
	// Sau khi truyen xong thi kiem tra xem flag busy transmit hay khong -> FLAG_BSY = 0
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
```

### Demo

__Ví dụ:__
```c
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI


#define SPI1_NSS 	GPIO_Pin_4
#define SPI1_SCK	GPIO_Pin_5
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
	// Sau khi truyen xong thi kiem tra xem flag busy transmit hay khong -> FLAG_BSY = 0
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

```
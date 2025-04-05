


//RCC - AHB1
#define RCC_AHB1ENR	*((unsigned int *)0x40023830)

// PA0 - Button - INPUT - PULLDOWN
#define GPIOA_MODER	*((unsigned int *)0x40020000)
#define GPIOA_PUPDR	*((unsigned int *)0x4002000C)
#define GPIOA_IDR		*((unsigned int *)0x40020010)

// PD12 - LED - OUTPUT - PULLDOWN
#define GPIOD_MODER	*((unsigned int *)0x40020C00)
#define GPIOD_PUPDR	*((unsigned int *)0x40020C08) 
#define GPIOD_OSPEEDR	*((unsigned int *)0x40020C0C) 
#define GPIOD_ODR		*((unsigned int *)0x40020C14) 

void delay(unsigned int time);
void delay(unsigned int time){
	
	for(unsigned int i = 0; i< time; i++);
}

int main(void)
{
	// PA - PD
	RCC_AHB1ENR |= ((1 << 0)|(1 << 3));
	
	GPIOD_MODER |= (1 << 24 );
	GPIOD_MODER &= ~ (1U << 25);
	
	GPIOD_OSPEEDR |= (1 << 24 );
	GPIOD_OSPEEDR &= ~ (1U << 25);
	
	GPIOD_PUPDR |= (1 << 25 );
	GPIOD_PUPDR &= ~ (1U << 24);
	
	while(1){
		
		delay(1000000);
		GPIOD_ODR |= (1 << 12);
		delay(1000000);
		GPIOD_ODR &= ~(1U << 12);
		
	}
}


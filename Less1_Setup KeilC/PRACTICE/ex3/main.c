#define RCC_APB2ENR *(unsigned int *)0x40021018
#define GPIOC_CRH 	*(unsigned int *)0x40011004
#define GPIOC_ODR 	*(unsigned int *)0x4001100C

typedef struct
{
  unsigned int CR;
  unsigned int CFGR;
  unsigned int CIR;
  unsigned int APB2RSTR;
  unsigned int APB1RSTR;
  unsigned int AHBENR;
  unsigned int APB2ENR;
  unsigned int APB1ENR;
  unsigned int BDCR;
  unsigned int CSR;
} RCC_TypeDef;


typedef struct
{
  unsigned int CRL;
  unsigned int CRH;
  unsigned int IDR;
  unsigned int ODR;
  unsigned int BSRR;
  unsigned int BRR;
  unsigned int LCKR;
} GPIO_TypeDef;


#define RCC 	((RCC_TypeDef *)0x40021000)
#define GPIOA ((GPIO_TypeDef *)0x40010800)
#define GPIOC ((GPIO_TypeDef *)0x40011000)

void delay(unsigned int ms);
void WritePin(GPIO_TypeDef *GPIO_Port, unsigned char Pin, unsigned char state);

void WritePin(GPIO_TypeDef *GPIO_Port, unsigned char Pin, unsigned char state)
{
	if(state == 1){
		GPIO_Port->ODR |= (1 << Pin);
	}else{
		GPIO_Port->ODR &= ~(1U << Pin);
	}
		
}
void delay(unsigned int ms)
{
	unsigned int i;
	for(i = 0; i < 1000 * ms; i++);
}

int main(){
	
		/* B?t d?ng h? cho GPIOC và GPIOA */

    RCC->APB2ENR |= (1 << 4);
    RCC->APB2ENR |= (1 << 2);
	
	GPIOC->CRH &= ~((1U << 23) | (1U << 22));
	GPIOC->CRH |= ((1U << 21) | (1U << 20));
	
	
	GPIOA->CRL |= (1 << 3);
	GPIOA->CRL &= ~(7U);
	GPIOA->ODR |= (1 << 0);
	
	WritePin(GPIOC,13,0);
	
	while(1)
	{
//		if(GPIOA->IDR & (1 << 0)){
//			WritePin(GPIOC,13,1);
//		}else{
//			WritePin(GPIOC,13,0);
//		}
	}
}


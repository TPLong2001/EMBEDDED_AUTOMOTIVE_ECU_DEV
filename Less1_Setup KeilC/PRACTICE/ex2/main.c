//#define RCC_APB2ENR *((unsigned int *)0x40021018)	//dereferance
//#define GPIOC_CRH 	*((unsigned int *)0x40011004)
//#define GPIOC_ODR 	*((unsigned int *)0x4001100C)
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

#define RCC 		((RCC_TypeDef*)0x40021000)
#define GPIOC 	((GPIO_TypeDef*)0x40011000)

// CAU HINH CHO CHAN PC13 -> LED

// HAM DELAY de he thong khong lam gi ca trong 1 khoang thoi gian
void delay(unsigned int timedelay){
	unsigned int i;
	for(i = 0; i < timedelay *1000; i++){}
}

int main()
{
	// 1. CAP CLOCK
	// 	su dung ky thuat bit mask de bat 1 bit naof do len 1 ma khong lam anh huowng den gia tri cua cac bit khac
	// 	su dung phep dich bit de den dung vi tri can thay doi
	RCC->APB2ENR |= (1 << 4);	// cap clock cho GPIOC tai vi tri bit 4 cua APB2
	// 2. CAU HINH CHE DO CHAN
	// 	che do output, speed 50MHZ => MODE13 = 11
	//  che do output push-pull => CNF13 = 00
	GPIOC->CRH &= ~((3 << 20)|(3 << 22));	// CLEAR 20 21 22 23
	GPIOC->CRH |= (3 << 20);								// SET BIT 20 21 len 1
	
	// 3. BAT/ TAT DEN LIEN TUC DE TAO BLINK LED
	// 	den PC13 voi dien tro keo len => xuat data ra chan = 0 LED ON, data = 1 LED OFF
	while(1){
		// Xuat data = 0 LED ON
		GPIOC->ODR &= ~(1 << 13);
		// doi 1 khoang thoi gian
		delay(1000);
		// Xuat data = 1 LED OFF
		GPIOC->ODR |= (1 << 13);
		// doi 1 khoang thoi gian
		delay(1000);
	}
}
#include <stdint.h>
#include <stm32l4xx.h>

int mux = 0;
int tick = 0;
int uren = 0;
int minuten = 0;

void delay(unsigned int n){
	volatile unsigned int delay = n;
	while (delay){
		if (tick){
			delay--;
			tick = 0;
		}
	}
}

void clear(){
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
}

void seg7(int n){
	switch (n){
		case 0:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1);
			break;
		case 1:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			break;
		case 2:
			GPIOA->ODR |= (GPIO_ODR_OD7);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD2 | GPIO_ODR_OD15 | GPIO_ODR_OD12);
			break;
		case 3:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD2 | GPIO_ODR_OD12);
			break;
		case 4:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
		case 5:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD2 | GPIO_ODR_OD12);
			break;
		case 6:
			GPIOA->ODR |= (GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD1 | GPIO_ODR_OD15 | GPIO_ODR_OD12 | GPIO_ODR_OD2);
			break;
		case 7:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0);
			break;
		case 8:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
		case 9:
			GPIOA->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD5);
			GPIOB->ODR |= (GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
			break;
	}
}

void SysTick_Handler(void){
	tick++;
	switch(mux){
	case 0:
		clear();
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 00
		seg7(uren / 10);
		GPIOA->ODR &= ~(GPIO_ODR_OD6);
		break;

	case 1:
		if (uren >= 24){
			clear();
			uren = 0;
		}
		else{
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8);
			GPIOA->ODR &= ~(GPIO_ODR_OD15);		// 10
			seg7(uren % 10);
		}
		GPIOA->ODR |= (GPIO_ODR_OD6);
		break;

	case 2:		clear();
		GPIOA->ODR &= ~(GPIO_ODR_OD8);
		GPIOA->ODR |= (GPIO_ODR_OD15);		// 01
		seg7((minuten / 10) % 10);
		GPIOA->ODR &= ~(GPIO_ODR_OD6);
		break;

	case 3:
		if (minuten >= 60){
			clear();
			minuten = 0;
			uren ++;
		}

		else{
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8);
			GPIOA->ODR |= (GPIO_ODR_OD15);		// 11
			seg7(minuten % 10);
		}
		GPIOA->ODR &= ~(GPIO_ODR_OD6);
		break;
	}
	mux++;

	if (mux>3){
		mux = 0;
	}
}

int main(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    //7seg leds
	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE7_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;

	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE0_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;

	GPIOB->MODER &= ~GPIO_MODER_MODE12_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE12_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT12;

	GPIOB->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE15_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOB->MODER &= ~GPIO_MODER_MODE1_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE1_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT1;

	GPIOB->MODER &= ~GPIO_MODER_MODE2_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;

	//mux
	GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE8_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

	GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE15_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE6_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

    while (1) {
    	minuten++;
    	delay(1000);

    GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk;//Knop A
    GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk;//Knop B

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;//alle bits van de pull up weerstand van knop A op 0 zetten
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;//bit 0 van knop A hoog maken

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;//alle bits van de pull up weerstand van knop B op 0 zetten
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;//bit 0 van knop B hoog maken


      }

    }
}

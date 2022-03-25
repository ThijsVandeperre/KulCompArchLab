#include <stdint.h>
#include <stm32l4xx.h>

int ms = 0;
int mux = 0;
int uren = 0;
int minuten = 0;

void delay(unsigned int n) {
	volatile unsigned int delay = n;
	while (delay--) {
		;
	}
}

void clear() {//alle segmenten laag zetten
	GPIOA->ODR &= ~(GPIO_ODR_OD7 | GPIO_ODR_OD5);
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD12 | GPIO_ODR_OD15 | GPIO_ODR_OD1 | GPIO_ODR_OD2);
}

void seg7(int n) {
	switch (n) {
		case 0: //getal van elke case is het getal dat getoond wordt
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

void SysTick_Handler(void) {
	switch (mux) {
		case 0://00
			clear();
			GPIOA->ODR &= ~(GPIO_ODR_OD8); //Disp 1 laag zetten
			GPIOA->ODR &= ~(GPIO_ODR_OD15);//Disp 2 laag zetten
			seg7(uren / 10);
			GPIOA->ODR &= ~(GPIO_ODR_OD6); //Seg DP laag zetten
			break;

		case 1:// 10
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8);  //Disp 1 hoog zetten
			GPIOA->ODR &= ~(GPIO_ODR_OD15);//Disp 2 laag zetten
			seg7(uren % 10);
			GPIOA->ODR |= (GPIO_ODR_OD6);  //Seg DP hoog zetten
			break;

		case 2:// 01
			clear();
			GPIOA->ODR &= ~(GPIO_ODR_OD8);//Disp 1 laag zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7(minuten / 10);
			GPIOA->ODR &= ~(GPIO_ODR_OD6);//Seg DP laag zetten
			break;

		case 3:// 11
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8); //Disp 1 hoog zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7(minuten % 10);
			GPIOA->ODR &= ~(GPIO_ODR_OD6);//Seg DP laag zetten
			break;
	}
	mux++;
	ms++;

	if (ms == 60000) {
		ms = 0;
		minuten++;
		if (minuten >= 60) {
			minuten = 0;
			uren++;
			if (uren >= 24) {
				uren = 0;
			}
		}
	}
	else if (mux > 3) {
		mux = 0;
	}
}

int main(void) {
	//Activeren klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	//Knoppen A en B
	GPIOB->MODER &= ~GPIO_MODER_MODE13_Msk; // Knop A configureren naar input
	GPIOB->MODER &= ~GPIO_MODER_MODE14_Msk; // Knop B configureren naar input

	//Knop A
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;//pull up weerstand van Knop A wordt hoog gezet
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;

	//Knop B
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14_Msk;//pull up weerstand van Knop B wordt hoog gezet
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

	//7seg LED's
	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk; //Seg A
	GPIOB->MODER |= GPIO_MODER_MODE0_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;

	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk; //Seg B
	GPIOA->MODER |= GPIO_MODER_MODE7_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;

	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk; //Seg C
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOB->MODER &= ~GPIO_MODER_MODE12_Msk; //Seg D
	GPIOB->MODER |= GPIO_MODER_MODE12_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT12;

	GPIOB->MODER &= ~GPIO_MODER_MODE15_Msk; //Seg E
	GPIOB->MODER |= GPIO_MODER_MODE15_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;

	GPIOB->MODER &= ~GPIO_MODER_MODE1_Msk; //Seg F
	GPIOB->MODER |= GPIO_MODER_MODE1_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT1;

	GPIOB->MODER &= ~GPIO_MODER_MODE2_Msk; //Seg G
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;

	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk; //Seg DP
	GPIOA->MODER |= GPIO_MODER_MODE6_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

	//mux
	GPIOA->MODER &= ~GPIO_MODER_MODE8_Msk; //Disp 0
	GPIOA->MODER |= GPIO_MODER_MODE8_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;

	GPIOA->MODER &= ~GPIO_MODER_MODE15_Msk; //Disp 1
	GPIOA->MODER |= GPIO_MODER_MODE15_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT15;

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	while (1) {

		if (!(GPIOB->IDR & GPIO_IDR_ID14)) {
			uren++;
			if (uren >= 24) {
				uren = 0;
			}
			delay(1000000);
		}

		else if (!(GPIOB->IDR & GPIO_IDR_ID13)) {

			minuten++;
			if (minuten >= 60) {
				minuten = 0;
			}

			delay(1000000);
		}

	}

}

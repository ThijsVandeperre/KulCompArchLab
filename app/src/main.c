#include <stdint.h>
#include <stm32l4xx.h>
#include <math.h>


int mux = 0;
int alpha = 0;

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
			seg7(alpha / 1000);
			break;

		case 1:// 10
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8);  //Disp 1 hoog zetten
			GPIOA->ODR &= ~(GPIO_ODR_OD15);//Disp 2 laag zetten
			seg7((alpha / 100)%10);
			break;

		case 2:// 01
			clear();
			GPIOA->ODR &= ~(GPIO_ODR_OD8);//Disp 1 laag zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7((alpha % 100)/10);
			break;

		case 3:// 11
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8); //Disp 1 hoog zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7((alpha % 100)%10);
			break;
	}
	mux++;

	if (mux > 3) {
				mux = 0;
			}

}

int __io_putchar(int ch){
		    while(!(USART1->ISR & USART_ISR_TXE));
		    USART1->TDR = ch;
		}
void accelerometer_write(int data, int reg){
	I2C1->CR2 &= ~(1 << 10);
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
	I2C1->CR2 |= (1 << 13)|(2 << 16)|(0x53 << 1);
	while((I2C1->ISR & (1 << 4)) == 0 && (I2C1->ISR & (1 << 1)) == 0);
		if((I2C1->ISR & (1 << 4)) != 0){
			return;
	}
	I2C1->TXDR = reg;

	while(I2C1->ISR & (1 << 4) == 0 && I2C1->ISR & ( 1<<1 ) == 0);
	if((I2C1->ISR & (1 << 4)) != 0){
		return;
	}
	I2C1->TXDR = data;
	while((I2C1->ISR & I2C_ISR_STOPF) == 0);
}

int accelerometer_read(int reg){
	while((I2C1->ISR & I2C_ISR_BUSY));
	I2C1->CR2 &= ~(1 << 10);
	I2C1->CR2 &= ~I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 &= ~I2C_CR2_NBYTES_Msk;
	I2C1->CR2 |= I2C_CR2_NACK_Msk;
	I2C1->CR2 |= (1 << 13)|(1 << 16)|(0x53 << 1);
	while(((I2C1->ISR & (1 << 4)) == 0) && ((I2C1->ISR & (1 << 1)) == 0));
	if((I2C1->ISR & (1 << 4)) != 0){
		return;
	}
	I2C1->TXDR = reg;
	while((I2C1->ISR & (1 << 6)) == 0);

	I2C1->CR2 |= I2C_CR2_AUTOEND_Msk;
	I2C1->CR2 |= (1 << 10);
	I2C1->CR2 |= (1 << 16)|(0x53 << 1);
	I2C1->CR2 |= (1 << 13);
	while(!(I2C1->ISR & I2C_ISR_RXNE));
	return I2C1->RXDR;
}

int main(void) {

	//NTC
	GPIOA->MODER &= ~GPIO_MODER_MODE0_Msk; //NTC configureren naar Analog
	GPIOA->MODER |= GPIO_MODER_MODE0_0;
	GPIOA->MODER |= GPIO_MODER_MODE0_1;

	//Potmeter
	GPIOA->MODER &= ~GPIO_MODER_MODE1_Msk; //NTC configureren naar Analog
	GPIOA->MODER |= GPIO_MODER_MODE1_0;
	GPIOA->MODER |= GPIO_MODER_MODE1_1;

	//Activeren klok
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//Klok selecteren, hier gebruiken we syslck
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

	//Deep powerdown modus uitzetten
	ADC1->CR &= ~ADC_CR_DEEPPWD;

	//ADC voltage regulator aanzetten
	ADC1->CR |= ADC_CR_ADVREGEN;

	//Delay a few milliseconds, see datasheet for exact timing
	delay(1500);

	//Kalibratie starten
	ADC1->CR | ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	//ADC aanzetten
	ADC1->CR |= ADC_CR_ADEN;

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

	//GPIO configureren en alternative mode aanzetten
	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |=  GPIO_MODER_MODE9_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL9_Msk)) | (0x7 << GPIO_AFRH_AFSEL9_Pos);

	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & (~GPIO_AFRH_AFSEL10_Msk)) | (0x7 << GPIO_AFRH_AFSEL10_Pos);

	// UART configureren
	USART1->CR1 = 0;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->BRR = 417;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	// CPU Frequentie = 48 MHz
	// Systick interrupt elke 1 ms (1kHz)  --> 48000000 Hz / 1000 Hz --> Reload = 48000
	SysTick_Config(48000);

	// Interrupt aanzetten met een prioriteit van 128
	NVIC_SetPriority(SysTick_IRQn, 128);
	NVIC_EnableIRQ(SysTick_IRQn);

	// TIMER
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	GPIOB->MODER &= ~GPIO_MODER_MODE8_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE8_1;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT8;
	GPIOB->AFR[1] = (GPIOB->AFR[1] & (~GPIO_AFRH_AFSEL8_Msk)) | (0xE << GPIO_AFRH_AFSEL8_Pos);

	TIM16->PSC = 0;

	// I2C
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	// De GPIO correct configureren voor de alternative mode
	GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL6_Msk)) | (0x4 << GPIO_AFRL_AFSEL6_Pos);

	GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
	GPIOB->MODER |=  GPIO_MODER_MODE7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->AFR[0] = (GPIOB->AFR[0] & (~GPIO_AFRL_AFSEL7_Msk)) | (0x4 << GPIO_AFRL_AFSEL7_Pos);

	// De I2C module instellen voor 100kHz klok en deze dan ook aanzetten.
	I2C1->TIMINGR = 0x20303E5D;
	I2C1->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);
	I2C1->CR1 |= I2C_CR1_PE;

	volatile int16_t array[3];
	accelerometer_write(1 << 3,0x2D);
	array[0] = accelerometer_read(0x2D);

	while(1){
		for (int i = 0; i<3; i++){
			array[i] = accelerometer_read(0x32 + i*2) << 8 + accelerometer_read(0x32 + i*2 + 1);
		}
		int xy = (sqrt(array[0]^2) + (array[1]^2));
		int xyz = sqrt((xy^2) + (array[2]^2));
		alpha = (acos(array[2]/(sqrt(array[0]*array[0] + array[1]*array[1] + array[2]*array[2]))))*(180/3.14);
		printf("%2.2f",alpha);
		printf("\n\r");

	}
}

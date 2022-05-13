#include <stdint.h>
#include <stm32l4xx.h>

int mux = 0;
int temperatuur = 0;
int input_NTC;
int input_potmeter;
float voltage;
float weerstand;
int i=0;
float t;
int mseconden=0;

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
			seg7(temperatuur / 1000);
			GPIOA->ODR &= ~(GPIO_ODR_OD6); //Seg DP laag zetten
			break;

		case 1:// 10
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8);  //Disp 1 hoog zetten
			GPIOA->ODR &= ~(GPIO_ODR_OD15);//Disp 2 laag zetten
			seg7((temperatuur / 100)%10);
			GPIOA->ODR &= ~(GPIO_ODR_OD6);  //Seg DP laag zetten
			break;

		case 2:// 01
			clear();
			GPIOA->ODR &= ~(GPIO_ODR_OD8);//Disp 1 laag zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7((temperatuur % 100)/10);
			GPIOA->ODR |= (GPIO_ODR_OD6);//Seg DP hoog zetten
			break;

		case 3:// 11
			clear();
			GPIOA->ODR |= (GPIO_ODR_OD8); //Disp 1 hoog zetten
			GPIOA->ODR |= (GPIO_ODR_OD15);//Disp 2 hoog zetten
			seg7((temperatuur % 100)%10);
			GPIOA->ODR &= ~(GPIO_ODR_OD6);//Seg DP laag zetten
			break;
	}
	mux++;
	mseconden++;

}

int __io_putchar(int temperatuur){
		    while(!(USART1->ISR & USART_ISR_TXE));
		    USART1->TDR = temperatuur;
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

	while(1){
		// Start de ADC en wacht tot de sequentie klaar is

		// Kanalen instellen
		ADC1->SMPR1 &= ~(ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_2);
		ADC1->SMPR1 = ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2;
		ADC1->SQR1 = ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_2;
		ADC1->CR |= ADC_CR_ADSTART;
		input_potmeter = ADC1->DR;
		while(!(ADC1->ISR & ADC_ISR_EOC));

		// Lees de waarde in
		input_NTC = ADC1->DR;
		voltage = (input_NTC*3.0f)/4096.0f;
		weerstand = (10000.0f*voltage)/(3.0f-voltage);
		temperatuur = ((1.0f/((logf(weerstand/10000.0f)/3936.0f)+(1.0f/298.15f)))-273.15f)*10;

		// Kanalen instellen
		ADC1->SMPR1 &= ~(ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 | ADC_SMPR1_SMP5_2);//reset
		ADC1->SMPR1 = ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_2;
		ADC1->SQR1 = ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2;
		ADC1->CR |= ADC_CR_ADSTART;
		while(!(ADC1->ISR & ADC_ISR_EOC));

		TIM16->CCMR1 &= ~TIM_CCMR1_CC1S;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1FE;
		TIM16->CCER |= TIM_CCER_CC1E;
		TIM16->CCER &= ~TIM_CCER_CC1P;
		TIM16->CR1 |= TIM_CR1_CEN;

		if (input_potmeter > input_NTC ) {
			TIM16->BDTR |= TIM_BDTR_MOE;
			if (i<1000) {
				TIM16->ARR = 24000;
				TIM16->CCR1 = 12000;
				delay(10);
			}
			else if (i<2000) {
				TIM16->ARR = 12000;
				TIM16->CCR1 = 6000;
				delay(10);
			}
			else {
				i=0;
				delay(10);
			}
			i++;
		}
		else {
			TIM16->BDTR &= ~TIM_BDTR_MOE;
			TIM16->CR1 &= ~TIM_CR1_CEN;
		}

		if (mseconden > 1000) {
			mseconden = 0;
			t=temperatuur;
			printf("%.1f°C\n\r",t/10);
		}
		if (mux > 3) {
			mux = 0;
		}
	}
}

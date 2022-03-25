/**
 * @file main.c
 * @version 0.1
 *
 */

#include <stdint.h>
#include <stm32l4xx.h>

int main(void) {

  while (1) {
  }
}


GPIOA->MODER |= GPIO_MODER_MODE0_Msk;
GPIOA->MODER |= GPIO_MODER_MODE0_0;


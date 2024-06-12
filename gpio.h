#ifndef __GPIO_H
#define __GPIO_H
#include "stm32f10x.h"


#define SPI1_NSS 	GPIO_Pin_4
#define SPI1_SCK	GPIO_Pin_5
#define SPI1_MISO 	GPIO_Pin_6
#define SPI1_MOSI 	GPIO_Pin_7
#define SPI1_GPIO 	GPIOA
void GPIO_Config(void);
#endif
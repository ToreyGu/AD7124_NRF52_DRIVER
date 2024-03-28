
#ifndef NRF_AD7124_PINMAP_H
#define NRF_AD7124_PINMAP_H

#include "nrf_gpio.h" 

//DEFAULT SPI PIN CONFIG 
#define AD7124_SPI_SCK_PIN	NRF_GPIO_PIN_MAP(0,23)
#define AD7124_SPI_MISO_PIN	NRF_GPIO_PIN_MAP(0,19)
#define AD7124_SPI_MOSI_PIN	NRF_GPIO_PIN_MAP(0,21)
#define AD7124_SPI_CS_PIN   NRF_GPIO_PIN_MAP(1,9)

#endif
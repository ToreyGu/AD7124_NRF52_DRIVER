/*
 * @C++ FILES: 
 * @Descripttion: 
 * @version: 
 * @Author: TheiiKu
 * @Date: 2024-03-20 15:16:48
 * @LastEditors: TheiiKu
 * @LastEditTime: 2024-03-20 16:50:39
 * @afterAnnotation: CRATED IN JLU SENSOR LAB
 */
#ifndef NRF_AD7124_SPI_H
#define NRF_AD7124_SPI_H

#include <stdint.h>
#include <stdbool.h>

#define NRF_SPI_BUFFER_SIZE 256

    #ifndef AD7124_SPI_INSTANCE
    #define AD7124_SPI_INSTANCE 0
    #endif
//init ad7124 spi 
void NRF_AD7124_SPI_CS_Set(bool s_csStatus);
void NRF_AD7124_SPI_Init(void);
void NRF_AD7124_SPI_ReadWrite(uint8_t *pWriteData, uint8_t *pReadData, uint8_t writeDataLen , uint8_t readDataLen) ;
void NRF_AD7124_SPI_Enable(void);
void NRF_AD7124_SPI_Disable(void);

//CS PIN CONFIG MACRO 
#define CS_ENABLE  false
#define CS_DISABLE true
	
#define AD7124_CS_ENABLE NRF_AD7124_SPI_CS_Set(CS_ENABLE)
#define AD7124_CS_DISABLE NRF_AD7124_SPI_CS_Set(CS_DISABLE)

#endif

/*
 * @C++ FILES: 
 * @Descripttion: 
 * @version: 
 * @Author: TheiiKu
 * @Date: 2024-03-20 15:15:59
 * @LastEditors: TheiiKu
 * @LastEditTime: 2024-03-20 17:07:01
 * @afterAnnotation: CRATED IN JLU SENSOR LAB
 */
#include <stdio.h>
#include <string.h>
#include "NRF_AD7124_SPI.h"
#include "nrf_drv_spi.h"
#include "NRF_AD7124_PINMAP.h"
#include "nrf_gpio.h"
#include "app_error.h"

// CONVERTING FINISHED FLAG 
static volatile bool s_transferOk = true; 
//ADD A SPI INSTANCE FOR NRF DEVICE 
static const nrf_drv_spi_t d_HSPI_AD7124 = NRF_DRV_SPI_INSTANCE(AD7124_SPI_INSTANCE);

//HANDEL FUNCTION FOR NRF52
//WHEN SPI TRANSFORM DINISHED FLAG s_transferOk WILL BE TRUE  
static void h_NRF_AD7124_SPI_Callback(nrf_drv_spi_evt_t const* p_event,void* p_context){
     s_transferOk = true;
}

void NRF_AD7124_SPI_CS_Set(bool s_csStatus){
	  nrf_gpio_cfg_output(AD7124_SPI_CS_PIN);
    if (s_csStatus )
    {
        nrf_gpio_pin_set(AD7124_SPI_CS_PIN);
    }else{

        nrf_gpio_pin_clear(AD7124_SPI_CS_PIN);
    }

}

//INIT FUNCTION FOR AD7124 SPI 
void NRF_AD7124_SPI_Init(void){


    NRF_AD7124_SPI_CS_Set(CS_DISABLE);
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
		//spi_config.ss_pin = AD7124_SPI_CS_PIN;
		spi_config.mode     = NRF_DRV_SPI_MODE_0;
		spi_config.miso_pin = AD7124_SPI_MISO_PIN;
		spi_config.mosi_pin = AD7124_SPI_MOSI_PIN;
		spi_config.sck_pin  = AD7124_SPI_SCK_PIN;
		APP_ERROR_CHECK(nrf_drv_spi_init(&d_HSPI_AD7124, &spi_config, h_NRF_AD7124_SPI_Callback, NULL));
}
/*
 @brief SPI READ DATA AND WRITE DATA 
 @param pWriteData -[in] WRITE DATA
 @param pReadData -[out] READ DATA 
 @param writeDataLen -[in] DATALENTH
 @return NONE 
*/
void NRF_AD7124_SPI_ReadWrite(uint8_t *pWriteData, uint8_t *pReadData, uint8_t writeDataLen , uint8_t readDataLen ){

    s_transferOk = false;
	
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&d_HSPI_AD7124, pWriteData, writeDataLen, pReadData, readDataLen));
    
    while ( !s_transferOk)
    {
       __WFE();
    }
		
}

void NRF_AD7124_SPI_Enable(void){

    NRF_AD7124_SPI_CS_Set(CS_DISABLE);
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
	//spi_config.ss_pin = AD7124_SPI_CS_PIN;
	spi_config.miso_pin = AD7124_SPI_MISO_PIN;
	spi_config.mosi_pin = AD7124_SPI_MOSI_PIN;
	spi_config.sck_pin = AD7124_SPI_SCK_PIN;
	APP_ERROR_CHECK(nrf_drv_spi_init(&d_HSPI_AD7124, &spi_config, h_NRF_AD7124_SPI_Callback, NULL));

}

void NRF_AD7124_SPI_Disable(void){

    nrf_drv_spi_uninit(&d_HSPI_AD7124);

}


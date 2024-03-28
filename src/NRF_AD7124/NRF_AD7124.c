/*
 * @C++ FILES: 
 * @Descripttion: 
 * @version: 
 * @Author: TheiiKu
 * @Date: 2024-03-20 15:14:42
 * @LastEditors: TheiiKu
 * @LastEditTime: 2024-03-27 21:55:32
 * @afterAnnotation: CRATED IN JLU SENSOR LAB
 */
#include <stdio.h>
//#include "nrf_log.h"
#include "NRF_AD7124.h"
#include "NRF_AD7124_SPI.h"
#include "NRF_AD7124_error.h"
#include "NRF_AD7124_regs.h" 
#include "nrf_delay.h"

#define AD7124_CONFIG_NUM 8
#define AD7124_CHANNEL_NUM 16

    #ifdef ENABLE_STATUS
        static uint8_t rxDataLenth = 4;
    #else
        static uint8_t rxDataLenth = 3;
    #endif

//REGISITERS MAP FOR AD7124

/********************************************************
* Function name ：AD7124_Reset
* Description   : RESET AD7124 CHIP , THIS FUNCTION MUST RUN BEFORE R&W CHIP REGISITER
* Parameter     ：
* Return        ：1 --success  ,  other -- fail
**********************************************************/
int32_t AD7124_Reset(){
    AD7124_CS_ENABLE;
    uint8_t cmd_reset[9]= {0xff , 0xff , 0xff , 0xff ,0xff , 0xff ,0xff , 0xff ,0xff };
		uint8_t rx_buffer[8];
		
    NRF_AD7124_SPI_ReadWrite(
        cmd_reset,
        rx_buffer,
        9,
        rxDataLenth
     );

    nrf_delay_us(60);

    AD7124_CS_DISABLE;

    return AD7124_SUCCESS;
}


/********************************************************
* Function name ：AD7124_Init
* Description   : INIT AD7124 CHIP 
* Parameter     ：
* Return        ：1 --success  ,  other -- fail
**********************************************************/

int32_t AD7124_Init(void){
  
		NRF_AD7124_SPI_Init();
        AD7124_Reset();//MUST RESET CHIP BEFORE R&W REGISTERS 
	
    return AD7124_SUCCESS;
	
}

/********************************************************
* Function name ：AD7124_Write_Register
* Description   : WRITE A REGISITER STRUCT TO CHIP
* Parameter     ：struct ad7124_st_reg reg : REGISITER PREPARE TO WRITE 
* Return        ：1 --success  ,  other -- fail
**********************************************************/

int32_t AD7124_Write_Register(struct ad7124_st_reg reg ){
    
    int32_t reg_value = reg.value;//buffer of regisiter value 
    uint8_t write_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};//buffer of AD7124 regisiter 

    if (reg.rw == AD7124_R){
        return ERROR_RW;
    }

    AD7124_CS_ENABLE;
    //WRITE ADDR & CMD FLAGS
    write_buffer[0] = AD7124_COMM_REG_WEN | AD7124_COMM_REG_WR | AD7124_COMM_REG_RA(reg.addr);
    //convert register value(1 to 3 byte) to a buffer array
    for(uint8_t i = 0 ; i < reg.size  ; i++ ){
        write_buffer[reg.size - i] = reg_value & 0xFF;
				reg_value >>= 8;
    }
//		for(uint8_t i = 0 ; i < reg.size+1  ; i++ ){
//		NRF_LOG_DEBUG("WRITE BUFFER %d %x" ,i , write_buffer[i]);
//			
//    }

    //use NRF52 SPI wirte CMD
    NRF_AD7124_SPI_ReadWrite(
        write_buffer,
        NULL,
        reg.size + 1,
        rxDataLenth
     );

    nrf_delay_us(60);

    AD7124_CS_DISABLE;
    
    return AD7124_SUCCESS;
}

int32_t AD7124_Read_Register(struct ad7124_st_reg reg , uint8_t *p_ReadBuffer){
    
    uint8_t write_buffer[8] =  {0, 0, 0, 0, 0, 0, 0, 0};

    if (reg.rw == AD7124_W){
        return ERROR_RW;
    }
		AD7124_CS_ENABLE;


    //WRITE ADDR & CMD FLAGS
    write_buffer[0] =  AD7124_COMM_REG_WEN | AD7124_COMM_REG_RD | AD7124_COMM_REG_RA(reg.addr);
    //TODO 需要加入写缓存转换
    NRF_AD7124_SPI_ReadWrite(
        write_buffer,
        p_ReadBuffer,
        reg.size + 1,
        rxDataLenth
    );

    
    
    AD7124_CS_DISABLE;
		
    return AD7124_SUCCESS;

}




void AD7124_Config_ADCCtrl(struct ad7124_st_reg reg ){
   
    AD7124_Write_Register(reg );
    //CHECK IF OPEN AD7124 DATA+STATUS MODE
    if (reg.value && AD7124_DATA_STATUS )
    {
        rxDataLenth = 4 ;
    }
}


void AD7124_Load_RegisiterMap(struct ad7124_st_reg  *reg){
    while (reg->value)
    {
        AD7124_Write_Register(*reg);
        reg++;
    }
    
}

void AD7124_Load_TogglePDSW(){
    ad7124_regs[AD7124_IOCon1].value = reversebit(ad7124_regs[AD7124_IOCon1].value,AD7124_IOCTRL_PDSW);
    AD7124_Write_Register(ad7124_regs[AD7124_IOCon1]);

}

uint8_t AD7124_Get_Channel_Setup(uint8_t channel){//获得通道的配置寄存器映射(Config_x)

    return (ad7124_regs[AD7124_Channel_0 + channel].value >> 12) & 0x7;

}

uint8_t AD7124_Get_PGA(uint8_t channel){//通过寄存器映射获得PGA
    uint8_t setup = AD7124_Get_Channel_Setup(channel);
    return (ad7124_regs[AD7124_Config_0 + setup].value) & 0x7;
}

bool AD7124_Get_Channel_Bipolar(uint8_t channel){//获取通道是否为双极性

	uint8_t setup = AD7124_Get_Channel_Setup(channel);
	return ((ad7124_regs[AD7124_Config_0 + setup].value >> 11) & 0x1) ? true : false;
}

float AD7124_Convert_Sample_To_Voltage(uint8_t channel, uint32_t sample){
	bool isBipolar = AD7124_Get_Channel_Bipolar(channel);
	uint8_t channelPGA = AD7124_Get_PGA(channel);

	float convertedValue;

	if (isBipolar) {
        convertedValue = ( ((float)sample / (1 << (AD7124_ADC_N_BITS -1))) -1 ) * \
        		              (AD7124_REF_VOLTAGE / AD7124_PGA_GAIN(channelPGA));
	} else {
		convertedValue = ((float)sample * AD7124_REF_VOLTAGE)/(AD7124_PGA_GAIN(channelPGA) * \
								                              (1 << AD7124_ADC_N_BITS));
	}

    return (convertedValue);
}

int32_t AD7124_Set_Config(uint8_t config , int32_t value ){//修改配置
    
    if (config >= 0 && config < AD7124_CONFIG_NUM)
    {    

        ad7124_regs[AD7124_Config_0 + config].value = value ;
        AD7124_Write_Register(ad7124_regs[AD7124_Config_0 + config]);

        return AD7124_SUCCESS;

    }

        return ERROR_ADDR_OVERFLOW;

}

int32_t AD7124_Set_PGA(uint8_t config , int32_t PGA){
    if (config >= 0 && config < AD7124_CONFIG_NUM)
    {    

        int32_t value = ad7124_regs[AD7124_FILT0_REG + config].value; 
        value = (value & 0x0FF8) | PGA ;
        ad7124_regs[AD7124_FILT0_REG + config].value = value ; 
        AD7124_Write_Register(ad7124_regs[AD7124_FILT0_REG + config]);
        return AD7124_SUCCESS;

    }
        return ERROR_ADDR_OVERFLOW;
    
}

int32_t AD7124_Set_Filter(uint8_t config ,int32_t Filter){
    
    if (config >= 0 && config <AD7124_CONFIG_NUM)
    {   
        int32_t value = ad7124_regs[AD7124_FILT0_REG + config].value;
        value = (value & 0x1F07FF )| Filter << 21;
        ad7124_regs[AD7124_FILT0_REG + config].value = value ; 
        AD7124_Write_Register(ad7124_regs[AD7124_FILT0_REG + config]);
        return AD7124_SUCCESS;
    }
        return ERROR_ADDR_OVERFLOW;
}

int32_t AD7124_Enable_Channel(uint8_t channel ){

        
    if (channel >= 0 && channel <AD7124_CHANNEL_NUM)
    {    

        int32_t value = setbit(ad7124_regs[AD7124_Channel_0 + channel ].value , 16) ;
        ad7124_regs[AD7124_Channel_0 + channel ].value = value ; 
        AD7124_Write_Register(ad7124_regs[AD7124_Channel_0 + channel ]);
        return AD7124_SUCCESS;

    }
        return ERROR_ADDR_OVERFLOW;

}

int32_t AD7124_Disable_Channel(uint8_t channel){

    if (channel >= 0 && channel <AD7124_CHANNEL_NUM)
    {    

        int32_t value = clrbit(ad7124_regs[AD7124_Channel_0 + channel ].value , 16) ;
        ad7124_regs[AD7124_Channel_0 + channel ].value = value ; 
        AD7124_Write_Register(ad7124_regs[AD7124_Channel_0 + channel ]);
        return AD7124_SUCCESS;

    }
        return ERROR_ADDR_OVERFLOW;
}


int32_t AD7124_Set_Channel_Input(uint8_t channel , uint8_t inputP ,uint8_t inputM ){

    if (channel >= 0 && channel <AD7124_CHANNEL_NUM)
    {    
        int32_t value = ad7124_regs[AD7124_Channel_0 + channel ].value & 0xFC00;
        value |= inputM | (inputP<<5);
        ad7124_regs[AD7124_Channel_0 + channel ].value  = value ;
        AD7124_Write_Register(ad7124_regs[AD7124_Channel_0 + channel ]);
        
        return AD7124_SUCCESS;
    }
        return ERROR_ADDR_OVERFLOW;
}

int32_t AD7124_Get_OutputData(){
        uint8_t rxBuffer[4] = { 0 , 0 , 0 , 0};

        AD7124_Read_Register(ad7124_regs[AD7124_DATA_REG] ,rxBuffer );


}
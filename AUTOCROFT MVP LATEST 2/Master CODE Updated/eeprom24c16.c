#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "i2c.h"
#include "gpio.h"
#include "systick.h"
#include "eeprom24c16.h"

#define PAGE_SIZE	    	16

void EEPROM_Init(void)
{
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN6_OUTPUT_MODE_2MHZ | GPIO_PIN7_OUTPUT_MODE_2MHZ),
								 (GPIO_PIN6_ALT_FUNC_OPEN_DRAIN | GPIO_PIN7_ALT_FUNC_OPEN_DRAIN));
	
	I2C_Init(I2C1,
					 I2C_PERIPH_FREQ_8MHZ,
					 I2C_STANDARD_MODE_8MHZ_CCR, 
					 I2C_STANDARD_MODE_8MHZ_TRISE);
}

void EEPROM_WritePage(uint8_t pageAddr, uint8_t* pData, uint8_t len)
{
	if (len > PAGE_SIZE)
	{//Page size must not be exceeded.
		return;
	}
	//optional but ensures the user's input doesn't exceed 127
	pageAddr = pageAddr % 128; 
	//extract 3 MSB of EEPROM page address and store in EEPROM slave address
	uint8_t deviceAddr = ((((pageAddr >> 3) & 0x0E) | 0xA0)>>1);
	//extract 4 LSB of EEPROM page address and store in EEPROM word address
	uint8_t wordAddr = ((pageAddr & 0x0F) << 4);
	//I2C communication involving EEPROM slave and word address
	I2C_WriteMultiByte(I2C1,deviceAddr ,wordAddr, pData, len);
	//write cycle time
	SysTick_DelayMs(5);
}

void EEPROM_ReadPage(uint8_t pageAddr, uint8_t* pReceiveBuffer, uint8_t len)
{
	if (len > PAGE_SIZE)
	{//Page size must not be exceeded.
		return;
	}
	//optional but ensures the user's input doesn't exceed 127
	pageAddr = pageAddr % 128; 
	//extract 3 MSB of EEPROM page address and store in EEPROM slave address
	uint8_t deviceAddr = ((((pageAddr >> 3) & 0x0E) | 0xA0)>>1);
	//extract 4 LSB of EEPROM page address and store in EEPROM word address
	uint8_t wordAddr = ((pageAddr & 0x0F) << 4);
	//I2C communication involving EEPROM slave and word address
	I2C_ReadMultiByte(I2C1,deviceAddr, wordAddr, pReceiveBuffer, len);
}

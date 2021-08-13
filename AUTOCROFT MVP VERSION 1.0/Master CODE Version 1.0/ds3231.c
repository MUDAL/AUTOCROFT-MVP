#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "i2c.h"
#include "ds3231.h"

/*
NB: 
1.) BCDToHex() is used to convert time data (seconds,minutes and hours) from the DS3231 
to a form usable by the main application.

2.) HexToBCD() is used to convert time data (seconds,minutes and hours) from a form used
by the application (i.e. binary or decimal or hexadecimal etc.) to a form usable by the
DS3231 (i.e. BCD)
*/

enum Register_address
{
	DS3231_ADDR = 0x68,
	SEC_REG_ADDR = 0x00,
	MIN_REG_ADDR = 0x01,
	HOUR_REG_ADDR = 0x02,
	ALARM2_MIN_REG_ADDR = 0x0B,
	ALARM2_HOUR_REG_ADDR = 0x0C,
	ALARM2_DY_DT_REG_ADDR = 0x0D,
	CONTROL_REG_ADDR = 0x0E,
	STATUS_REG_ADDR = 0x0F
};

static uint8_t BCDToHex(uint8_t bcd)
{
	/*
	Description:
	Converts BCD to hexadecimal
	
	Parameters:
	1.) bcd: this parameter is passed with a value to be converted
	to hexadecimal.
	
	Return:
	1.) Hexadecimal equivalent of the argument passed to 'bcd'.
	*/
	uint8_t hex;
	
	hex = (((bcd & 0xF0)>>4)*10) + (bcd&0x0F);
	return hex;
}

static uint8_t HexToBCD(uint8_t hex)
{
	/*
	Description:
	Converts Hexadecimal to BCD.
	
	Parameters:
	1.) hex: this parameter is passed with a value to be
	converted to BCD. The argument doesn't necessarily need
	to be hexadecimal. It can be binary, decimal etc. If its
	any number system other than hexadecimal, the compiler
	would be able to interprete the number system as hexadecimal
	and the algorithm would still behave the same.
	
	Return:
	1.) BCD equivalent of the argument passed to 'hex'.
	
	*/
	uint8_t bcd;
	uint8_t multipleOfTen = 0;
	
	while (hex >= 10)
	{
		hex -= 10;
		multipleOfTen++;
	}
	bcd = ((multipleOfTen<<4) | hex);
	return bcd;
}

void DS3231_Init(void)
{
	/*
	Description:
	Initializes the DS3231 RTC module.
	
	Parameters:
	None
	
	Return:
	None
	*/
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN6_OUTPUT_MODE_2MHZ | GPIO_PIN7_OUTPUT_MODE_2MHZ),
								 (GPIO_PIN6_ALT_FUNC_OPEN_DRAIN | GPIO_PIN7_ALT_FUNC_OPEN_DRAIN));
	
	I2C_Init(I2C1,
					 I2C_PERIPH_FREQ_8MHZ,
					 I2C_STANDARD_MODE_8MHZ_CCR, 
					 I2C_STANDARD_MODE_8MHZ_TRISE);
}

void DS3231_GetTime(ds3231_t* pTime)
{
	
	uint8_t timeBCD[3]; //sec,min,hour
	uint8_t periodOfDay;
	
	I2C_ReadMultiByte(I2C1,DS3231_ADDR,SEC_REG_ADDR,timeBCD,3);
	
	if ((timeBCD[2] & (1<<6)) == (1<<6))
	{
		/*
		1.)read AM/PM status of 12 hour clock
		2.)if 12 hour format is the current clock format, 
		read only bits 4-0 of ds3231 hour register*/
		periodOfDay = ((timeBCD[2] & (1<<5)) >> 5);
		timeBCD[2] = (timeBCD[2] & 0x1F);
	}
	else
	{
		/*
		1.)no AM/PM status for 24 hour clock
		2.)if 24 hour format is the current clock format, 
		read only bits 5-0 of ds3231 hour register*/
		periodOfDay = DS3231_NO_PERIOD;
		timeBCD[2] = (timeBCD[2] & 0x3F); 
	}
	
	pTime->seconds = BCDToHex(timeBCD[0]);
	pTime->minutes = BCDToHex(timeBCD[1]);
	pTime->hours = BCDToHex(timeBCD[2]);
	pTime->period = periodOfDay;
}

void DS3231_SetTime(uint8_t hour, uint8_t min)
{
	uint8_t prevHoursBCD;
	uint8_t timeBCD[3] = {0,0,0}; //sec,min,hour
	
	timeBCD[1] = HexToBCD(min);
	timeBCD[2] = HexToBCD(hour);
	
	I2C_ReadByte(I2C1, DS3231_ADDR, HOUR_REG_ADDR, &prevHoursBCD);
	/* 0xE0 preserves settings of the ds3231 hour register
	so that a write to the register doesn't clear the hour configurations.
	*/
	timeBCD[2] = (timeBCD[2] | (prevHoursBCD & 0xE0));
	
	I2C_WriteMultiByte(I2C1,DS3231_ADDR,SEC_REG_ADDR,timeBCD,3);
}

void DS3231_12HourFormat(uint8_t periodOfDay)
{
	uint8_t hoursBCD;
	
	I2C_ReadByte(I2C1, DS3231_ADDR, HOUR_REG_ADDR, &hoursBCD);
	if (periodOfDay == DS3231_PERIOD_PM)
	{
		hoursBCD |= ((1<<6) | (1<<5));
	}
	else
	{
		hoursBCD &= ~(1<<5);
		hoursBCD |= (1<<6);
	}
	I2C_WriteByte(I2C1, DS3231_ADDR, HOUR_REG_ADDR, hoursBCD);
}

void DS3231_24HourFormat(void)
{
	uint8_t hoursBCD;
	
	I2C_ReadByte(I2C1, DS3231_ADDR, HOUR_REG_ADDR, &hoursBCD);
	hoursBCD &= ~((1<<6) | (1<<5));
	I2C_WriteByte(I2C1, DS3231_ADDR, HOUR_REG_ADDR, hoursBCD);
}

void DS3231_SetAlarm2(uint8_t min)
{
	/*
	Description:
	Sets the minute match in which an alarm is generated. e.g.
	if min = 40, an alarm will be generated at 9:40, 10:40, 2:40 .....
	etc. regardless of AM/PM and 12/24-hour format.
	
	Parameters:
	1.) min: minutes (in binary or hex or decimal) which when matched
	by the 'Alarm 2 Minutes register' results in an alarm being generated.
	
	Return:
	None
	*/

	uint8_t alarm2RegArrBCD[3]; //alarm2 min,hour,day/date
	uint8_t ctrlRegBCD = ((1<<2)|(1<<1)); //set INTCN and A2IE bits
	
	alarm2RegArrBCD[0] = (HexToBCD(min) & 0x7F); //convert 'min' to BCD and clear A2M2
	alarm2RegArrBCD[1] = (1<<7); //set A2M3
	alarm2RegArrBCD[2] = (1<<7); //set A2M4
	
	I2C_WriteMultiByte(I2C1, DS3231_ADDR, ALARM2_MIN_REG_ADDR, alarm2RegArrBCD, 3);
	I2C_WriteByte(I2C1, DS3231_ADDR, CONTROL_REG_ADDR, ctrlRegBCD);
	
}

bool DS3231_GetAlarm2_Status(void)
{
	/*
	Description:
	Detects whether an alarm (due to the matching of minutes between
	the alarm2 minutes register and RTC minutes register) 
	is/was triggered. 
	
	Parameters:
	None
	
	Return:
	1.) true: if the alarm is/was triggered.
	2.) false: if the specified alarm hasn't been triggered.
	
	*/
	bool alarm2Flag = false;
	uint8_t statusRegBCD;
	
	I2C_ReadByte(I2C1,DS3231_ADDR,STATUS_REG_ADDR,&statusRegBCD);
	alarm2Flag = (statusRegBCD & (1<<1)) >> 1; //get logic state of A2F bit
	
	return alarm2Flag;
}

void DS3231_ResetAlarm2(void)
{
	/*
	Description:
	Clears alarm2 flag bit (required if the alarm is expected to be
	periodic). (See DS3231 datasheet)
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	uint8_t statusRegBCD;
	
	I2C_ReadByte(I2C1,DS3231_ADDR,STATUS_REG_ADDR,&statusRegBCD);
	statusRegBCD &= ~(1<<1); //clear A2F bit
	I2C_WriteByte(I2C1,DS3231_ADDR,STATUS_REG_ADDR,statusRegBCD);
}

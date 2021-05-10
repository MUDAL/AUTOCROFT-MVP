#ifndef I2C_H
#define I2C_H

#define I2C_PERIPH_FREQ_8MHZ						I2C_CR2_FREQ_3
#define I2C_STANDARD_MODE_8MHZ_CCR			40 //see datasheet for calculation
#define I2C_STANDARD_MODE_8MHZ_TRISE		9	 //see datasheet for calculation


extern void I2C_Init(I2C_TypeDef* i2cPort, 
										 uint32_t i2cClock, 
										 uint32_t i2cCCR, 
										 uint32_t i2cTRISE);

extern void I2C_Write(I2C_TypeDef* i2cPort, 
											uint8_t slaveAddr,
											uint8_t regAddr,
											uint8_t data);

extern void I2C_Write_Multiple(I2C_TypeDef* i2cPort, 
															 uint8_t slaveAddr,
															 uint8_t regAddr,
															 uint8_t* data,
															 uint32_t length);

extern void I2C_Read_1Byte(I2C_TypeDef* i2cPort,
													 uint8_t slaveAddr,
													 uint8_t regAddr, 
													 uint8_t* data); 

extern void I2C_Read_2Bytes(I2C_TypeDef* i2cPort, 
														uint8_t slaveAddr,
														uint8_t regAddr,
														uint8_t* data);

extern void I2C_Read_Multiple(I2C_TypeDef* i2cPort,
															uint8_t slaveAddr,
															uint8_t regAddr,
															uint8_t* data,
															uint32_t length); 


#endif //I2C_H

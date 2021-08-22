#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "wifi.h"

void WiFi_Init(void)
{
	//GPIO configuration for USART2 Tx
	GPIO_OutputInit(GPIOA,
									GPIO_PORT_REG_LOW,
									GPIO_PIN2_OUTPUT_MODE_2MHZ,
									GPIO_PIN2_ALT_FUNC_PUSH_PULL);
	//GPIO configuration for USART2 Rx
	GPIO_InputInit(GPIOA,
								 GPIO_PORT_REG_LOW,
								 GPIO_PIN3,
								 GPIO_PIN3_INPUT_PULLUP_OR_PULLDOWN,
								 GPIO_PULLUP_ENABLE);
	//USART2 configuration
	USART_Init(USART2,
						 BAUD_115200,
						 TX_RX_DMA_DISABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}

void WiFi_TransmitBytes(uint8_t* bytes, uint8_t len)
{
	USART_TransmitBytes(USART2,bytes,len);
}

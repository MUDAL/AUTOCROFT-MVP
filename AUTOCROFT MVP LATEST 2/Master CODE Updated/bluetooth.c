#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "bluetooth.h"

void Bluetooth_Init(void)
{
	//GPIO configuration for USART3 Tx
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN10_OUTPUT_MODE_2MHZ,
									GPIO_PIN10_ALT_FUNC_PUSH_PULL);
	//GPIO configuration for USART3 Rx
	GPIO_InputInit(GPIOB,
								 GPIO_PORT_REG_HIGH,
								 GPIO_PIN11,
								 GPIO_PIN11_INPUT_PULLUP_OR_PULLDOWN,
								 GPIO_PULLUP_ENABLE);
	//USART3 configuration
	USART_Init(USART3,
						 BAUD_9600,
						 RX_DMA_ENABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}

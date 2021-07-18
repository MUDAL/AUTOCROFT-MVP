#ifndef _ADC_H
#define _ADC_H

#define ADC_CHANNEL5										5
#define ADC_CH5_MAX_SAMPLE_TIME					ADC_SMPR2_SMP5
#define ADC_CHANNEL6										6
#define ADC_CH6_MAX_SAMPLE_TIME					ADC_SMPR2_SMP6
#define ADC_CHANNEL8										8
#define ADC_CH8_MAX_SAMPLE_TIME					ADC_SMPR2_SMP8

extern void ADC_SingleConv_Init(ADC_TypeDef* adcPort,
																uint8_t channel,
																uint32_t sampleTimeReg);
extern uint16_t ADC_Read(ADC_TypeDef* adcPort);

#endif //_ADC_H

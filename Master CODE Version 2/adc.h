#ifndef _ADC_H
#define _ADC_H

#define ADC_CHANNEL5										5
#define ADC_CHANNEL5_MAX_SAMPLE_TIME		ADC_SMPR2_SMP5

#define ADC_CHANNEL6										6
#define ADC_CHANNEL6_MAX_SAMPLE_TIME		ADC_SMPR2_SMP6

#define ADC_CHANNEL8										8
#define ADC_CHANNEL8_MAX_SAMPLE_TIME		ADC_SMPR2_SMP8

extern void ADC_Single_Conv_Init(ADC_TypeDef* adcPort, uint8_t channel, uint32_t sampleTime);
extern uint16_t ADC_Read(ADC_TypeDef* adcPort);

#endif //_ADC_H

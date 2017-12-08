/*
 * config_exti.c
 *
 *  Created on: 2017. 12. 4.
 *      Author: hw_3
 */

#include "config_exti.h"

void m_EXTI_GPIO_Interrupt(uint8_t GPIO_PortSourceGPIOx, uint8_t GPIO_PinSourcex, uint32_t EXTI_Linex, int EXTIx_IRQn){
	EXTI_InitTypeDef exti_;
	NVIC_InitTypeDef nvic_;
	exti_.EXTI_Line = EXTI_Linex;
	exti_.EXTI_LineCmd = ENABLE;
	exti_.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti_);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOx, GPIO_PinSourcex);

	nvic_.NVIC_IRQChannel = EXTIx_IRQn;
	nvic_.NVIC_IRQChannelCmd = ENABLE;
	nvic_.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic_);
}
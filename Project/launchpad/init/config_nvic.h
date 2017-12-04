/*
 * config_interrupt.h
 *
 *  Created on: 2017. 11. 24.
 *      Author: hw_3
 */

#ifndef CONFIG_INTERRUPT_H_
#define CONFIG_INTERRUPT_H_

#include <stm32f10x.h>
#include <misc.h>

/*
 * Priority
 * USART2 : 0/0
 * USART3 : 0/0
 *
 *
 */

//Initialize nested vectored interrupt controller configuration for USART2, bluetooth
void m_Init_USART2_NVIC(void);
//Initialize nested vectored interrupt controller configuration for USART3, bluetooth
void m_Init_USART1_NVIC(void);
//Initialize nested vectored interrupt controller configuration for I2C1, MPR121
void m_Init_MPR121_NVIC(void);

#endif /* CONFIG_INTERRUPT_H_ */

#ifndef CONFIG_MPR121_H_
#define CONFIG_MPR121_H_

#include <Adafruit_MPR121.h>
#include <stm32f10x.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_gpio.h>

void m_Init_MPR121(void);
void m_Init_MPR121_I2C1(void);
void m_Init_MPR121_GPIOB(void);
void m_Init_MPR121_I2C(void);
void m_printState(void);

#endif /* CONFIG_MPR121_H_ */

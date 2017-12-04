/*
 * main.c
 *
 *  Created on: 2017. 11. 24.
 *      Author: hw_3
 */

/*
 * flash setting
 *
flash load "./launchpad/flashclear.axf"

flash load "./launchpad/Debug/launchpad.axf"
 */
#include "bluetooth/config_bluetooth.h"
#include "init/config_system.h"
#include "init/config_nvic.h"
#include "init/config_exti.h"
#include <lcd.h>
#include <touch.h>
#include "core_cm3.h"



#define CONST_CHRACTER_ROW 2
#define CONST_CHRACTER_COL 2
#define CONST_MUSIC_NOTE_ROW 24

int Xposition;
int Yposition;
uint32_t DestAddr;

int EXTI0_flag;
int EXTI1_flag;
int EXTI2_flag;
int EXTI3_flag;
int EXTI4_flag;
int EXTI5_flag;


//uint16_t MUSIC_NOTE[CONST_MUSIC_NOTE_ROW] ={
//	0b0000000000000000,
//	0b0001111111111110,
//	0b0001111111111110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001111111111110,
//	0b0001111111111110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000000110,
//	0b0001100000001110,
//	0b0001100000011111,
//	0b0011100000111111,
//	0b0111110000111111,
//	0b1111110000011110,
//	0b1111110000001100,
//	0b0111100000000000,
//	0b0011000000000000
//};

void printObject(int** data, int start_x, int start_y, int data_row, int data_col){
	//for()
}

void InterruptEnable(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//Interrupt
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

//void Show_LCD_Status(){
//		for(Xposition=1;Xposition<=240;Xposition++){
//				for(Yposition=1;Yposition<=320;Yposition++){
//					LCD_DrawPoint(Xposition,Yposition);
//		}
//	}
//}


void EXTI4_IRQHandler(void){ // draw music status
	if(EXTI_GetITStatus(EXTI_Line4) != RESET){
		EXTI4_flag =1;
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}


void EXTI9_5_IRQHandler(void){ //volume up status
	if(EXTI_GetITStatus(EXTI_Line5) != RESET){
		EXTI5_flag =1;
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

void EXTI2_IRQHandler(void){ //volume down status
	if(EXTI_GetITStatus(EXTI_Line2) != RESET){
		EXTI2_flag =1;
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}


//void USART2_IRQHandler(void){
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
//		uint16_t data = USART_ReceiveData(USART2);
//		m_USART_byteSend(USART1, data);
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	}
//}
//
//void USART1_IRQHandler(void){
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
//		uint16_t data = USART_ReceiveData(USART1);
//		//m_USART_DataSend(USART1, data);
//		m_USART_byteSend(USART2, data);
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//	}
//}

void Configure_LED() {
   GPIO_InitTypeDef GPIO_init;

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

   GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_init.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7);
   GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;

   GPIO_Init(GPIOD, &GPIO_init);
}



int main(){
	/*char buffer_[BT_STR_MAX_LENGTH];
	//System Configuration Initialization
	SystemInit();
	m_System_Clock();


	//GPIOx
	m_Init_GPIOA_Clock();

	//USARTx
	m_Init_USART2_Clock();
	m_Init_USART1_Clock();

	//Interrupt Configuration - EXTI
	m_Init_BT_USART2_EXIT();
	m_Init_BT_USART1_EXIT();

	//Initialize USART1/2 GPIOA
	m_Init_BT_USART1_GPIOA();
	m_Init_BT_USART2_GPIOA();
	//MCO Port Init
	m_Init_MCO_GPIOA();

	//Initialize USART1/2
	m_Init_BT_USART1();
	m_Init_BT_USART2();

	//Interrupt Configuration - NVIC
	m_Init_USART2_NVIC();
	m_Init_USART1_NVIC();
	m_USART_DataSend(USART2, "AT+BTSCAN", buffer_);

	m_MCO_Enable();*/

//	GPIOD->BSRR = GPIO_BSRR_BS2;
//	GPIOD->BSRR = GPIO_BSRR_BS3;
//	GPIOD->BSRR = GPIO_BSRR_BS4;
//	GPIOD->BSRR = GPIO_BSRR_BS7;
	SystemInit();
	Configure_LED();
	LCD_Init();
	m_Init_AFIO_Clock();
	m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOC, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn);
	m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOC, GPIO_PinSource5, EXTI_Line5, EXTI9_5_IRQn); // volume up
	m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOC, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn); // volume down
	//Main Loop
	while(1){

		if(EXTI4_flag){
			//Show_LCD_Status();
			EXTI4_flag = 0;
		}
		if(EXTI5_flag) {
			LCD_Clear(WHITE);
			LCD_ShowNum(100, 100, DestAddr++, 4, BLACK, WHITE);
			EXTI5_flag = 0;
		}
		if(EXTI2_flag) {
			LCD_Clear(WHITE);
			LCD_ShowNum(100, 100, DestAddr--, 4, BLACK, WHITE);
			EXTI2_flag = 0;
		}
	}
}

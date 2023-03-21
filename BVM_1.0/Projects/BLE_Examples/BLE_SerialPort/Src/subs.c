//======================================================
// Header: subroutines source file
// File Name: subs.c
// Author: June Ra
// Date: 2022. 3. 3
// Device name : BlueNRG-LPM
//======================================================

#include "subs.h"
#include "clock.h"

void KENDOL2_HAL_Init(void)
{
	BSP_PB_Init(BSP_PUSH1,BUTTON_MODE_GPIO);			//june
	BSP_LED_Init(BSP_LED1);
}

/*
void GPIOB_IRQHandler(void)
{  
  if(BSP_PB_GetITPendingBit(BSP_PUSH1) == SET) 
  {
    BSP_PB_ClearITPendingBit(BSP_PUSH1);
    button1_pressed = TRUE;
  }  
}


void GPIOA_IRQHandler(void)
{  
  if(BSP_PB_GetITPendingBit(BSP_PUSH2) == SET) 
  {
    BSP_PB_ClearITPendingBit(BSP_PUSH2);
    button1_pressed = TRUE;
  }  
}
*/

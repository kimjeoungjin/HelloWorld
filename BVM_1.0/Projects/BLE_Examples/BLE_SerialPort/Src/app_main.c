//======================================================
// Header: Mirror Project source file
// File Name: app_main.c
// Author: June Ra
// Date: 2022. 3. 3
// Device name : BlueNRG-LPM
//======================================================

#include "bluenrg_lp_evb_com.h"
#include "rf_driver_ll_gpio.h"
#include "TIM_PWMOutput_main.h"
#include "subs.h"

extern unsigned char	TimeOver_flag;

uint8_t		mainmode = 0;
uint32_t	keycount = 0;
uint8_t		key_on_flag = 0;
uint8_t		led_on_flag = 0, led_toggle_flag = 0;
uint32_t	led_counter = 0;

uint8_t		from_app[20] = {0,};
uint8_t		active_command[5] = {0,};
uint8_t		command_received = 0;
uint8_t		data_wrong_flag = 0;

uint8_t		app_on_flag = 0;

void	KENDOL2_main(void)
{
	if(TimeOver_flag == 1)
	{
		TimeOver_flag = 0;					//june
		if(mainmode == 0)					// key check
		{
			mainmode = 1;
			if(LL_GPIO_IsInputPinSet(BSP_PUSH1_GPIO_PORT, BSP_PUSH1_PIN) == 1)		// key HIGH
			{
				if(key_on_flag == 1)
				{
					if(keycount < KEY_CHATTERING)							//keycount < 20ms
					{
						keycount = 0;
						key_on_flag = 0;
					}
					else if(keycount < LONGKEY_INTERVAL)					//short key
					{
						keycount = 0;
						key_on_flag = 0;
						
						if(led_toggle_flag == 1)
						{
							led_toggle_flag = 0;
							led_on_flag = 0;
							BSP_LED_Off(BSP_LED1);
						}
						else
						{
							if(led_on_flag == 1)
							{
								led_on_flag = 0;
								BSP_LED_Off(BSP_LED1);
							}
							else
							{
								led_on_flag = 1;
								BSP_LED_On(BSP_LED1);
							}
						}
					}
					else													//long key
					{
						keycount = 0;
						key_on_flag = 0;
					}
				}
			}
			else			// key LOW
			{
				if(key_on_flag == 0)
				{
					keycount = 0;
					key_on_flag = 1;
				}
				else
				{
					if(++keycount == LONGKEY_INTERVAL)
						
						if(led_toggle_flag == 0)
						{
							led_toggle_flag = 1;
							led_on_flag = 1;
							led_counter = 0;
							BSP_LED_On(BSP_LED1);
						}
						
				}
			}
			
			if(command_received == 1)							//june 220719
			{
				command_received = 0;
				data_wrong_flag = 0;
				
				for(uint8_t i = 0 ; i < 5 ; i++)
				{
					if(from_app[i] == from_app[i+5])
					{
						active_command[i] = from_app[i];
						continue;
					}
					else if(from_app[i] == from_app[i+10])
					{
						active_command[i] = from_app[i];
						continue;
					}
					else if(from_app[i+5] == from_app[i+10])
					{
						active_command[i] = from_app[i+5];
						continue;
					}
					else
					{
						data_wrong_flag = 1;
						break;
					}
				}
				
				if(data_wrong_flag == 0)
				{
					if(active_command[2] == 0)
					{
						if(led_toggle_flag == 0)
						{
							led_toggle_flag = 1;
							led_on_flag = 1;
							led_counter = 0;
							BSP_LED_On(BSP_LED1);
						}
					}
					else if(active_command[2] == 50)
					{
						led_toggle_flag = 0;
						led_on_flag = 0;
						BSP_LED_Off(BSP_LED1);
					}
					else if(active_command[2] == 100)
					{
						led_toggle_flag = 0;
						led_on_flag = 1;
						BSP_LED_On(BSP_LED1);
					}
				}
			}
		}
		else				//mainmode == 1, led toggle
		{
			mainmode = 0;
			if(led_toggle_flag == 1)
			{
				if(++led_counter >= LED_TOGGLE_TIME)
				{
					led_counter = 0;
					if(led_on_flag == 1)
					{
						led_on_flag = 0;
						BSP_LED_Off(BSP_LED1);
					}
					else
					{
						led_on_flag = 1;
						BSP_LED_On(BSP_LED1);
					}
				}
			}
		}
	}
}



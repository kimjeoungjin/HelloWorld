#include <stdio.h>
#include <string.h>
#include "bluenrg_lp_it.h"
#include "ble_const.h"
#include "bluenrg_lp_stack.h"
#include "app_state.h"
#include "serial_port.h"
#include "SerialPort_config.h"
#include "OTA_btl.h" 
#include "rf_driver_hal_vtimer.h"
#include "bluenrg_lp_evb_com.h"
#include "clock.h"
#include "bleplat.h"
#include "nvm_db.h"
#include "pka_manager.h"
#include "rng_manager.h"
#include "aes_manager.h"
#include "ble_controller.h"
#include "rf_driver_hal.h"
#include "bluenrg_lp_evb_config.h"
#include "rf_driver_hal_adc.h"
#include "rf_driver_hal_def.h"
#include "rf_driver_ll_tim.h"
#include "rf_driver_ll_lpuart.h"

#define USER_SAMPLERATE       (ADC_SAMPLE_RATE_28)

#define USER_DATAWIDTH        (ADC_DS_DATA_WIDTH_16_BIT)
#define USER_RATIO                  (ADC_DS_RATIO_128)

#define LED0 	LL_GPIO_PIN_1
#define LED1 	LL_GPIO_PIN_7
#define LED2 	LL_GPIO_PIN_6
#define LED3 	LL_GPIO_PIN_5
#define LED4 	LL_GPIO_PIN_4
#define LED5 	LL_GPIO_PIN_15

#define BVM_LED_ON(port,pin)		 LL_GPIO_ResetOutputPin(port, pin)
#define BVM_LED_OFF(port,pin) 		 LL_GPIO_SetOutputPin(port, pin)	

#define BAT_LEVEL_0		29700
#define BAT_LEVEL_20		31300
#define BAT_LEVEL_40		33000	
#define BAT_LEVEL_60		34900	
#define BAT_LEVEL_80		35800
#define BAT_LEVEL_100	36500

/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
#define PRESCALER_VALUE (uint32_t)((LL_TIM_GetPeriphClock(TIM1) / 1000000) - 1)
#define PERIOD_VALUE (10000 - 1);


#define TIMx TIM1
#define LL_EnableClock(LL_PERIPH_TIMx)              LL_APB0_EnableClock(LL_APB0_PERIPH_TIM1);
#define TIMx_IRQn                                   TIM1_IRQn
#define TIMx_IRQHandler                             TIM1_IRQHandler


#define LPUART_TX_PORT 						GPIOB
#define LPUART_TX_PIN						LL_GPIO_PIN_6
#define LPUART_TX_AF						LL_GPIO_AF_3
#define LL_GPIO_SetAFPin_LPUART_TX()                            LL_GPIO_SetAFPin_0_7(LPUART_TX_PORT, LPUART_TX_PIN, LPUART_TX_AF)
#define LPUART_RX_PORT 						GPIOB
#define LPUART_RX_PIN						LL_GPIO_PIN_7
#define LPUART_RX_AF						LL_GPIO_AF_3
#define LL_GPIO_SetAFPin_LPUART_RX()                            LL_GPIO_SetAFPin_0_7(LPUART_RX_PORT, LPUART_RX_PIN, LPUART_RX_AF)
#define LL_EnableClock_LPUART()                                	LL_APB1_EnableClock(LL_APB1_PERIPH_LPUART)
#define LL_EnableClock_LPUART_TX_PORT()                         LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB)
#define LL_EnableClock_LPUART_RX_PORT()                         LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB)


void Gpio_Configuration( void );
void ALL_LED_OFF ( void );
void ALL_LED_ON ( void );
void BAT_Cheak ( uint32_t bat_adc );
void Adc_Configuration (void);
void MX_TIMx_Init(void);
void LPUART1_Configure(void);



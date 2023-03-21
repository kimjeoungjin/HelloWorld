/******************** (C) COPYRIGHT 2020 STMicroelectronics ********************
* File Name          : BLE_SerialPort_main.c
* Author             : RF Application Team
* Version            : V3.0.0
* Date               : 23-January-2010
* Description        : BlueNRG-LP main file for Serial Port demo (previously  
*                      known as Serial Port demo)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRGLP_demonstrations_applications
 * BlueNRG-LP Serial Port demo \see BLE_SerialPort_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */
/* Includes ------------------------------------------------------------------*/

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
#include "rf_driver_ll_adc.h"
#include "rf_driver_ll_utils.h"

#include "configuration.h"
#include "uart.h"
#include "dsp.h"
#include "gatt_db.h"



NO_INIT(uint32_t dyn_alloc_a[DYNAMIC_MEMORY_SIZE>>2]);
uint8_t	radio_state_error_count = 0;	

uint32_t nTemperatureRawVal = 0;
volatile float nVBattRawVal = 0;
volatile uint16_t nVSensorRawVal = 0;
float adcValue = 0.0;

uint32_t ReceivedData = 0;


BOOL bat_Flag = FALSE;
BOOL sensor_Flag = FALSE;
BOOL Rx_Flag = FALSE;
BOOL flag1 = 0 , flag2 = 1;
BOOL Connection_Flag = FALSE;

void ModulesInit(void)
{
	uint8_t ret;
	BLE_STACK_InitTypeDef BLE_STACK_InitParams = BLE_STACK_INIT_PARAMETERS;

	LL_AHB_EnableClock(LL_AHB_PERIPH_PKA|LL_AHB_PERIPH_RNG);

	ret = BLE_STACK_Init(&BLE_STACK_InitParams);
	if (ret != BLE_STATUS_SUCCESS) 
	{
		while(1);
	}

	BLECNTR_InitGlobal();

	HAL_VTIMER_InitType VTIMER_InitStruct = {HS_STARTUP_TIME, INITIAL_CALIBRATION, CALIBRATION_INTERVAL};
	HAL_VTIMER_Init(&VTIMER_InitStruct);

	BLEPLAT_Init();  

	if (PKAMGR_Init() == PKAMGR_ERROR)
	{
	  while(1);
	}
	if (RNGMGR_Init() != RNGMGR_SUCCESS)
	{
	  while(1);
	}

	AESMGR_Init();
}

void ModulesTick(void)
{
	HAL_VTIMER_Tick();

	BLE_STACK_Tick();

	NVMDB_Tick();
}

//4.2 4800 5100
//3.7 4800 5200
//3.0 4300 



int main(void) 
{
	uint8_t ret;

  	if (SystemInit(SYSCLK_64M, BLE_SYSCLK_32M) != SUCCESS) 
	{
		while(1);
	}
	
	BSP_IO_Init();

	Clock_Init();

	ModulesInit(); 
  
	Gpio_Configuration( );
	
//	MX_TIMx_Init( );
	
	ret = Serial_port_DeviceInit();
	if (ret != BLE_STATUS_SUCCESS) 
	{
		while(1);
	}

	while(1) 
	{
		ModulesTick();

		APP_Tick();
//		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
	}
		
		
}

void hci_hardware_error_event(uint8_t Hardware_Code)

{
	if (Hardware_Code <= 0x03)
	{
		if(Hardware_Code == 1)
		{						
			Hardware_Code = 0;	
			radio_state_error_count++;				
		}											
		else										
		{
			NVIC_SystemReset();
		}
	}
}

void aci_hal_fw_error_event(uint8_t FW_Error_Type, uint8_t Data_Length, uint8_t Data[])
{
	uint16_t connHandle;
	
	if (FW_Error_Type <= 0x03)
	{
		connHandle = LE_TO_HOST_16(Data);

		aci_gap_terminate(connHandle, BLE_ERROR_TERMINATED_REMOTE_USER); 
	}
}

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
		
	}
}







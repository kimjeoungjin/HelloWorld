
/* Includes ------------------------------------------------------------------*/
#include "bluenrg_lp_it.h"
#include "ble_const.h"
#include "bluenrg_lp_stack.h"
#include "bluenrg_lp_evb_com.h"
#include "clock.h"
#include "hal_miscutil.h"
#include "crash_handler.h"
#include "rf_driver_hal_vtimer.h"
#include "gatt_db.h"
#include "configuration.h"
#include "key.h"
#include "uart.h"


#define BAT_MODE 	10000
#define SENSOR_MODE 	100

extern uint32_t nVBattRawVal[ 2 ];

NOSTACK_FUNCTION(NORETURN_FUNCTION(void NMI_IRQHandler(void)))
{
	HAL_CrashHandler(__get_MSP(), NMI_SIGNATURE);  

	while (1)
	{
	
	}
}

NOSTACK_FUNCTION(NORETURN_FUNCTION(void HardFault_IRQHandler(void)))
{
	HAL_CrashHandler(__get_MSP(), HARD_FAULT_SIGNATURE);  

	while (1)
	{

	}
}

void SVC_IRQHandler(void)
{
	
}


uint16_t Cnt_Data = 0;
uint8_t data1 = 0;
uint8_t data2 = 255;

uint16_t cnt_10sec = 0;
uint16_t hz_250 = 0;
extern BOOL bat_Flag;
extern BOOL sensor_Flag;


void SysTick_IRQHandler(void)
{
	SysCount_Handler( );
	
	
//	if( ++Cnt_Data == 500 )
//	{
//		Cnt_Data = 0;
//		Breath_Update( nVBattRawVal[1] );
//		Bat_Update( nVBattRawVal[0] );
//	}

	


}

void TIMx_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIMx) == 1)
	{
//		PowerKeyScan( );
		
		if ( ++hz_250 == 4 )
		{
			
		hz_250 = 0;
		LL_ADC_StartConversion(ADC);
			
		}
		
		cnt_10sec++;
		
//		if ( Cnt_Data++ == 1000 ) 
//		{
//			Cnt_Data = 0;
//			LL_LPUART_TransmitData8 ( LPUART1 , 0x35 );
//		}
		
		if ( cnt_10sec == SENSOR_MODE )
		{
			sensor_Flag = TRUE;
		}
		
		if ( cnt_10sec ==  BAT_MODE )
		{
			cnt_10sec = 0;
			bat_Flag = TRUE;
		}
		
		LL_TIM_ClearFlag_UPDATE(TIMx);
	} 
}

void ADC_IRQHandler( void )
{
	
}

extern uint16_t Head_R;
extern uint16_t Tail_R;
extern uint8_t RingBuf_Receive_Data[ RingBuf_Size ];

extern BOOL Rx_Flag;

void LPUART1_IRQHandler(void)
{  
	uint16_t tmphead;
	uint8_t data;
	
	/* Check RXNE_RXFNE flag value in ISR register */
	if(LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1) && LL_LPUART_IsEnabledIT_RXNE(LPUART1))
	{
	  
		/* Call function in charge of handling Character reception */
		data = LL_LPUART_ReceiveData8(LPUART1);
		tmphead =  (Head_R+1) & RING_BUFF_MASK ; 
		
		if (  tmphead != Tail_R  )					 // Comparison of tmphead and RxHead.
		{
			Head_R = tmphead;        					// Store new index
			RingBuf_Receive_Data [ tmphead ] = data ;  	// Store received data(RingBuf_Receive_Data) in buffer //
		}	
		
		Rx_Flag = TRUE;
	}
}

void DMA_IRQHandler(void)
{
  
}

void BLE_WKUP_IRQHandler(void)
{
  HAL_VTIMER_WakeUpCallback();
}

void CPU_WKUP_IRQHandler(void) 
{
  HAL_VTIMER_TimeoutCallback();
}

void BLE_ERROR_IRQHandler(void)
{
	volatile uint32_t debug_cmd;

	BLUE->DEBUGCMDREG |= 1;

	/* If the device is configured with 
	 System clock = 64 MHz and BLE clock = 16 MHz
	 a register read is necessary to end fine  
	 the clear interrupt register operation,
	 due the AHB down converter latency */ 
	debug_cmd = BLUE->DEBUGCMDREG;
}

void BLE_TX_RX_IRQHandler(void)
{
	uint32_t blue_status = BLUE->STATUSREG;
	uint32_t blue_interrupt = BLUE->INTERRUPT1REG;

	/** clear all pending interrupts */
	BLUE->INTERRUPT1REG = blue_interrupt;

	BLE_STACK_RadioHandler(blue_status|blue_interrupt);
	HAL_VTIMER_RadioTimerIsr();

	/* If the device is configured with 
	 System clock = 64 MHz and BLE clock = 16 MHz
	 a register read is necessary to end fine  
	 the clear interrupt register operation,
	 due the AHB down converter latency */ 
	blue_interrupt = BLUE->INTERRUPT1REG;
}




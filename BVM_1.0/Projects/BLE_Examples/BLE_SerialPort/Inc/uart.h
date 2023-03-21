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

#define RingBuf_Size 			256	// Must be 2^x �ڡڡڡڡڡ�
#define RING_BUFF_MASK		(RingBuf_Size - 1) 


uint8_t USART2_Receive( void );
	



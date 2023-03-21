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



float IirFilter(float* input, float* output, const float* ma, const float* ar, uint8_t fsize);
void ADC_DataProcess( float adc_data );



	
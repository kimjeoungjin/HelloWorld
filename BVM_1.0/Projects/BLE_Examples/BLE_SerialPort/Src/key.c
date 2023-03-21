#include "key.h"
#include "configuration.h"

#define POWER_KEY_RPT_START_DLY		CNT3SEC


enum PowerCase{ POWER_KEY_STATE_UP, POWER_KEY_STATE_UP_DEBOUNCE, POWER_KEY_STATE_RPT_START_DLY, POWER_KEY_STATE_RPT_DLY, POWER_KEY_STATE_DOWN_DEBOUNCE, POWER_KEY_STATE_DOWN };
uint8_t Power_KeyScanState = POWER_KEY_STATE_UP;
uint16_t	Power_KeyRptStartDlyCtr;


BOOL  Power_KeyIsKeyDown (void)
{
	if( LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_9) )  return TRUE;		// POWER_SIG pin
	else	return FALSE;
}


void  PowerKeyScan (void)
{
	switch (Power_KeyScanState) 
	{
		case POWER_KEY_STATE_UP:                            
			if (Power_KeyIsKeyDown()) 	Power_KeyScanState = POWER_KEY_STATE_UP_DEBOUNCE;
			break;

		case POWER_KEY_STATE_UP_DEBOUNCE:
			if (Power_KeyIsKeyDown()) 
			{
				Power_KeyRptStartDlyCtr = POWER_KEY_RPT_START_DLY;
				Power_KeyScanState      = POWER_KEY_STATE_RPT_START_DLY;

			} 
			else 
			{
				Power_KeyScanState  = POWER_KEY_STATE_UP;
			}
			break;

		case POWER_KEY_STATE_RPT_START_DLY:
			if (Power_KeyIsKeyDown()) 
			{
				if (Power_KeyRptStartDlyCtr-- == 1) 
				{
					// Power Off
					LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);		//Power EN
					while(1)
					{
						ALL_LED_OFF();
					}
				 }
			}
			else 
			{
				Power_KeyScanState = POWER_KEY_STATE_DOWN;
			}
			break;
			
		case POWER_KEY_STATE_DOWN:
			if (!Power_KeyIsKeyDown()) 		Power_KeyScanState = POWER_KEY_STATE_DOWN_DEBOUNCE;
			else						Power_KeyScanState = POWER_KEY_STATE_RPT_START_DLY;
			break;
		
		case POWER_KEY_STATE_DOWN_DEBOUNCE:
			if (!Power_KeyIsKeyDown()) 
			{
				Power_KeyScanState = POWER_KEY_STATE_UP;
			} 
			else 
			{
				Power_KeyScanState = POWER_KEY_STATE_DOWN;
			}
			break;
			
		default : 
			break;
	}
}

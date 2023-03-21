#include "uart.h"
#include "configuration.h"

uint16_t Head_R = 0;
uint16_t Tail_R = 0;

uint8_t RingBuf_Receive_Data[ RingBuf_Size ];


uint8_t USART2_Receive( void )
{	
	uint16_t tmptail;
	
	if( Head_R != Tail_R  )  			// If the receive buffer is empty, the executed.
	{		
		 tmptail = (Tail_R + 1) & RING_BUFF_MASK ;      // Calculate buffer index 
		 Tail_R = tmptail ;      					// Store new index 
		
		return  RingBuf_Receive_Data [ tmptail ]; 		 // Return data ( RingBuf_Receive_Data ) 
	} 
	else 
	{
		return 0;
	}
}



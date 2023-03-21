//======================================================
// Header: subroutine header file
// File Name: subs.h
// Author: June Ra
// Date: 2022. 3. 3
// Device name : BlueNRG-LPM
//======================================================

#include "steval_idb011V1_config.h"

#if KENDOL2 == KENDOL2_MIRROR

void	led_on(unsigned int led1_step, unsigned int led2_step);
void	KENDOL2_main(void);
void	KENDOL2_HAL_Init(void);
void	KENDOL2_Option_Check(void);

#elif KENDOL2 == KENDOL2_STANDARD

void	KENDOL2_HAL_Init(void);
void	KENDOL2_main(void);

#endif




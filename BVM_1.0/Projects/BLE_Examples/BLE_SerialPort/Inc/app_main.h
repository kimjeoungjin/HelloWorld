//======================================================
// Header: application main header file
// File Name: app_main.h
// Author: June Ra
// Date: 2022. 3. 3
// Device name : BlueNRG-LPM
//======================================================

/** PROJECT OPTION FOR BLUENRG-LPM ********************************************/	

#define	NO_KENDOL2		0
#define KENDOL2_MIRROR	1
#define	KENDOL2_HEALTH	2
#define KENDOL2_STANDARD 99

#define KENDOL2 KENDOL2_STANDARD

/******************************************************************************/

#if KENDOL2 == KENDOL2_MIRROR

#define		UNIT_TIME				2
#define		KEY_CHATTERING			20/UNIT_TIME				// ms
#define		LONGKEY_INTERVAL		500/UNIT_TIME
#define		DIMMING_STEP_MAX		4800			//40	 220427 step flicker 800
#define		DIMMING_STEP_UNIT		1200			//220721 for app
#define		DIMMING_STEP_MIN		0				//0		 220427 step flicker 10
#define		DIMMING_STEP_INTERVAL	4/UNIT_TIME		//100		 220427 step flicker 4

#elif KENDOL2 == KENDOL2_STANDARD

#define		UNIT_TIME				2
#define		KEY_CHATTERING			20/UNIT_TIME				// ms
#define		LONGKEY_INTERVAL		1000/UNIT_TIME
#define		LED_TOGGLE_TIME			300/UNIT_TIME

#endif

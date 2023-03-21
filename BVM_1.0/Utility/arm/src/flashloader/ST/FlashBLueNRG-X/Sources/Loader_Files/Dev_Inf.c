/* Flash Loader for BlueNRG */
#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
   "0x200",                  	// Device Name + version number
   MCU_FLASH,                   // Device Type
   0x10010000,                  // Device Start Address
   0x00010000,                  // Device Size in Bytes (64KBytes)
   0x00000040,                  // Programming Page Size 
   0xFF,                        // Initial Content of Erased Memory  
   
// Specify Size and Address of Sectors (view example below)
   
   0x00000020, 0x00000800,      //64KBytes Flash
   //0x00000001, 0x00000800,      //2KBytes Information IFR
   0x00000000, 0x00000000,
}; 

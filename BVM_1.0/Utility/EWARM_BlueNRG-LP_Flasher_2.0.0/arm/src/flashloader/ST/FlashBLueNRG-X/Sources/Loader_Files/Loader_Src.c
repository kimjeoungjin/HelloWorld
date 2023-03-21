#include "BlueNRG_Flash.h"
#include <stdint.h>

#define TimeOut 250//300

/*******************************************************************************
* Description :									
* Initilize the MCU Clock, the GPIO Pins corresponding to the 		        
* device and initilize the FSMC with the chosen configuration 		        
* Inputs 	:								
* 				 None 						
* outputs 	:								
* 				"1" 		: Operation succeeded		
* 				"0" 		: Operation failure		
* Note: Mandatory for all types of device 					
********************************************************************************/
int Init (void)
{  
  //Flash Unlock
  *((uint32_t*)0xA100001C) = 0xFFFFFFFF;
  *((uint32_t*)0xA1000020) = 0xFFFFFFFF;
  
  //Clear status
  *((uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;
  
  return 1;
}

/*Flash Block Fill*/
void Flash_BlockFill ( uint32_t Address, uint32_t Size, uint32_t* buffer)
{
  uint32_t loop = 0;
  //Address
  *((uint32_t*)NVM_REG_ADDRESS) = Address;
  
  while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;
  //Data
  *((uint32_t*)NVM_REG_DATA) = *buffer;
  while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;
  
  //CMD write
  *((uint32_t*)NVM_REG_COMMAND) = NVM_CMD_WRITE;
  uint32_t stat = *((uint32_t*)NVM_REG_ADDRESS);
  while(stat && (NVM_IRQ_CMDDONE|NVM_IRQ_CMDBUSY) != 0x3)
  {
  }
 /*while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;*/
  buffer+=1;
  for(int i = 1; i<Size; i++)
  {
     //Data
    *((uint32_t*)NVM_REG_DATA) = *buffer;
   /* while(1)//
    {
       loop+=1;
       if (loop >TimeOut)
         break;
    }
    loop = 0;*/
    //CMD write
    *((uint32_t*)NVM_REG_COMMAND) = NVM_CMD_WRITE;
    stat = *((uint32_t*)NVM_REG_ADDRESS);
    while(stat && (NVM_IRQ_CMDDONE|NVM_IRQ_CMDBUSY) != 0x3)
    {
    }
    /*while(1)
    {
       loop+=1;
       if (loop >TimeOut)
         break;
    }
    loop = 0;*/ 
    buffer+=1;
  }
}

/*******************************************************************************
* Description :																			
* Write into flash memory 												
* Inputs :																					
* 				Address	: Start adress 
*                               Size    : Memory Size
*                               buffer  : Pointer to data
* outputs :																				
* 				"1" : Operation succeeded											
* 				"0" : Operation failure													
* Note : Not Mandatory for SRAM PSRAM and NOR_FLASH			
********************************************************************************/
int Write (uint32_t Address, uint32_t Size, uint32_t* buffer)
{
  uint32_t block_size, writtenSize = 0;
  int Addr = (Address - 0x10010000)/4;
  int i = 0;
 
  while (i<Size/4)
  {
    if (i>(Size-0x40))
      block_size = Size - i;
    else if (i == Size)
      break;
    else
      block_size = 0x40;
    
    
    Flash_BlockFill(Addr, block_size, buffer + writtenSize);
    writtenSize += block_size;
    Addr+= block_size;
    i+=block_size;   
  }
  
  uint32_t stat = *((uint32_t*)NVM_REG_IRQSTAT);
  if( stat & (NVM_IRQ_CMDERR | NVM_IRQ_ILLCMD) )
    return 0;
  else
    return 1;
} 

/* Write IFR Sector */
int WriteIFR (uint32_t Address, uint32_t Size, uint32_t* buffer)
{
  uint32_t loop = 0;
  //Address
  *((uint32_t*)NVM_REG_ADDRESS) = (Address - 0x10020000)/4;
    while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;
  //Data
  *((uint32_t*)NVM_REG_DATA) = *buffer;
    while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;
  
  //CMD write
  *((uint32_t*)NVM_REG_COMMAND) = NVM_CMD_IFRWRITE;
  while(1)
  {
      loop+=1;
      if (loop >TimeOut)
        break;
  }
  loop = 0;
  buffer+=1;
  for(int i = 1; i<Size/4; i++)
  {
     //Data
    *((uint32_t*)NVM_REG_DATA) = *buffer;
    while(1)//(stat && NVM_IRQ_CMDDONE) == NVM_IRQ_CMDDONE )
    {
       loop+=1;
       if (loop >TimeOut)
         break;
    }
    loop = 0;
    //CMD write
    *((uint32_t*)NVM_REG_COMMAND) = NVM_CMD_IFRWRITE;
    while(1)
    {
       loop+=1;
       if (loop >TimeOut)
         break;
    }
    loop = 0; 
    buffer+=1;
  }
  
  uint32_t stat = *((uint32_t*)NVM_REG_IRQSTAT);
  if( stat & (NVM_IRQ_CMDERR | NVM_IRQ_ILLCMD) )
    return 0;
  else
    return 1;
}

/*******************************************************************************
* Description :																			
* Full erase of the device 															
* Inputs :																					
* 				None 																		
* outputs :																				
* 				"1" : Operation succeeded											
* 				"0" : Operation failure													
* Info :																						
* Note : Not Mandatory for SRAM PSRAM and NOR_FLASH			
********************************************************************************/
int MassErase (void)
{
  //Clear status
  *((uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;

  //Launch mass erase
  *((uint32_t*)NVM_REG_COMMAND) = NVM_CMD_MASSERASE;
  
  uint32_t stat = *((uint32_t*)NVM_REG_IRQSTAT);
  if( stat & (NVM_IRQ_CMDERR | NVM_IRQ_ILLCMD) )
    return 0;
  else
    return 1;
}

/*******************************************************************************
* Description :																			
* Erase a full sector in the device 												
* Inputs :																					
* 				SectrorAddress	: Start of sector 								
* outputs :																				
* 				"1" : Operation succeeded											
* 				"0" : Operation failure													
* Note : Not Mandatory for SRAM PSRAM and NOR_FLASH			
********************************************************************************/
int SectorErase (uint32_t StartAddress, uint32_t EndAddress)
{  
  while (StartAddress <= EndAddress)
  {
    *((uint32_t*)0xA1000018) = (StartAddress - 0x10010000)/4;
    if((StartAddress>=0x10020000))
      *((uint32_t*)0xA1000000) =  NVM_CMD_IFRERASE;
    else
      *((uint32_t*)0xA1000000) =  NVM_CMD_ERASE;
    
    StartAddress+=0x800;
  }
  
  uint32_t stat = *((uint32_t*)NVM_REG_IRQSTAT);
  if( stat & (NVM_IRQ_CMDERR | NVM_IRQ_ILLCMD) )
    return 0;
  else
    return 1;
}

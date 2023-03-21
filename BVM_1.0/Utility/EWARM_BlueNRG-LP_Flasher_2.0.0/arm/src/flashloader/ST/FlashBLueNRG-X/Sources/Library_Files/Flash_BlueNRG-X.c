/**************************************************
* Copyright 2004-2005 IAR Systems. All rights reserved.
*
* $Revision: 1166 $
**************************************************/
#include "flash_loader.h"
#include <intrinsics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BlueNRG_Flash-X.h"
#include <stdint.h>

#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
#endif
{    
  //Clear status
  *((volatile uint32_t*)NVM_REG_IRQMASK) = 0x00000000;
  *((volatile uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;
  
  *((volatile uint32_t*)0x200000C0) = 0x0; // disable blue controller
  
  return RESULT_OK;
}


uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
  uint32_t * src = (uint32_t *)buffer;
  uint32_t stat = 0;
  
  int Addr = ((uint32_t )block_start + offset_into_block - 0x10040000)/4;
  
  if (count % 16) // for BURST WRITING: if needed, count is rounded to the next
    // multiple of 16
    count = count - (count % 16) + 16 ;
  
  
  for ( ; count; count -= 16) 
  {
    *((volatile uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;
    *((volatile uint32_t*)NVM_REG_ADDRESS) = (uint16_t)Addr;
    *((volatile uint32_t*)NVM_REG_DATA) = *src++;
    *((volatile uint32_t*)NVM_REG_DATA1) = *src++;
    *((volatile uint32_t*)NVM_REG_DATA2) = *src++;
    *((volatile uint32_t*)NVM_REG_DATA3) = *src++;   
    
    *((volatile uint32_t*)NVM_REG_COMMAND) = NVM_CMD_BURSTWRITE;
    
    do {
      stat = *((volatile uint32_t*)NVM_REG_IRQSTAT);
    } while((stat & NVM_IRQ_CMDDONE) != NVM_IRQ_CMDDONE);
    
    Addr += 4;
  }
  
  return RESULT_OK;
}



uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  uint32_t stat = 0;
  uint32_t StartAddress = (uint32_t) block_start ; 
  
  *((volatile uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;
  
  *((volatile uint32_t*)NVM_REG_ADDRESS) = (StartAddress - 0x10040000)/4;
  
  *((volatile uint32_t*)NVM_REG_COMMAND) =  NVM_CMD_ERASE;
  
  do {
    stat = *((volatile uint32_t*)NVM_REG_IRQSTAT);
  } while((stat & NVM_IRQ_CMDDONE) != NVM_IRQ_CMDDONE);
  
  if( stat & (NVM_IRQ_CMDERR | NVM_IRQ_ILLCMD) )
    return RESULT_ERROR;
  else
    return RESULT_OK;
}

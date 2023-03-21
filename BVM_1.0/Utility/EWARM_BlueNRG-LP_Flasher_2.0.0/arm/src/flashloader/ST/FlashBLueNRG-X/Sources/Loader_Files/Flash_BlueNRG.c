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
#include "BlueNRG_Flash.h"
#include <stdint.h>



static __no_init uint8_t blankcheck;

#ifdef DEBUG_MESS
int putchar(int c)
{
  USART1->DR = c;
  while(0 == (USART1->SR & (1UL << 7)));

  return(c);
}
#endif // DEBUG_MESS

#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
#endif
{

  blankcheck = 0;

//#ifdef USE_ARGC_ARGV
//
//  for(int i = 0; i < argc; i++)
//  {
//    if (strcmp("--skip_erase", argv[i]) == 0)
//    {
//      blankcheck = 1;
//    }
//  }
//
//#endif

//Flash Unlock
  *((uint32_t*)0xA100001C) = 0xFFFFFFFF;
  *((uint32_t*)0xA1000020) = 0xFFFFFFFF;
  
  //Clear status
  *((uint32_t*)NVM_REG_IRQSTAT) = 0xFFFFFFFF;

	return RESULT_OK;
}

uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
uint16_t * dst = (uint16_t *)((uint8_t *)block_start + offset_into_block);
uint16_t * src = (uint16_t *)buffer;
uint32_t status = RESULT_OK;

#ifdef DEBUG_MESS
  printf("FlashWrite block_start %8X, offset_into_block %8X, count %8X, buffer %8X\n\r",
         (uint32_t)block_start,offset_into_block,count,(uint32_t)buffer);
#endif // DEBUG_MESS

  //Zero Wait State
//  FLASH->ACR = FLASH_ACR_LATENCY_0;
//
//#ifdef STM32F10X_XL
//  if ((uint32_t )dst < 0x8080000)
//#endif
//  {
//    //Bank0 programming
//    while (FLASH->SR & FLASH_SR_BSY);
//    FLASH->SR  = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR);
//    FLASH->CR |= FLASH_CR_PG;
//
//#ifdef STM32F10X_XL
//		// calculate number of bytes to boundary of second flash area
//		uint32_t i = 0x8080000 - (uint32_t )dst;
//		// fit i to curret number of bytes in the buffer
//		if(i > count)
//		{
//			i = count;
//		}
//		count -= i;
//    for (; i; i -= 2)
//#else
//    for (; count; count -= 2)
//#endif
//    {
//      *dst++ = *src++;
//      while (FLASH->SR & FLASH_SR_BSY);
//    }
//
//    if (FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR))
//      status = RESULT_ERROR;
//
//    FLASH->CR &= ~FLASH_CR_PG;
//  }
//
//#ifdef STM32F10X_XL
//  if ( 	 ((uint32_t )dst >= 0x8080000)
//  	  && count
//  	  )
//  {
//    //Bank1 programming
//    while (FLASH->SR2 & FLASH_SR_BSY);
//    FLASH->SR2  = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR);
//    FLASH->CR2 |= FLASH_CR_PG;
//
//    for (; count; count -= 2)
//    {
//      *dst++ = *src++;
//      while (FLASH->SR2 & FLASH_SR_BSY);
//    }
//
//    if (FLASH->SR2 & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR))
//      status = RESULT_ERROR;
//
//    FLASH->CR2 &= ~FLASH_CR_PG;
//  }
//#endif
//
//#ifdef DEBUG_MESS
//  printf("FlashWrite .. done\n\r");
//#endif // DEBUG_MESS
  return status;
}

uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  
  
  uint32_t StartAddress= (uint32_t) block_start;
  uint32_t EndAddress=StartAddress + block_size;
  
  
#ifdef DEBUG_MESS
  printf("FlashErase block_start %8X, block_size %8X\n\r",(uint32_t)block_start,block_size);
  if(FLASH->CR & FLASH_CR_LOCK)
  {
    printf("\nFlash is locked!!!!!\n");
  }
#endif // DEBUG_MESS


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
    return RESULT_OK;
  else
    return RESULT_ERROR;

 
}

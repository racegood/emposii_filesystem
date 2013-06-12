#ident "$Id: flash.h$"

#ifndef SMART_FLASH_H
#define SMART_FLASH_H

#include <main.h>
#include <types.h>

bool	FlashCheck(volatile u32 *addr);
bool	EraseBlock(u32 *addr);
bool	Erase(u32 *addr, u32 len);
bool	FlashWrite(void *dest, void *src);
bool FlashWord(void *dest, void *src);
#endif

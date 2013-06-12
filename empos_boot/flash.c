#include <flash.h>
#include <reg.h>
#include <types.h>
#include <lib.h>

bool FlashCheck(volatile u32 *addr){

	u32	result;
	do {
		*addr = (u32)(STATUS_READ);
		result = *addr;

	} while  ((~result & STATUS_BUSY)); //플래쉬가 비지 상태이면 대기한다.

	*addr = (u32)(READ_ARRAY);

	// 플래쉬에 에러가 발생했는지 상태 비트를 정검한다.
	if ((result & STATUS_ERR_FILTER) == 0){
		return true;
	} else if (result == STATUS_LOCK_DETECT){
		printf("\nBlock Lock-bit detected.\n\n");
	} else if (result == STATUS_VOLT_RANG_ERR) {
		printf("\nVoltage Range Error.\n\n");
	} else if ((result == STATUS_CMD_SEQ_ERR)){
		printf("\nCommand Sequence Error.\n\n");
	} else if ((result == STATUS_ERASE_ERR)){
		printf("\nFail to erase. retry again.\n\n");
	} else {
		printf("\nFlash unknown error..\n\n");
	}
	
	return true;
}

bool EraseBlock(u32 *addr){
	volatile u32	*s=addr;

	if ((long)addr%FLASH_BLOCK_SIZE) return false; //블럭 사이즈에 맞는지 확인한다(4byte 단위)

	*s = (u32)(ERASE_SETUP);
	*s = (u32)(ERASE_CONFIRM);

	if (!FlashCheck(s)) return false;
	return true;
}

bool Erase(u32 *addr, u32 len){
	u32	*ptr, *end=addr+len/4;
	
	// erase block loop.
	for (ptr=addr; ptr<end; ptr+=FLASH_BLOCK_SIZE/4){
		ClearLine();
		printf("\tCurrent block address\t: 0x%08lx", (long)ptr);
		// erase block.
		if (!EraseBlock(ptr)){
			return false;
		}
	}
	printf("\n");
	return true;
}

bool FlashWrite(void *dest, void *src){
	u32		i=0;
	volatile u8 *s1=(u8 *)dest;
	u8 *s2=(u8 *)src;
	u16  writeBuf;

	writeBuf = 32;

	while(i < FLASH_BLOCK_SIZE)
	{
		int cnt = writeBuf / sizeof(u16);
		int result;

		do {
			__REG(s1) = WRITE_BUF;
			result = __REG(s1);
        	} while((result & STATUS_BUSY ) != STATUS_BUSY);

		__REG(s1) = (u32)((writeBuf / sizeof(u16) -1) |
			((writeBuf / sizeof(u16) -1 ) <<
			 (sizeof(u32) * 4)));

		while(cnt--)
		{
			__REG((s1+i)) = __REG(s2);
			i += sizeof(u32);
			s2 += sizeof(u32);
		}
		__REG((s1)) = (u32)(WRITE_BUF_COMFIRM);

		do {
			result = __REG(s1);
		} while((result & STATUS_BUSY ) != STATUS_BUSY);

		__REG(s1) = (u32)(READ_ARRAY);

	}
	return true;
}


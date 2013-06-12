#include "serial.h"

void SerialInit(void)
{
	// GPDR1 Setting,
	__REG(GPIO_BASE + GPDR1) &= 0xFFFFFFFB;
	__REG(GPIO_BASE + GPDR1) |= 0x00000080;

	// GAFR1_L Setting
	__REG(GPIO_BASE + GAFR1_L) &= 0xFFFF3FCF;
	__REG(GPIO_BASE + GAFR1_L) |= 0x00008010;

	// CKEN Setting
	__REG(CLOCK_MANAGER_BASE + CKEN) &= 0xFFFFFFFBF;
	__REG(CLOCK_MANAGER_BASE + CKEN) |= 0x000000040;

	// FCR Setting
	__REG(FFUART_BASE + FFFCR) &= 0xFFFFFFF8;
	__REG(FFUART_BASE + FFFCR) |= 0x00000007;

	return;
}

void PutChar(const char ch)
{
	while (!(__REG(FFUART_BASE + FFLSR) & 0x00000040));

	__REG(FFUART_BASE + FFTHR) = ((u32)ch & 0xFF);

	if (ch=='\n') PutChar('\r');


}

void PutString(const char *ch)
{
	while(*ch != '\0')
		PutChar(*ch++);
}

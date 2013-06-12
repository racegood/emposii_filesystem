////////////////////////////////////////////////////////////////////////////////////////////////////
// 설명 : 아직 타이머에 관한 본 파일 함수들이 완전하게 구현되어 있지 않다.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <config.h>
#include <reg.h>
#include <time.h>
#include <types.h>

void InitTimer(void)
{
//	unsigned long old_rcnr = RCNR;

	/* Force 32-kHz oscillator on */
//	PCFR |= PCFR_FO;

	/* disable timer interrupts and clear the AL and HZ bits*/
//	RTSR = (RTSR_AL | RTSR_HZ);

	/* put the counter to 0 */
	/* strange enough, this doesn't seem to work -- Erik */
	/* RCNR = 0x0; */
	/* RCNR writes may be delayed by a 32-kHz clock cycle */

//	RCNR = 0x0;
//	while (RCNR > old_rcnr)
//	;
}

#if 1 
u32 GetTime(u32 *t) {
	*t = RCNR;
	return *t;
}
#else
u32 GetTime() {
	return ((u32) RCNR);
}
#endif

void msleep(unsigned int msec)
{
	unsigned long startTime, currentTime;
	time_t now;
	startTime = GetTime(&now);
	while(1) {
		currentTime = GetTime(&now);

		/* check timeout value */
		if((currentTime - startTime) >
		   msec) {
			break;
		}
	}

}

clock_t clock(void) {
        return OSCR;    // 3.6864 MHz
}




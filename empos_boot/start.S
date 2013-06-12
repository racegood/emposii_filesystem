///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 부팅시 가장 처음으로 실행 되는 파일이다.
//
// 1. Vector 태이블 작성
// 2. Cpu 설정
// 3. GPIO 설정
// 4. 메모리 관련 래지스터 설정
// 5. 메모리 카피(ROM -> RAM)
// 6. C 함수로 이동
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* 코드의 ident */
.ident "$Id: start.S,v$"

#include <reg.h>				// xscale 관련 래지스터들이 정의되어 있다.
#include <config.h>				// 관련 설정값들이 저장되어 있다.

.text

/* vector 태이블 작성 */
.globl 	_start
_start:
	b	reset
	b	Undefined_Instruction		// 정의되지 않는 명령이 명령 인스트럭터에 패치되었을때 실행된다.
	b	Software_Interrupt      	// 소프트웨어적인 인터럽트 명령(SWI)가 수행되었을 때 실행된다.
	b	Prefetch_Abort		// 데이타를 프리 패치 할때 버스 에러가 발생하면 실행된다.
	b	Data_Abort			// 데이타 에러가 발생하면 실행된다.
	b	Not_Used			// 사용되지 않는다.
	b	IRQ				// IRQ 인터럽트가 발생되면 실행된다.
	b	FIQ				// IRQ 인터럽트보다 우선권이 있는 인터럽트가 발생되면 실행된다.

reset:

///////////////////////////////////////////////////////////////////////////////////////////////////
// cpu를 설정한다.
///////////////////////////////////////////////////////////////////////////////////////////////////
xscale_init:
	///////////////////////////////////////////////////////////////////////////////////////////
	//슈퍼바이저 하위 다섯 비트=10011
	//IRQ, FRQ, 디저블
	//32 bit 명령어 처리
	//슈퍼 바이저 모드로 이동, coprocessor 연산을 가능하게 한다.
	///////////////////////////////////////////////////////////////////////////////////////////

	mov     r0, #0x000000D3  //11010011
        msr     cpsr_c, r0

	//mcr 명령을 통해 co processror와 연산을 한다.
	//코프로세서 연산은 현재 이해안가는 부분이 많으므로 후로 미룬다.

        //ldr     r0, =0x2001
        //mcr    p15, 0, r0, c15, c1, 0 //r0의 값을 c15에 값을 넣는다.


	//Intel has macro call (CPWAIT) here, but they don't define it
        //disable MMU, instruction and data caches, and the write buffer
        ldr     r0, =0x00000078
	mcr     p15, 0, r0, c1, c0, 0
        //Intel has macro call (CPWAIT) here, but they don't define it

        //flush instruction and data tlb
        ldr     r0, =0x00000000
        mcr     p15, 0, r0, c8, c7, 0

        //flush instruction and data caches
        mcr     p15, 0, r0, c7, c7, 0

        //drain the write buffer
        mcr     p15, 0, r0, c7, c10, 4

        //intel does it, so I will too...
		.word 0xa00c0444 // coreSWIHandler
		.word 0xa00c047c // coreIRQHandler
        nop

        //grant manager access to all domains
        mvn     r0, #0
        mcr     p15, 0, r0, c3, c0, 0

        ldr     r12, =INTERRUPT_CONTROL_BASE

        ldr     r0, =0x00000000
        str     r0, [r12, #ICMR] 	// 부팅 도중 모든 인터럽트가 발생하지 않게 마스크 한다
        str     r0, [r12, #ICLR] 	// Interrupt controller Level Register
	                           	// ICLR에 모두 0을 넣음으로써 FIQ가 아닌 IRQ레벨에서 인터럽트가 동작한다.

        ldr     r12, =CLOCK_MANAGER_BASE

        ldr     r0, =CKEN_VALUE
        str     r0, [r12, #CKEN] 	//CKEN-Clock enable Register에 CKEN6비트를 설정하여.
					//AC97, SSP, FFUART, BTUART, USB, MMC, LCD

        ldr     r0, =OSCC_VALUE	//OSCC-Oscillator Configure Register
        str     r0, [r12, #OSCC]	//OON비트를 1로 설정 하여. 32.768Hz oscillator를 enable한다.
					//한번 설정된 값은 Reset을 제외하고 이값을 바꿀수 없다.


        ldr     r0, =CCCR_VALUE	//CCCR-Core Clock Configure Register
        str     r0, [r12, #CCCR]	//Memory Frequncy is 99.53Mhz ~ 3.6864Mhz
					//Run Mode Frequncy = Memory Frequncy * 2
					//Toubo Mode Frequncy = Run Mode Fre * 2

        mov     r1, #3		//cp14는 클럭과 파워에 관련되어 있다
        mcr     p14, 0, r1, c6, c0, 0//cp14의 6번 래지스터에 3을 넣는다.
					//FCS비트를 설정하여 변경된 레지스터의 내용에따라 Frequncy를 변경한다.
					//TOURBO 비트를 설정하여 터보 모드로 들어간다.


	ldr	r1, =OSCR		//OSCR-OS Timer Counter Register의 처음값에 0을 넣는다.
	ldr	r0, =0
	str	r0, [r1]

        ldr     r0, =0x300
wait_for_clock:			//클럭 증가를 300까지 해보는 부분으로 클럭 태스트를 한다.
        ldr	r2, [r1]		//하지만 이부분도 사용자에 따라 필요하지 않을수도 있다.
        cmp     r0, r2
        bne     wait_for_clock



init_gpio:
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//이곳에서 QPIO를 초기화 해주는 이유는 81개의 QPIO가 최대 3개의 alternate function를 가지고 있으므로.
	//해당하는 alternate function을 정의 해주어야 재대로 동장한다.
	//보드에 관련된 가능한 모든 alternate function을 설정하여. 차후 보드 동작에 이상이없도록 한다.
	//81개의 GPIO를 처리하기 위해 9개 래지스터의 3개 set을 가지고 있다.
	//GPAF래지스터에 alternate function을 정의 하고(2개 비트 사용->2개씩 총 6개로 구성), GPDR, GPSR,GPCR을
	//function의 특성에 맞게 설정한다.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ldr     r12, =GPIO_BASE

        ldr     r0, =GAFR0L_VALUE
        str     r0, [r12, #GAFR0_L]
        ldr     r0, =GAFR0U_VALUE
        str     r0, [r12, #GAFR0_U]

        ldr     r0, =GAFR1L_VALUE
        str     r0, [r12, #GAFR1_L]
        ldr     r0, =GAFR1U_VALUE
        str     r0, [r12, #GAFR1_U]

        ldr     r0, =GAFR2L_VALUE
        str     r0, [r12, #GAFR2_L]
        ldr     r0, =GAFR2U_VALUE
        str     r0, [r12, #GAFR2_U]

        ldr     r0, =GPSR0_VALUE
        str     r0, [r12, #GPSR0]
        ldr     r0, =GPSR1_VALUE
        str     r0, [r12, #GPSR1]
        ldr     r0, =GPSR2_VALUE
        str     r0, [r12, #GPSR2]
        ldr     r0, =GPCR0_VALUE
        str     r0, [r12, #GPCR0]
        ldr     r0, =GPCR1_VALUE
        str     r0, [r12, #GPCR1]
        ldr     r0, =GPCR2_VALUE
        str     r0, [r12, #GPCR2]

        ldr     r0, =GPDR0_VALUE
        str     r0, [r12, #GPDR0]
        ldr     r0, =GPDR1_VALUE
        str     r0, [r12, #GPDR1]
        ldr     r0, =GPDR2_VALUE
        str     r0, [r12, #GPDR2]

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//PSSR - Power Manager Sleep Status Register
	//RDH - Read-Disable Hold is set
	//RDH - RDH는 GPIO input이 디저블 됨을 나타낸다. (GPIO를 설정후 사용전 clear해야함)
	//PH - Periperal Control Hold
	//PH - GPIO 핀이 GPIO configuration에 따라 설정되었는지, 슬립모드에 있는지 알려준다. (peripheral interface가 설정된후 사용전 clear 해야함)
	//각각의 비트값에 1을 넣음으로써 각 status flags를 clear 할수 있다.
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	ldr     r1, =PSSR
	ldr     r2, =0x37		//RDH, PH, VFS,BFS,SSS를 모두 설정한다.
	str     r2, [r1]

init_sdram:
	//메모리 관련 레지스터를 설정하는 부분이다.
	//차후 커널이 부팅하여도, 특별한 재설정이 없는한, 이부분이 바뀌지 않으므로, 안정적인 동작을 위해 각각 레지스터
	//의 값을 신중히 결정해야 한다(특히 DRAM부분)

        ldr     r12, =MEM_CTL_BASE

        ldr     r0, =MSC0_VALUE
        str     r0, [r12, #MSC0]

 	//Intel's code reads it back to make sure it works...
    	//ldr     r0, [r12, #MSC0]

        ldr     r0, =MSC1_VALUE
        str     r0, [r12, #MSC1]
        ldr     r0, [r12, #MSC1]

        ldr     r0, =MSC2_VALUE
        str     r0, [r12, #MSC2]
        ldr     r0, [r12, #MSC2]

        ldr     r0, =MECR_VALUE
        str     r0, [r12, #MECR]

        ldr     r0, =MCMEM0_VALUE
        str     r0, [r12, #MCMEM0]

        ldr     r0, =MCMEM1_VALUE
        str     r0, [r12, #MCMEM1]

        ldr     r0, =MCATT0_VALUE
        str     r0, [r12, #MCATT0]

        ldr     r0, =MCATT1_VALUE
        str     r0, [r12, #MCATT1]

        ldr     r0, =MCIO0_VALUE
        str     r0, [r12, #MCIO0]

        ldr     r0, =MCIO1_VALUE
        str     r0, [r12, #MCIO1]


	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MDREFR 비트를 설정하는데 있어서 정확하게 설정되게 하기위해. 여러 단개로 진행한다.
	// 설정 순서
	// 1. SDLCK1.2.3를 not free runnung 그리고 DRI값을 설정한다.
	// 2. 우리 보드는 SRAM이 포함되므로 SXCNFG의 값을 설정하여야 한다.
	//	MDREFR이전에 MSC설정후 설정해도 문자없을듯 보인다.
	//	이곳에 값을 설정하면, 이미 MSC등에 관련된 값이 설정되어 있으면 이전 값은 무시된다.
	// 3. Self Reflash 를 디저블 해야한다. 이곳에서는 생략되었슴.
	//
	// 4. SDCLK에 값을 넣어 메모리를 가동 시킬 준비를 한다. Self Reflash, APD등을 설정
	// 5. MDREFR의 나머지 값을 설정한다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

        ldr     r0, =MDREFR_VALUE
        ldr     r3, [r12, #MDREFR]	//r3에 현재 설정되어 있는 MDREFR의 값을 저장한다.
        ldr     r1, =0xFFF
        and    r0, r0, r1		//설정할 DRI 값만을 r0에 넣는다.

	// r1 = 0xfff
	// r3 = 현재 설정되어 있는 MDREFR의 값
	// r0 = 설정할 DRI의 값.
        bic     r3, r3, r1
	bic	r3, r3, #0x03800000		//DRI 설정에 앞서서 25,24,23비트를 클래어한다. 따라서 SDCLK0,1, 2를 모두 not free running으로 설정
        orr     r3, r3, r0

        str     r3, [r12, #MDREFR]		//이미 정의한 DRI값을 설정한다.


	// 우리 보드는 SRAM이 포함되므로 SXCNFG의 값을 설정하여야 한다.

        ldr     r0, =MDREFR_VALUE
	ldr	r1, =0xF6000		//23,22,21,20,18,17 비트를 1로 설정한다.
					//23 비트 - Static Memory클럭
					//22 비트 - Self Reflash를 Enable
					//21 비트 - Reserve
					//20 비트 - APD(Auto Power Down)를 1로 설정, 접근 되지 않는 SDRAM, Static Memory는 Power Down
					//18 비트 - SDCLK2를 Enable
					//17 비트 - SDCLK1 은 메모리 클럭의 반으로 동작한다.
	orr	r3, r3, r0		//위의 값을 설정한다.

        str     r3, [r12, #MDREFR]
	ldr	r3, [r12, #MDREFR]		//최근 까지 설정한 값을 가져온다.

	bic     r3, r3, #0x00400000		//Self-Reflash를 desabel한다.
        str     r3, [r12, #MDREFR]  	//K1RUN등의 비트는 Self-Refreah가 디저블 상태에서 설정하고, 인에블 될때까지 유지되어야한다.

	ldr	r0, =MDREFR_VALUE	//마지막 남은 비트의 값을 설정한다.
	ldr	r1, =0x03809000		//위의 과정을 통해 E1PIN이 디저블(Self-Reflash를 디저블 하는 과정에서)된다.
	and	r0, r0, r1   			//SDCLK값등을 설정
	orr	r3, r3, r0
        str     r3, [r12, #MDREFR]
        nop
        nop

	//위의 과정을 통해 MDREFR래지스터의 모든값의 설정이 끝났다. 하단부에서 Self-Reflash를  Enable한다.

	//MDCNFG또한 0,1,2,16,17를 클래어하여 디저블 시킨후 값을 설정한다. 그런후 마지막에 SDRAM patition을 enable한다.
        
	ldr     r0, =MDCNFG_VALUE

        bic     r0, r0, #0x00000003		//디저블 하는 부분
        bic     r0, r0, #0x00030000		//

        bic     r0, r0, #0x00000004		// 32bits bus width를 설정
        str     r0, [r12, #MDCNFG]		//값을 설정, 아직 인에블 하는 부분이 없다.

        ldr     r0, =OSCR
	mov	r1, #0
	str	r1, [r0]			//OSCR의 값을 r1에 가지고 있다.

	//pause for approx 200 usecs
        ldr     r4, =0x300			//타이머를 이용해서 딜레이를 주는 부분이다.
sdram_dly:
        ldr     r1, [r0]
        cmp    r4, r1
        bgt     sdram_dly			//

        //turn everything off
        //mov     r0, #0x78
        //mcr     p15, 0, r0, c1, c0, 0

        //Access memory that has not been enabled for CBR refresh cycles (8)
	ldr	r0, =BOOTLOADER_RAM_BASE
	str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]

        //they mention dcache should
        //be enabled here if it is desired (we don't care)

	//	ldr	r0, =0x0000007A
	//	mcr	p15, 0, r0, c1, c0, 0

        ldr     r0, [r12, #MDCNFG]
        //orr     r0, r0, #0x00000001
	orr     r0, r0, #0x00000003
        str     r0, [r12, #MDCNFG]		//SDRAM patition0, 1을 enable 시킨다.

        //write MDMRS again
        ldr     r0, =MDMRS_VALUE
        str     r0, [r12, #MDMRS]

        ldr     r0, [r12, #MDREFR]
	ldr	r11, =0xFFEFFFFF		//Auto Power Down을 설정해서 메모리 관련 클럭등을 자동으로 Enable한다.
	and	r0, r0, r11
        str     r0, [r12, #MDREFR]

	//////////////////////////////////////////////////////////////////////////////////////
	//롬에 있는 부트로더를 램상으로 복사후 C함수로 점프한다.
	/////////////////////////////////////////////////////////////////////////////////////
	ldr	r0, =0
	ldr	r1, =BOOTLOADER_RAM_RUNNING
	ldr	r2, =BOOTLOADER_LENGTH


rom2ram:
	ldr	r3, [r0]
	str	r3, [r1]
	add	r0, r0, #4
	add	r1, r1, #4
	subs	r2, r2, #4
	bne	rom2ram

	ldr	sp, =STACK_POINTER


	ldr	r0, =c_main
	mov	pc, r0

boot_falut:
	b	reset



Undefined_Instruction:	// 알수 없는 명령 처리에 대한 수행 루틴

Software_Interrupt:		// 소프트웨어 인터럽트가 발생되었을때에 대한 수행 루틴

Prefetch_Abort:		// 데이타를 프리 패치 할때 버스 에러가 발생하면 실행된다.

Data_Abort:			// 데이타 에러가 발생하면 실행된다.

Not_Used:			// 사용되지 않는다.

IRQ:				// IRQ 인터럽트가 발생되면 실행된다.

FIQ:				// FIQ 인터럽트가 발생되면 실행된다.

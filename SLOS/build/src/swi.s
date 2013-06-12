@ ****************************************************************************
@ * Simple Little Operating System - SLOS
@ ****************************************************************************

@ ****************************************************************************
@ *
@ * Module     : swi.s
@ * Project	: SLOS
@ * Version	: 0.09
@ * Originator	: Andrew N. Sloss
@ * History    :
@ * 
@ * 16th July 2003 Andrew N. Sloss
@ * - added header information
@ *
@ ****************************************************************************

     .text
     .code 32 
     .align 0
     .global coreSWIHandler
     .global coreCallSWI
     .extern eventsSWIHandler

@ ****************************************************************************
@ * kernel swi handler event
@ ****************************************************************************

@
@  void coreCallSWI(registerstr *r0)
@

coreCallSWI:
     STMFD   r13!,{r0-r4,r12,lr}
     MOV     r4,r0
     LDMIA   r4,{r0-r3}
     SWI     5075
     STMIA   r4,{r0-r3}
     LDMFD   r13!,{r0-r4,r12,pc}

@
@  void coreSWIHandler(void)
@

coreSWIHandler:
     STMFD   sp!,{r0-r12,lr}   	@ store user's gp registers    
     LDR     r10,[lr,#-4]        @ extract the SWI number  
     BIC     r10,r10,#0xff000000  @ get SWI # by bit-masking
     MOV     r1,sp				@ pointer to parameters on stack
     MRS     r2,SPSR        	@ get SPSR into gp registers
	 STMFD	 sp!, {r2}
	 MOV r0, r10
	 BL 		 eventsSWIHandler
	 LDMFD	 sp!, {r2}
     MSR     SPSR_cxsf,r2       @ csxf flags
     LDMFD   sp!,{r0-r12,pc}^	@ unstack user's registers, return from handler
     .end

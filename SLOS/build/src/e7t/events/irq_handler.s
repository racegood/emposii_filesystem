@  ____________________________________________________________________
@ 
@  Copyright (c) 2002, Andrew N. Sloss, Chris Wright and Dominic Symes
@  All rights reserved.
@  ____________________________________________________________________
@ 
@  NON-COMMERCIAL USE License
@  
@  Redistribution and use in source and binary forms, with or without 
@  modification, are permitted provided that the following conditions 
@  are met: 
@  
@  1. For NON-COMMERCIAL USE only.
@ 
@  2. Redistributions of source code must retain the above copyright 
@     notice, this list of conditions and the following disclaimer. 
@ 
@  3. Redistributions in binary form must reproduce the above 
@     copyright notice, this list of conditions and the following 
@     disclaimer in the documentation and/or other materials provided 
@     with the distribution. 
@ 
@  4. All advertising materials mentioning features or use of this 
@     software must display the following acknowledgement:
@ 
@     This product includes software developed by Andrew N. Sloss,
@     Chris Wright and Dominic Symes. 
@ 
@   THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY 
@   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
@   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
@   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE 
@   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
@   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
@   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES@ LOSS OF USE, DATA, 
@   OR PROFITS@ OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
@   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
@   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
@   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
@   OF SUCH DAMAGE. 
@ 
@  If you have questions about this license or would like a different
@  license please email :
@ 
@ 	andrew@sloss.net
@ 
@ 
@ ****************************************************************************
@ * Simple Little Operating System - SLOS
@ ****************************************************************************

@ ****************************************************************************
@ *
@ * Module      : handler.s
@ * Description : handles the irq events
@ * OS          : SLOS
@ * Platform    : e7t
@ * History     :
@ *
@ * 21 December 2001 Andrew N. Sloss
@ * - started working on the handlers
@ *   
@ ****************************************************************************

@ ****************************************************************************
@ * EXPORT
@ ****************************************************************************

     .text 
     .code 32
     .align 0
     .global eventsIRQHandler

@ ****************************************************************************
@ * IMPORT
@ ****************************************************************************

     .extern eventTickService
     .extern kernelScheduler

@ ****************************************************************************
@ * ROUTINES
@ ****************************************************************************

@ -- address of the IRQ controller for the EMPOS II

INTPND:
     .word 0x40D00000    @ IRQ controller
     
@ -- events_irq_handler -------------------------------------------------------
@ ;
@ Description  : handles the IRQ interrupt and determines the source and then
@                vectors to the correct interrupt rountine.
@ ;

eventsIRQHandler:

     SUB     r14, r14, #4
     STMFD   r13!, {r0-r3, r12, r14}    @ save context
     LDR     r0, INTPND	 
     LDR     r0, [r0]                   @ get IRQ source		
     TST     r0, #0x04000000
     BNE     eventTickVeneer            @ if timer interrupt
     LDMFD   r13!, {r0-r3, r12, pc}^    @ unknown event ignore

eventTickVeneer:

     BL      eventTickService
     B       kernelScheduler
     .end


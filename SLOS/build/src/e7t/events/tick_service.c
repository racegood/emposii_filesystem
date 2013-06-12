/*
 *  ____________________________________________________________________
 * 
 *  Copyright (c) 2002, Andrew N. Sloss, Chris Wright and Dominic Symes
 *  All rights reserved.
 *  ____________________________________________________________________
 * 
 *  NON-COMMERCIAL USE License
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  
 *  1. For NON-COMMERCIAL USE only.
 * 
 *  2. Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 * 
 *  3. Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials provided 
 *     with the distribution. 
 * 
 *  4. All advertising materials mentioning features or use of this 
 *     software must display the following acknowledgement:
 * 
 *     This product includes software developed by Andrew N. Sloss,
 *     Chris Wright and Dominic Symes. 
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY 
 *   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE 
 *   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 *   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 *   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 *   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 *   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 *   OF SUCH DAMAGE. 
 * 
 *  If you have questions about this license or would like a different
 *  license please email :
 * 
 * 	andrew@sloss.net
 * 
 * 
 */

/*****************************************************************************
 * Simple Little Operating System - SLOS
 *****************************************************************************/

/*****************************************************************************
 *
 * Module      : tick_service.c
 * Description : Header File
 * Platform    : Evaluator7T
 * History     : 
 *
 * 2000-03-25 Andrew N. Sloss
 * - implemented a tick service
 *
 *****************************************************************************/


/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../headers/macros.h"

#include "../../headers/api_types.h"

#include "../../serial/serial.h"

extern unsigned int semaphore;
/*****************************************************************************
 * MACRO'S
 *****************************************************************************/

/* none ... */

/*****************************************************************************
 * GLOBAL
 *****************************************************************************/

UINT countdown;

/*****************************************************************************
 * EXTERN's
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * GLOBALS
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * ROUTINES
 *****************************************************************************/

/* -- eventTickInit -----------------------------------------------------------
 *                                                            
 * Description  : Initialises the counter timer in milliseconds
 *
 * Parameters   : UINT msec - sets periodic timer in milliseconds
 * Return       : none...
 * Notes        : none...
 *                                                                             
 */

void eventTickInit (UINT msec)
{
	/* ----------------------------------------------------------------------
	 *
	 * Initalize the Tick hardware on the PXA255 part.
	 *
	 * ----------------------------------------------------------------------
	 */

	__REG32(INTERRUPT_CONTROL_BASE+ICCR) = 0x01;
	__REG32(INTERRUPT_CONTROL_BASE+ICMR) |= (1<<26);
	__REG32(INTERRUPT_CONTROL_BASE+ICLR) &= ~(1<<26);
	__REG32(0x40A00010) = 0x0;	// TIMER_BASE + OSCR

	/* ----------------------------------------------------------------------
	 *
	 * Set the countdown value depending on msec.
	 *
	 * ----------------------------------------------------------------------
	 */

	countdown = msec * 368640;

	__REG32(TIMER_BASE+OSMR0) = (u32)countdown;

}

/* -- eventTickService --------------------------------------------------------
 *
 * Description : interrupt service routine for timer0 interrupt.
 *
 * Parameters  : none...
 * Return      : none...
 * Notes       : 
 * 
 */ 

void eventTickService(void) 
{ 
	disable_irq();	// IRQ Disable
	if((__REG32(INTERRUPT_CONTROL_BASE+ICIP) & (1<<26)) != 0) // pin26 : OSTimer 0
	{
		__REG32(0x40A00010) = 0x0;		// TIMER_BASE + OSCR Count Clear
		__REG32(0x40A00014) &= 0xFFFFFFFE;	// TIMER_BASE + OSSR
		__REG32(0x40A00014) |= 0x1;	// TIMER_BASE + OSSR Timer Status Clear
	}
	enable_irq();	// IRQ Enable
} 

/* -- eventTickStart ----------------------------------------------------------
 *
 * Description  : switches on the periodic tick event
 *
 * Parameters   : none...
 * Return       : none...
 * Notes        : none...
 *
 */  

void eventTickStart(void)
{	
	__REG32(TIMER_BASE+OIER) |= (1<<0); // Timer 0
	__REG32(TIMER_BASE+OSSR) = (1<<0);	// Timer Status Clear
}

void enable_irq(void)
{
	__asm__ __volatile__("mrs	r1, cpsr");
	__asm__ __volatile__("bic	r1, r1, #0x80");
	__asm__ __volatile__("msr	cpsr, r1");
}

void disable_irq(void)
{
	__asm__ __volatile__("mrs	r1, cpsr");
	__asm__ __volatile__("orr	r1, r1, #0x80");
	__asm__ __volatile__("msr	cpsr, r1");
}


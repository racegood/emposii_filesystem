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
 * Module       : macros.h
 * Description  : brings in macro headers
 * OS           : SLOS
 * Platform     : e7t
 * History      : 
 *
 * 16th November 2001 Andrew N. Sloss
 * - added a macro header  
 *
 *****************************************************************************/

#ifndef _H_MACRO_
#define _H_MACRO_

/* Macro */
#define __REG *(volatile unsigned char *)
#define __REG8 *(volatile unsigned short *)
#define __REG16 *(volatile unsigned long *)
#define __REG32 *(volatile unsigned long *)

/* -- Interrupt controller ------------------------------------------------- */

#define CLOCK_MANAGER_BASE		0x41300000
#define CKEN	0x04

#define INTERRUPT_CONTROL_BASE	0x40D00000
#define ICIP	0x00
#define ICMR	0x04
#define ICLR	0x08
#define ICCR	0x14

#define TIMER_BASE	0x40A00000
#define OSMR0	0x00
#define OSMR1	0x04
#define OSMR2	0x08
#define OSMR3	0x0C
#define OSCR	0x10
#define OSSR	0x14
#define OIER	0x1C

#define GPIO_BASE	0x40E00000
#define GPDR1	0x10
#define GAFR1_L	0x5C

#define FFUART_BASE	0x40100000
#define FFTHR 	0x00
#define FFRBR	0x00
#define FFFCR	0x08
#define FFLCR	0x0C
#define FFLSR	0x14

/* -- General -------------------------------------------------------------- */

#define IRQVector        0x18

#endif


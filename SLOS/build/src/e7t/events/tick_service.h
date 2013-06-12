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
 * Simple Little Operating System
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

/* none... */

/*****************************************************************************
 * MACRO'S
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * GLOBALS
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * ROUTINES
 *****************************************************************************/

void disable_irq(void);
void enable_irq(void);

/* -- eventTickInit -----------------------------------------------------------
 *                                                            
 * Description  : Initialises the counter timer in milliseconds
 *
 * Parameters   : UINT msec - sets periodic timer in milliseconds
 * Return       : none...
 * Notes        : none...
 *                                                                             
 */

void eventTickInit(UINT msec);

/* -- eventTickService --------------------------------------------------------
 *                                                            
 * Description  : services an Tick event and resets the timer 
 *
 * Parameters   : none...
 * Return       : none...
 * Notes        : none...
 *                                                                             
 */

void eventTickService(void);

/* -- eventTickStart ---------------------------------------------------------
 *
 * Description  : switches on the periodic tick event
 *
 * Parameters   : none...
 * Return       : none...
 * Notes        : none...
 *
 */  

void eventTickStart(void);


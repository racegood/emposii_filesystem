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
 * Module      : serial_driver.c
 * Description : low level device driver for the serial device
 *               based on DDF v0.01. 
 * OS          : SLOS 0.09
 * Platform    : e7t
 * History     :
 *
 * 24th November 2001 Andrew N. Sloss
 * - create template to speed up driver development.
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "serial_driver.h"
#include "../headers/macros.h"
#include "../../serial/serial.h"

#define DEVICE_SERIAL_E7T  55095

/*****************************************************************************
 * MACROS
 *****************************************************************************/

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/

typedef struct 
{
	unsigned int uid;
} internal_serialstr;

/*****************************************************************************
 * STATICS
 *****************************************************************************/

internal_serialstr		node;

/* -- internalSerialE7TSetup --------------------------------------------------
 *
 * Description : internal serial port setup routine
 *
 * Parameters  : unsigned int port - UART address
 *             : unsigned int baud - baud rate
 * Return      : none...
 * Notes       :
 * 
 */

void internalSerialE7TSetup(void)
{
	/*
	 *  Initialize UART here
	 *  SET FFLCR, FFFCR, FFDLL, FFLSR , etc...
	 */

	/*
	// GPDR1 Setting,
	__REG32(GPIO_BASE + GPDR1) &= 0xFFFFFFFB;
	__REG32(GPIO_BASE + GPDR1) |= 0x00000080;

	// GAFR1_L Setting
	__REG32(GPIO_BASE + GAFR1_L) &= 0xFFFF3FCF;
	__REG32(GPIO_BASE + GAFR1_L) |= 0x00008010;

	// CKEN Setting
	__REG32(CLOCK_MANAGER_BASE + CKEN) &= 0xFFFFFFFBF;
	__REG32(CLOCK_MANAGER_BASE + CKEN) |= 0x000000040;

	// FCR Setting
	__REG32(FFUART_BASE + FFFCR) &= 0xFFFFFFF8;
	__REG32(FFUART_BASE + FFFCR) |= 0x00000007;
	*/

}

/* -- serial_init -------------------------------------------------------------
 *
 * Description : initalize serial driver.
 * 
 * Parameters  : none...
 * Return      : none...
 * Notes       : none...
 *
 *  Initializes the node data structures
 */

void serial_init(void)
{
	/* initalize physical device ... */

	/* initialize COM0 ......................... */
	internalSerialE7TSetup();

	/* setup internal structure ... */

	node.uid   = NONE;
}

/* -- serial_open -------------------------------------------------------------
 *
 * Description : open the serial device driver
 * 
 * Parameters  : unsigned major - DEVICE_SEGMENT_E7T 
 *             : unsigned com - COM0 | COM1
 * Return      : if (success) return UART0_BASE or UART1_BASE 
 *                  else if (alreadyopen) return DEVICE_IN_USE
 *                  else if (unknown) return DEVICE_NEXT
 * Notes       : 
 *
 */

UID serial_open(unsigned int major) 
{
	if (major==DEVICE_SERIAL_E7T)
	{
		if (node.uid!=NONE) 
		{
			return DEVICE_IN_USE;
		} 
		else
		{
			node.uid = uid_generate ();	
			return node.uid;  
		} 
	}

	return DEVICE_NEXT;
}

/* -- serial_close ------------------------------------------------------------
 *
 * Description : close serial device driver
 * 
 * Parameters  : UID id 
 * Return      : 
 *   DEVICE_SUCCESS - successfully close the device
 *   DEVICE_UNKNOWN - couldn't identify the UID
 * Notes       : 
 *
 */

int serial_close(UID id) 
{
	if (node.uid==id) 
	{ 	
		node.uid = NONE;
		return DEVICE_SUCCESS;
	}
	return DEVICE_UNKNOWN;
}

/* -- serial_write_byte -------------------------------------------------------
 *
 * Description : waits for response from hardware and writes a 
 *               byte to the device 
 				 write a byte to a serial device
 * 
 * Parameters  : UID device
 *             : BYTE data
 * Return      : none...
 *
 * Notes       : an example of a led write bit
 *
 */

void serial_write_byte(UID id,BYTE ch)		
{
	if(node.uid == id)
	{
		// if non-zero, TEMP is 1. That means FIFO is empty
		while(!(__REG32(FFUART_BASE + FFLSR) & 0x40)); // wait until buffer is empty
		__REG32(FFUART_BASE + FFTHR) = (ch&0xFF);	/// put data 
	}
}

/* -- serial_read_byte --------------------------------------------------------
 *
 * Description : read a particular byte value
 * 
 * Parameters  : UID id - COM0 | COM1 
 * Return      : BYTE - read byte otherwise 255
 *
 * Notes       :
 */

BYTE serial_read_byte(UID id) 
{
	if (node.uid==id) 
	{
		while(!(__REG32(FFUART_BASE + FFLSR) & 0x01));
		return __REG32(FFUART_BASE + FFRBR) & 0xFF;
	}

	return -1;
}


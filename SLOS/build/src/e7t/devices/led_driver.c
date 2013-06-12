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
 * Module      : led_driver.c
 * Description : device driver for led on the e7t
 * OS          : SLOS 0.09
 * Platform    : e7t
 * History     :
 *
 * 19th November 2001 Andrew N. Sloss
 * - started working on an example device driver
 *
 * December 2nd 2001 Andrew N. Sloss
 * - added uid generator to the LED driver
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "led_driver.h"
#include "../headers/macros.h"
#include "../../serial/serial.h"

/*****************************************************************************
 * MACROS
 *****************************************************************************/

/* -- base register */

#define MSC				0x4800000C

/* -- GPIO */

#define LED				0x10600000

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/

typedef struct
{
	unsigned int uid;
	BYTE value;
}device_LED;


/*****************************************************************************
 * STATICS
 *****************************************************************************/
device_LED device_led;

/* -- led_init ----------------------------------------------------------------
 *
 * Description : initalize the LED device driver internal 
 *               data structures and hardware. Set all the
 *               LED's to be zero.
 * 
 * Parameters  : none...
 * Return      : none...
 * Notes       : none...
 *
 */

void led_init(void) 
{
	__REG32(MSC) = 0x00005AA8;

	/* switch off all the LED's ....................... */
	__REG32(LED) = 0x0;

	device_led.uid = NONE;
	device_led.value = 0;
}

/* -- led_open ----------------------------------------------------------------
 *
 * Description : example open on the LED 
 * 
 * Parameters  : unsigned major = 55075 
 *             : unsigned minor = (0=GREEN1,1=RED,2=ORANGE,3=GREEN2)
 * Return      : UID -
 * Notes       : 
 *
 */

UID led_open(unsigned int major,unsigned int minor) 
{
	if (major==DEVICE_LED_E7T)
	{
		if (minor == 1) 
		{
			if (device_led.uid==NONE)
			{
				device_led.uid = uid_generate();
				return device_led.uid;      /* unique ID */
			}
			else
			{
				return DEVICE_IN_USE;	
			}
		} 
		else
		{
			return DEVICE_UNKNOWN;	
		}
	}

	return DEVICE_NEXT;
}

/* -- led_close ---------------------------------------------------------------
 *
 * Description : example open on the LED 
 * 
 * Parameters  : UID id 
 * Return      : 
 *
 *   DEVICE_SUCCESS - successfully close the device
 *   DEVICE_UNKNOWN - couldn't identify the UID
 *
 * Notes       : 
 *
 */

int led_close(UID id) 
{
	int x;

	if (device_led.uid==id) 
	{
		device_led.uid = NONE;
		return DEVICE_SUCCESS;
	}

	return DEVICE_UNKNOWN;
}

/* -- led_write_byte -----------------------------------------------------------
 *
 * Description : write a particular bit to an LED 
 * 
 * Parameters  : UID id = 55075
 *	     	: BYTE led_set - least significant bit is used
 * Return	     : none...
 *
 * Notes       : an example of a led write bit
 *
 */


void led_write_byte(UID id,BYTE led_set) 
{
	if (device_led.uid == id) 
	{
		device_led.value = led_set;
		__REG32(LED) = led_set;
	}
}

/* -- led_read_byte ------------------------------------------------------------
 *
 * Description : read a particular bit value 
 * 
 * Parameters  : UID id = 55075 + unit (0..3)
 * Return      : value return error if 255
 *
 * Notes       : an example of a led read bit
 */

BYTE led_read_byte(UID id) 
{
	if(device_led.uid == id)
	{
		return device_led.value;
	}

	/* 
	 * error correction goes here...
	 */

	return -1;
}

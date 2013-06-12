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
 * Module       : segment_driver.c
 * Description  : low level device driver for 7-segment display 
 *                provided on the e7t
 * OS           : SLOS 0.09
 * Platform     : e7t
 * History      :
 *
 * 24th November 2001 Andrew N. Sloss
 * - converted segment driver to the Wright Device Driver Framework.
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "segment_driver.h"
#include "../headers/macros.h"
#include "../../serial/serial.h"

#define DEVICE_SEGMENT_E7T  55090

/*****************************************************************************
 * MACROS
 *****************************************************************************/

/* -- base register */

#define MSC			0x4800000C

/* -- GPIO */

#define SEGMENT_LOW		0x10300000
#define SEGMENT_HIGH	0x10400000

#define DIGIT0			0x3F
#define DIGIT1			0x06
#define DIGIT2			0x5B
#define DIGIT3			0x4F
#define DIGIT4			0x66
#define DIGIT5			0x6D
#define DIGIT6			0x7D
#define DIGIT7			0x07
#define DIGIT8			0x7F
#define DIGIT9			0x6F
#define DIGITA			0x77
#define DIGITB			0x7C
#define DIGITC			0x39
#define DIGITD			0x5E
#define DIGITE			0x79
#define DIGITF			0x71

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/

typedef struct {
	int			seg; 
	int 	    uid;
} device_segment;

/*****************************************************************************
 * STATICS
 *****************************************************************************/

device_segment segment;

/* -- segment_init ------------------------------------------------------------
 *
 * Description  : initalize the segment device driver internal 
 *                data structures and hardware. Set segment's 
 *                to display zero.
 * 
 * Parameters   : none...
 * Return       : none...
 * Other        : none...
 *
 */

void segment_init(void)
{
	__REG32(MSC) = 0x00005AA8;

	/* switch off all the 7Segment */
	__REG32(SEGMENT_LOW) = 0x0;
	__REG32(SEGMENT_HIGH) = 0x0;

	segment.uid	= NONE;
	segment.seg = 0; 

}

/* -- segment_open ------------------------------------------------------------
 *
 * Description : open the segment device driver 
 * 
 * Parameters  : unsigned major = DEVICE_SEGMENT_E7T 
 *             : unsigned ignored = anything
 * Return      : if (success) return DEVICE_SEGMENT_E7T 
 *                else if (alreadyopen) return DEVICE_IN_USE
 *                else if (unknown) return DEVICE_NEXT
 * Other       : 
 *
 */

UID segment_open(unsigned int major,unsigned int minor) 
{
	if (major==DEVICE_SEGMENT_E7T)
	{
		if (segment.uid==NONE)
		{
			segment.uid = uid_generate ();
			return segment.uid;      /* unique ID */
		}
		else
		{
			return DEVICE_IN_USE;	
		}
	} 

	return DEVICE_NEXT;
}

/* -- segment_close -----------------------------------------------------------
 *
 * Description  : close open handle to segment display 
 * 
 * Parameters   : UID id 
 * Return       : 
 *   DEVICE_SUCCESS - successfully close the device
 *   DEVICE_UNKNOWN - couldn't identify the UID
 * Other        : 
 *
 */

int segment_close(UID id) 
{
	if (segment.uid==id) 
	{
		segment.uid = NONE;
		return DEVICE_SUCCESS;
	}

	return DEVICE_UNKNOWN;
}

/* -- segment_write_byte ------------------------------------------------------
 *
 * Description  : write a particular bit to an LED 
 * 
 * Parameters   : UID id = 55075 + unit (0..3)
 *              : BYTE led_set - least significant bit is used
 * Return       : none...
 *
 * Other        : an example of a led write bit
 *
 */

BYTE getSegmentCode(BYTE seg)
{
	BYTE code;
	switch(seg)
	{
		case 0x0:	code = DIGIT0;	break;
		case 0x1:	code = DIGIT1;	break;
		case 0x2:	code = DIGIT2;	break;
		case 0x3:	code = DIGIT3;	break;
		case 0x4:	code = DIGIT4;	break;
		case 0x5:	code = DIGIT5;	break;
		case 0x6:	code = DIGIT6;	break;
		case 0x7:	code = DIGIT7;	break;
		case 0x8:	code = DIGIT8;	break;
		case 0x9:	code = DIGIT9;	break;
		case 0xA:	code = DIGITA;	break;
		case 0xB:	code = DIGITB;	break;
		case 0xC:	code = DIGITC;	break;
		case 0xD:	code = DIGITD;	break;
		case 0xE:	code = DIGITE;	break;
		case 0xF:	code = DIGITF;	break;

	}
	return code;
}

void segment_write_byte(UID id,int val) 
{
	BYTE seg[4];

	if (segment.uid==id) 
	{
		segment.seg = val;

		seg[0] = getSegmentCode(val & 0xF);
		seg[1] = getSegmentCode((val>>4) & 0xF);
		seg[2] = getSegmentCode((val>>8) & 0xF);
		seg[3] = getSegmentCode((val>>12) & 0xF);

		__REG32(SEGMENT_LOW) = (seg[1] << 8) | seg[0];
		__REG32(SEGMENT_HIGH) = (seg[3] << 8) | seg[2];
	}
}

/* -- segment_read_byte -------------------------------------------------------
 *
 * Description  : read a particular bit value 
 * 
 * Parameters   : UID id = 55075 + unit (0..3)
 * Return       : value return error if 255
 *
 * Other        : an example of a led read bit
 */

BYTE  segment_read_byte(UID id) 
{
	if (segment.uid==id) 
	{  
		return segment.seg;
	}

	/* 
	 * error flag goes here...
	 */

	return 255;
}


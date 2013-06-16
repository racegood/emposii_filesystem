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
 * Module       : task1_start.c
 * Description  : Shell TASK routine that toggles the LED2
 * Platform     : e7t
 * History      :
 *
 * 2000-03-27 Andrew N. Sloss
 * - implemented on Evalautor7T
 * - added header and changed the delay time
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../core/mutex.h"

#include "../headers/api_types.h"
#include "../devices/ddf_io.h"
#include "../e7t/devices/ddf_types.h"

#include "../e7t/events/swis.h"
#include "../headers/api_device.h"

#include "../serial/serial.h"

/*****************************************************************************
 * STATICS
 *****************************************************************************/

device_treestr *blkdev;
UID ublkdev;

/*****************************************************************************
 * ROTUINES
 *****************************************************************************/

/* -- openRedLED --------------------------------------------------------------
 *
 * Description   : opens the communication port to the red LED
 *
 * Parameters    : none..
 * Return        : BOOLean - successful | unsucessful 
 * Notes         :
 *
 */

BOOL openBLKDEV(void) 
{ 
	unsigned int delay;
	/* --------------------------------------------------------------
	 *
	 * eventIODeviceOpen - 
	 *
	 * open the LED driver node...
	 * 
	 * --------------------------------------------------------------
	 */

	blkdev = eventIODeviceOpen(&blkdev,DEVICE_BLOCK_EMPOSII,1);
	ublkdev = blkdev->uid;

	/* check the UID .................. */

	switch (ublkdev) 
	{
		case DEVICE_IN_USE:
		case DEVICE_UNKNOWN:
			return FALSE;
	} 

	return TRUE; 
}

block_datastr *writeBLKDEV(UID uid, char* dat)
{
	block_datastr *blk_datastr = NULL;

	blk_datastr = eventIODeviceWriteBlock(blkdev, uid, dat);
	printf("blk_datastr : %x\n", blk_datastr);
	return blk_datastr;
}

char *readBLKDEV(UID uid, block_datastr *blk_datastr)
{
	char *ret;
	ret = eventIODeviceReadBlock(blkdev, ublkdev, blk_datastr);
	return ret;
}

/* -- C_EntryTask1 ------------------------------------------------------------
 *
 * Description  : C entry to Task1
 *
 * Parameter    : none...
 * Return       : none...
 * Notes	:
 *
 * Never terminates
 */

void C_EntryTask1(void)
{
	int i=0, j=1;
	unsigned int delay;
	char dat[2000];
	block_datastr *blk_datastr;

	if(openBLKDEV() == TRUE)
	{
		printf("open blkdevice - uid[%d]\n",ublkdev);
		int i;

		for(i=0;i<2000;i++)
		{
			if(i % 10 == 0)
				dat[i] = '\n';
			else
				dat[i] = '1';
		}
		dat[i] = '\0';
		blk_datastr = writeBLKDEV(ublkdev, dat);
		printf("%s\n", readBLKDEV(ublkdev, blk_datastr));
	}

	/* 
	 * error loop
	 */

	while (1) 
	{
		delay=0xBEEF0001;
	};
}


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
 * Modules      : ddf_tree.h
 * Description  : device driver tree
 * OS           : SLOS 0.09
 * Platform     : e7t
 * History      :
 *
 * 20th November 2001 Andrew N. Sloss
 * - created the first device driver tree
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "../devices/led_driver.h"
#include "../devices/segment_driver.h"
#include "../devices/serial_driver.h"
#include "../devices/blkdev_driver.h"

/*****************************************************************************
 * DEVICES
 *****************************************************************************/

#include "../devices/ddf_types.h"

/*****************************************************************************
 * STRUCTURE
 *****************************************************************************/

/* -- device_treestr ----------------------------------------------------------
 *
 * Description : complete static device drive tree
 *
 */
 
device_treestr devices[] = 
{

/* ------------------------------------------------------------------------
 * LED device
 *
 * Simple LED device driver controls 4 leds 
 */
  {
  "LED",
  DEVICE_LED_E7T,  	
  led_init, 
  led_open,
  led_close,
    {led_write_byte},
    {led_read_byte},
 	 0
  } ,

/* ------------------------------------------------------------------------
 * Segment device
 *
 * 7-Segment display device driver controls 
 */

  {
  "SEGMENT",
  DEVICE_SEGMENT_E7T,  	
  segment_init, 
  segment_open,
  segment_close,
    {segment_write_byte},
    {segment_read_byte},
	0
  },
/* ------------------------------------------------------------------------
 * Serial device
 *
 * serial display device driver controls 
 */

  {
  "SERIAL",
  DEVICE_SERIAL_E7T,
  serial_init,
  serial_open,
  serial_close,
    {serial_write_byte},
    {serial_read_byte},
	0
  },

/* ------------------------------------------------------------------------
 * Block device
 *
 * block device driver
 */
  {
  "BLOCK",
  DEVICE_BLOCK_EMPOSII,
  blkdev_init,
  blkdev_open,
  blkdev_close,
    {blkdev_write_block},
    {blkdev_read_block},
	0
  },
/* -- End of tree -- */

  {
  "\0",
  0,
  0,
  0,
  0,
  0
  }
};


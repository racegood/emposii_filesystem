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
 * Simple Little Operation System - SLOS
 *****************************************************************************/

/*****************************************************************************
 * 
 * Module       : ddf_frame.h
 * Description  : Simple low level Device Driver Framework
 * OS           : SLOS 0.09
 * Platform     : e7t
 * History      : 
 *
 * 19th November 2001 Andrew N. Sloss
 * - implemented version DDF 0.01 from Chris Wright's 
 *   specification
 *
 * December 2nd 2001 Andrew N. Sloss
 * - added uid generator
 *
 *****************************************************************************/
#ifndef _H_DDFFRAME_
#define _H_DDFFRAME_

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../core/uid.h" /* UID generator ................... */

/*****************************************************************************
 * MACROS
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/


#define BLOCK_SIZE ((1<<10) - 4) // 4 byte for next ptr
#define EOF			4			// end of file

typedef struct block_struct{
	char data[BLOCK_SIZE];
	int *next;
} block_datastr;


typedef struct device_tree {

  /* -----------------------------------------------------------
   * Field          : name
   * Description    : name of the device driver
   * Example        : "LED Driver"
   *
   */	

  char  name[12];
  
  /* -----------------------------------------------------------
   * Field          : driver_id
   * Description    : is a unique identifer for the driver
   * Example        : for LED driver
   *
   */

  UID driver_id;

  /* -----------------------------------------------------------
   * Field          : fn * init
   * Description    : initialize internel specific
   *	               : device driver functions
   * Example        : led_init ();
   * Returns        : none...
   */

  void 	(*init)   	(void);

  /* -----------------------------------------------------------
   * Field          : fn * open
   * Description    : opens a device for access
   * Example        : uid = open (SERIAL,COM1);
   * Returns        : UID
   *
   *	>0 - SUCCESS
   *	-1 - DEVICE_IN_USE
   * -2 - DEVICE_UNKNOWN
   *
   */

  UID	(*open)   	(unsigned int, unsigned int);

  /* -----------------------------------------------------------
   * Field          : fn * close
   * Description    : opens a device for access
   * Example        : uid = open (SERIAL,COM1);
   * Returns        : UID
   *
   *	 0 - DEVICE_SUCCESS
   *  -2 - DEVICE_UNKNOWN
   *
   */

  int	(*close)	(UID);

    /**********************************************************
     * WRITE
     **********************************************************/	

    union  
    {

    /* --------------------------------------------------------
     * Field        : fn * byte
     * Description  : write a byte to a particular device
     * Example      : write.byte (SEGMENT,SEVEN);
     * Returns      : none...
     *
     */

    void		(*byte)	 (UID, BYTE);

    /* --------------------------------------------------------
     * Field        : fn * bit
     * Description  : write a bit to a particular device
     * Example      : write.bit (LEDRED,REDON);
     * Returns      : none...
     *
     */

    void		(*bit)	 (UID, BYTE);

    /* --------------------------------------------------------
     * Field        : fn * bit
     * Description  : write a block to a particular device
     * Example      : write.block (ETHER,&PACKET);
     * Returns      : none...
     *
     */

    block_datastr *		(*block) (UID, char *);

    } write;


    /**********************************************************
     * READ
     **********************************************************/	

    union 
    {

    /* --------------------------------------------------------
     * Field        : fn * byte
     * Description  : read a byte from a particular device
     * Example      : c = read.block (SERIAL,COM1);
     * Returns      : BYTE - 8-bit unsigned char
     *
     */

    BYTE 		(*byte)  (UID); 
    
    /* --------------------------------------------------------
     * Field        : fn * bit
     * Description  : read a bit from a particular device
     * Example      : is_redled_on = read.bit (LED_RED);
     * Returns      : BYTE - only least significant bit is 
     *                important  
     *
     */

    BYTE 		(*bit)   (UID);

    /* --------------------------------------------------------
     * Field        : fn * block
     * Description  : write a block to a particular device
     * Example      : recieved = write.block (ETHER);
     * Returns      : block_datastr 
     *
     */

    char 	*(*block) (UID, block_datastr *);
    
    } read;

	UID uid;
} device_treestr ;

extern device_treestr devices[];

#endif

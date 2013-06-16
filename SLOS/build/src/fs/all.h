#ifndef __ALL_H__
#define __ALL_H__

/*******************************************
 *                                         *
 *        2013. 06. 03.                    *
 *        File System Proto Type           *
 *        FileName :                       * 
 *  Note :                                 *
 *                                         *
 *                                         *
 *    - End of Note -                      *
 *******************************************/
/***********************
 *   Include File      *
 ***********************/

/* Standard Library */
#include "../serial/serial.h"
#include "../core/all.h"
#include "../headers/api_types.h"
#include "../devices/ddf_io.h"
#include "../e7t/devices/ddf_types.h"
#include "../e7t/events/swis.h"
#include "../headers/api_device.h"

/* User Define Library */

/***********************
 *    Type Define      *
 ***********************/
#define debug				1
#define true				1
#define false				0
#define nil					0x0
#define boolean				char
#define Addr				unsigned int
#define Max_Size			2048
#define Max_Length			32

/**************************
 *  User Define Structure *
 **************************/

/***********************
 *    Static Value     *
 ***********************/

device_treestr *blkdev;
UID  ublkdev;
/**********************
 *  Function List     *
 **********************/

/*
 * --------  Function 
 * -- Name : 
 * -- note :
 *
 * - End of Note -
 */
#endif

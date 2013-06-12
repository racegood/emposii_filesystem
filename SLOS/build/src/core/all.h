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
 * Module      : all.h
 * Description : brings in all the specific core headers
 * OS          : SLOS 0.09
 * Platform    : e7t
 * History     :
 *
 * 16th November 2001 Andrew N. Sloss
 * - started creating 
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/
 
#ifndef _CORE_ALL

#define _CORE_ALL 1

#include "../headers/api_types.h"
#include "cli_com.h"
#include "cli_readline.h"
#include "../devices/ddf_io.h"
#include <assert.h>
#include <time.h>

/* Bring in a header for the first task .......... */

#include "../apps/task1_start.h"

/* Bring in cli header ........................... */

#include "cli_com.h"

/* Bring in all the headers for the platform ..... */

#include "../e7t/headers/all.h"
#include "../headers/api_device.h"
#include "../headers/api_lldebug.h"

#endif



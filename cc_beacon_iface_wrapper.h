/***********************************************************************************************//**
 *  \brief      CC1101 Beacon Handler Wrapper
 *  \details    Provides a single secure function that all processes can use to output messages to
 *              CC1101 beacon (using cc_beacon_iface library) without having to care about errors.
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       28-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

#ifndef __CC_BEACON_IFACE_WRAPPER_
#define __CC_BEACON_IFACE_WRAPPER_

/*** INCLUDES *************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>

#include "cc_beacon_iface.h"


/*** PARAMETERS ***********************************************************************************/
#define MSG_LENGTH      100
#define SOCK_IP_ADDR    "0.0.0.0"
#define SOCK_PORT       "1000"

#define DBG_PURPLE      "\x1b[35;1m"
#define DBG_NOCOLOR     "\x1b[0m"

/*** GLOBAL CONSTANTS: ****************************************************************************/

/*** GLOBAL VARIABLES: ****************************************************************************/

/*** TYPEDEFS *************************************************************************************/

/*** FUNCTION HEADERS *****************************************************************************/
void send_beacon_msg(MsgSource process_id, const char * fmt, ...);


#endif

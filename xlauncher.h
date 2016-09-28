/***********************************************************************************************//**
 *  \brief      Experiment Launcher and Process Handler
 *  \details    Executes the system modules and mantains them alive to be able to guarantee a
 *              constant execution. Resets the computer when errors are found.
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       27-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

#ifndef __XLAUNCHER_H_
#define __XLAUNCHER_H_

/*** INCLUDES *************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "cc_beacon_iface_wrapper.h"


/*** PARAMETERS ***********************************************************************************/
#define READ        0
#define WRITE       1
#define DBG_REDB    "\x1b[31;1m"
#define DBG_REDD    "\x1b[31m"
#define DBG_GREENB  "\x1b[32;1m"
#define DBG_GREEND  "\x1b[32m"
#define DBG_BLUE    "\x1b[34;1m"
#define DBG_YELLOW  "\x1b[33;1m"
#define DBG_NOCOLOR "\x1b[0m"

/*** GLOBAL CONSTANTS: ****************************************************************************/


/*** GLOBAL VARIABLES: ****************************************************************************/
extern int pipe0[2];
extern int pipe1[2];
extern int pid_tee;
extern int pid_raspivid;
extern int pid_vitow;

/*** MACROS: **************************************************************************************/
#ifdef XLAUNCHER_DEBUG
    #define printfd(fmt, ...) do { \
            printf("[launcher:%s] (" DBG_BLUE    "d" DBG_NOCOLOR ") " \
            fmt, curr_time_format(), ## __VA_ARGS__); \
        } while(0)
    #define printfe(fmt, ...) do { \
            printf("[launcher:%s] (" DBG_REDB    "E" DBG_NOCOLOR ") " \
            DBG_REDD fmt DBG_NOCOLOR, curr_time_format(), ## __VA_ARGS__); \
        } while(0)
    #define printfw(fmt, ...) do { \
            printf("[launcher:%s] (" DBG_YELLOW  "W" DBG_NOCOLOR ") " \
            fmt, curr_time_format(), ## __VA_ARGS__); \
        } while(0)
    #define printfo(fmt, ...) do { \
            printf("[launcher:%s] (" DBG_GREENB  "o" DBG_NOCOLOR ") " \
            DBG_GREEND fmt DBG_NOCOLOR, curr_time_format(), ## __VA_ARGS__); \
        } while(0)
#else
    #define printfd(fmt, ...) do { } while (0)
#endif

/*** TYPEDEFS *************************************************************************************/


/*** FUNCTION HEADERS *****************************************************************************/
const char * curr_time_format(void);



#endif

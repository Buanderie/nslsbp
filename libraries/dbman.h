/***********************************************************************************************//**
 *  \brief      Database Manager
 *  \details    Implements MySQL wrapper functions to be able to save and retrieve beacon data from
 *              the system databases.
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       29-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

#ifndef __DBMAN_H__
#define __DBMAN_H__

/*** INCLUDES *************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mysql/mysql.h>

#include "dbman_credentials.h"  /* Defines DB_USR and DB_PASSWD. */
#include "sensors.h"            /* Defines Housekeeping structures. */

#include "nslsbp.h"

/*** PARAMETERS ***********************************************************************************/
#define DB_HOST         "localhost"
#define DB_DBNAME       "nslsbp"
#define DB_TABLE_HK     "hk_data"
#define DB_TABLE_XBEE   "xbee_data"


/*** GLOBAL CONSTANTS: ****************************************************************************/

/*** GLOBAL VARIABLES: ****************************************************************************/

/*** TYPEDEFS *************************************************************************************/

/*** FUNCTION HEADERS *****************************************************************************/
int dbman_get_hk_data(hk_data_t * data);
int dbman_get_all_data(balloon_data_t * data);
int dbman_get_last_position(double *lat, double *lon, double *alt); /* unused function */
int dbman_save_hk_data(hk_data_t * data);
int dbman_save_xbee_data(xbee_hk_data_t * data);

#endif

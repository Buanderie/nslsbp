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
#include <mysql/mysql.h>

#include "dbman_credentials.h"  /* Defines DB_USR and DB_PASSWD. */

/*** PARAMETERS ***********************************************************************************/
#define DB_HOST         "localhost"
#define DB_DBNAME       "barbie_space_explorer"
#define DB_TABLE_GPS    "gps"
#define DB_TABLE_DBG    "dbg"


/*** GLOBAL CONSTANTS: ****************************************************************************/

/*** GLOBAL VARIABLES: ****************************************************************************/

/*** TYPEDEFS *************************************************************************************/
typedef struct GPS_data {
    time_t time_local;
    time_t time_gps;
    double lat;
    double lng;
    double v_kph;
    double sea_alt;
    double geo_alt;
    double course;
    double temp;
    double cpu_temp;
    double gpu_temp;
} GPS_data;

/*** FUNCTION HEADERS *****************************************************************************/
int dbman_save_gps_data(GPS_data * data);
int dbman_get_gps_data(GPS_data * data);
int dbman_save_dbg_data(time_t time_local, time_t time_sbc, const char * message);
int dbman_get_last_position(double *lat, double *lon, double *alt);


#endif

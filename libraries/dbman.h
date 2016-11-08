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

/*** PARAMETERS ***********************************************************************************/
#define DB_HOST         "localhost"
#define DB_DBNAME       "barbie_space_explorer"
#define DB_TABLE_GPS    "gps"
#define DB_TABLE_DBG    "dbg"


/*** GLOBAL CONSTANTS: ****************************************************************************/

/*** GLOBAL VARIABLES: ****************************************************************************/

/*** TYPEDEFS *************************************************************************************/
typedef struct GPS_data {
    char time_local[12];
    char time_gps[12];
    double lat;
    double lng;
    double v_kph;
    double sea_alt;
    double geo_alt;
    double course;
    double temp;
    double cpu_temp;
    double gpu_temp;
    char trick[4];      /* This is a TEMPORAL trick to get the system to work. TODO: solve memcpy issue */
} GPS_data;

/* this last structure is conserved for historic purposes */
/* the new structure includes the .h SENSORS from module_gps_temp/include */
#include "../module_gps_temp/include/sensors.h"
/* this .h includes:
typedef struct _ambient_sensors{
    float cpu_temp;
    float gpu_temp;
    float in_temp;
    float out_temp;
    float pressure;
    float calc_alt;
}_ambient_sensors;

typedef struct _motion_sensors{
    float acc_x;
    float acc_y;
    float acc_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float mag_x;
    float mag_y;
    float mag_z;
}_motion_sensors;

typedef struct _gps_data{   
    time_t time_local;
    time_t time_gps;
    float lat;
    float lng;
    float gspeed;
    float sea_alt;
    float geo_alt;
}_gps_data;
*/
/* the new structure that will go into the DB is a wrapper from all those 3 structs */
typedef struct hk_data_t{
    _gps_data gps;
    _motion_sensors mot;
    _ambient_sensors amb;
}hk_data_t;

/*** FUNCTION HEADERS *****************************************************************************/
int dbman_save_gps_data(GPS_data * data);
int dbman_get_gps_data(GPS_data * data);
int dbman_save_dbg_data(time_t time_local, time_t time_sbc, const char * message);
int dbman_get_last_position(double *lat, double *lon, double *alt);


#endif

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

/*** INCLUDE SECTION ******************************************************************************/
#include "dbman.h"

static MYSQL * mysql_handle;


/***********************************************************************************************//**
 * Connects to the remotet MySQL database.
 **************************************************************************************************/
static int dbman_connect(void)
{
    if((mysql_handle = mysql_init(NULL)) != NULL) {
        if(mysql_real_connect(mysql_handle, DB_HOST, DB_USR, DB_PASSWD, DB_DBNAME, 0, NULL, 0) != NULL) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -2;
    }
}


/***********************************************************************************************//**
 * Disconencts from the remote MySQL database.
 **************************************************************************************************/
static void dbman_disconnect(void)
{
    if(mysql_handle != NULL) {
        mysql_close(mysql_handle);
    }
    mysql_library_end();
}

static int mysql_query_no_answer(const char *query)
{
    if(mysql_query(mysql_handle, query) != 0) {
        return mysql_errno(mysql_handle);
    }
    return 0;
}

/***********************************************************************************************//**
 * Retrieves GPS and temperature data from the remote MySQL database.
 **************************************************************************************************/
int dbman_get_hk_data(HKData * data)
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    if(dbman_connect() == 0) {
        if(mysql_query(mysql_handle, "SELECT * FROM " DB_TABLE_HK " ORDER BY "
                                     "`row_id` DESC LIMIT 0,1") == 0) {
            if((result = mysql_store_result(mysql_handle)) != NULL) {
                while((row = mysql_fetch_row(result)) != NULL) {
                    /* Changed strtod to strtof in all the float values */
                    data->gps.time_local    = strtol(row[1],  NULL, 10);
                    data->gps.time_gps      = strtol(row[2],  NULL, 10);
                    data->gps.lat           = strtof(row[3],  NULL);
                    data->gps.lng           = strtof(row[4],  NULL);
                    data->gps.gspeed        = strtof(row[5],  NULL);
                    data->gps.sea_alt       = strtof(row[6],  NULL);
                    data->gps.geo_alt       = strtof(row[7],  NULL);
                    data->mot.acc_x         = strtof(row[8],  NULL);
                    data->mot.acc_y         = strtof(row[9],  NULL);
                    data->mot.acc_z         = strtof(row[10], NULL);
                    data->mot.gyro_x        = strtof(row[11], NULL);
                    data->mot.gyro_y        = strtof(row[12], NULL);
                    data->mot.gyro_z        = strtof(row[13], NULL);
                    data->mot.mag_x         = strtof(row[14], NULL);
                    data->mot.mag_y         = strtof(row[15], NULL);
                    data->mot.mag_z         = strtof(row[16], NULL);
                    data->amb.cpu_temp      = strtof(row[17], NULL);
                    data->amb.gpu_temp      = strtof(row[18], NULL);
                    data->amb.in_temp       = strtof(row[19], NULL);
                    data->amb.in_pressure   = strtof(row[20], NULL);
                    data->amb.in_calc_alt   = strtof(row[21], NULL);
                    data->amb.out_temp      = strtof(row[22], NULL);
                    data->amb.out_pressure  = strtof(row[23], NULL);
                    data->amb.out_calc_alt  = strtof(row[24], NULL);
                }
                mysql_free_result(result);
                dbman_disconnect();
                return 0;
            } else {
                /* Result was NULL. */
                return mysql_errno(mysql_handle);
            }
        } else {
            /* SQL query returned an error. */
            return mysql_errno(mysql_handle);
        }
    } else {
        return -1;
    }
}

/***********************************************************************************************//**
 * Inserts GPS and temperature data into the remote MySQL database.
 **************************************************************************************************/
int dbman_save_hk_data(HKData * data)
{
    char query[1000];
    int retval = 0;

    sprintf(query, "INSERT INTO " DB_TABLE_HK " (`time_local`, `time_gps`, `lat`, `lng`, `gspeed`, "
        "`sea_alt`, `geo_alt`, `acc_x`, `acc_y`, `acc_z`, `gyro_x`, `gyro_y`, `gyro_z`, `mag_x`, "
        "`mag_y`, `mag_z`, `cpu_temp`, `gpu_temp`, `in_temp`, `in_pressure`, `in_calc_alt`, "
        "`out_temp`, `out_pressure`, `out_calc_alt`) VALUES "
        "(%u, %u, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, "
        "%f, %f, %f, %f, %f)",
        data->gps.time_local, data->gps.time_gps, data->gps.lat, data->gps.lng, data->gps.gspeed,
        data->gps.sea_alt, data->gps.geo_alt, data->mot.acc_x, data->mot.acc_y, data->mot.acc_z,
        data->mot.gyro_x, data->mot.gyro_y, data->mot.gyro_z, data->mot.mag_x, data->mot.mag_y,
        data->mot.mag_z, data->amb.cpu_temp, data->amb.gpu_temp, data->amb.in_temp,
        data->amb.in_pressure, data->amb.in_calc_alt, data->amb.out_temp, data->amb.out_pressure,
        data->amb.out_calc_alt);

    if(dbman_connect() == 0) {
        retval = mysql_query_no_answer(query);
        dbman_disconnect();
        return retval;
    } else {
        return -1;
    }
}


/***********************************************************************************************//**
 * Fetches last latitude and longitude values from the remote MysQL database and returns them
 * through the pointers passed as argument.
 **************************************************************************************************/
int dbman_get_last_position(double *lat, double *lon, double *alt)
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    *lat = 0.0;
    *lon = 0.0;
    *alt = 0.0;

    if(dbman_connect() == 0) {
        if(mysql_query(mysql_handle, "SELECT lat, lng, sea_alt FROM " DB_TABLE_HK
                                     " ORDER BY `row_id` DESC LIMIT 0,1") == 0) {
            if((result = mysql_store_result(mysql_handle)) != NULL) {
                while((row = mysql_fetch_row(result)) != NULL) {
                    *lat = strtof(row[0], NULL);
                    *lon = strtof(row[1], NULL);
                    *alt = strtof(row[2], NULL);
                }
                mysql_free_result(result);
                dbman_disconnect();
                return 0;
            } else {
                /* Result was NULL. */
                return mysql_errno(mysql_handle);
            }
        } else {
            /* SQL query returned an error. */
            return mysql_errno(mysql_handle);
        }
    } else {
        return -1;
    }
}

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
 * Inserts GPS and temperature data into the remote MySQL database.
 **************************************************************************************************/
int dbman_save_gps_data(time_t time_local, time_t time_gps, const char * lat, const char * lng,
    const char * v_kph, const char * sea_alt, const char * geo_alt, const char * course,
    const char * temp)
{
    char query[500];
    int retval = 0;

    sprintf(query, "INSERT INTO " DB_TABLE_GPS " (`time_local`, `time_gps`, `lat`, `lng`, `v_kph`,"
                   " `sea_alt`, `geo_alt`, `course`, `temp`) VALUES "
                   "(%ld, %ld, '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
                   time_local, time_gps, lat, lng, v_kph, sea_alt, geo_alt, course, temp);

    if(dbman_connect() == 0) {
        retval = mysql_query_no_answer(query);
        dbman_disconnect();
        return retval;
    } else {
        return -1;
    }
}


/***********************************************************************************************//**
 * Inserts debug messages into the remote MySQL database.
 **************************************************************************************************/
int dbman_save_dbg_data(time_t time_local, time_t time_sbc, const char * message)
{
    char query[500];
    int retval = 0;

    sprintf(query, "INSERT INTO " DB_TABLE_DBG " (`time_local`, `time_sbc`, `message`) "
                   "VALUES (%ld, %ld, '%s')", time_local, time_sbc, message);
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
        if(mysql_query(mysql_handle, "SELECT lat, lng, sea_alt FROM " DB_TABLE_GPS
                                     " ORDER BY `row_id` DESC LIMIT 0,1") == 0) {
            if((result = mysql_store_result(mysql_handle)) != NULL) {
                while((row = mysql_fetch_row(result)) != NULL) {
                    *lat = strtod(row[0], NULL);
                    *lon = strtod(row[1], NULL);
                    *alt = strtod(row[2], NULL);
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
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

static int dbman_connect(void)
{
    if((mysql_handle = mysql_init(NULL)) != NULL) {
        if(mysql_real_connect(mysql_handle, DB_HOST, DB_USR, DB_PASSWD, DB_DBNAME, 0, NULL, 0) == NULL) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -2;
    }
}

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

int dbman_save_gps_data()
{
    if(dbman_connect() == 0) {
        mysql_query_no_answer("INSERT");
        dbman_disconnect();
        return 0;
    } else {
        return -1;
    }
}

int dbman_save_dbg_data()
{
    if(dbman_connect() == 0) {
        mysql_query_no_answer("INSERT");
        dbman_disconnect();
        return 0;
    } else {
        return -1;
    }
}

int dbman_get_last_position(double *lat, double *lon, double *alt)
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    *lat = 0.0;
    *lon = 0.0;
    *alt = 0.0;

    if(dbman_connect() == 0) {
        if(mysql_query(mysql_handle, "SELECT * FROM gps") == 0) {
            if((result = mysql_store_result(mysql_handle)) != NULL) {
                while((row = mysql_fetch_row(result)) != NULL) {
                    *lat = strtod(row[3], NULL);
                    *lon = strtod(row[4], NULL);
                    *alt = strtod(row[6], NULL);
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

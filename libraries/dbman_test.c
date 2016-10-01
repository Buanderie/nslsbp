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
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "dbman.h"

/***********************************************************************************************//**
 * Test program entry point.
 **************************************************************************************************/
int main(int argc, char **argv)
{
    double a, b, c;
    GPS_data gd;

    sprintf(gd.time_local, "%11ld", time(NULL));
    sprintf(gd.time_gps,   "%11ld", time(NULL) + 1);
    gd.lat = 41.3947688;
    gd.lng = 2.0787279;
    gd.v_kph = 0.0;
    gd.sea_alt = 100.0;
    gd.geo_alt = 111.1;
    gd.course = 22.2;
    gd.temp = 25.5;
    gd.cpu_temp = 47.7;
    gd.gpu_temp = 48.8;

    if(dbman_save_gps_data(&gd) == 0) {
        printf("GPS data saved successfully\n");
    } else {
        printf("Error saving GPS data\n");
    }

    if(dbman_save_dbg_data(time(NULL), time(NULL)+10, "This is a debug message") == 0) {
        printf("Debug data saved successfully\n");
    } else {
        printf("Error saving debug data\n");
    }

    if(dbman_get_last_position(&a, &b, &c) == 0) {
        printf("GPS data fetched: %f, %f, %f\n", a, b, c);
    } else {
        printf("Error fetching data\n");
    }

    memset(&gd, 0, sizeof(gd));
    if(dbman_get_gps_data(&gd) == 0) {
        printf("GPS data fetched:\n gd.time_local = %s,\n gd.time_gps = %s,\n gd.lat = %lf,\n "
                "gd.lng = %lf,\n gd.v_kph = %lf,\n gd.sea_alt = %lf,\n gd.geo_alt = %lf,\n "
                "gd.course = %lf,\n gd.temp = %lf,\n gd.cpu_temp = %lf,\n gd.gpu_temp = %lf\n",
                gd.time_local, gd.time_gps, gd.lat, gd.lng, gd.v_kph, gd.sea_alt, gd.geo_alt,
                gd.course, gd.temp, gd.cpu_temp, gd.gpu_temp);
    } else {
        printf("Error fetching data\n");
    }

    printf("GPS and Temperature data is %ld Bytes\n", sizeof(gd));
    return 0;
}

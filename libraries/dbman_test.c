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

/* No Makefile provided. Compile with:
    gcc -o dbman_test dbman_test.c -ldbman -L. -I../module_gps_temp/include
 */

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
    HKData d;

    /* Inserted values: */
    d.gps.time_local    = time(NULL);
    d.gps.time_gps      = time(NULL) + 1;
    d.gps.lat           = 1.f;
    d.gps.lng           = 2.f;
    d.gps.gspeed        = 3.f;
    d.gps.sea_alt       = 4.f;
    d.gps.geo_alt       = 5.f;
    d.mot.acc_x         = 6.f;
    d.mot.acc_y         = 7.f;
    d.mot.acc_z         = 8.f;
    d.mot.gyro_x        = 9.f;
    d.mot.gyro_y        = 10.f;
    d.mot.gyro_z        = 11.f;
    d.mot.mag_x         = 12.f;
    d.mot.mag_y         = 13.f;
    d.mot.mag_z         = 14.f;
    d.amb.cpu_temp      = 15.f;
    d.amb.gpu_temp      = 16.f;
    d.amb.in_temp       = 17.f;
    d.amb.in_pressure   = 18.f;
    d.amb.in_calc_alt   = 19.f;
    d.amb.out_temp      = 20.f;
    d.amb.out_pressure  = 21.f;
    d.amb.out_calc_alt  = 22.f;

    if(dbman_save_hk_data(&d) == 0) {
        printf("Housekeeping data saved successfully\n");
    } else {
        printf("Error saving GPS data\n");
    }

    if(dbman_get_last_position(&a, &b, &c) == 0) {
        printf("GPS data fetched: %f, %f, %f\n", a, b, c);
    } else {
        printf("Error fetching data\n");
    }

    memset(&d, 0, sizeof(d));
    if(dbman_get_hk_data(&d) == 0) {
        printf("GPS data fetched:\n"
                "  d.gps.time_local    = %u\n"
                "  d.gps.time_gps      = %u\n"
                "  d.gps.lat           = %.4f\n"
                "  d.gps.lng           = %.4f\n"
                "  d.gps.gspeed        = %.4f\n"
                "  d.gps.sea_alt       = %.4f\n"
                "  d.gps.geo_alt       = %.4f\n"
                "  d.mot.acc_x         = %.4f\n"
                "  d.mot.acc_y         = %.4f\n"
                "  d.mot.acc_z         = %.4f\n"
                "  d.mot.gyro_x        = %.4f\n"
                "  d.mot.gyro_y        = %.4f\n"
                "  d.mot.gyro_z        = %.4f\n"
                "  d.mot.mag_x         = %.4f\n"
                "  d.mot.mag_y         = %.4f\n"
                "  d.mot.mag_z         = %.4f\n"
                "  d.amb.cpu_temp      = %.4f\n"
                "  d.amb.gpu_temp      = %.4f\n"
                "  d.amb.in_temp       = %.4f\n"
                "  d.amb.in_pressure   = %.4f\n"
                "  d.amb.in_calc_alt   = %.4f\n"
                "  d.amb.out_temp      = %.4f\n"
                "  d.amb.out_pressure  = %.4f\n"
                "  d.amb.out_calc_alt  = %.4f\n",
                d.gps.time_local,
                d.gps.time_gps,
                d.gps.lat,
                d.gps.lng,
                d.gps.gspeed,
                d.gps.sea_alt,
                d.gps.geo_alt,
                d.mot.acc_x,
                d.mot.acc_y,
                d.mot.acc_z,
                d.mot.gyro_x,
                d.mot.gyro_y,
                d.mot.gyro_z,
                d.mot.mag_x,
                d.mot.mag_y,
                d.mot.mag_z,
                d.amb.cpu_temp,
                d.amb.gpu_temp,
                d.amb.in_temp,
                d.amb.in_pressure,
                d.amb.in_calc_alt,
                d.amb.out_temp,
                d.amb.out_pressure,
                d.amb.out_calc_alt);
    } else {
        printf("Error fetching data\n");
    }

    printf("The houskeeping data is stored in %ld Bytes\n", sizeof(d));
    return 0;
}

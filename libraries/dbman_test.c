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

#include "dbman.h"

/***********************************************************************************************//**
 * Test program entry point.
 **************************************************************************************************/
int main(int argc, char **argv)
{
    double a, b, c;
    if(dbman_save_gps_data(time(NULL), time(NULL)+10, 41.3947688, 2.0787279, 0.0, 100.00, 200.00, 1.0, 25.00) == 0) {
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
    return 0;
}

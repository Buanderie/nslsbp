/***********************************************************************************************//**
 *  \brief      Ground Station
 *  \details    Reads beacon data from the CC1101 sockets and drives the antenna motors accordingly
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       30-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

/*** INCLUDE SECTION ******************************************************************************/
#include "ground_station.h"

/*** GLOBAL VARIABLES *****************************************************************************/
double gs_lat;
double gs_lng;
double gs_alt;
bool gs_exit = false;
double az, el;

/***********************************************************************************************//**
 * Program entry point
 **************************************************************************************************/
int main(int argc, char ** argv)
{
    pthread_t rotors_th;
    time_t last_update_gps = -2;
    time_t last_update_sbc = -2;
    time_t time_local;
    GPS_data gd;
    double delta_lat, delta_lng, delta_alt, dist_x, dist_y;

    if(argc != 4) {
        printfe("Wrong number of arguments. Local (GS) latitude, longitude and altitude are required\n");
        printfd("Issue: ./ground_station <lat> <lon> <alt>\n");
        return -1;
    }

    gs_lat = strtod(argv[1], NULL);
    gs_lng = strtod(argv[2], NULL);
    gs_alt = strtod(argv[3], NULL);

    pthread_create(&rotors_th, NULL, rotor_control, NULL);

    time_local = time(NULL);
    printfd("Ground Station process started. " DBG_BLUE "Local time is %s" DBG_NOCOLOR, ctime(&time_local));
    printfd("Ground Station location (lat, lon, alt): %.6lf, %.6lf, %.1lf\n", gs_lat, gs_lng, gs_alt);
    while(!gs_exit) {
        if(dbman_get_gps_data(&gd) == 0) {
            if( (last_update_gps < strtol(gd.time_gps, NULL, 10)) ||
                (last_update_sbc < strtol(gd.time_local, NULL, 10))) {
                /* A new value arrived: */
                last_update_gps = strtol(gd.time_gps, NULL, 10);
                last_update_sbc = strtol(gd.time_local, NULL, 10);
                printfdg("[GPS data        ] lat = %.7lf, lng = %.7lf, sea_alt = %.2lf, geo_alt = %.2lf\n",
                    gd.lat, gd.lng, gd.sea_alt, gd.geo_alt);
                printfdg("[GPS data        ] time(SBC) = %s, time(GPS) = %s, vel = %.2lf, course = %.2lf\n",
                    gd.time_local, gd.time_gps, gd.v_kph, gd.course);
                if(gd.gpu_temp >= 60.0 || gd.cpu_temp >= 60.0) {
                    printfe("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", gd.temp, gd.cpu_temp, gd.gpu_temp);
                } else {
                    printfdg("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", gd.temp, gd.cpu_temp, gd.gpu_temp);
                }

                /* Calculate new azimuth and elevation: */
                delta_lat = gs_lat - gd.lat;
                delta_lng = gs_lng - gd.lng;
                delta_alt = gd.sea_alt - gs_alt;
                dist_x = delta_lat * KM_DEG_LAT;
                dist_y = delta_lng * KM_DEG_LNG * cos(gs_lat);
                printfd("Delta Lat: %lfº, Lng: %lfº, alt: %.1lfº; Distance: %.2ld km\n",
                    delta_lat, delta_lng, delta_alt, (sqrt((dist_x * dist_x) + (dist_y * dist_y)) / 1000.0));
                az = atan(delta_lng / delta_lat) * 180.0 / PI;
                if((delta_lat > 0.0 && delta_lng < 0.0) || (delta_lat < 0.0 && delta_lng < 0.0)) {
                    az += 180.0;
                }
                el = atan(delta_alt / sqrt((delta_lat * delta_lat) + (delta_lng * delta_lng)));
                printfo("New AZ: %.2lf, EL: %.2lf\n", az, el);
            }
        } else {
            printfe("Error retrieving beacon data\n");
        }
        sleep(1);
    }

    return 0;
}

/***********************************************************************************************//**
 * Rotor control thread routine
 **************************************************************************************************/
void * rotor_control(void * arg)
{
    int local_az, local_el;
    control_mode mode = MODE_MANUAL;

    while(!gs_exit) {

    }
    return NULL;
}

/***********************************************************************************************//**
 * Set a given azimuth and elevation to the rotors.
 **************************************************************************************************/
void set_az_el(int v_az, int v_el)
{

}

/***********************************************************************************************//**
 * Tell the rotors to go to calibration point "home".
 **************************************************************************************************/
void rotors_home(void)
{
    sleep(3);
    printfo("Antenna rotors calibrated\n");
}


/***********************************************************************************************//**
 * Returns the current time in the format %H:%M:%S.
 **************************************************************************************************/
const char * curr_time_format(void)
{
    time_t t; // Current time.
    static char retval[21];
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    // strftime(retval, 21, "%Y-%m-%d %H:%M:%S", tmp);
    strftime(retval, 21, "%H:%M:%S", tmp);

    return retval;
}

/***********************************************************************************************//**
 * Performs a read with timeouts.
 **************************************************************************************************/
int read_timed(int fd, void * buf, size_t count, int timeout)
{
    int             rdop, ready;
    fd_set          set;
    struct timeval  to;

    FD_ZERO(&set);
    FD_SET(fd, &set);

    to.tv_sec = 0;
    to.tv_usec = timeout * 1000;

    ready = select(fd+1, &set, NULL, NULL, &to);
    if(ready == 0)
    {
        // Timeout
        printfe("Read timed out\n");
        rdop = -1;
    }else if(ready > 0){
        // Ready
        rdop = read(fd,buf,count);
    }else{
        // Error
        printfe("Unknown error in select\n");
        rdop = -1;
    }

    return rdop;
}

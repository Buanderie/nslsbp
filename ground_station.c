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
static BeaconMessageHandler beacon_handler;
static bool gs_exit = false;
static int az, el;

/***********************************************************************************************//**
 * Program entry point
 **************************************************************************************************/
int main(int argc, char ** argv)
{
    char buf[MSG_LENGTH];
    MsgSource process_id;
    char str0[MSG_LENGTH];
    char str1[MSG_LENGTH];
    int hour, minute, second;
    struct tm * time_fields;
    time_t time_sbc;
    time_t time_local;
    time_t time_gps;
    int gps_quality, gps_sv;
    double lat, lng, v_kph, sea_alt, geo_alt, course, temp, cpu_temp, gpu_temp;
    pthread_t rotors_th;

    printfd("Connecting to beacon socket...\n");
    if(BeaconConnect(SOCK_IP_ADDR, SOCK_PORT, &beacon_handler, beacon_receiver) <= 0) {
        printfe("Could not connect to the beacon socket\n");
        return -1;
    }
    printfo("Beacon socket connection accepted\n");

    printfd("Enter GS latitude: ");
    fflush();
    scanf("%f", )
    pthread_create(rotors_th, NULL, rotor_control, NULL);

    time_local = time(NULL);
    printfd("Ground Station process started. Local time is %s\n", ctime(&time_local));
    while(1) {
#ifdef FAKE_BEACON_MSG
        sleep(5);
        int v_rand = rand() % 100;
        lat = 41.391832 + ((rand() % 2 ? 1.0 : -1.0) * (rand() % 100)/10000.0);
        lng = 2.1155701 + ((rand() % 2 ? 1.0 : -1.0) * (rand() % 100)/10000.0);
        if(v_rand > 50 && v_rand < 75) {
            /* Generate a VITOW message */
            sprintf(buf, "01:02:03|VITOW debug message");
            process_id = VITOW;
        } else if(v_rand >= 75) {
            /* Generate a SYSTEM message */
            sprintf(buf, "60:50:40|SYSTEM debug message");
            process_id = SYSTEM;
        } else {
            /* Generate a GPS_TEMP message */
            sprintf(buf, "%ld,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                time(NULL), (rand() % 100), 2, lat, lng, 100.0, 10.01, 20.02, 180.0, 22.5);
            process_id = GPS_TEMP;
        }
        if(1) {
#else
        if(BeaconRead(&beacon_handler, (unsigned char *)buf, MSG_LENGTH, &process_id) > 0) {
#endif
            switch(process_id) {
                case SYSTEM:
                case VITOW:
                    /* This is a debug message from VITOW or xlauncher. */
                    if(sscanf(buf, "%[^|\n\t\r ] %*[|] %s", str0, str1) == 2) {
                        /* Match: */
                        time_local = time(NULL);
                        if(sscanf(str0, "%d:%d:%d", &hour, &minute, &second) == 3) { /* Time detected. */
                            time_fields = localtime(&time_local);
                            time_fields->tm_sec = second % 60;
                            time_fields->tm_min = minute % 60;
                            time_fields->tm_hour = hour % 24;
                            time_sbc = mktime(time_fields);
                        } else {
                            time_sbc = 0;
                        }
                        printfd("[dbg msg %s] %s\n", str0, str1);
                        dbman_save_dbg_data(time_local, time_sbc, str1);
                    } else {
                        printfw("Beacon with unexpected format: %s\n", buf);
                    }
                    break;
                case GPS_TEMP:
                    /* This is GPS data. */
                    sscanf(buf, "%ld,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                        &time_gps, &gps_quality, &gps_sv, &lat, &lng, &v_kph, &sea_alt, &geo_alt,
                        &course, &temp, &cpu_temp, &gpu_temp);
                    printfdg("[GPS data Q:%-5d] lat = %.7lf, lng = %.7lf, sea_alt = %.2lf, geo_alt = %.2lf\n",
                        gps_quality, lat, lng, sea_alt, geo_alt);
                    printfdg("[GPS data Q:%-5d] time = %ld, gps_sv = %d, vel = %.2lf, course = %.2lf\n",
                        gps_quality, time_gps, gps_sv, v_kph, course);
                    if(gpu_temp >= 50.0 || cpu_temp >= 50.0) {
                        printfe("[GPS data Q:%-5d] Temp. sensor: %.1lf ºC, CPU temp: %.1lf, GPU temp: %.1lf \n", gps_quality, temp, cpu_temp, gpu_temp);
                    } else {
                        printfdg("[GPS data Q:%-5d] Temp. sensor: %.1lf ºC, CPU temp: %.1lf, GPU temp: %.1lf \n", gps_quality, temp, cpu_temp, gpu_temp);
                    }
                    dbman_save_gps_data(time(NULL), time_gps, lat, lng, v_kph, sea_alt, geo_alt, course, temp, cpu_temp, gpu_temp);

                    break;
                default:
                    break;
            }
        } else {
            printfe("Error reding from the beacon socket\n");
            sleep(1);
        }
    }

    BeaconClose(&beacon_handler);
    return 0;
}

/***********************************************************************************************//**
 * Rotor control thread routine
 **************************************************************************************************/
void rotor_control(void)
{
    int local_az, local_el;
    control_mode mode = MODE_MANUAL;

    while(!gs_exit) {

    }
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

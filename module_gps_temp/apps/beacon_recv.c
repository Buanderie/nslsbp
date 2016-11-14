#if 0
#include <stdio.h>
#include <unistd.h>     //Used for UART
#include <fcntl.h>      //Used for UART
#include <termios.h>    //Used for UART
#include <stdlib.h>
#include <string.h>

#include "../../libraries/dbman.h"

#include <cc_beacon_iface.h>

#include <sensors.h>

void main( void ) {
    int beacon_handler = BeaconConnect("localhost", "52001", beacon_receiver);
    hk_data_t data;
    printf("Size of sending struct: %d\n", (int)sizeof(hk_data_t));
    printf("Size of gps_data: %d\n", (int) sizeof(_gps_data));
    printf("Size of motion_data: %d\n", (int) sizeof(_motion_sensors));
    printf("Size of ambient_data: %d\n", (int) sizeof(_ambient_sensors));
  /* Display measurements: µs, m/s/s, deg/s, µG respectively */
    printf("Connection done, going to receive data from beacon\n");
    while(1){
        if (BeaconRead(beacon_handler, (void *) &data, sizeof(hk_data_t)) > 0){
            printf("Lat: %f, Long: %f, GSpeed: %f\n", data.gps.lat, data.gps.lng, data.gps.gspeed);

            printf("\tIn Temp: %f Press: %f, Alt: %f\n", data.amb.in_temp, data.amb.in_pressure, data.amb.in_calc_alt);  
            printf("\tIn Temp CPU: %f Temp GPU %f\n", data.amb.cpu_temp, data.amb.gpu_temp);

            printf( "\tAx: %-8.2f Ay: %-8.2f Az: %-8.2f\n", data.mot.acc_x, data.mot.acc_y, data.mot.acc_z);
            printf( "\tGx: %-8.2f Gy: %-8.2f Gz: %-8.2f\n", data.mot.gyro_x, data.mot.gyro_y, data.mot.gyro_z);
            printf( "\tMx: %-8.2f My: %-8.2f Mz: %-8.2f\n", data.mot.mag_x, data.mot.mag_y, data.mot.mag_z);
        }
    }
}
#endif
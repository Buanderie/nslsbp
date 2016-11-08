#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <stdint.h>

typedef struct _ambient_sensors{
	float cpu_temp;
	float gpu_temp;
	float in_temp;
	float in_pressure;
	float in_calc_alt;
    float out_temp;
    float out_pressure;
    float out_calc_alt;
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
	uint32_t time_local;
	uint32_t time_gps;
    float lat;
    float lng;
    float gspeed; /* this is ground speed in mm/second?? */
    float sea_alt;
    float geo_alt;
}_gps_data;

#endif
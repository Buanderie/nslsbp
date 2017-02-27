#ifndef __NSL_STRATO_BALLOON_PAYLOAD_H__
#define __NSL_STRATO_BALLOON_PAYLOAD_H__

#include "sensors.h"

typedef struct hk_data_s {
    _gps_data gps;
    _motion_sensors mot;
    _ambient_sensors amb;
} hk_data_t;

typedef struct tc_data_s{
	unsigned int cmd;
	unsigned int ping;
}tc_data_t;

typedef struct gps_data_s{
	unsigned int time;
	float lat;
	float lng;
	float alt;
}gps_data_t;

typedef struct imu_data_s{
	float roll;
	float pitch;
	float yaw;
}imu_data_t;

typedef struct vc_data_s{
	float vsys_v;
	float vsys_i;
	float vsys_w;
	unsigned char enabled_3;
	float v3_v;
	float v3_i;
	float v3_w;
	unsigned char enabled_5;
	float v5_v;
	float v5_i;
	float v5_w;
	unsigned char enabled_12;
	float v12_v;
	float v12_i;
	float v12_w;
}vc_data_t;

typedef struct therm_data_s{
	unsigned char auto_manual_flag; /* 1 means auto, 0 means manual */
	float out_t;
	float gen_t;
	unsigned char gen_enabled;
	float bat_t;
	unsigned char bat_enabled;
	float pay_t;
	unsigned char pay_enabled;
}therm_data_t;

typedef struct xbee_hk_data_s{
	tc_data_t 		tc;
	gps_data_t 		gps;
	imu_data_t 		imu;
	vc_data_t  		vc;
	therm_data_t 	therm;
}xbee_hk_data_t;


typedef struct balloon_data_s{
	hk_data_t 			hk;
	xbee_hk_data_t 		xbee;
} balloon_data_t;

#endif

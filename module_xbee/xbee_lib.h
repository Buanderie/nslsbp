#ifndef __XBEE_LIB_H__
#define __XBEE_LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>

#include "nslsbp.h"
#include "dbman.h"


#define xbee_delay_ms(x) usleep( x * 1000 )

typedef struct xbee_serial_parms_s{
	int 			fd;
	int 			ret;
	unsigned char 	buffer[256];
	unsigned int 	timeout;
}xbee_serial_parms_t;

typedef enum telemetry_modules_e{
	EEPROM,
	GPS_INIT,
	IMU_INIT,
	VC_INIT,
	VC_ERROR,
	THERM_INIT,
	THERM_ERROR,
	HEATERS_INIT,
	SD_INIT,
	TELECOMMAND,
	GPS,
	IMU,
	VCSENS,
	THERMAL,
	TELEMETRY_MESSAGE_COUNT,
}telemetry_modules_t;


#endif

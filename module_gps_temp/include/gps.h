#ifndef __GPS_H__
#define __GPS_H__

#include "../../libraries/dbman.h"
#include <time.h>
#include <sensors.h>

/* header (2)	class (1) 	id (1) 	length (2)	payload (xx) crc (2) */

typedef enum MessageType{	
	CFG_DISABLE_NMEA, /* disable nmea output on UART */
	CFG_NAV5,	/* Navigation Engine Settings */
	CFG_NAVX5,	/* Navigation Engine Expert Settings */
	CFG_NMEA,	/* NMEA output configuration */
	NAV_PVT,	/* get the NAV-PVT message using UBLOX */
}MessageType;

typedef enum MessageIdentifier{
	NMEA_ID = 0x24,
	UBX_ID 	= 0xB5,
	ERROR 	= -1,
}MessageIdentifier;

int OpenGPSIface(int ms_timeout);
int GPSReceiveMessage(int uart_fd, unsigned char * recv_message, int * len);
int SetGPSMessage(int uart_fd, MessageType t);
int GetGPSMessage(int uart_fd, MessageType t, unsigned char * msg_out);
void ParseUBXPVT(unsigned char * buff, int len, _gps_data * gps);

#endif
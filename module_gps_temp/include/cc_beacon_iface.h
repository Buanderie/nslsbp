
#ifndef __CC_BEACON_IFACE_H__
#define __CC_BEACON_IFACE_H__

#ifndef BYTE
#define BYTE unsigned char
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdint.h>

typedef enum MsgSource{
	VITOW,
	GPS_TEMP,
	SYSTEM,
}MsgSource;

typedef enum ConnectionPurpose{
	beacon_sender,
	beacon_receiver,
}ConnectionPurpose;

/* Init functions */
int 	BeaconConnect (const char * ip, const char * port, ConnectionPurpose trx);
void 	BeaconClose (int fd);

/* Reading functions */
int 	BeaconWrite (int fd, const void * msg, int32_t len);
int 	BeaconRead (int fd, void * msg, int32_t maxbuflen); /* returns the source if wanted */

#endif

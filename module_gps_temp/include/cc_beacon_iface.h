
#ifndef __CC_BEACON_IFACE_H__
#define __CC_BEACON_IFACE_H__

/* Init functions */
int 	BeaconConnect ();
void 	BeaconClose ();

/* Reading functions */
int 	BeaconWrite (const void * msg, int len);
int 	BeaconRead (void * msg, int maxbuflen); /* returns the source if wanted */

#endif

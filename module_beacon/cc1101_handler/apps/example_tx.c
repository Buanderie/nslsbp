#include <stdio.h>
#include <string.h>
#include <cc_beacon_iface.h>
#include <stdlib.h>
#include <unistd.h>

int main (void)
{
	BYTE buff[20];
	strcpy((char *) buff, "HOLAHOLA");
	BeaconMessageHandler b;
	MsgSource m = SYSTEM;
	int fd = BeaconConnect("localhost", "52001", &b, beacon_sender);
	while(1)
	{
		BeaconWrite(&b, buff, strlen((char * )buff)+1, m);
		sleep(1);
	}
	exit(0);
}

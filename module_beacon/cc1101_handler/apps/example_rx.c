#include <stdio.h>
#include <string.h>
#include <cc_beacon_iface.h>
#include <stdlib.h>
#include <unistd.h>

int main (void)
{
	BYTE buff[100];
	strcpy((char *) buff, "HOLAHOLA");
	BeaconMessageHandler b;
	MsgSource m = SYSTEM;
	int fd = BeaconConnect("localhost", "52001", &b, beacon_receiver);
	while(1)
	{
		BeaconRead(&b, buff, 100, &m);
		printf("Buff: %s\n", buff);
	}
	exit(0);
}

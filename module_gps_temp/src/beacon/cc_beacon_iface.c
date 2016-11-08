#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <poll.h>
#include <stdlib.h>

#include <cc_beacon_iface.h>

static int InitClientSocket(const char * ip, const char * port, ConnectionPurpose trx)
{
    int sockfd, portno, n;
    int serverlen;
    int set_option_on = 1;
    struct sockaddr_in serveraddr, cli_addr;
    struct hostent *server;
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(0);
    }

    /* gethostbyname: get the server's DNS entry */
    portno = atoi(port);
    server = gethostbyname(ip);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host as %s\n", ip);
        exit(0);
    }
    serverlen = sizeof(serveraddr);
    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
      (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    if (trx == beacon_sender){
    	if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        	perror("connect error");
        	exit(-1);
    	}
        return sockfd;
    }
    if (trx == beacon_receiver){
       if (bind(sockfd, (struct sockaddr *) &serveraddr,
              sizeof(serveraddr)) < 0) 
              perror("ERROR on binding");
        listen(sockfd, 5);
        int clilen = sizeof(cli_addr);
        return (accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen));  
    }
}

int BeaconConnect (const char * ip, const char * port, ConnectionPurpose trx)
{
	return InitClientSocket(ip, port, trx);
}

void BeaconClose (int fd)
{
	close (fd);		
}

/* Just to ensure is a 32 bits int -> int32_t */
int BeaconWrite (int fd, const void * msg, int32_t len)
{
	int ret = -1;
	/* if write returns -1, error */
	if (send(fd, &len, sizeof(int32_t), 0) > 0){
		ret = send(fd, msg, len, 0);
	}
	return ret;
}

/* */
int BeaconRead (int fd, void * msg, int32_t maxbuflen)
{
	BYTE buffer[maxbuflen];
	int len = 0;
	int ret = 0;
	/* blocking read waiting for a beacon */
	if (read(fd, &len, sizeof(int32_t)) > 0 ){
		if (len <= maxbuflen){
			ret = read(fd, msg, len);
			/* ensure the whole message has been readed */
			while (ret != len){
				ret += read(fd, msg+ret, len-ret);
			}
			return len;
		}else{
			return 0;
		}
	}else{
		/* End of socket */
		return -1;
	}
}

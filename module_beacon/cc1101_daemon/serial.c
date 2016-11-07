/******************************************************************************/
/* PiCC1101  - Radio serial link using CC1101 module and Raspberry-Pi         */
/*                                                                            */
/* Serial definitions                                                         */
/*                                                                            */
/*                      (c) Edouard Griffiths, F4EXB, 2015                    */
/*                                                                            */
/******************************************************************************/

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

#include "main.h"
#include "serial.h"

/* Have to be rewritten:
    - set_serial_parameters -> this opens the UNIX SOCKET
    - write_serial -> write to socket
    - read_serial -> read non block using POLL
*/

// ------------------------------------------------------------------------------------------------
// Get serial speed
speed_t get_serial_speed(uint32_t speed, uint32_t *speed_n)
// ------------------------------------------------------------------------------------------------
{
    printf("Get serial speed has no sense, now is a UNIX SOCKET\n");
    return B0;
}

int openTCPSocket(char * port_num){
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(port_num);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
    return newsockfd;
}

int openUnixSocket(char * sock_path){
    struct sockaddr_un addr;
    int fd;
    int client_fd;
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    unlink(sock_path);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    /** Give permission to the file, so others can access without SUDO in case if needed */

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    if ( (client_fd = accept(fd, NULL, NULL)) == -1) {
        perror("accept error");
        exit(-1);
    }
    return client_fd;
}
static struct sockaddr_in serveraddr;

int openUDPSocket(char * port, int trx)
{
    int sockfd, portno, n;
    int serverlen;
    int set_option_on = 1;
    struct hostent *server;
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(0);
    }

    /* gethostbyname: get the server's DNS entry */
    portno = atoi(port);
    server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host as %s\n", "localhost");
        exit(0);
    }
    serverlen = sizeof(serveraddr);
    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
    (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* meaning 0 as we will RX from air and forward (sendto) */
    /* meaning 1 as we will TX to air and recv from udp (recvfrom) */
    if (trx == 1)
    {
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &set_option_on, sizeof(set_option_on)) != 0)
      {
        fprintf(stderr, "set sockopt failed [%s]\n", strerror(errno));
        exit(0);    
      }
      if (bind(sockfd, (struct sockaddr *)&serveraddr, serverlen) != 0)
      {
         fprintf(stderr, "bind failed [%s]\n", strerror(errno));
         /* Address is binded, some is doing */
      }
    }
    return sockfd;
}

void set_serial_parameters(serial_t *serial_parameters, arguments_t *arguments)
{
    /* socket init */
//    serial_parameters->sock_fd = openUnixSocket(arguments->serial_device);
//      serial_parameters->sock_fd = openTCPSocket("52001");
      /* set the intention, to rx or to tx */
      serial_parameters->sock_fd = openUDPSocket("52001", arguments->trx);
}

int check_serial(serial_t *serial_parameters)
{
    struct pollfd pfd;
    int rv;
    int len = 256;
    char buf[256];
    pfd.fd = serial_parameters->sock_fd;
    pfd.events = POLLIN;
    rv = poll(&pfd, 1, 0);
    if (rv == -1){
        perror("Poll error: ");
        return -1;
    }else if (rv == 0){
        return 1;
    }else{
        return read(pfd.fd, buf, len);
    }
}

int write_serial(serial_t *serial_parameters, char *msg, int msglen)
// ------------------------------------------------------------------------------------------------
{
    /* write msglen as int32_t, then write the message */
    int32_t len = (int32_t) msglen;
    #ifdef __USE_SOCAT__
    return write(serial_parameters->sock_fd, msg, len);
    #else
    if (sendto(serial_parameters->sock_fd, &len, sizeof(int32_t), 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) > 0)
    {
      return (sendto(serial_parameters->sock_fd, msg, len, 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr)));
    }
    else
    {
        printf("Error writing\n");
        return 0;
    }
    #endif
}

// ------------------------------------------------------------------------------------------------
// Read from serial interface
int read_serial(serial_t *serial_parameters, char *buf, int buflen)
// ------------------------------------------------------------------------------------------------
{
    struct pollfd pfd;
    int rv;
    int32_t len;
    int readed;
    pfd.fd = serial_parameters->sock_fd;
    pfd.events = POLLIN;
    rv = poll(&pfd, 1, -1); /* read without timeout */
    if (rv == -1){
        perror("Poll error: ");
        return -1;
    }else if (rv == 0){
        printf("Error reading\n");
        return -1;
    }else{
        if (pfd.revents & POLLIN){
            #ifdef __USE_SOCAT__
            return read(pfd.fd, buf, buflen);
            #else
            if (read(pfd.fd, &len, sizeof(int32_t)) > 0){
                if (len <= buflen){
                     readed = read(pfd.fd, buf, len);
                     while(readed != len){
                        readed += read(pfd.fd, buf+readed, len-readed);
                     }
                     return len;
                }else{
                    return 0;
                }
            }else{
                return 0;
            }
            #endif
        }
    }
} 

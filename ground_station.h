/***********************************************************************************************//**
 *  \brief      Ground Station
 *  \details    Reads beacon data from the CC1101 sockets and drives the antenna motors accordingly
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       30-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

#ifndef __GROUND_STATION_H__
#define __GROUND_STATION_H__

/*** INCLUDES *************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "dbman.h"

/*** PARAMETERS ***********************************************************************************/
#define PI              3.14159265
#define EARTH_RADIUS    6371e3 /* in metres. */
#define DEG2RAD(x)      (x * PI / 180.0)
#define RAD2DEG(x)      (x * 180.0 / PI)

#define KEY_MENU_UP         '8'
#define KEY_MENU_DOWN       '2'
#define KEY_MENU_LEFT       '4'
#define KEY_MENU_RIGHT      '6'
#define KEY_MENU_HOME       '0'
#define KEY_MENU_CENTER     '5'
#define KEY_MENU_QUIT       'q'
#define KEY_MENU_QUITC      'Q'
#define KEY_MENU_HELP       'h'
#define KEY_MENU_HELPC      'H'
#define KEY_MENU_COMMAND    '+'
#define KEY_MENU_QUERY      '*'

#define DBG_REDB        "\x1b[31;1m"
#define DBG_REDD        "\x1b[31m"
#define DBG_GREENB      "\x1b[32;1m"
#define DBG_GREEND      "\x1b[32m"
#define DBG_BLUE        "\x1b[34;1m"
#define DBG_YELLOW      "\x1b[33;1m"
#define DBG_WHITEB      "\x1b[37;1m"
#define DBG_GREY        "\x1b[30;1m"
#define DBG_NOCOLOR     "\x1b[0m"

/*** GLOBAL CONSTANTS: ****************************************************************************/


/*** MACROS: **************************************************************************************/
#ifdef GROUND_STATION_DEBUG
    #define printfd(fmt, ...) do { \
            printf("\r[groundst:%s] (" DBG_BLUE    "d" DBG_NOCOLOR ") %c " \
            fmt "\r", curr_time_format(), (mode == MODE_AUTO ? '+' : ' '), ## __VA_ARGS__); \
        } while(0)
    #define printfdg(fmt, ...) do { \
            printf("\r[groundst:%s] (" DBG_BLUE    "d" DBG_NOCOLOR ") %c " DBG_GREY \
            fmt DBG_NOCOLOR "\r", curr_time_format(), (mode == MODE_AUTO ? '+' : ' '), ## __VA_ARGS__); \
        } while(0)
    #define printfe(fmt, ...) do { \
            printf("\r[groundst:%s] (" DBG_REDB    "E" DBG_NOCOLOR ") %c " \
            DBG_REDD fmt DBG_NOCOLOR "\r", curr_time_format(), (mode == MODE_AUTO ? '+' : ' '), ## __VA_ARGS__); \
        } while(0)
    #define printfw(fmt, ...) do { \
            printf("\r[groundst:%s] (" DBG_YELLOW  "W" DBG_NOCOLOR ") %c " \
            fmt "\r", curr_time_format(), (mode == MODE_AUTO ? '+' : ' '), ## __VA_ARGS__); \
        } while(0)
    #define printfo(fmt, ...) do { \
            printf("\r[groundst:%s] (" DBG_GREENB  "o" DBG_NOCOLOR ") %c " \
            DBG_GREEND fmt DBG_NOCOLOR "\r", curr_time_format(), (mode == MODE_AUTO ? '+' : ' '), ## __VA_ARGS__); \
        } while(0)
#else
    #define printfd(fmt, ...) do { } while (0)
#endif

/*** TYPEDEFS *************************************************************************************/
typedef enum control_mode {MODE_MANUAL, MODE_AUTO} control_mode;



/*** GLOBAL VARIABLES: ****************************************************************************/
extern double gs_lat;
extern double gs_lng;
extern double gs_alt;
extern bool gs_exit;
extern double az, el;
extern char tty_dev_name[26];
extern int fd;
extern control_mode mode;
extern struct termios orig_termios;  /* TERMinal I/O Structure */
extern bool req_el_up;
extern bool req_el_down;
extern bool req_az_cw;
extern bool req_az_ccw;
extern bool req_go_home;

/*** FUNCTION HEADERS *****************************************************************************/
void print_menu(void);
const char * curr_time_format(void);
void rotors_get_az_el(int fd, double * az, double * el);
void rotors_set_az_el(int fd, double az, double el);
void rotors_home(int fd);
void * rotor_control(void * arg);

void init_rotor_control (int fd);
int open_rotor_interface(const char * tty_path);
int uart_write(int fd, void *buffer, int size);
int uart_read(int fd, unsigned char *buffer, int * buf_size, long long timeout);

void tty_raw(void);
void exit_ground_station(void);

#endif

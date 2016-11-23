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

/*** INCLUDE SECTION ******************************************************************************/
#include "ground_station.h"

/*** GLOBAL VARIABLES *****************************************************************************/
double gs_lat;
double gs_lng;
double gs_alt;
bool gs_exit = false;
double az, el;
char tty_dev_name[26];
int fd;
control_mode mode = MODE_MANUAL;
struct termios orig_termios;  /* TERMinal I/O Structure */
bool req_el_up = false;
bool req_el_down = false;
bool req_az_cw = false;
bool req_az_ccw = false;
bool req_go_home = false;

/***********************************************************************************************//**
 * Program entry point
 **************************************************************************************************/
int main(int argc, char ** argv)
{
    pthread_t rotors_th;
    time_t last_update_gps = -2;
    time_t last_update_sbc = -2;
    time_t time_local;
    HKData hk;
    double delta_lat, delta_lng, delta_alt, dist;
    double aux, arc, x, y;

    if(argc != 5) {
        printfe("Wrong number of arguments. Local (GS) device, latitude, longitude and altitude are required\n");
        printfd("Issue: ./ground_station </dev/tty...> <lat> <lon> <alt>\n");
        return -1;
    }
    /* Save arguments: -------------------------------------------------------------------------- */
    strncpy(tty_dev_name, argv[1], 26);
    gs_lat = DEG2RAD(strtod(argv[2], NULL));
    gs_lng = DEG2RAD(strtod(argv[3], NULL));
    gs_alt = strtod(argv[4], NULL);

    /* Configure terminal and user input settings: ---------------------------------------------- */

    if(!isatty(0)) {   /* Check that input is from a tty */
        printfe("Error, terminal is not a tty\n");
        return -1;
    }
    /* Store current tty settings in orig_termios: */
    if(tcgetattr(0, &orig_termios) < 0) {
        printfe("Unable to store original tty settings. Existing now\n");
        return -1;
    }
    if(atexit(exit_ground_station) != 0) {
        printfw("Could not register exit function\n");
    }
    tty_raw();  /* Set tty in raw mode */

    fd = open_rotor_interface(tty_dev_name);
    init_rotor_control(fd);

    pthread_create(&rotors_th, NULL, rotor_control, NULL);

    time_local = time(NULL);
    printfd("Ground Station process started. " DBG_BLUE "Local time is %s" DBG_NOCOLOR, ctime(&time_local));
    printfd("Ground Station location (lat, lon, alt): %.6lf, %.6lf, %.1lf\n", gs_lat, gs_lng, gs_alt);

    /* Set initial conditions: */
    dbman_get_hk_data(&hk);
    last_update_gps = hk.gps.time_gps;
    last_update_sbc = hk.gps.time_local;

    /* Start control loop: */
    while(!gs_exit) {
        if(dbman_get_hk_data(&hk) == 0) {
            if( (last_update_gps < hk.gps.time_gps) ||
                (last_update_sbc < hk.gps.time_local)) {
                /* A new value arrived: */
                last_update_gps = hk.gps.time_gps;
                last_update_sbc = hk.gps.time_local;
                printfdg("[GPS data        ] lat = %.7lf, lng = %.7lf, sea_alt = %.2lf, geo_alt = %.2lf\n",
                    hk.gps.lat, hk.gps.lng, hk.gps.sea_alt, hk.gps.geo_alt);
                printfdg("[GPS data        ] time(SBC) = %u, time(GPS) = %u, vel = %.2lf\n",
                    hk.gps.time_local, hk.gps.time_gps, hk.gps.gspeed);
                if(hk.amb.gpu_temp >= 60.0 || hk.amb.cpu_temp >= 60.0) {
                    printfe("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", hk.amb.in_temp, hk.amb.cpu_temp, hk.amb.gpu_temp);
                } else {
                    printfdg("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", hk.amb.in_temp, hk.amb.cpu_temp, hk.amb.gpu_temp);
                }


                /* Calculate new azimuth and elevation using Haversine formulas to calculate great-circle
                 * distances. Implementation is extracted from the following resource:
                 * ** http://www.movable-type.co.uk/scripts/latlong.html
                 */
                hk.gps.lat = DEG2RAD(hk.gps.lat);
                hk.gps.lng = DEG2RAD(hk.gps.lng);
                delta_lat = hk.gps.lat - gs_lat;
                delta_lng = hk.gps.lng - gs_lng;
                delta_alt = hk.gps.sea_alt - gs_alt;
                aux = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) +
                      cos(gs_lat) * cos(hk.gps.lat) *
                      sin(delta_lng / 2.0) * sin(delta_lng / 2.0);

                arc = 2.0 * atan2(sqrt(aux), sqrt(1 - aux));    /* Great-circle distance in Radians.    */
                dist = arc * EARTH_RADIUS;                      /* Great-circle distance in kilometers. */

                /* Formula: θ = atan2( sin Δλ ⋅ cos φ2 , cos φ1 ⋅ sin φ2 − sin φ1 ⋅ cos φ2 ⋅ cos Δλ )
                 *  where   φ1,λ1 is the start point, φ2,λ2 the end point (Δλ is the difference in longitude)
                 */
                if(mode == MODE_AUTO) {
                    y = sin(delta_lng) * cos(hk.gps.lng);
                    x = (cos(gs_lat) * sin(hk.gps.lat)) - (sin(gs_lat) * cos(hk.gps.lat) * cos(delta_lng));
                    az = RAD2DEG(atan2(y, x));
                    az = fmod((az + 360.0), 360.0);

                    el = RAD2DEG(atan((delta_alt) / dist));
                    printfo("Distance: %.3lf km., Azimuth: %.1lf, Elevation: %.1lf\n", dist / 1000.0, az, el);
                }
                if(mode == MODE_AUTO) {
                    rotors_set_az_el(fd, az, el);
                }

            }
        } else {
            //printfe("Error retrieving beacon data\n");
        }
        if(mode == MODE_MANUAL) {
            if(req_go_home) {
                az = 0.0;
                el = 0.0;
                rotors_home(fd);
                req_go_home = false;
            } else if(req_el_up) {
                if(el < 85.0) {
                    el += 5.0;
                    rotors_set_az_el(fd, az, el);
                } else if(el > 85.0 && el <= 90.0) {
                    el = 90.0;
                    rotors_set_az_el(fd, az, el);
                } else {
                    printfw("Elevation is 90º\n");
                }
                req_el_up = false;
            } else if(req_el_down) {
                if(el > 5.0) {
                    el -= 5.0;
                    rotors_set_az_el(fd, az, el);
                } else if(el > 0.0 && el <= 5.0) {
                    el = 0.0;
                    rotors_set_az_el(fd, az, el);
                } else {
                    printfw("Elevation is 0º\n");
                }
                req_el_down = false;
            } else if(req_az_cw) {
                if(az < 354.0) {
                    az += 5.0;
                    rotors_set_az_el(fd, az, el);
                } else if(az > 354.0 && az <= 359.0) {
                    az = 359.0;
                    rotors_set_az_el(fd, az, el);
                } else {
                    printfw("Azimuth is 359º\n");
                }
                req_az_cw = false;
            } else if(req_az_ccw) {
                if(az > 5.0) {
                    az -= 5.0;
                    rotors_set_az_el(fd, az, el);
                } else if(az > 0.0 && az <= 5.0) {
                    az = 0.0;
                    rotors_set_az_el(fd, az, el);
                } else {
                    printfw("Azimuth is 0º\n");
                }
                req_az_ccw = false;
            }
        }
        usleep(500000);
    }

    if(fd > 0) {
        close(fd);
    }

    exit(0);
    return 0;
}


/***********************************************************************************************//**
 * Rotor control thread routine
 **************************************************************************************************/
void * rotor_control(void * arg)
{
    int r_op;
    char buf;
    double real_az = 0.0, real_el = 0.0;

    while(!gs_exit) {
        r_op = read(0, &buf, 1);
        if(r_op == 1) {
            switch(buf) {
                case KEY_MENU_UP:
                    if(mode == MODE_MANUAL) {
                        req_el_up = true;
                    }
                    break;
                case KEY_MENU_DOWN:
                    if(mode == MODE_MANUAL) {
                        req_el_down = true;
                    }
                    break;
                case KEY_MENU_LEFT:
                    if(mode == MODE_MANUAL) {
                        req_az_ccw = true;
                    }
                    break;
                case KEY_MENU_RIGHT:
                    if(mode == MODE_MANUAL) {
                        req_az_cw = true;
                    }
                    break;
                case KEY_MENU_HOME:
                    if(mode == MODE_MANUAL) {
                        req_go_home = true;
                    }
                    break;
                case KEY_MENU_CENTER:
                    if(mode == MODE_AUTO) {
                        mode = MODE_MANUAL;
                        printfd("Manual mode set\n");
                    } else {
                        mode = MODE_AUTO;
                        printfd("Auto mode set\n");
                    }
                    break;
                case KEY_MENU_QUERY:
                    if (req_go_home == false){
                        printfd("Set   azimuth = %.2lf º;  elevation = %.2lf º\n", az, el);
                        real_az = 0.0;
                        real_el = 0.0;
                        rotors_get_az_el(fd, &real_az, &real_el);
                        printfd("Read  azimuth = %.2lf º;  elevation = %.2lf º\n", real_az, real_el);
                    }else{
                        printfe("Rotors cannot be queried while autohome\n");
                    }
                    break;
                case KEY_MENU_HELP:
                case KEY_MENU_HELPC:
                    print_menu();
                    break;
                case KEY_MENU_QUIT:
                case KEY_MENU_QUITC:
                    gs_exit = true;
                    break;
                case KEY_MENU_COMMAND:
                    printfw("Command mode not implemented\n");
                    break;
            }
        }
    }
    return NULL;
}

void print_menu(void) {
    printfd(DBG_WHITEB "MENU   COMMAND            KEY      COMMAND             KEY\n" DBG_NOCOLOR);
    printfd("       Azimuth CCW (+5º)  [" DBG_REDD "4" DBG_NOCOLOR "]      Azimuth CW (-5º)    [" DBG_REDD "6" DBG_NOCOLOR "]\n");
    printfd("       Elevation Up (+5º) [" DBG_REDD "8" DBG_NOCOLOR "]      Elevation Down(-5º) [" DBG_REDD "2" DBG_NOCOLOR "]\n");
    printfd("       Go home            [" DBG_REDD "0" DBG_NOCOLOR "]      Mode switch         [" DBG_REDD "5" DBG_NOCOLOR "]\n");
    printfd("       Query az./el.      [" DBG_REDD "*" DBG_NOCOLOR "]      Help                [" DBG_REDD "H" DBG_NOCOLOR "]\n");
    printfd("       Enter cmd.         [" DBG_REDD "+" DBG_NOCOLOR "]      QUIT                [" DBG_REDD "Q" DBG_NOCOLOR "]\n");
}

/***********************************************************************************************//**
 * Set a given azimuth and elevation to the rotors.
 **************************************************************************************************/
void rotors_get_az_el(int fd, double * v_az, double * v_el)
{
    char f1[30];
    char f2[30];
    char buf[52];
    int len = 52;

    /* length of the floats + the type of command */
    buf[0] = (char) 'G';
    buf[1] = (char) '\n';
    uart_write(fd, buf, 2);
    /* Recv the yack/nack */
    if (uart_read(fd, (unsigned char *) buf, &len, 2 * 1000 * 1000) == 0){
        buf[len - 1] = '\0';
        sscanf((const char *)buf, "%[^,] %*[,] %[^,] %*[,]", f1, f2);
        *v_az = atof(f1);
        *v_el = atof(f2);
    }else{
        printfe("[Set az/el] Error reading from UART\n");
    }
}

/***********************************************************************************************//**
 * Set a given azimuth and elevation to the rotors.
 **************************************************************************************************/
void rotors_set_az_el(int fd, double v_az, double v_el)
{
    char buf[52];
    int len;

    if(v_az < 0.0) v_az = 0.0;
    if(v_el < 0.0) v_el = 0.0;
    len = sprintf(buf+1, "%lf,%lf\n", v_az, v_el);
    if (len > 0){
        printfd("Setting azimuth (%.2lf) and elevation (%.2lf)\n", v_az, v_el);
        /* length of the floats + the type of command */
        buf[0] = (char) 'S';
        uart_write(fd, buf, len + 1);
        /* Recv the yack/nack */
        if (uart_read(fd, (unsigned char *) buf, &len, 1000 * 1000) == 0){
            buf[len - 1] = '\0';
            if (strcmp("YACK", buf) != 0){
                printfe("[Set az/el] Error setting position\n");
            }
        }else{
            printfe("[Set az/el] Error reading from UART\n");
        }
    }
}

/***********************************************************************************************//**
 * Tell the rotors to go to calibration point "home".
 **************************************************************************************************/
void rotors_home(int fd)
{
    char buf[52];
    /* this indicates max length and ret length */
    int len = 5;
    buf[0] = 'A';
    printfd("Setting rotors home...\n");
    uart_write(fd, buf, 2);
    /* Recv the yack/nack */
    /* up to 2 minutes of timeout */
    if((uart_read(fd, (unsigned char *) buf, &len, 2 * 1000 * 1000)) == 0) {
        buf[len - 1] = '\0';
        if(strcmp("YACK", buf) == 0){
            if((uart_read(fd, (unsigned char *) buf, &len, 120 * 1000 * 1000)) == 0) {
                buf[len - 1] = '\0';
                if(strcmp("DONE", buf) != 0) {
                    printfe("[Rotors home] Error sending command -> Autohome does not work\n");
                }else{
                    printfo("[Rotors home] Rotors correctly set to home\n");
                }
            }else{
                printfe("[Rotors home] Error reading from UART -> Autohome timedout\n");
            }
        }else{
           printfe("[Rotors home] Error sending command -> No AutoHome set\n");
        }
    }else{
        printfe("[Rotors home] Error reading from UART -> No AutoHome set\n");
    }
}


/***********************************************************************************************//**
 * Returns the current time in the format %H:%M:%S.
 **************************************************************************************************/
const char * curr_time_format(void)
{
    time_t t; // Current time.
    static char retval[21];
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    // strftime(retval, 21, "%Y-%m-%d %H:%M:%S", tmp);
    strftime(retval, 21, "%H:%M:%S", tmp);

    return retval;
}

static int input_timeout(int fd, long long microsec)
{
    fd_set set;
    struct timeval timeout;
    /* Initialize the file descriptor set. */
    FD_ZERO (&set);
    FD_SET (fd, &set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = 0;
    timeout.tv_usec = microsec;

    /* select returns 0 if timeout, 1 if input available, -1 if error. */
    return (select (FD_SETSIZE, &set, NULL, NULL, &timeout));   
}


/***********************************************************************************************//**
 * Performs a write to uart interface after flushing the uart buffer
 **************************************************************************************************/
int uart_write(int fd, void *buffer, int size)
{
    int sel_op, ret;
    int flush_max = 0;
    unsigned char aux[1];
    while(flush_max < 256){
        sel_op = input_timeout(fd, 0);
        if (sel_op > 0){
            ret = read(fd, aux, 1);
            if (ret != 1){
                /* error at uart line */
                return -1;
            }
        }else if (sel_op == 0){
            break;
        }else{
            /* error at uart line */
            return -2;
        }
        flush_max++;
    }
    return write(fd, buffer, size);
}


/***********************************************************************************************//**
 * Performs a read with timeouts.
 **************************************************************************************************/
int uart_read(int fd, unsigned char *buffer, int * size, long long timeout)
{
    int r_op, sel_op;
    struct timeval end;    /* 1-byte timeout represented as a timeval struct. */
    struct timeval start;         /* Time at which the final timeout will occur. */
    long long diff_time;        /* Time difference between select and read (without timeout), in microseconds. */
    long long wait_microsec;
    int buf_size = *size;
    int accumulated_size = 0;
    if(timeout > 0)
    {    
        do{
            gettimeofday(&start, NULL);
            wait_microsec = timeout;
            sel_op = input_timeout(fd, wait_microsec);
            if(sel_op == 0)
            {
                // Timeout
                printfe("[UART control] Timed out\n");
                return -1;
            }else if(sel_op < 0){
                printfe("[UART control] Error setting select\n");
                // Error setting the select
                if(errno == EINTR) continue;
                else return -2;
            }

            // Read from UART:
            if((r_op = read(fd, buffer+accumulated_size, 1)) == 1)
            {
                accumulated_size++;
            }else{
                printfe("[UART control] Error reading from UART\n");
                return -1;
            }

            // Get the current time and calculate the difference in order to reduce the timeout:
            gettimeofday(&end, NULL);
            diff_time = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
            timeout -= diff_time;
        }while( (buffer[accumulated_size - 1] != '\n') && ((accumulated_size - 1) < buf_size) && timeout > 0);
        *size = accumulated_size;
    }
    return 0;
}

int open_rotor_interface(const char * tty_path)
{
    int fd;    // File descriptor for the port
    struct termios options;

    fd = open(tty_path, O_RDWR | O_NOCTTY);

    if (fd == -1){
        printfe("Unable to open %s %s\n", tty_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    tcgetattr(fd, &options);        // Get the current options for the port
    cfsetispeed(&options, B38400);   // Set the baud rates to 230400
    cfsetospeed(&options, B38400);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable the receiver and set local mode
    options.c_cflag &= ~PARENB;             // No parity bit
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;              // Mask data size
    options.c_cflag |=  CS8;                // Select 8 data bits
    options.c_cflag &= ~CRTSCTS;            // Disable hardware flow control

    // Set the new attributes
    tcflush( fd, TCIFLUSH );
    tcsetattr(fd, TCSANOW, &options);
    return (fd);
}

void init_rotor_control (int fd)
{
    char buf[52];
    int ret;
    int len = 5;
    int limit = 0;
    
    /* The arduino when powered for the first time is not initialised in the two following seconds */
    /* Either sleep or repeat the write/read sequence... */
    do{
        buf[0] = (char) 'I';
        buf[1] = (char) '\n';
        uart_write(fd, buf, 2);
        /* Recv the yack/nack */
        if ((ret = uart_read(fd, (unsigned char *) buf, &len, 2000 * 1000)) == 0){
            buf[len - 1] = '\0';
            if (strcmp(buf, "NACK") == 0) {
                printfo("Rotors successfully initialized\n");
                return;
            } else {
                printfd("Unable to initialize the antenna rotors\n");
                return;
            }
        }else{
            printfe("[Init rotor control] Error reading from UART\n");
        }
    } while(ret != 0 && ++limit < 4);
}



/***********************************************************************************************//**
 * Resets TTY setup to leave it in its original state.
 **************************************************************************************************/
void exit_ground_station(void)
{
    tcsetattr(0, TCSAFLUSH, &orig_termios);
}
/***********************************************************************************************//**
 * Configures the TTY in "raw" mode.
 **************************************************************************************************/
void tty_raw(void)
{
    struct termios raw;

    raw = orig_termios;  /* Copy original and then modify below */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    // raw.c_cc[VMIN] = 5; raw.c_cc[VTIME] = 8;
    // raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0;
    // raw.c_cc[VMIN] = 2; raw.c_cc[VTIME] = 0;
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 5; /* Return condition, (min number of bytes = 0):
                                              * timer after 0 bytes or .5 seconds after first byte
                                              * seen.
                                              */

    /* Put terminal in raw mode after flushing: */
    if (tcsetattr(0, TCSAFLUSH, &raw) < 0) {
        printfe("Could not set the tty in raw mode");
    }
}

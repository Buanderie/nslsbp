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

/***********************************************************************************************//**
 * Program entry point
 **************************************************************************************************/
int main(int argc, char ** argv)
{
    pthread_t rotors_th;
    time_t last_update_gps = -2;
    time_t last_update_sbc = -2;
    time_t time_local;
    GPS_data gd;
    double delta_lat, delta_lng, delta_alt, dist_x, dist_y;

    if(argc != 5) {
        printfe("Wrong number of arguments. Local (GS) latitude, longitude and altitude are required\n");
        printfd("Issue: ./ground_station /dev/tty... <lat> <lon> <alt>\n");
        return -1;
    }

    /* 26 -> max tty dev name size */
    strncpy(tty_dev_name, argv[1], 26);
    gs_lat = strtod(argv[2], NULL);
    gs_lng = strtod(argv[3], NULL);
    gs_alt = strtod(argv[4], NULL);

    pthread_create(&rotors_th, NULL, rotor_control, NULL);

    time_local = time(NULL);
    printfd("Ground Station process started. " DBG_BLUE "Local time is %s" DBG_NOCOLOR, ctime(&time_local));
    printfd("Ground Station location (lat, lon, alt): %.6lf, %.6lf, %.1lf\n", gs_lat, gs_lng, gs_alt);
    while(!gs_exit) {
        if(dbman_get_gps_data(&gd) == 0) {
            if( (last_update_gps < strtol(gd.time_gps, NULL, 10)) ||
                (last_update_sbc < strtol(gd.time_local, NULL, 10))) {
                /* A new value arrived: */
                last_update_gps = strtol(gd.time_gps, NULL, 10);
                last_update_sbc = strtol(gd.time_local, NULL, 10);
                printfdg("[GPS data        ] lat = %.7lf, lng = %.7lf, sea_alt = %.2lf, geo_alt = %.2lf\n",
                    gd.lat, gd.lng, gd.sea_alt, gd.geo_alt);
                printfdg("[GPS data        ] time(SBC) = %s, time(GPS) = %s, vel = %.2lf, course = %.2lf\n",
                    gd.time_local, gd.time_gps, gd.v_kph, gd.course);
                if(gd.gpu_temp >= 60.0 || gd.cpu_temp >= 60.0) {
                    printfe("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", gd.temp, gd.cpu_temp, gd.gpu_temp);
                } else {
                    printfdg("[Temperature data] Temp. sensor: %.1lf ºC, CPU temp: %.1lf ºC, GPU temp: %.1lf ºC\n", gd.temp, gd.cpu_temp, gd.gpu_temp);
                }

                /* Calculate new azimuth and elevation: */
                delta_lat = gs_lat - gd.lat;
                delta_lng = gs_lng - gd.lng;
                delta_alt = gd.sea_alt - gs_alt;
                dist_x = delta_lat * KM_DEG_LAT;
                dist_y = delta_lng * KM_DEG_LNG * cos(gs_lat);
                printfd("Delta Lat: %lfº, Lng: %lfº, alt: %.1lfº; Distance: %.2ld km\n",
                    delta_lat, delta_lng, delta_alt, (sqrt((dist_x * dist_x) + (dist_y * dist_y)) / 1000.0));
                az = atan(delta_lng / delta_lat) * 180.0 / PI;
                if((delta_lat > 0.0 && delta_lng < 0.0) || (delta_lat < 0.0 && delta_lng < 0.0)) {
                    az += 180.0;
                }
                el = atan(delta_alt / sqrt((delta_lat * delta_lat) + (delta_lng * delta_lng)));
                printfo("New AZ: %.2lf, EL: %.2lf\n", az, el);
            }
        } else {
            printfe("Error retrieving beacon data\n");
        }
        sleep(1);
    }

    return 0;
}

/***********************************************************************************************//**
 * Rotor control thread routine
 **************************************************************************************************/
void * rotor_control(void * arg)
{
    int local_az, local_el;
    control_mode mode = MODE_MANUAL;

    while(!gs_exit) {
        /* Example -> each 5 seconds send a SET AZ EL */
        sleep(5);
        rotors_set_az_el(10.0, 20.0);
    }
    return NULL;
}

/***********************************************************************************************//**
 * Set a given azimuth and elevation to the rotors.
 **************************************************************************************************/
static int open_rotor_interface(const char * tty_path);
int uart_read(int fd, unsigned char *buffer, int buf_size, int timeout);

void rotors_set_az_el(double v_az, double v_el)
{
    int rotor_fd;
    char buf[52];
    int len = sprintf(buf+2, "%lf,%lf", v_az, v_el);
    if (len > 0){
        rotor_fd = open_rotor_interface(tty_dev_name);
        /* length of the floats + the type of command */
        buf[0] = (char) len + 1;
        buf[1] = (char) 'G';
        printf("Query len: %02x ---> Query: %s\n", buf[0], buf);
        write(rotor_fd, buf, len + 2);
        /* Recv the yack/nack */
        if (uart_read(rotor_fd, buf, 4, 5000 * 1000) == 0){
            buf[4] = '\0';
            printf("Received: %s\n", buf);
            close(rotor_fd);
        }else{
            printf("No data on arduino...\n");
            close(rotor_fd);
        }
    }
    return;
}

/***********************************************************************************************//**
 * Tell the rotors to go to calibration point "home".
 **************************************************************************************************/
void rotors_home(void)
{
    int rotor_fd;
    char buf[52];
    int len = 0;
    rotor_fd = open_rotor_interface(tty_dev_name);
    buf[0] = (char) len + 1;
    buf[1] = (char) 'H';
    printf("Query len: %u ---> Query: %s\n", (unsigned int) buf[0], buf+1);
    write(rotor_fd, buf, len + 2);
    /* Recv the yack/nack */
    read(rotor_fd, buf, 4);
    buf[4] = '\0';
    printf("Received: %s\n", buf);
    if ((uart_read(rotor_fd, buf, 4, 5000 * 1000)) == 0){
        buf[4] = '\0';
        printf("Received: %s\n", buf);
        close(rotor_fd);
    }else{
        printf("No data on arduino...\n");
        close(rotor_fd);
    }
    sleep(3);
    printfo("Antenna rotors calibrated\n");
    return;
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

/***********************************************************************************************//**
 * Performs a read with timeouts.
 **************************************************************************************************/
/* UART READ FUNCTION *******************************************************************************
 *  Name    :   uart_read                                                                           *
 *  Descr.  :   Read buffer from UART.                                                              *
 *  Args.   :                                                                                       *
 *  - int       fd              :   File descriptor where the read will be performed                *
 *  - unsigned char * buffer    :   Buffer where data received will be stored                       *
 *  - int       buf_size        :   Expected size of data                                           *
 *  - int       timeout         :   Number of microseconds of timeout                               *
 *                                                                                                  *
 *  Returns :   NOERROR    -> Reading succesful, buffer has the recieved data                       *
 *              EHWFAULT1  -> Reading error, contents of buffer undefined                           *
 *              EHWTIMEOUT -> Timeout error                                                         *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int uart_read(int fd, unsigned char *buffer, int buf_size, int timeout)
{
    int r_op, sel_op;
    fd_set uart_fd;
    struct timeval max_time;    // 1-byte timeout represented as a timeval struct.
    struct timeval end;         // Time at which the final timeout will occur.
    int diff_time;              // Time difference between select and read (without timeout), in microseconds.

    if(timeout > 0) 
    {
        // Timed read:
        // -- Prepare counters
        gettimeofday(&end, NULL);
        end.tv_sec += (timeout / 1000000);
        end.tv_usec += (timeout % 1000000);
        
        max_time.tv_sec = (timeout / 1000000);
        max_time.tv_usec = (timeout % 1000000);

        while(buf_size > 0)
        {
            // Prepare select structures
            FD_ZERO(&uart_fd);
            FD_SET(fd, &uart_fd);
            
            sel_op = select(FD_SETSIZE, &uart_fd, NULL, NULL, &max_time);
            if(sel_op == 0)
            {
                // Timeout
                return -1;
            }else if(sel_op < 0){
                // Error setting the select
                if(errno == EINTR) continue;
                else return -2;
            }

            // Read from UART:
            if((r_op = read(fd, buffer++, 1)) == 1)
            {
                --buf_size;
            }else{
                return -1;
            }
            
            // Get the current time and calculate the difference in order to reduce the timeout:
            gettimeofday(&max_time, NULL);
            diff_time = ((end.tv_sec * 1000000) + end.tv_usec) - ((max_time.tv_sec * 1000000) + max_time.tv_usec);
            
            // Calculate/set the new timeout:
            max_time.tv_sec  = diff_time / 1000000;
            max_time.tv_usec = diff_time % 1000000;
        }
    }else{
        // No timeouts:
        while(buf_size > 0)
        {
            // Read from UART:
            if((r_op = read(fd, buffer++, 1)) == 1)
            {
                --buf_size;
            }else{
                if(errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    return -1;
                }else{
                    return -1;
                }
            }
        }
    }
    return 0;
}

int open_rotor_interface(const char * tty_path)
{
    int fd;    // File descriptor for the port
    struct termios options;
    fd = open(tty_path, O_RDWR | O_NOCTTY);

    if (fd == -1){
        fprintf(stderr, "open_port: Unable to open %s %s\n", tty_path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    fcntl(fd, F_SETFL);             // Configure port reading
    tcgetattr(fd, &options);        // Get the current options for the port
    cfsetispeed(&options, B38400);   // Set the baud rates to 230400
    cfsetospeed(&options, B38400);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable the receiver and set local mode
    options.c_cflag &= ~PARENB;             // No parity bit
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;              // Mask data size
    options.c_cflag |=  CS8;                // Select 8 data bits
    options.c_cflag &= ~CRTSCTS;            // Disable hardware flow control  

    // Enable data to be processed as raw input
    options.c_lflag &= ~(ICANON | ECHO | ISIG);

    options.c_cc[VMIN]  =  1;           // 5 bytes to read
    options.c_cc[VTIME]  =  0;         // 10 * 0.1 seconds read timeout

    // Set the new attributes
    tcflush( fd, TCIFLUSH );
    tcsetattr(fd, TCSANOW, &options);
    return (fd);
}

/***********************************************************************************************//**
 *  \brief      Experiment Launcher and Process Handler
 *  \details    Executes the system modules and mantains them alive to be able to guarantee a
 *              constant execution. Resets the computer when errors are found.
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       27-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

/*** INCLUDE SECTION ******************************************************************************/
#include "xlauncher.h"

/*** GLOBAL VARIABLES *****************************************************************************/
int pipe0[2];
int pipe1[2];
int pid_tee;
int pid_raspivid;
int pid_vitow;
int vitow_error_count = 0;

/***********************************************************************************************//**
 * Program entry point.
 **************************************************************************************************/
int main(int argc, char **argv)
{
    int retval, pid;
    char vitow_saved_filename[100];

restart:
    if(pipe(pipe0) < 0) {
        perror("Unable to create PIPE0");
        return -1;
    }

    pid_raspivid = fork();
    if(pid_raspivid == 0) {
        /* Raspivid process: -------------------------------------------------------------------- */
        close(pipe0[READ]);         /* Raspivid does not read from Pipe 0. */
        close(1);
        dup(pipe0[WRITE]);
        close(pipe0[WRITE]);        /* stdout is now redirected to Pipe 0 (W). */

        // send_beacon_msg(SYSTEM, "Launching raspivid process");
        execlp("raspivid", "raspivid", "-t", "0", "--rotation", "180", "-w", "854", "-h", "480",
            "-b", "750000", "-ih", "-fps", "25", "-n", "-pf", "high", "-o", "-", (char *)NULL);
        // send_beacon_msg(SYSTEM, "Raspivid failed to execute");

    } else if(pid_raspivid > 0) {

        if(pipe(pipe1) < 0) {
            perror("Unable to create PIPE1");
            kill(pid_raspivid, SIGKILL);
            close(pipe0[WRITE]);
            close(pipe0[READ]);
            return -1;
        }
        close(pipe0[WRITE]);        /* Neither tee nor vitow nor xLauncher will write to Pipe 0.  */
        pid_tee = fork();
        if(pid_tee == 0) {
            /* tee process: --------------------------------------------------------------------- */
            close(0);
            dup(pipe0[READ]);
            close(pipe0[READ]);     /* stdin is now redirected to Pipe 0 (R). */
            close(1);
            dup(pipe1[WRITE]);
            close(pipe1[WRITE]);    /* stdout is now redirected to Pipe 1 (W). */
            close(pipe1[READ]);     /* tee does not read from Pipe 1. */

            sprintf(vitow_saved_filename, "vitow_input_%ld", time(NULL));
            execlp("tee", "tee", vitow_saved_filename, (char *)NULL);

        } else if(pid_tee > 0) {

            pid_vitow = fork();
            if(pid_vitow == 0) {
                close(0);
                dup(pipe1[READ]);
                close(pipe1[READ]);     /* stdin is now redirected to Pipe 1 (R). */
                close(pipe1[WRITE]);    /* VITOW does not need to write to Pipe 1. */
                close(pipe0[READ]);     /* VITOW does not read from Pipe 0. */

                /* VITOW process: --------------------------------------------------------------- */
                printfd("Disabling wlan0\n");
                system("ifdown wlan0");
                printfd("Setting monitor mode in WiFi donlge interface\n");
                system("rmmod ath9k_htc");
                sleep(3);
                system("modprobe ath9k_htc");
                sleep(3);
                system("/home/pi/bbs/module_vitow/monitor.sh wlan1 13 > /dev/null");
                printfo("WiFI IF should now be in monitor mode\n ");
                // send_beacon_msg(SYSTEM, "Launching VITOW process");
                execlp("/home/pi/bbs/module_vitow/vitow_tx", "vitow_tx", "wlan1", (char *)NULL);

            } else if(pid_vitow > 0) {
                /* xLauncher process: ----------------------------------------------------------- */
                close(pipe1[READ]);     /* xLauncher does not need to read from Pipe 1. */
                close(pipe1[WRITE]);    /* xLauncher does not need to write to Pipe 1. */
                close(pipe0[READ]);     /* xLauncher does not read from Pipe 0. */

                printfd("Process `vitow_tx`: %d\n", pid_vitow);
                printfd("Process `tee`     : %d\n", pid_tee);
                printfd("Process `raspivid`: %d\n", pid_raspivid);
                // send_beacon_msg(SYSTEM, "All system processes have been started");

                while(1) {
                    pid = wait(&retval);
                    if(pid > 0) {
                        if(pid == pid_vitow) {
                            printfe("Process `vitow_tx` has terminated\n");
                            vitow_error_count++;
                            // send_beacon_msg(SYSTEM, "Process `vitow_tx` has terminated with error code %d", retval >> 8);
                            kill(pid_raspivid, SIGINT);
                            kill(pid_tee, SIGINT);
                            break;
                        } else if(pid == pid_tee) {
                            printfe("Process `tee` has terminated\n");
                            // send_beacon_msg(SYSTEM, "Process `tee` has terminated with error code %d", retval >> 8);
                            kill(pid_raspivid, SIGINT);
                            kill(pid_vitow, SIGINT);
                            break;
                        } else if(pid == pid_raspivid) {
                            printfe("Process `raspivid` has terminated\n");
                            // send_beacon_msg(SYSTEM, "Process `raspivid` has terminated with error code %d", retval >> 8);
                            kill(pid_tee, SIGINT);
                            kill(pid_vitow, SIGINT);
                            break;
                        } else {
                            printfe("An unknown process with PID %d has terminated\n", pid);
                        }
                    } else {
                        break;
                    }

                }
                if(vitow_error_count < 5) {
                    // send_beacon_msg(SYSTEM, "VITOW has failed %d times. Trying to reset processes without system reboot", vitow_error_count);
                    printfw("VITOW has failed %d times. Trying to reset processes without system reboot", vitow_error_count);
                    goto restart;
                } else {
                    // send_beacon_msg(SYSTEM, "System will now reboot");
                    printfe("Exiting now\n");
                }
            } else {
                /* Error on fork: */
                printfe("Error while forking this process (1)\n");
                // send_beacon_msg(SYSTEM, "System will now reboot");
                printfe("Exiting now\n");
            }
        } else {
            /* Error on fork: */
            printfe("Error while forking this process (2)\n");
            // send_beacon_msg(SYSTEM, "System will now reboot");
            printfe("Exiting now\n");
        }
    } else {
        /* Error on fork: */
        printfe("Error while forking this process (3)\n");
        // send_beacon_msg(SYSTEM, "System will now reboot");
        printfe("Exiting now\n");
    }

    return 1;
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

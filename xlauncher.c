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

/***********************************************************************************************//**
 * Program entry point.
 **************************************************************************************************/
int main(int argc, char **argv)
{
    int retval, pid;

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

        // buf = 'a';
        // write(1, &buf, 1);
        // sleep(15);
        // fprintf(stderr, "I'm Raspivid (PID: %d) and I will exit now!\n", getpid());
        // return -1;
        execlp("raspivid", "raspivid", "-t", "0", "-w", "854", "-h", "480", "-b", "750000", "-ih", "-fps", "25", "-n", "-pf", "high", "-o", "-", (char *)NULL);

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

            // read(0, &buf, 1);
            // fprintf(stderr, "I'm tee (PID: %d) and I just read `%c` from stdin and I will write to stdout\n", getpid(), buf);
            // sleep(1);
            // buf++;
            // write(1, &buf, 1);
            // sleep(15);
            // fprintf(stderr, "I'm tee (PID: %d) and I will exit now!\n", getpid());
            // return -2;
            execlp("tee", "tee", "vitow_input", (char *)NULL);

        } else if(pid_tee > 0) {

            pid_vitow = fork();
            if(pid_vitow == 0) {
                close(0);
                dup(pipe1[READ]);
                close(pipe1[READ]);     /* stdin is now redirected to Pipe 1 (R). */
                close(pipe1[WRITE]);    /* VITOW does not need to write to Pipe 1. */
                close(pipe0[READ]);     /* VITOW does not read from Pipe 0. */

                /* VITOW process: --------------------------------------------------------------- */
                // read(0, &buf, 1);
                // printfd("I'm VITOW (PID: %d) and I just read `%c` from stdin and written to stdout.\n", getpid(), buf);
                // sleep(15);
                // printfd("I'm VITOW (PID: %d) and I will exit now\n", getpid());
                // return -3;
                execlp("vitow_tx", "vitow_tx", "wlan1", (char *)NULL);

            } else if(pid_vitow > 0) {
                /* xLauncher process: ----------------------------------------------------------- */
                close(pipe1[READ]);     /* stdin is now redirected to Pipe 1 (R). */
                close(pipe1[WRITE]);    /* xLauncher does not need to write to Pipe 1. */
                close(pipe0[READ]);     /* xLauncher does not read from Pipe 0. */

                // printfd("I'm xLauncher (PID: %d) and I read from nowhere\n", getpid());
                // printfo("PID: %d just returned with retval %d\n", wait(&retval), retval);
                // printfo("PID: %d just returned with retval %d\n", wait(&retval), retval);
                // printfo("PID: %d just returned with retval %d\n", wait(&retval), retval);

                printfd("Process `vitow_tx`: %d\n", pid_vitow);
                printfd("Process `tee`     : %d\n", pid_tee);
                printfd("Process `raspivid`: %d\n", pid_raspivid);
                while(1)
                {
                    pid = wait(&retval);
                    if(pid > 0) {
                        if(pid == pid_vitow) {
                            printfe("Process `vitow_tx` has terminated\n");
                        } else if(pid == pid_tee) {
                            printfe("Process `tee` has terminated\n");
                        } else if(pid == pid_raspivid) {
                            printfe("Process `raspivid` has terminated\n");
                        } else {
                            printfe("An unknown process with PID %d has terminated\n", pid);
                        }
                    } else {
                        break;
                    }
                }
                printfe("Exiting now\n");
            }
        } else {
            /* Error on fork: */
            return -1;
        }
    } else {
        /* Error on fork: */
        return -1;
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

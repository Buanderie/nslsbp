/***********************************************************************************************//**
 *  \brief      CC1101 Beacon Handler Wrapper
 *  \details    Provides a single secure function that all processes can use to output messages to
 *              CC1101 beacon (using cc_beacon_iface library) without having to care about errors.
 *  \author     Carles Araguz <carles.araguz@upc.edu>
 *  \version    1.0
 *  \date       28-sep-2016
 *  \copyright  GNU Public License (v3). This files are part of an on-going non-commercial research
 *              project at NanoSat Lab (http://nanosatlab.upc.edu) of the Technical University of
 *              Catalonia - UPC BarcelonaTech. Third-party libraries used in this framework might be
 *              subject to different copyright conditions.
 **************************************************************************************************/

/*** INCLUDE SECTION ******************************************************************************/
#include "cc_beacon_iface_wrapper.h"

/*
int   BeaconConnect (const char * ip, const char * port, BeaconMessageHandler * bmh);
void  BeaconClose (BeaconMessageHandler * bmh);
int   BeaconWrite (BeaconMessageHandler * bmh, BYTE * msg, int32_t len, MsgSource m);
*/

/***********************************************************************************************//**
 * Returns the current time in the format %H:%M:%S.
 **************************************************************************************************/
static const char * curr_time_format(void)
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
 * Opens the Beacon interface, writes the message and exits.
 **************************************************************************************************/
void send_beacon_msg(MsgSource process_id, const char * fmt, ...)
{
    va_list args;
    char msg_user[MSG_LENGTH];
    int err;
#ifndef CC_BEACON_IFACE_WRAPPER_DEBUG
    char buf[MSG_LENGTH];
    BeaconMessageHandler beacon_handler;
#endif

    va_start(args, fmt);
    err = vsnprintf(msg_user, (MSG_LENGTH - 1), fmt, args);   /* Writes formated message. */
    va_end(args);

    if(err > 0) {
#ifndef CC_BEACON_IFACE_WRAPPER_DEBUG
        if(snprintf(buf, (MSG_LENGTH - 1), "%s|%s", curr_time_format(), msg_user) > 0) {
            if(BeaconConnect(SOCK_IP_ADDR, SOCK_PORT, &beacon_handler) > 0) {
                BeaconWrite(&beacon_handler, (unsigned char *)buf, strlen(buf), process_id);
                BeaconClose(&beacon_handler);
            }
        }
#else
        printf("[ccbeacon:%s] (" DBG_PURPLE "B" DBG_NOCOLOR ") " DBG_PURPLE "%s\n" DBG_NOCOLOR, curr_time_format(), msg_user);
#endif
    }
}

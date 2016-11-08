#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

// Error codes:
// -- Common: reserved from 0 to 29
#define     NOERROR      0      // Succesful operation.
#define     EUNDEF      -1      // Undefined error.
#define     EMKFIFO     -2      // Error creating the FIFO's
#define     EOPEN       -3      // Error opening a file descriptor.
#define     EMUTEX      -4      // Tried to accuire a shared resources which is not available.
#define     ERWSYSCORE  -5      // Error while reading or writing to Syscore FD.
#define     ERWPROCMAN  -6      // Error while reading or writing to Procman FD.
#define     ERWSDB      -7      // Error while reading or writing to SDB FD.
#define     ERWHWMOD    -8      // Error while reading or writing to a HWmod FD.
#define     EPROG       -9      // Programming error (e.g. Segmentation Fault catched)
#define     ESOFT       -10     // Error during operation. The error is spontaneous and/or repairable.
#define     EWARNING    -11     // No error found, but a probably unexpected behaviour occured.
#define     EFIFO       -12     // Error reading, writing, creating, opening or closing a FIFO.
#define     ECONFIG     -13     // Error in a .conf file.
#define     EXENOMAI    -14     // An error ocurred during Xenomai start or program execution.
#define     ETIMEOUT    -15     // A timeout has occurred.
#define     EWRONGSTATE -16     // A wrong state or condition is reached.
//#define                 -17
//#define                 -18
//#define                 -19
//#define                 -20
//#define                 -21
//#define                 -22
//#define                 -23
//#define                 -24
//#define                 -25
//#define                 -26
//#define                 -27
//#define                 -28
//#define                 -29
// -- Syscore: reserved from 30 to 59
#define     EXENOINIT   -30     // Unable to initialize Xenomai.
#define     ECREATERT   -31     // Unable to create tasks.
#define     EBATTERY    -32     // Errors related to battery or SoC.
#define     ELOWENERGY  -33     // Low energy detected.
//#define                 -34
//#define                 -35
//#define                 -36
//#define                 -37
//#define                 -38
//#define                 -39
//#define                 -40
//#define                 -41
//#define                 -42
//#define                 -43
//#define                 -44
//#define                 -45
//#define                 -46
//#define                 -47
//#define                 -48
//#define                 -49
//#define                 -51
//#define                 -52
//#define                 -53
//#define                 -54
//#define                 -55
//#define                 -56
//#define                 -57
//#define                 -58
//#define                 -59
// -- Process Manager: reserved from 60 to 89
#define     EBADACTION  -60     // hwmod_action function does not handle the given action.
#define     EFORCE      -61     // Process Manager could not force an action (procman_force).
#define     EBADHWMODID -62     // The provided HWmod ID is not valid.
#define     ETHREAD     -63     // Could not create/join
#define     ETASK       -64     // A task handler has failed
#define     EPOLDIS     -65     // The POL you were trying to handle is marked as disabled.
#define     EFILESYS    -66     // Actions on the file system returned with errors (not open).
#define     EPOLERR     -67     // Error enabling or disabling a POL
#define     EFSM        -68     // Problem executing a state transition of the Process Manager.
#define     EWATCHDOG   -69     // Watchdog has detected a non-responding module.
//#define                 -70
//#define                 -71
//#define                 -72
//#define                 -73
//#define                 -74
//#define                 -75
//#define                 -76
//#define                 -77
//#define                 -78
//#define                 -79
//#define                 -81
//#define                 -82
//#define                 -83
//#define                 -84
//#define                 -85
//#define                 -86
//#define                 -87
//#define                 -88
//#define                 -89
// -- System Data Bus: reserved from 90 to 119
#define     ESDBBUSY    -90     // SDB is busy and can't attend the command.
#define     ESDBTIMEOUT -91     // SDB timeout.
#define     ESDBPROC    -92     // HWmod ID has no process associated.
#define     ECMDFAULT   -93     // HWmod can't attend the command.
//#define                 -94
//#define                 -95
//#define                 -96
//#define                 -97
//#define                 -98
//#define                 -99
//#define                 -100
//#define                 -101
//#define                 -102
//#define                 -103
//#define                 -104
//#define                 -105
//#define                 -106
//#define                 -107
//#define                 -108
//#define                 -109
//#define                 -111
//#define                 -112
//#define                 -113
//#define                 -114
//#define                 -115
//#define                 -116
//#define                 -117
//#define                 -118
//#define                 -119
// -- Hardware Modules: reserved from 120 to 149
#define     ESETUP      -120    // Setup could not be completed.
#define     EHWFAULT1   -121    // Hardware fault (repeat allowed).
#define     EHWFAULT2   -122    // Hardware fault: Fatal.
#define     EHWINTERNAL -123    // Internal error in the hardware (not affecting the HWmod code).
#define     ENOTSET     -124    // The function asked is not set in the HWmod.
#define     EHWTIMEOUT  -125    // The waiting for a hardware response has timed out.
#define     EUNEXPECTED -126    // An unexpected situation occured.
#define     EBADCMD     -127    // Bad/wrong command/reply.
#define     ECHECKSUM   -128    // Invalid checksum (or CRC).
#define     EDISCONNECT -129    // Unable to disconnect.
//#define                 -130
//#define                 -131
//#define                 -132
//#define                 -133
//#define                 -134
//#define                 -135
//#define                 -136
//#define                 -137
//#define                 -138
//#define                 -139
//#define                 -141
//#define                 -142
//#define                 -143
//#define                 -144
//#define                 -145
//#define                 -146
//#define                 -147
//#define                 -148
//#define                 -149
// -- General Purpose: reserved from 150 to 159
#define     EGP0        -150    // General Purpose Error 0
#define     EGP1        -151    // General Purpose Error 1
#define     EGP2        -152    // General Purpose Error 2
#define     EGP3        -153    // General Purpose Error 3
#define     EGP4        -154    // General Purpose Error 4
#define     EGP5        -155    // General Purpose Error 5
#define     EGP6        -156    // General Purpose Error 6
#define     EGP7        -157    // General Purpose Error 7
#define     EGP8        -158    // General Purpose Error 8
#define     EGP9        -159    // General Purpose Error 9

// -- Init function error codes:
/*
 *  Errors are codified following the rule below:
 *
 *  ERROR: 0 0 0 0 0 0 0 0  0 0 1 0 0 1 0 1      (2 bytes)
 *         | | | | | | | |  | | | | | | | |
 *         | | | | | | | |  | | | | | | +-+----> (2 bits) Primary error number (type).
 *         | | | | | | | |  | | | +-+-+--------> (3 bits) Stage (if any). From 0 to 7.
 *         +-+-+-+-+-+-+-+--+-+-+--------------> (11 bits) Secondary error number.
 */
// -- -- Primary errors:
#define     EINITFATAL      0x0001  // INIT function failed completely.
#define     EINITERROR      0x0002  // INIT function encountered an error, but it is sporadic or repairable.
#define     EINITWARN       0x0003  // INIT function succeed, but something probably unexpected was found during the test.
// -- -- Init stages:
#define     EINITSTAGE0     0x0000  // INIT Stage 0
#define     EINITSTAGE1     0x0004  // INIT Stage 1
#define     EINITSTAGE2     0x0008  // INIT Stage 2
#define     EINITSTAGE3     0x000c  // INIT Stage 3
#define     EINITSTAGE4     0x0010  // INIT Stage 4
#define     EINITSTAGE5     0x0014  // INIT Stage 5
#define     EINITSTAGE6     0x0018  // INIT Stage 6
#define     EINITSTAGE7     0x001c  // INIT Stage 7
// -- -- Secondary errors:
#define     EINITDEVICE     0x0020  // UART or SPI open error.
#define     EINITHANDSHAKE  0x0040  // Hardware handshake error.
#define     EINITTIMEOUT    0x0060  // Hardware communication timed out.
#define     EINITDATA       0x0080  // Incoherent data (from sensors for instance)
#define     EINITFILES      0x00a0  // Can't open/write/read a file.
#define     EINITCONFIG     0x00c0  // file.conf has an error.
#define     EINITINTERNAL   0x00e0  // An error has been (correctly) reported from the hardware.
#define     EINITPOL1       0x0100  // EPS: POL 1 test failed.
#define     EINITPOL2       0x0120  // EPS: POL 2 test failed.
#define     EINITPOL3       0x0140  // EPS: POL 3 test failed.
#define     EINITPOL4       0x0160  // EPS: POL 4 test failed.
#define     EINITPOL5       0x0180  // EPS: POL 5 test failed.
#define     EINITPOL6       0x01c0  // EPS: POL 6 test failed.
#define     EINITPOL7       0x01e0  // EPS: POL 7 test failed.
#define     EINITPOL8       0x0200  // EPS: POL 8 test failed.
#define     EINITHWFAULT    0x0220  // Hardware fault.
#define     EINITPOL        0x0240  // EPS: some POL test failed.
#define     EINITGPIO       0x0280  // Can't read/write to a GPIO pin.

#endif
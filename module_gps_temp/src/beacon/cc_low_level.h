#ifndef __INC_CC_LOW_LEVEL_H__
#define __INC_CC_LOW_LEVEL_H__
/*** INCLUDE SECTION ********************************************************************************/
// C standard libraries:
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <ctype.h>

#include "spidev.h"
#include "error_def.h"
#include "wiringPi.h"

#define SPI_DEVICE "/dev/spidev0.0"

#define PKTLEN          255                 // Packet length when fixed length is set (and FEC is enabled).
#define MAX_PKT_LEN     (PKTLEN - 1)        // Maximum packet length (in bytes). The -1 accounts for the length field (datasheet page 38).
#define RS_PKT_LEN      PKTLEN - 32

#define GPIO_COMMS_PAC  7       // WIRING PI PIN REF. Pin 7 of the raspy header!!

// Strobe commands
#define STROBE_SRES     0x30    // Reset chip.
#define STROBE_SCAL     0x33    // Calibrate frequency synthesizer and turn it off.
#define STROBE_SRX      0x34    // Enable RX. Perform calibration first if coming from IDLE and
                                // MCSM0.FS_AUTOCAL = 1.
#define STROBE_STX      0x35    // Enable TX. Perform calibration first if coming from IDLE and
                                // MCSM0.FS_AUTOCAL = 1. If in RX state and CCA is enabled, go to
                                // TX if channel is clear.
#define STROBE_SIDLE    0x36    // Exit RX / TX, turn off frequency synthesizer and exit WOR mode if
                                // applicable.
#define STROBE_SFRX     0x3A    // Flush the RX FIFO buffer.
#define STROBE_SFTX     0x3B    // Flush the TX FIFO buffer.
#define PATABLE         0x3E    // PATABLE register address.
#define WRITE_BURST     0x40
#define READ_SINGLE     0x80
// #define READ_BURST      0xC0

#define RXTX_FIFO_LEN   64      // RX and TX FIFO length in bytes.

#define FREQ0           0x0F    // Address of Frequency control word, low byte
#define FREQ1           0x0E    // Address of Frequency control word, medium byte
#define FREQ2           0x0D    // Address of Frequency control word, high byte

#define SYNC0           0x05    // Sync word, high byte
#define SYNC1           0x04    // Sync word, low byte

#define DRATE           0x10    // Data rate

#define MARCSTATE       0xF5    // Control state machine state
#define PKTSTATUS       0xF8    // Current GDOx status and packet status
#define RXBYTES         0xFB    // Overflow and number of bytes in the RX FIFO
#define TXBYTES         0xFA    // Underflow and number of bytes in the TX FIFO
#define SBA_RXFIFO      0xBF    // Single Byte Access to RX FIFO
#define SBA_TXFIFO      0x3F    // Single Byte Access to TX FIFO
#define BA_RXFIFO       0xFF    // Burst Access to RX FIFO
#define BA_TXFIFO       0x7F    // Burst Access to TX FIFO
#define GDO0_pos        0       // Bit position of GDO0 in PKTSTATUS register
#define GDO2_pos        2       // Bit position of GDO2 in PKTSTATUS register
#define GDO0_bit        ( (CC1101_readsingle(spifd, PKTSTATUS) >> GDO0_pos ) & 1 ) // Value of PKTSTATUS:GDO0
#define GDO2_bit        ( (CC1101_readsingle(spifd, PKTSTATUS) >> GDO2_pos ) & 1 ) // Value of PKTSTATUS:GDO2
#define TIMEOUT_CAL     50      // Num. cycles until a calibration produces a timeout error.

#define MAX_LINE_SIZE   40      // comms.conf maximum line length

// Default configuration values
#define CONF_DEF_HW_TIMEOUT             1000000 // Default timeout (in microseconds) for the CC1101 handlers.
#define CONF_DEF_DRATE                  0xf8    // Default CC1101 register configuration
#define CONF_DEF_SYNC1                  0xd2    // Default CC1101 register configuration
#define CONF_DEF_SYNC0                  0x59    // Default CC1101 register configuration
#define CONF_DEF_ADDR                   0x00    // Default CC1101 register configuration
#define CONF_DEF_FEC_DIS                1       // Default value for FEC disable configuration (1 = FEC disabled, 0 = enabled).

#define CONF_DEF_FREQ2                  0x10    // Default CC1101 register configuration: RX freq. Ground Station
#define CONF_DEF_FREQ1                  0xd1    // Default CC1101 register configuration: RX freq. Ground Station
#define CONF_DEF_FREQ0                  0x3b    // Default CC1101 register configuration: RX freq. Ground Station

extern int timeouts;

/*** MACROS *****************************************************************************************/
#ifdef COMMS_DEBUG
#define printfd(fmt, ...) printf(fmt, ## __VA_ARGS__)
#else
#define printfd(fmt, ...) do { } while (0)
#endif

#ifdef COMMS_DEBUG
#define printfdv(fmt, ...) printf(fmt, ## __VA_ARGS__)
#else
#define printfdv(fmt, ...) do { } while (0)
#endif

#ifdef COMMS_DEBUG
    #define DBG_RED     printf("\x1b[31;1m");
    #define DBG_GREEN   printf("\x1b[32;1m");
    #define DBG_NOCOLOR printf("\x1b[0m");
#endif

inline int CC1101_wait(int spifd);
char CC1101_readsingle(int spifd, char addr);
void CC1101_writesingle(int spifd, char addr, char *value);
void CC1101_writeburst(int spifd, char addr, char* buffer, char count);
void CC1101_strobe(int spifd, char strobe);

#endif
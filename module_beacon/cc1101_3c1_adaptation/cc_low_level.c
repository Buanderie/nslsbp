#include "cc_low_level.h"

/* CURRENT TIME FUNCTION ****************************************************************************
 *  Name    :   microtime                                                                           *
 *  Descr.  :   Returns current time in microsecond resolution.                                     *
 *  Returns :   Microseconds since Unix time (unsigned long long).                                  *
 *  Author  :   Marc Marí Barceló.                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
unsigned long long microtime(){
    struct timeval time;
    unsigned long long microsec;
    gettimeofday(&time, NULL);
    microsec = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;
    return microsec;
}

/* COMMS PACKET SENDING FUNCTION ********************************************************************
 *  Name    :   send_packet                                                                         *
 *  Descr.  :   Send packets of data using the CC1101 chipset.                                      *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - unsigned char* data_to_send : Array containing the data to be sent.                           *
 *  - char      size_dts        :   Size of data_to_send.                                           *
 *  Returns :   Error codes:                                                                        *
 *              - NOERROR       -> Everything went well.                                            *
 *              - EWRONGSTATE   ->  CC1101 was not able to return to a valid state.                 *
 *              - ETIMEOUT      -> Some part of the process has timed out.                          *
 *  Author  :   Marc Marí Barceló, Juan Fran Muñoz Martín, Carles Araguz López                      *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int send_packet(int spifd, unsigned char *data_to_send, unsigned char size)
{
    char *data;                                                 // Changed to "unsigned" for meaninful declaration.
    int buffer_count;                                           // Data remaining in the buffer to be sent.
    int data_size = (size > 59 ? 59 : size) + 1;
    int timeout = timeouts;                                     // Timeout in microseconds.
    unsigned long long start;                                   // Start time.
    int time = 0;                                               // Current time.

    CC1101_strobe(spifd, STROBE_SFTX);                          // Flush CC1101 TX FIFO
    CC1101_strobe(spifd, STROBE_SIDLE);                         // Initialize the CC1101
    if(CC1101_wait(spifd) != NOERROR) return EWRONGSTATE;       // Waits until CC1101 FSM reaches a valid state.

    data = (char *)malloc(data_size);                           // Allocate space for the data buffer.
    memset(data,0,data_size);                                   // Leave the data buffer correctly initialized.
    data[0] = size;                                             // Write the size of the first burst in the first position.
    memcpy(&data[1], data_to_send, data_size-1);                // Write 0..59 bytes to the data buffer.

    start = microtime();                                        // Start counting time (timeout purposes).
    CC1101_writeburst(spifd, BA_TXFIFO, data, data_size);       // Write the whole data buffer to CC1101 TX FIFO.
    CC1101_strobe(spifd, STROBE_STX);                           // Start transmission.
    // Wait for transmission to be finished:
    while((GDO0_bit == 0) && (time < timeout))
        time = (int)(microtime() - start);
    if(time >= timeout) return ETIMEOUT;                        // If a timout occurs, should not continue and return error.

    time = 0;                                                   // Reset the timout.
    start = microtime();                                        // Reset the start time.
    if(size > 59)                                               // There is still data to be sent.
    {
        data_to_send += 59;                                     // Increment the pointer to the new start position.
        buffer_count = size - 59;                               // How many bytes are remaining.
        time = 0;                                               // Reset the timout.
        start = microtime();                                    // Reset the start time.
        while((buffer_count > 0) && (time < timeout))
        {
            if(CC1101_readsingle(spifd, TXBYTES) < 60)
            {
                CC1101_writesingle(spifd, SBA_TXFIFO, (char *)data_to_send);
                buffer_count--;
                data_to_send++;
            }
            time = (int)(microtime()-start);
        }
        if(time >= timeout) return ETIMEOUT;
    }

    time = 0;                                                   // Reset the timout.
    start = microtime();                                        // Reset the start time.
    while((GDO0_bit != 0) && (time < timeout))
        time = (int)(microtime() - start);
    if(time >= timeout)                                         // If a timout occurs, should not continue and return error.
        return ETIMEOUT;

    CC1101_strobe(spifd, STROBE_SFTX);                          // Flush CC1101 TX FIFO (not really needed)
    CC1101_strobe(spifd, STROBE_SIDLE);                         // Leaves the CC1101 in idle state.
    CC1101_wait(spifd);                                         // Waits until CC1101 FSM reaches a valid state.

    free(data);
    return NOERROR;
}

/* COMMS PACKET RECEIVE FUNCTION ********************************************************************
 *  Name    :   receive_packet                                                                      *
 *  Descr.  :   Receive packets of data using the CC1101 chipset.                                   *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - unsigned char* data_to_receive : Array that will contain the data received.                   *
 *  - int*      len             :   Integer that will contain the length received.                  *
 *  - int       timeout         :   Maximum time of execution in microseconds.                      *
 *  Returns :   Error codes:                                                                        *
 *              - NOERROR       ->  Everything went well.                                           *
 *              - EWRONGSTATE   ->  CC1101 was not able to return to a valid state.                 *
 *              - ECHECKSUM     ->  Data received but wrong CRC.                                    *
 *              - ETIMEOUT      ->  Data reception timed out.                                       *
 *  Author  :   Marc Marí Barceló, Juan Fran Muñoz Martín, Carles Araguz López                      *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int receive_packet(int spifd, unsigned char *data_to_receive, int *len)
{
    bool crc = false;                                           // CRC ok (initialized to "wrong CRC")
    int timeout = timeouts;                                     // Timeout in microseconds.
    int  bytes_read = 0;                                        // Counter of read bytes from the CC1101.
    unsigned long long start;                                   // Start time.
    int time = 0;                                               // Current time.

    CC1101_strobe(spifd, STROBE_SFRX);                          // Flush CC1101 RX FIFO.
    CC1101_strobe(spifd, STROBE_SIDLE);                         // Initialize the CC1101.
    if(CC1101_wait(spifd) != NOERROR) return EWRONGSTATE;       // Waits until CC1101 FSM reaches a valid state.
    CC1101_strobe(spifd, STROBE_SRX);                           // Set CC1101 in RX mode.
    start = microtime();
    while((GDO0_bit == 0) && (time < timeout))                  // Waits until bit GD0 is 0
        time = (int)(microtime()-start);
    if(time >= timeout)                                         // If a timout occurs, should not continue and return error.
    {
        *len = 0;                                               // Set length to 0.
        printfdv("RX: Timeout expired.\n");
        return ETIMEOUT;                                        // Return error.
    }else{
        time = 0;                                               // Reset the timout.
        start = microtime();                                    // Reset the start time.
        while(CC1101_readsingle(spifd, RXBYTES) <= 1);          // Wait until CC1101 is ready to read from the RX buffer.
        if((*len = CC1101_readsingle(spifd, SBA_RXFIFO)) > 0)   // Get the length of the packet to be received and store it in the pointer.
        {
            // Has data to read from the RX FIFO:
            /** Note:
              * The transmitter RX FIFO buffer has to be read synchronously with the states. Reading
              * N bytes from the buffer must not leave the buffer empty until the PKTSTATUS.0 bit is
              * not 0, otherwise a buffer underflow will occur, and the transmitter will go to Idle.
              * (see state diagram in CC1101 datasheet page 28)
              */
            while((bytes_read < (*len)-1) && (time < timeout))  // Reads all bytes except the last one.
            {
                if(CC1101_readsingle(spifd, RXBYTES) > 1)       // There is at least two bytes to read:
                {
                    data_to_receive[bytes_read] = CC1101_readsingle(spifd,SBA_RXFIFO); // Read a single byte from the RX FIFO.
                    bytes_read++;                               // Increment the byte counter.
                }
                time = (int)(microtime()-start);                // Update the timeout.
            }
            if(time < timeout)                                  // If timeout has not expired:
            {
                while(GDO0_bit != 0) usleep(10);                // Wait until GDO0 flag is unset, to indicate that the last byte shall be read.
                data_to_receive[bytes_read] = CC1101_readsingle(spifd,SBA_RXFIFO); // Read the last byte from the RX FIFO.
                crc = GDO2_bit;                                 // Check whether CRC is correct. (1 => correct; 0 => wrong)
                data_to_receive[bytes_read+1] = 0;
            }else{
                // Timeout expired:
                printfd("RX: Timeout expired. Read %d of %d bytes\n",bytes_read,*len);
                *len = 0;
                CC1101_strobe(spifd, STROBE_SFRX);              // Flush CC1101 RX FIFO.
                CC1101_strobe(spifd, STROBE_SIDLE);             // Leaves the CC1101 in idle.
                CC1101_wait(spifd);                             // Waits until CC1101 FSM reaches a valid state.
                return ETIMEOUT;
            }
        }else{
            // Has no data to read from the RX FIFO:
            crc = true;
            *len = 0;
        }
    }
    CC1101_strobe(spifd, STROBE_SFRX);                          // Flush CC1101 RX FIFO.
    CC1101_strobe(spifd, STROBE_SIDLE);                         // Leaves the CC1101 in idle.
    CC1101_wait(spifd);                                         // Waits until CC1101 FSM reaches a valid state.

    return (crc == true ? NOERROR : ECHECKSUM);
}

/* WAIT VALID FSM STATE *****************************************************************************
 *  Name    :   CC1101_wait                                                                         *
 *  Descr.  :   Waits until the CC1101 Finite State Machine reaches a valid state. This function    *
 *              waits a finite amount of cycles to prevent deadlocks and allow unexpected           *
 *              situations to be solved (probably with a CC1101 reset, performed outside this       *
 *              routine).                                                                           *
 *  Args    :   spifd -> the current SPI file descriptor.                                           *
 *  Returns :   Error code NOERROR or EUNDEF.                                                       *
 *  Author  :   Carles Araguz López.                                                                *
 *  Remarks :   This function is inlined to avoid overhead.                                         *
 ****************************************************************************************************/
inline int CC1101_wait(int spifd)
{
    int count = 10000;
    while(CC1101_readsingle(spifd, MARCSTATE) != 0x01 && --count) // Waits until CC1101 FSM reaches a valid state.
        usleep(100);
    if(count <= 0) return EUNDEF;
    else return NOERROR;
}

/* CC1101 READ SINGLE REGISTER FUNCTION *************************************************************
 *  Name    :   CC1101_readsingle                                                                   *
 *  Descr.  :   Read the value of one register on the CC1101 chipset.                               *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - char      addr            :   Address of the register to read.                                *
 *  Returns :   The value of the register.                                                          *
 *  Author  :   Marc Marí Barceló, Roger Olivé Muñiz.                                               *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
char CC1101_readsingle(int spifd, char addr)
{
    char command = {addr | READ_SINGLE };
    char buffer[1];                                                                                 // $ca$ "char buffer[1]" === "char buffer". No need to declare the size nor malloc. Use &buffer.
    spi_read(spifd, command, buffer, 1);
    return buffer[0];
}

/* CC1101 WRITE SINGLE REGISTER FUNCTION ************************************************************
 *  Name    :   CC1101_writesingle                                                                  *
 *  Descr.  :   Write the value of one register on the CC1101 chipset.                              *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - char      addr            :   Address of the register to write.                               *
 *  - char      value           :   Value of the register to write.                                 *
 *  Author  :   Marc Marí Barceló, Roger Olivé Muñiz.                                               *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void CC1101_writesingle(int spifd, char addr, char *value)
{
    char data[] = {addr, *value};
    spi_write(spifd, data, 2);
}

/* CC1101 WRITE BURST REGISTER FUNCTION *************************************************************
 *  Name    :   CC1101_writeburst                                                                   *
 *  Descr.  :   Write the on registers, starting in the address given on the CC1101 chipset.        *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - char      addr            :   Address of the register to write.                               *
 *  - char*     buffer          :   Data to write starting from the address given.                  *
 *  - char      count           :   Size of buffer.                                                 *
 *  Author  :   Marc Marí Barceló, Roger Olivé Muñiz.                                               *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void CC1101_writeburst(int spifd, char addr, char* buffer, char count)
{
    char* data;
    data = malloc(count+1);
    data[0] = addr | WRITE_BURST;
    memcpy(data+1, buffer, count);
    spi_write(spifd, data, count+1);
    free(data);
}

/* CC1101 COMMAND STROBES FUNCTION ******************************************************************
 *  Name    :   CC1101_strobe                                                                       *
 *  Descr.  :   Send a strobe command to the CC1101.                                                *
 *  Args.   :                                                                                       *
 *  - int       spifd           :   File descriptor to a valid SPI interface.                       *
 *  - char      strobe          :   Value of the strobe command to send.                            *
 *  Author  :   Marc Marí Barceló, Roger Olivé Muñiz.                                               *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void CC1101_strobe(int spifd, char strobe)
{
    char command[] = {strobe};
    spi_write(spifd, command, 1);
}

// EOF

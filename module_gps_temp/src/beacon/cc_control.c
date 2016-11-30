#include "cc_low_level.h"
#include "rs_work.h"
/*** FUNCTIONS **************************************************************************************/

/* COMMS SETUP FUNCTION *****************************************************************************
 *  Name    :   setup                                                                               *
 *  Descr.  :   Setup for the comms subsystem.                                                      *
 *  Returns :   0      -> Setup successful .                                                        *
 *              != 0   -> Error while on setup.                                                     *
 *  Author  :   Marc Marí Barceló.                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/

int timeouts;
unsigned char freq[3];
bool global_fec_dis;
int  gpiofd;                 // The GPIO pin file descriptor.
int  spifd;

/* PREPARE GPIO DRIVERS *****************************************************************************
 *  Name    :   gpio_prepare                                                                        *
 *  Descr.  :   Setups the GPIO drivers.                                                            *
 *  Returns :   ESETUP  ->  Problem when preparing the interface.                                   *
 *              NOERROR ->  Ok.                                                                     *
 *  Author  :   --                                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int gpio_prepare()
{
    wiringPiSetup();
    pinMode(GPIO_COMMS_PAC,OUTPUT);
    digitalWrite(GPIO_COMMS_PAC,LOW);
    return NOERROR;
}

/* SET GPIO PIN HIGH ********************************************************************************
 *  Name    :   gpio_high                                                                           *
 *  Descr.  :   Sets the PAC pin (in the ³Cat) or the relay control pin (in Ground Station) high.   *
 *  Returns :   NOERROR if everything goes as expected, a documented error code otherwise.          *
 *  Author  :   --                                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int gpio_high()
{
    digitalWrite(GPIO_COMMS_PAC, HIGH);
    usleep(100000);
    return NOERROR;
}

/* SET GPIO PIN LOW *********************************************************************************
 *  Name    :   gpio_low                                                                            *
 *  Descr.  :   Sets the PAC pin (in the ³Cat) or the relay control pin (in Ground Station) low.    *
 *  Returns :   NOERROR if everything goes as expected, a documented error code otherwise.          *
 *  Author  :   --                                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int gpio_low()
{
    digitalWrite(GPIO_COMMS_PAC, LOW);
    usleep(100000);
    return NOERROR;
}

/* COMMS HALT FUNCTION ******************************************************************************
 *  Name    :   halt                                                                                *
 *  Descr.  :   Halt action for the comms subsystem.                                                *
 *  Author  :   Marc Marí Barceló.                                                                  *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void halt()
{
    int spifd;
    spifd = spi_open(SPI_DEVICE);
    if(spifd == 0)
        CC1101_strobe(spifd, STROBE_SRES);

    spi_close(spifd);
    gpio_low();

    printfd("Halting Comms\n");
}

/* This function opens a SPIFD and a GPIOFD, both are internal global variables from cc_control.c*/
/* The GPIO is intended for use to enable PAC pin of RFC-1100H */
int setup()
{
    FILE *  cfgfd;                  // comms.conf file descriptor.
    int     datarate;               // CC1101 configuration: data rate.
    int     syncword1, syncword0;   // CC1101 configuration: synchronization words.
    int     addr;                   // CC1101 configuration: address.
    int     setup_error;            // Error storage for this function.
    char    line[MAX_LINE_SIZE];    // Single comms.conf line.
    char    str1[MAX_LINE_SIZE];    // comms.conf variable name.
    char    str2[MAX_LINE_SIZE];    // comms.conf value.
    int     match;                  // Number of sscanf matches in a comms.conf line
    char    patable[] = {0x12, 0x0E, 0x1D, 0x34, 0x60, 0x84, 0xC8, 0xC0}; // Register configuration.

    // Open config file
    cfgfd = fopen("comms.conf","r");
    if(cfgfd == NULL) return EOPEN;
    // Set default values:
    timeouts = CONF_DEF_HW_TIMEOUT;
    freq[0] = CONF_DEF_FREQ0;         // Sets RX frequency.
    freq[1] = CONF_DEF_FREQ1;         // Sets RX frequency.
    freq[2] = CONF_DEF_FREQ2;         // Sets RX frequency.
    datarate = CONF_DEF_DRATE;
    syncword1 = CONF_DEF_SYNC1;
    syncword0 = CONF_DEF_SYNC0;
    addr = CONF_DEF_ADDR;
    global_fec_dis = CONF_DEF_FEC_DIS;

    while(fgets(line, MAX_LINE_SIZE, cfgfd) != NULL)
    {
        match = sscanf(line,"%[^=\n ] %*[=] %s",str1,str2);

        if(match == 2)
        {
            // Valid line:
            if(!strcmp(str1,"HW_TIMEOUT")){
                timeouts = strtol(str2,NULL,10);
            }else if(!strcmp(str1,"FREQ0")){
                freq[0] = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"FREQ1")){
                freq[1] = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"FREQ2")){
                freq[2] = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"DRATE")){
                datarate = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"SYNC1")){
                syncword1 = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"SYNC0")){
                syncword0 = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"SYNC2")){
                printfd("WARNING!! Using the deprecrated parameter SYNC2 in the comms.conf file. Will setup SYNC0 instead.\n");
                syncword0 = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"ADDR")){
                addr = (int)strtol(str2,NULL,16);
            }else if(!strcmp(str1,"FEC_DIS")){
                printfd("FEC_DIS --> %ld\n", strtol(str2, NULL, 10));
                if((int)strtol(str2, NULL, 10) > 0)
                {
                    global_fec_dis = true;  // 1.
                } else {
                    global_fec_dis = false; // 0.
                }
            }else{
                printfd("Unknown parameter %s\n",str1);
            }
        }
    }

    if(timeouts <= 0) timeouts = CONF_DEF_HW_TIMEOUT;

    fclose(cfgfd);

    printfd("\tHardware timeout: %ld miliseconds %s\n", timeouts,
                                        (timeouts == CONF_DEF_HW_TIMEOUT ? "(default)" : ""));
    printfd("\tTransmission frequency: 0x%.2x%.2x%.2x\n", freq[2], freq[1], freq[0]);

    char rfSettings[] = {
        0x00, 0x0F, // IOCFG2 GDO2 output pin configuration.
        0x02, 0x06, // IOCFG0 GDO0 output pin configuration.
        0x03, 0x47, // FIFOTHR RXFIFO and TXFIFO thresholds.
        0x04, syncword1, // SYNC1 Sync Word.
        0x05, syncword0, // SYNC0 Sync Word.
        0x06, PKTLEN,   // PKTLEN Packet length.
        0x07, 0x00, /* PKTCTRL1 Packet automation control: (0x00 = b0000 0000)
                     *  7..5: PQT ------------ Preamble quality estimator threshold.
                     *     4: (Not used.)
                     *     3: CRC_AUTOFLUSH -- 0 = no (disabled).
                     *     2: APPEND_STATUS -- 0 = do not append 2 status bytes to payload.
                     *  1..0: ADR_CHK -------- 00 = no address check.
                     */
        0x08, (0x44 | (int)global_fec_dis),
                    /* PKTCTRL0 Packet automation control. (0x44 = b0100 010X) --------------------- !!!
                     *     7: (Not used.)
                     *     6: WHITE_DATA ----- 1 = on.
                     *  5..4: PKT_FORMAT ----- 00 = normal mode. FIFO's for RX and TX.
                     *     3: (Not used.)
                     *     2: CRC_EN --------- 1 = enabled.
                     *  1..0: LENGTH_CONFIG -- 00 = fixed packet lentgh, or
                     *        ---------------- 01 = variable packet length.
                     */
        0x09, addr, // ADDR Device address.
        0x0A, 0x00, // CHANNR Channel number.
        0x0B, 0x06, // FSCTRL1 Frequency synthesizer control.
        0x0C, 0x00, // FSCTRL0 Frequency synthesizer control.
        0x0D, freq[2], // FREQ2 Frequency control word, high unsigned char.
        0x0E, freq[1], // FREQ1 Frequency control word, middle unsigned char.
        0x0F, freq[0], // FREQ0 Frequency control word, low unsigned char.
        0x10, datarate, // MDMCFG4 Modem configuration. Sets the BW of the filter and the data rate.
        0x11, 0x83, // MDMCFG3 Modem configuration.
        0x12, 0x13, // MDMCFG2 Modem configuration.
        0x13, (0x32 | ((!global_fec_dis) << 7)),
                    /* MDMCFG1 Modem configuration. (0x32 = X011 0010)
                     *     7: FEC_EN -------- 1 = enable; 0 = disable.
                     *  6..4: NUM_PREAMBLE -- 011 = 6 (min. preamble bytes)
                     *  3..2: (Not used.)
                     *  1..0: CHANSPC_E ----- channel spacing exponent.
                     */
        0x14, 0xF8, // MDMCFG0 Modem configuration.
        0x15, 0x14, // DEVIATN Modem deviation setting (when FSK modulation is enabled).
        0x17, 0x00, // MCSM1 Main Radio Control State Machine configuration.
        0x18, 0x34, // MCSM0 Main Radio Control State Machine configuration.
        0x19, 0x47, // FOCCFG Frequency Offset Compensation Configuration.
        0x1A, 0x6C, // BSCFG Bit synchronization Configuration.
        0x1B, 0x03, // AGCCTRL2 AGC control.
        0x1C, 0x38, // AGCCTRL1 AGC control.
        0x1D, 0x91, // AGCCTRL0 AGC control.
        0x21, 0x51, // FREND1 Front end RX configuration.
        0x22, 0x17, // FREND0 Front end TX configuration.
        0x23, 0xE9, // FSCAL3 Frequency synthesizer calibration.
        0x24, 0x2A, // FSCAL2 Frequency synthesizer calibration.
        0x25, 0x00, // FSCAL1 Frequency synthesizer calibration.
        0x26, 0x1F, // FSCAL0 Frequency synthesizer calibration.
        0x29, 0x59, // FSTEST Frequency synthesizer calibration.
        0x2C, 0x81, // TEST2 Various test settings.
        0x2D, 0x35, // TEST1 Various test settings.
        0x2E, 0x09  // TEST0 Various test settings.
    };
    // Open SPI port
    spifd = spi_open(SPI_DEVICE);
    if(spifd < 0)
    {
        printfd("Could not open SPI device "SPI_DEVICE"\n");
        return ESETUP;
    }
    // Open GPIO port
    if(gpio_prepare() != NOERROR)
    {
        printfd("Could not prepare GPIO ports\n");
        return ESETUP;
    }

    printfd("Writing reset strobe (SRES)\n");
    CC1101_strobe(spifd, STROBE_SRES);

    printfd("Writing PATABLE configuration\n");
    CC1101_writeburst(spifd, PATABLE, patable, sizeof(patable));
    printfd("Writing RF settings (%d bytes)\n",sizeof(rfSettings));
    spi_write(spifd, rfSettings, sizeof(rfSettings));

    setup_error = CC1101_readsingle(spifd, MARCSTATE);
    if(setup_error != 0x01){
        if(setup_error == 0x00 || setup_error == 0xFF)
            return EHWFAULT2;
    }
    printfd("Starting calibration (SCAL)\n");
    CC1101_strobe(spifd, STROBE_SCAL);
    if(CC1101_wait(spifd) != NOERROR)
    {
        printfd("Calibration timed out\n");
        return EHWFAULT1;
    }else{
        printfd("Calibration Ok. Mode: Automatic Calibration before TX and RX\n");
    }
    // Check that the configuration has been written correctly:
    str1[0] = CC1101_readsingle(spifd, FREQ2);
    str1[1] = CC1101_readsingle(spifd, FREQ1);
    str1[2] = CC1101_readsingle(spifd, FREQ0);
    str1[3] = CC1101_readsingle(spifd, SYNC1);
    str1[4] = CC1101_readsingle(spifd, SYNC0);
    str1[5] = CC1101_readsingle(spifd, DRATE);
    str1[6] = CC1101_readsingle(spifd, 0x08);
    str1[7] = CC1101_readsingle(spifd, 0x13);
    if( str1[0] == freq[2]   &&
        str1[1] == freq[1]   &&
        str1[2] == freq[0]   &&
        str1[3] == syncword1    &&
        str1[4] == syncword0    &&
        str1[5] == datarate     &&
        str1[6] == (0x44 | (int)global_fec_dis) &&
        str1[7] == (0x32 | ((!global_fec_dis) << 7)))
    {
        printfd("Register Setup: FREQ: %.2x %.2x %.2x SYNC: %.2x %.2x DRATE: %.2x\n",
                freq[2],
                freq[1],
                freq[0],
                syncword1,
                syncword0,
                datarate);

        printfd("Forward Error Correction (PKTCTRL0 0x%.2x): %s\n", str1[6], (global_fec_dis ? "disabled" : "enabled"));
        printfd("Packet length setup (MDMCFG1 0x%.2x): %s\n", str1[7], (global_fec_dis ? "variable" : "fixed"));
        printfd("Packet length (PKTLEN 0x%.2x): %d bytes\n", PKTLEN, PKTLEN);
        printfd("Maximum packet length: %d\n", MAX_PKT_LEN);
    }else{
        printfd("Register setup failed:\n");
        printfd("FREQ2 (@%.2x) %.2x / %.2x\n",FREQ2,str1[0],freq[2]);
        printfd("FREQ1 (@%.2x) %.2x / %.2x\n",FREQ1,str1[1],freq[1]);
        printfd("FREQ0 (@%.2x) %.2x / %.2x\n",FREQ0,str1[2],freq[0]);
        printfd("SYNC1 (@%.2x) %.2x / %.2x\n",SYNC1,str1[3],syncword1);
        printfd("SYNC0 (@%.2x) %.2x / %.2x\n",SYNC0,str1[4],syncword0);
        printfd("DRATE (@%.2x) %.2x / %.2x\n",DRATE,str1[5],datarate);

        spi_close(spifd);
        return EHWINTERNAL;
    }
    return NOERROR;
}

int BeaconConnect()
{
    int err
    err = setup();
    if (err != NOERROR){
        fprintf(stderr, "SETUP error: %d\n", err);
        return -1;
    }else{
        return 0;
    }
}

/* this function encodes a 222 padded array, turns it into a 255 RS protected array and sends it */
/* this function uses a spifd that has been opened at SETUP function */
/* is called 223, but is a 222 array that you are passing */
void BeaconWrite(const void * buff, int size)
{
    unsigned char data[223];
    unsigned char coded_data[PKTLEN];
    int i;

    if (size > 222){
        return;
    }
    /*  thats a trick, but as send_packet is adding a byte, we have to tell to the RS encoder
        that the first byte will be == to the size of the PKT, in that case is 0xFE (254) */
    /*  part of the trick, send data+1, without passing the 0xFE length byte 
        not that MAX_PKT_LEN is 254. */

    /* We do not care, we do the padding here xD */
    for (i = 0; i < 223; i++){
        /* initialise with random bytes! */
        data[i] = rand()%256;
    }
    data[0] = (unsigned char) size;
    memcpy(data+1, buff, size);
    /* now data is filled with 223 bytes */
    /* go into the RS encoder */
    encode_rs_message(data, 223, coded_data, 255);
    gpio_high();
    send_packet(spifd, coded_data, PKTLEN);
    gpio_low();
}
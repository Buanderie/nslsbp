/* CubeCAT ******************************************************************************************
 *  File:   hwmod_spi.c                                                                             *
 *  Descr.: SPI communication interface.                                                            *  
 *  Author: Marc Marí Barceló.                                                                      *
 *  Date:   2013-may-15                                                                             *
 *  Vers.:  0.1                                                                                     *
 *                                                                                                  *
 *  This file is part of the CubeCAT v1.0 project. "CubeCAT" is an educational project developed at *
 *  the Technical University of Catalonia - BarcelonaTech (Universitat Politècnica de Catalunya).   * 
 ****************************************************************************************************
 *  Changelog:                                                                                      *
 *  - v0.1  Marí M.     Creation.                                                                   *
 ****************************************************************************************************/

/*** INCLUDE SECTION ********************************************************************************/
#include "spidev.h"
#include "error_def.h" 

/*** FUNCTIONS **************************************************************************************/

/* SPI OPEN FUNCTION ********************************************************************************
 *  Name    :   spi_open                                                                            *
 *  Descr.  :   Open connection to a SPI port.                                                      *
 *  Args.   :                                                                                       *
 *  - char*     port            :   Path to the port on which perform the connection                *
 *                                                                                                  *
 *  Returns :   EOPEN     -> Error opening the port                                                 *
 *              != EOPEN  -> A valid file descriptor for writing or reading through port            *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int spi_open(char* port)
{
    int spifd;
    spifd = open(port, O_RDWR | O_NONBLOCK);
    if(spifd == -1)
        return EOPEN;
    else
        return spifd;
}

/* SPI READ FUNCTION ********************************************************************************
 *  Name    :   spi_read                                                                            *
 *  Descr.  :   Read buffer from SPI.                                                               *
 *  Args.   :                                                                                       *
 *  - int       fd              :   File descriptor where the read will be performed                *
 *  - char      command         :   Command to start reading                                        *
 *  - char*     buffer          :   Buffer where data recived will be stored                        *
 *  - int       buf_size        :   Expected size of data                                           *
 *                                                                                                  *
 *  Returns :   NOERROR    -> Reading succesful, buffer has the recieved data                       *
 *              EHWFAULT1  -> Reading error, contents of buffer undefined                           *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int spi_read(int fd, char command, char* buffer, int size)
{
    struct spi_ioc_transfer	xfer[2];
    int status;

	memset(buffer, 0, sizeof(buffer));
    memset(xfer, 0, sizeof(xfer));
    
    xfer[0].tx_buf = (unsigned int)(&command);
    xfer[0].len = 1;
    
    xfer[1].rx_buf = (unsigned int)buffer;
    xfer[1].len = size;

    status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
	if(status < 0)
	    return NOERROR;
	else
	    return EHWFAULT1;
}

/* SPI WRITE FUNCTION *******************************************************************************
 *  Name    :   spi_write                                                                           *
 *  Descr.  :   Write buffer through SPI.                                                           *
 *  Args.   :                                                                                       *
 *  - int       fd              :   File descriptor where write will be performed                   *
 *  - char*     buffer          :   Buffer to write through SPI                                     *
 *  - int       buf_size        :   Number of elements in buffer                                    *
 *                                                                                                  *
 *  Returns :   NOERROR   -> Writing successful                                                     *
 *              EHWFAULT1 -> Writing error                                                          *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int spi_write(int fd, char* buffer, int size)
{
    struct spi_ioc_transfer	xfer[1];
    int status;

    memset(xfer, 0, sizeof(xfer));
    
    xfer[0].tx_buf = (unsigned int)buffer;
    xfer[0].len = size;
    
    status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if(status < 0)
	    return EHWFAULT1;
	else
	    return NOERROR;
}

/* SPI READ AND WRITE FUNCTION **********************************************************************
 *  Name    :   spi_full                                                                            *
 *  Descr.  :   Perform a read and write operation through SPI using full duplex.                   *
 *  Args.   :                                                                                       *
 *  - int       fd              :   File descriptor where write will be performed                   *
 *  - char*     bufferin        :   Buffer to write through SPI                                     *
 *  - char*     bufferout       :   Buffer to put read data                                         *
 *  - int       size            :   Number of elements in buffer                                    *
 *                                                                                                  *
 *  Returns :   NOERROR    -> Read and write successful                                             *
 *              EHWFAULT1  -> Reading or writing error                                              *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
int spi_full(int fd, char* bufferin, char* bufferout, int size)
{
    struct spi_ioc_transfer	xfer[1];
    int status;

	memset(bufferout, 0, sizeof(bufferout));
    memset(xfer, 0, sizeof(xfer));
    
    xfer[0].tx_buf = (unsigned int)(bufferin);
    xfer[0].rx_buf = (unsigned int)(bufferout);
    xfer[0].len = size;
    
    status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if(status < 0)
	    return EHWFAULT1;
	else
	    return NOERROR;
}

/* SPI CLOSE FUNCTION *******************************************************************************
 *  Name    :   spi_close                                                                           *
 *  Descr.  :   Close a SPI connection previously open.                                             *
 *  Args.   :                                                                                       *
 *  - int       fd              :   File descriptor to close                                        *
 *                                                                                                  *
 *  Author  :   Marc Marí Barceló                                                                   *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void spi_close(int fd)
{
	close(fd);
}

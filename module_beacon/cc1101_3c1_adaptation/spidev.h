/* CubeCAT ******************************************************************************************
 *  File:   hwmod_spi.h                                                                             *
 *  Descr.: SPI communication interface.                                                            *
 *  Author: Marc Marí Barceló.                                                                      *
 *  Date:   2013-apr-15                                                                             *
 *  Vers.:  0.1                                                                                     *
 *                                                                                                  *
 *  This file is part of the CubeCAT v1.0 project. "CubeCAT" is an educational project developed at *
 *  the Technical University of Catalonia - BarcelonaTech (Universitat Politècnica de Catalunya).   * 
 ****************************************************************************************************
 *  Changelog:                                                                                      *
 *  - v0.1  Marí M.     Creation.                                                                   *
 ****************************************************************************************************/

#ifndef __INCL_HWMODSPI
#define __INCL_HWMODSPI

/*** INCLUDE SECTION ********************************************************************************/
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

/*** FUNCTIONS **************************************************************************************/
int spi_open(char* port);
int spi_read(int fd, char command, char* buffer, int size);
int spi_write(int fd, char* buffer, int size);
int spi_full(int fd, char* bufferin, char* bufferout, int size);
void spi_close(int fd);

#endif

/*
 *  imupi.c: test i2c access to the Invensense MPU-9150 Gyro, Accelerometer, and Compass chip
 * 
 *  To enable i2c dev:  sudo apt-get install i2c-tools libi2c-dev
 *  To load kernel mod: in "/etc/modprobe.d/raspi-blacklist.conf" comment "blacklist i2c-bcm2708"
 *                      in "/etc/modules" add 2 lines "i2c-dev" and "i2c-bcm2708"
 *                      sudo bash -c "echo options i2c_bcm2708 baudrate=400000 > /etc/modprobe.d/i2c.conf"
 *                      reboot
 *  Check i2c address:  sudo i2cdetect -y 1
 *  Access i2c as user: sudo usermod -G i2c pi
 *  Compile with:       gcc -o imupi imupi.c -lrt
 *  Run with:           ./imupi
 * 
 *  JG, 1/1/2014
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/i2c-dev.h>
#include <mpu-9150.h>

static int             imupi_dev         = -1;
static unsigned char   mpu_9150_address  = MPU_9150_I2C_ADDRESS;
static double  g_off[IMUPI_NB_AXIS]      = { 0.0, 0.0, 0.0 };

/*
 *  imupi_init: initialize the board registers.
 */
int imupi_init( void )  {
  double  dummy_a[IMUPI_NB_AXIS], g[IMUPI_NB_AXIS], dummy_m[IMUPI_NB_AXIS];
  double  offset[IMUPI_NB_AXIS] = { 0.0, 0.0, 0.0 };
  int     ret, i;
  
  /* Open I2C device */
  
  imupi_dev = open( IMUPI_DEVNAME, O_RDWR | O_NONBLOCK);  
  if ( imupi_dev == -1 )
    return IMUPI_I2C_OPEN_ERROR;
    
  /* Initialize Invensens board */
    
  /* Initialize accelerometer and gyro */
  
  if ( ioctl( imupi_dev, I2C_SLAVE, mpu_9150_address ) < 0 )  {
      return IMUPI_I2C_DEV_NOT_FOUND;
  }
  
  // 1 kHz sampling rate: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_SMPRT_DIV, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // No ext sync, DLPF at 184Hz for the accel and 188Hz for the gyro: 0b00000001
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_DEFINE, 0x01 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // Gyro range at +/-1000 °/s: 0b00010000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_GYRO_CONFIG, 0x10 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
    
  // Accel range at +/-2g: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_ACCEL_CONFIG, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // Disable all FIFOs: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_FIFO_EN, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // Bypass mode enabled: 0b00000010
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_INT_PIN_CFG, 0x02 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // Disable all interrupts: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_INT_ENABLE, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  // No FIFO and no I2C slaves: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_USER_CTRL, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
    
  // No power management, internal clock source: 0b00000000
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_PWR_MGMT_1, 0x00 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  /* Initialize magnetometer */
  
  if ( ioctl( imupi_dev, I2C_SLAVE, MPU_9150_I2C_MAGN_ADDRESS ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;

  // Check for the AKM device ID
  ret = i2c_smbus_read_byte_data( imupi_dev, MPU_9150_WIA );
  if ( ret < 0 )
    return IMUPI_I2C_READ_ERROR;
  if ( ret != MPU_9150_AKM_ID )
    return IMUPI_I2C_DEV_NOT_FOUND;

  // Single measurement mode: 0b00000001
  if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_CNTL, 0x01 ) == -1 )
    return IMUPI_I2C_WRITE_ERROR;
  
  /* Give some time to the chip to initialize */

  usleep( 100000 );

  /* Test data reading */
  
  if ( imupi_read( dummy_a, g, dummy_m ) )
      return IMUPI_INIT_ERROR;

  /* Calculate gyro offsets */
  
  for ( i = 0; i < IMUPI_GOFF_NB_ITER; i++ )  {
    if ( imupi_read( dummy_a, g, dummy_m ) )
      return IMUPI_INIT_ERROR;

    offset[0] += g[0];
    offset[1] += g[1];
    offset[2] += g[2];
  }
  
  g_off[0] = offset[0] / IMUPI_GOFF_NB_ITER;
  g_off[1] = offset[1] / IMUPI_GOFF_NB_ITER;
  g_off[2] = offset[2] / IMUPI_GOFF_NB_ITER;
  
    
  return IMUPI_NO_ERROR;
}

/*
 *  imupi_terminate: close i2c device.
 */
void imupi_terminate( void )  {
  
  if ( imupi_dev != -1 )  {
    close( imupi_dev );
    imupi_dev = -1;
  }
}

/*
 *  imupi_read: read all sensor values on the board and return them as double.
 */
int imupi_read( double *a, double *g, double *m ) {
  uint8_t         block[IMUPI_BLOCK_SIZE];
  int             ret, i;
  static double   last_m[3] = { 0.0, 0.0, 0.0 };
  static int      mag_state = 0;
  static uint8_t  mblock[IMUPI_BLOCK_SIZE] = { 0, 0, 0, 0, 0, 0 };
  
  a[0] = a[1] = a[2] = g[0] = g[1] = g[2] = m[0] = m[1] = m[2] = 0.0;
  
  if ( imupi_dev == -1 )
    return IMUPI_INIT_ERROR;
    
  /* Read accelerations */
  
  if ( ioctl( imupi_dev, I2C_SLAVE, mpu_9150_address ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;
  
  if ( i2c_smbus_read_i2c_block_data( imupi_dev, IMUPI_I2C_AUTO_INCREMENT | MPU_9150_ACCEL_XOUT_H, IMUPI_BLOCK_SIZE, block ) != IMUPI_BLOCK_SIZE )
    return IMUPI_I2C_READ_ERROR;
  
  a[0] = (double)( (int16_t)( block[0] << 8 | block[1] ) * IMUPI_A_GAIN );
  a[1] = (double)( (int16_t)( block[2] << 8 | block[3] ) * IMUPI_A_GAIN );
  a[2] = (double)( (int16_t)( block[4] << 8 | block[5] ) * IMUPI_A_GAIN );
  
  
  /* Read gyro */
  
  if ( i2c_smbus_read_i2c_block_data( imupi_dev, IMUPI_I2C_AUTO_INCREMENT | MPU_9150_GYRO_XOUT_H, IMUPI_BLOCK_SIZE, block ) != IMUPI_BLOCK_SIZE )
    return IMUPI_I2C_READ_ERROR;
  
  g[0] = (double)( (int16_t)( block[0] << 8 | block[1] ) * IMUPI_G_GAIN ) - g_off[0];
  g[1] = (double)( (int16_t)( block[2] << 8 | block[3] ) * IMUPI_G_GAIN ) - g_off[1];
  g[2] = (double)( (int16_t)( block[4] << 8 | block[5] ) * IMUPI_G_GAIN ) - g_off[2];
  
  
  /* Read magnetometer */

  if ( ioctl( imupi_dev, I2C_SLAVE, MPU_9150_I2C_MAGN_ADDRESS ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;

  /* Read sequentially X, Y and Z to avoid too long delays */
  
  switch( mag_state ) {
    
    case 0:
      // Check if data is ready.

      ret = i2c_smbus_read_byte_data( imupi_dev, MPU_9150_ST1 );
        
      if ( ret < 0 )
        return IMUPI_I2C_READ_ERROR;
      
      if ( ret & 0x01 ) {
        mag_state = 1;
      }
      
      // Duplicate last measurements
      m[0] = last_m[0];
      m[1] = last_m[1];
      m[2] = last_m[2];
      break;
    
    case 1:
      // Read X axis

      for ( i = 0; i < 2; i++ )  {
        ret = i2c_smbus_read_byte_data( imupi_dev, MPU_9150_HXL + i );
        if ( ret < 0 )
          return IMUPI_I2C_READ_ERROR;
        mblock[i] = ret;
      }
      mag_state = 2;
      
      // Duplicate last measurements
      m[0] = last_m[0];
      m[1] = last_m[1];
      m[2] = last_m[2];
      break;
      
    case 2:
      // Read Y axis

      for ( i = 2; i < 4; i++ )  {
        ret = i2c_smbus_read_byte_data( imupi_dev, MPU_9150_HXL + i );
        if ( ret < 0 )
          return IMUPI_I2C_READ_ERROR;
        mblock[i] = ret;
      }
      mag_state = 3;
      
      // Duplicate last measurements
      m[0] = last_m[0];
      m[1] = last_m[1];
      m[2] = last_m[2];
      break;
    
    case 3:
      // Read Z axis

      for ( i = 4; i < 6; i++ )  {
        ret = i2c_smbus_read_byte_data( imupi_dev, MPU_9150_HXL + i );
        if ( ret < 0 )
          return IMUPI_I2C_READ_ERROR;
        mblock[i] = ret;
      }
      
      m[1] = (double)( (int16_t)( mblock[1] << 8 | mblock[0] ) * IMUPI_M_GAIN );
      m[0] = (double)( (int16_t)( mblock[3] << 8 | mblock[2] ) * IMUPI_M_GAIN );
      m[2] = -(double)( (int16_t)( mblock[5] << 8 | mblock[4] ) * IMUPI_M_GAIN );
      
      last_m[0] = m[0];
      last_m[1] = m[1];
      last_m[2] = m[2];
      
      // Re-arm single measurement mode
      if ( i2c_smbus_write_byte_data( imupi_dev, MPU_9150_CNTL, 0x01 ) == -1 )
        return IMUPI_I2C_WRITE_ERROR;
        
      mag_state = 0;
      break;
      
    default:
      mag_state = 0;
  }
  
  return IMUPI_NO_ERROR;
}

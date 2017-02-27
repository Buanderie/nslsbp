#include "xbee_lib.h"

static const char * telemetry_modules_array[TELEMETRY_MESSAGE_COUNT] = {
  "EEPROM" , 
  "GPS_INIT",
  "IMU_INIT",
  "VC_INIT",
  "VC_ERROR",
  "THERM_INIT",
  "THERM_ERROR",
  "HEATERS_INIT",
  "SD_INIT",
  "TC",
  "GPS",
  "IMU",
  "VC",
  "TH"
  };

/* This array goes from 0 to 13 -> 14 */

static xbee_hk_data_t hk;

static bool has_vc;
static bool has_tc;
static bool has_gps;
static bool has_imu;
static bool has_thermal;


static int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static void set_mincount(int fd, int mcount, int timeout)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    /* if mcount == 0, timed read with 500ms */
    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = timeout;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


/* 
 * Methods from Arduino compatible:
 *  - begin(port)
 *  - available(port)
 *  - readBytesUntil(port)	
 */

/* UART port initialization */
void begin(const char * device, int baud, unsigned int timeout_ms, xbee_serial_parms_t * handler)
{
	handler->fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (handler->fd < 0) {
        printf("Error opening %s: %s\n", device, strerror(errno));
        handler->ret = -1;
        return;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(handler->fd, baud);
    /* Convert milli-seconds to deca-seconds */
    if (timeout_ms == 0){
    	/* pure non-blocking */
		set_mincount(handler->fd, 0, 0);                /* set to pure timed read */	
    }else if (timeout_ms < 100){
    	/* 100ms blocking */
    	set_mincount(handler->fd, 0, 1);                /* set to pure timed read */	
    }else{
    	/* specified timeout ms */
    	set_mincount(handler->fd, 0, timeout_ms/100);   /* set to pure timed read */
    }
    handler->timeout = timeout_ms;
    return;
}


int available(xbee_serial_parms_t * input_handler)
{
	int bytes_avail;
	ioctl(input_handler->fd, FIONREAD, &bytes_avail);
	return bytes_avail;
}

int read_port(xbee_serial_parms_t * input_handler)
{
	return read(input_handler->fd, input_handler->buffer, 1);
}

void clear (xbee_serial_parms_t * input_handler)
{
	if(available(input_handler) > 0)
	{
		while(read_port(input_handler) > 0);
	}
}

int readBytesUntil(xbee_serial_parms_t * input_handler, char to_find, char * buffer, int max_size)
{	
	int cnt = 0;
	if(available(input_handler) > 0){
		while (read_port(input_handler) > 0){
			/* keep reading */
			buffer[cnt] = input_handler->buffer[0];
			if ((char) buffer[cnt] == to_find){
				cnt++;
				return cnt;
			}else{
				cnt++;
				if (cnt >= max_size)
					return max_size;
			}
		}
	}
	return 0;
}

bool newMessage(xbee_serial_parms_t * input_handler, char * buff, int max_size)
{
  int ret;
  int i;
  int r_pos;
  /* Get something from DEBUG iface */
  /* Process a command from DEBUG first */
  int timeout = 0;
  while(available(input_handler) == 0 && ++timeout < input_handler->timeout){
    xbee_delay_ms(1);
  }
  if (available(input_handler) > 0){
    /* Read bytes until can return if timeout (input_handler->timeout has this info in ms) */
    ret = readBytesUntil(input_handler, '\n', buff, max_size - 1);
    if (ret > 0 && ret < max_size - 2){
      /* If there is \r present */    
      r_pos = -1;
      for (i = 0; i < ret; i++){
        if (buff[i] == '\r' && r_pos == -1){
          r_pos = i;
        }
      }
      if (r_pos == -1){
        buff[ret] = '\n';
        buff[ret+1] = '\0';
      }else{
        buff[r_pos] = '\n';
        buff[r_pos+1] = '\0';        
      }
    }
    return true;  
  }else{
  	return false;
  }
}

void upperCaseNoSpaces(char * buff){
  int i, j;
  for( i = 0, j = 0; i < strlen(buff); i++){
    buff[i-j] = buff[i];
    if(buff[i] == ' ' || buff[i] == '\n' || buff[i] == '\r'){
      j++;
    }
  }
  if(buff[i-j] == ' ' || buff[i-j] == '\n'){
    buff[i-j] = '\0';  
  }else{
    buff[i] = '\0';
  }
  for( i = 0; i < strlen(buff); i++){
    if (buff[i] >= 'a' && buff[i] <= 'z'){
      buff[i] = buff[i] - 'a' + 'A';
    }
  }
}

void processSubmessages(telemetry_modules_t system, char * str){
    int scan_matches;
    char * tok_ptr;
    printf("Message: %s\n", str);
    if (str == NULL){
      printf("Input message is NULL\n");
      return;
    }
    switch (system){
      case TELECOMMAND:
        scan_matches = sscanf(str, "%u,%u", &hk.tc.cmd, &hk.tc.ping);
        if (scan_matches == 2){
          has_tc = true;
          //printf("TC Message ->> Commands: %u, Pings: %u\n", hk.tc.cmd, hk.tc.ping);
        }
      break;
      case GPS:
        /*  GPS message has the format: 
         *  epochtime(uint),lat(float),lng(float),alt(float)
         */
        has_gps = true;
        scan_matches = sscanf(str, "%u,%f,%f,%f", &hk.gps.time, &hk.gps.lat, &hk.gps.lng, &hk.gps.alt);
        if (scan_matches == 4){
          
          //printf("GPS Message ->> Time is %u, Pos: %f, %f, %f\n", hk.gps.time, hk.gps.lat, hk.gps.lng, hk.gps.alt);
        }
      break;
      case IMU:
        /*  IMU message has the format: 
         *  R/(float),P/(float),Y/(float)
         */
        has_imu = true;      
        scan_matches = sscanf(str, "R/%f,P/%f,Y/%f", &hk.imu.roll, &hk.imu.pitch, &hk.imu.yaw);
        if (scan_matches == 3){

          //printf("IMU Message ->> Roll: %f Pitch: %f Yaw: %f\n", hk.imu.roll, hk.imu.pitch, hk.imu.yaw);
        }
      break;
      case VCSENS:
        /*  VCSens has the format:
         *  VSYS/;V3/;V5/;V12/;
         *  Separe each in witch STRTOK, then use strcmp() to detect VSYS,V3...
         *  Do sscanf(VSYS/%fV,%fA,%fW) 
         */
          has_vc = true;
          hk.vc.enabled_3 = 0;  hk.vc.enabled_5 = 0;  hk.vc.enabled_12 = 0; 
          hk.vc.vsys_v = 0;     hk.vc.vsys_i = 0;     hk.vc.vsys_w = 0;
          hk.vc.v3_v = 0;       hk.vc.v3_i = 0;       hk.vc.v3_w = 0;
          hk.vc.v5_v = 0;       hk.vc.v5_i = 0;       hk.vc.v5_w = 0;
          hk.vc.v12_v = 0;      hk.vc.v12_i = 0;      hk.vc.v12_w = 0;
          tok_ptr = strtok(str, ";");
          while(tok_ptr != NULL){
            if (strncmp(tok_ptr, "VSYS/", strlen("VSYS/")) == 0){
              scan_matches = sscanf(tok_ptr, "VSYS/%fV,%fA,%fW", &hk.vc.vsys_v, &hk.vc.vsys_i, &hk.vc.vsys_w);
              //printf("VSYS Message ->> %f Volts, %f Amps, %f Watts\n", hk.vc.vsys_v, hk.vc.vsys_i, hk.vc.vsys_w);
            }else if (strncmp(tok_ptr, "V3/", strlen("V3/")) == 0){
              scan_matches = sscanf(tok_ptr, "V3/%fV,%fA,%fW", &hk.vc.v3_v, &hk.vc.v3_i, &hk.vc.v3_w);
              if (scan_matches == 3){
                hk.vc.enabled_3 = 1;
              }
              //printf("V3 Message ->> %f Volts, %f Amps, %f Watts\n", hk.vc.v3_v, hk.vc.v3_i, hk.vc.v3_w);
            }else if (strncmp(tok_ptr, "V5/", strlen("V5/")) == 0){
              scan_matches = sscanf(tok_ptr, "V5/%fV,%fA,%fW", &hk.vc.v5_v, &hk.vc.v5_i, &hk.vc.v5_w);
              if (scan_matches == 3){
                hk.vc.enabled_5 = 1;
              }              
              //printf("V5 Message ->> %f Volts, %f Amps, %f Watts\n", hk.vc.v5_v, hk.vc.v5_i, hk.vc.v5_w);
            }else if (strncmp(tok_ptr, "V12/", strlen("V12/")) == 0){
              scan_matches = sscanf(tok_ptr, "V12/%fV,%fA,%fW", &hk.vc.v12_v, &hk.vc.v12_i, &hk.vc.v12_w);
              if (scan_matches == 3){
                hk.vc.enabled_12 = 1;
              }              
              //printf("V12 Message ->> %f Volts, %f Amps, %f Watts\n", hk.vc.v12_v, hk.vc.v12_i, hk.vc.v12_w);            
            }
            tok_ptr = strtok(NULL, ";");
          }
      break;
      case THERMAL:
          has_thermal = true;
          tok_ptr = strtok(str, ";");
          while(tok_ptr != NULL){
            if (strncmp(tok_ptr, "AUTO", strlen("AUTO")) == 0){
              hk.therm.auto_manual_flag = 1;
            }else if (strncmp(tok_ptr, "MANUAL", strlen("MANUAL")) == 0){
              hk.therm.auto_manual_flag = 0;
            }else if (strncmp(tok_ptr, "OUT/", strlen("OUT/")) == 0){
              scan_matches = sscanf(tok_ptr, "OUT/%f,0", &hk.therm.out_t);
              //printf("OUT Temp Message ->> %f C, Heater: 0\n", hk.therm.out_t);
            }else if (strncmp(tok_ptr, "PAY/", strlen("PAY/")) == 0){
              scan_matches = sscanf(tok_ptr, "PAY/%f,%u", &hk.therm.pay_t, (unsigned int *) &hk.therm.pay_enabled);
              //printf("PAY Temp Message ->> %f C, Heater: %u\n", hk.therm.pay_t, hk.therm.pay_enabled);              
            }else if (strncmp(tok_ptr, "BAT/", strlen("BAT/")) == 0){
              scan_matches = sscanf(tok_ptr, "BAT/%f,%u", &hk.therm.bat_t, (unsigned int *) &hk.therm.bat_enabled);
              //printf("BAT Temp Message ->> %f C, Heater: %u\n", hk.therm.bat_t, hk.therm.bat_enabled);                
            }else if (strncmp(tok_ptr, "GEN/", strlen("GEN/")) == 0){
              scan_matches = sscanf(tok_ptr, "GEN/%f,%u", &hk.therm.gen_t, (unsigned int *) &hk.therm.gen_enabled);
              //printf("GEN Temp Message ->> %f C, Heater: %u\n", hk.therm.gen_t, hk.therm.gen_enabled);                
            }
            tok_ptr = strtok(NULL, ";");
          }
      break;
      default:
        printf("Configuration message from %s ->> %s\n", telemetry_modules_array[system], str);
      break;
    }


 /* ptr = strtok(str, ";");
  while(ptr != NULL){
    printf("Param: %s. ", ptr);
    ptr = strtok(NULL, ";");
  }
  printf("\n");*/

}

telemetry_modules_t parseTelemetryModule(const char * str){
  telemetry_modules_t module;
  for (module = EEPROM; module < TELEMETRY_MESSAGE_COUNT; module++){
    if (strcmp(str, telemetry_modules_array[module]) == 0){
      //printf("Module: %s -->>\t", telemetry_modules_array[module]);
      break;
    }
  }
  return module;
}

bool pingDutyCycle(unsigned int now, int duty){
  static unsigned int last_time = 0;
  if ((last_time + duty) <= now){
    last_time = now;
    return true;
  }else{
    return false;
  }
}

void sendPing(xbee_serial_parms_t * output_handler){
  strncpy((char *) output_handler->buffer, "SYSTEM,PING", strlen("SYSTEM,PING"));
  write(output_handler->fd, output_handler->buffer, strlen("SYSTEM,PING"));
}

int work (char * dev_name)
{
	xbee_serial_parms_t xbee;
	char arg1[128], arg2[128];
	char buffer[256];

	begin(dev_name, B9600, 500, &xbee);
	/* Clear the input */
  has_thermal = false;
  has_vc = false;
  has_imu = false;
  has_gps = false;
  has_tc = false;
	clear(&xbee);
  
	while(1){
		if (newMessage(&xbee, buffer, sizeof(buffer))){
			/* This shit removes the \r\n */
			upperCaseNoSpaces(buffer);
			//printf("Received Message: %s --> length: %d\n", buffer, (int) strlen(buffer));
			/* From here just parse! */
			/* Get as many ; From each */
			/* This message is 1 line, corresponding to TC, GPS, IMU, etc. */
			/* Identify it! */
			/* Read until ':' char */
      printf("ucns: %s\n", buffer);
			if (sscanf(buffer, "%[^:] %*[:] %[^\r\n]", arg1, arg2) == 2){
			/* Copmpare the arg1 and then go into a function for arg2 */
				//printf("Scanned: %s -> %s\n", arg1, arg2);
        /* for each ; is a message into the message sub-block */
        /* strtok to split each ; into 1 single message and add to database or something */
        processSubmessages(parseTelemetryModule(arg1), arg2);
        if (has_thermal && has_imu && has_gps && has_tc && has_vc){
          printf("Saving to DB\n");
          dbman_save_xbee_data(&hk);
          has_thermal = false;
          has_vc = false;
          has_imu = false;
          has_gps = false;
          has_tc = false;
        }
			}
      /* Continue the loop again for a new message to be received */
      /* If no message in the millisecond input, send ping */
		}else if (pingDutyCycle(time(NULL), 2) ){
      sendPing(&xbee);
    }else{
      /* otherwise, sleep */
      xbee_delay_ms(100);
      has_thermal = false;
      has_vc = false;
      has_imu = false;
      has_gps = false;
      has_tc = false;
    }
	}
  return 0;
}

int main(int argc, char ** argv){
  if (argc == 2)
    work(argv[1]);
}
/*
 * @author  Alexander Rüedlinger <a.rueedlinger@gmail.com>
 * @date    08.03.2015
 *
 * A C driver for the sensor MCP9808.
 *  
 */
#ifndef MCP9808_H_
#define MCP9808_H_

void *mcp9808_init(int address, const char* i2c_device_filepath);

void mcp9808_close(void *_s);

float mcp9808_temperature(void *_s);


#endif
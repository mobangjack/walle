#pragma once

#include "cmsis/cbus.h"
#include "serial/serial.h"

class Robot
{
public:
	Robot(const char* port = "", uint32_t baudrate = 115200);
	~Robot();

	void setPort(const char* port);
	std::string getPort();
	void setBaudrate(uint32_t baudrate);
	uint32_t getBaudrate();

	void connect();
	bool isConnected();
	void release();
	bool cmd(uint8_t pan, uint8_t tilt, uint8_t zoom, uint8_t fire);
	
protected:
	serial::Serial serial;
	CBus cbus;
	uint8_t cbuf[CBUS_BUF_LEN];
};
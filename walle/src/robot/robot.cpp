#include "robot.hpp"
#include <iostream>

Robot::Robot(const char* port, uint32_t baudrate)
{
	std::cout << "Constructing Robot Object..." << std::endl;
	serial.setPort(port);
	serial.setBaudrate(baudrate);
	std::cout << "Robot constructed with serial port: " << port << ", baudrate: " << baudrate << std::endl;
}

Robot::~Robot()
{
	std::cout << "Destructing Robot Object..." << std::endl;
	if (isConnected())
	{
		std::cout << "Robot is still connected with serial port: " << serial.getPort() << std::endl;
		std::cout << "Closing Robot serial port: " << serial.getPort() << " ... " << std::endl;
		serial.close();
	}
	std::cout << "Robot Object destructed. " << std::endl;
}

void Robot::setPort(const char* port)
{
	std::cout << "Setting Robot serial at port: " << port << ", previous: " << getPort() << std::endl;
	serial.setPort(port);
	std::cout << "Robot serial port set done. Current @ " << getPort() << std::endl;
}

std::string Robot::getPort()
{
	return serial.getPort();
}

void Robot::setBaudrate(uint32_t baudrate)
{
	std::cout << "Setting Robot serial at baudrate: " << baudrate << ", previous: " << getBaudrate() << std::endl;
	serial.setBaudrate(baudrate);
	std::cout << "Robot serial baudrate set done. Current @ " << getBaudrate() << std::endl;
}

uint32_t Robot::getBaudrate()
{
	return serial.getBaudrate();
}

void Robot::connect()
{
	if (isConnected()) {
		std::cout << "WARNING: Try to reconnect serial port: " << serial.getPort() << ". Robot is already connected on it. Function connect() returned directly doing nothing." << std::endl;
		return;
	}
	else
	{
		std::cout << "Connecting Robot through serial port " << getPort() << " ... " << std::endl;
		try
		{
			serial.open();
			std::cout << "Robot connected @ " << getPort() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "Robot serial connection exception: " << e.what() << std::endl;
		}
	}
}

bool Robot::isConnected()
{
	return serial.isOpen();
}

void Robot::release()
{
	if (!isConnected()) 
	{
		std::cout << "Robot is not connected @ " << getPort() << "currently." << std::endl;
		return;
	}
	try
	{
		std::cout << "Releasing Robot serial port..." << std::endl;
		serial.close();
		std::cout << "Robot released @ " << getPort() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Robot release exception: " << e.what() << std::endl;
	}
}

bool Robot::cmd(uint8_t pan, uint8_t tilt, uint8_t zoom, uint8_t fire)
{
	uint32_t len = 0;
	std::cout << "Commanding Robot @ " << "pan=" << pan << "tilt=" << tilt << "zoom=" << zoom << "fire=" << std::hex << fire << std::endl;
	cbus.x = pan;
	cbus.y = tilt;
	cbus.z = zoom;
	cbus.f = fire;
	CBus_PackUp(&cbus, cbuf);
	len = serial.write(cbuf, CBUS_BUF_LEN);
	std::cout << "Command serial data total length: " << CBUS_BUF_LEN << ", wrote length: " << len << std::endl;
	return len == CBUS_BUF_LEN;
}

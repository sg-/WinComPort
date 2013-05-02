#ifndef COMPORT_H
#define COMPORT_H

// Windows Header Files:
#include "../stdafx.h"
// class dependencies - www.naughter.com/enumser.html
#include "enumser\enumser.h"
#include "ScheduledTask.h"

class ComPort : private ScheduledTask
{
private:
	// the device handle to access the hardware
	HANDLE _comport_handle;
	// the resolved hardware name to open the device by eg: "\\\\.\\COMX"
	wchar_t _port_name[16];
	// the friendly name we open the port by
	char _friendly_name[256];

protected:
	// this is a onTimer callback handler that manages the hot disconnect condition
	virtual void PerformTask(void);
	// converts a friendly name to the COM port number (0 - disconnected to 255)
	uint8_t friendlyNameToNum(char *friendly_name);

public:
	// constructor 
	ComPort();
	// de-constructor
	~ComPort();
	// check to see if a com port is available by number
	BOOL isAvailable(uint8_t port_num);
	// open a comport by number
	BOOL open(uint8_t port_num, uint32_t baudrate);
	// open a comport by friendly name
	BOOL open(char * friendly_name, uint32_t baudrate);
	// close an opened comport
	void close(void);
	
	// check to see if data is in the rx buffer 
	DWORD isReadable(void);
	// check to see if a comport is open
	BOOL isOpen(void);
	
	// different ways to write to a comport
	BOOL write(uint8_t data);
	BOOL write(wchar_t *data);
	BOOL write(char *data);

	// different ways to read from a comport
	BOOL read(uint8_t &data);
	BOOL read(char *data, uint16_t amt = 4096);
	BOOL read(wchar_t* data, uint16_t amt = 4096);
};

#endif



/* This implementation of the three com routines
	is for a WIN32 execution environment.
	Replace as necessary for your particular environment. */

#include "ComPort.h"

void ComPort::PerformTask(void)
{
	static int i = 0;
	printf("%d\n", i++);
	// this is called every xx ms when the port is opened by the friendly name
	//	make sure the device is connected
	if(isOpen())
	{
		if (0 == friendlyNameToNum(_friendly_name))
		{
			close();
		}
	}

	return;
}

uint8_t ComPort::friendlyNameToNum(char *friendly_name)
{
	CSimpleArray<UINT>ports;
	CSimpleArray<CString> friendlyNames;
	CString arg((char *)friendly_name);
	uint8_t result = 0;
	// looks for the friendly name passed in the parameter. If multiple exist only the last 
	//	through the loop is returned
	if (CEnumerateSerial::UsingSetupAPI2(ports, friendlyNames))
	{
		for (uint8_t i=0; i<ports.GetSize(); i++)
		{
			if (0 == friendlyNames[i].CompareNoCase(arg))
			{
				result = ports[i];
			}
		}
	}

	return result;
}

ComPort::ComPort() : ScheduledTask(1000)
{
	// initialize the memer variables
	_comport_handle = INVALID_HANDLE_VALUE;
	return;
}

ComPort::~ComPort()
{
	close();

	return;
}

BOOL ComPort::isAvailable(uint8_t port_num)
{
	wchar_t		 fmt_name[16] = {NULL};
	wsprintf(fmt_name, L"%s%d", L"\\\\.\\COM", port_num);
	
	HANDLE tmp_comport_handle = INVALID_HANDLE_VALUE;
	tmp_comport_handle = CreateFile(fmt_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	// test the handle for port validity
	if (tmp_comport_handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	
	// close the port and delete the handle if present
	DeleteFile(fmt_name);
	CloseHandle(tmp_comport_handle);
	
	return 1;
}

BOOL ComPort::open(uint8_t port_num, uint32_t baudrate)
{
	COMMTIMEOUTS timeouts;
	DCB          dcb;
	
	wchar_t		 fmt_name[16] = {NULL};
	wsprintf(fmt_name, L"%s%d", L"\\\\.\\COM", port_num);
	
	_comport_handle = CreateFile(fmt_name, (GENERIC_READ | GENERIC_WRITE), 0, 0, OPEN_EXISTING, 0, NULL);
	if(_comport_handle == INVALID_HANDLE_VALUE)
	{
	    return 0;
	}
	
	// store the port name for closure
	wmemcpy(_port_name, fmt_name, wcslen(fmt_name)+1);
	// set com port params
	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	
	// change the port identity and parameters
	if(GetCommState(_comport_handle, &dcb))
	{
		dcb.BaudRate = (DWORD)baudrate;
		dcb.Parity   = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;
		SetCommState(_comport_handle, &dcb);
	}
	else
	{
		close();
		return 0;
	}
	
	// set communication timeouts
	timeouts.ReadIntervalTimeout         = 0;
	timeouts.ReadTotalTimeoutMultiplier  = 0;
	timeouts.ReadTotalTimeoutConstant    = 1;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant   = 0;

	SetCommTimeouts(_comport_handle, &timeouts);

	return 1;

}

BOOL ComPort::open(char *friendly_name, uint32_t baudrate)
{
	uint8_t result = friendlyNameToNum(friendly_name);
	if(result > 0)
	{
		if (open(result, baudrate))
		{
			memcpy(_friendly_name, friendly_name, strlen(friendly_name)+1);
			// only looking for name method. If num is used I need a different handler
			ScheduledTask::Resume();
		}
	}

	return (result > 0) ? 1 : 0;
}

void ComPort::close(void)
{
	if(_comport_handle != INVALID_HANDLE_VALUE)
	{
		// stop the associated timed event
		ScheduledTask::Suspend();
		// close port, handle and set to invalid
		DeleteFile(_port_name);
		CloseHandle(_comport_handle);
		_comport_handle = INVALID_HANDLE_VALUE;
		memset(_friendly_name, 0, 256);
	}
	
	return;
}

DWORD ComPort::isReadable(void)
{
	COMSTAT status = {NULL};
	DWORD errors = 0;
	ClearCommError(_comport_handle, &errors, &status);
	
	return status.cbInQue;
}

BOOL ComPort::isOpen(void)
{
	return (_comport_handle != INVALID_HANDLE_VALUE) ? 1 : 0;
}

BOOL ComPort::write(uint8_t data)
{
	// force a write
	DWORD written = 0;
	return WriteFile(_comport_handle, &data, 1, &written, NULL);
}

BOOL ComPort::write(wchar_t *data)
{
	// only write a max amoount of 4096 bytes
	uint32_t size = wcslen(data);
	if (size > 4095)
	{
		return 0;
	}
	// convert from wchar to char
	char small_data[4096] = {NULL};
	uint32_t convert_size = 0;
	wcstombs(small_data, data, size);
	if(size != convert_size)
	{
		return 0;
	}
	
	DWORD written = 0;
	return WriteFile(_comport_handle, small_data, size, &written, NULL);
}

BOOL ComPort::write(char *data)
{
	int size = strlen(data);
	DWORD written = 0;
	return WriteFile(_comport_handle, data, size, &written, NULL);
}

BOOL ComPort::read(uint8_t &c)
{
	DWORD read = 0;
	return ReadFile(_comport_handle, &c, 1, &read, NULL);
}

BOOL ComPort::read(char *data, uint16_t amt)
{
	DWORD read = 0;
	return ReadFile(_comport_handle, data, amt, &read, NULL);
}

BOOL ComPort::read(wchar_t* data, uint16_t amt)
{
	char tmp_data[4096] = {NULL};
	if (0 == read(tmp_data, amt))
	{
		return 0;
	}
	uint16_t size = strlen(tmp_data), convert_size = 0;
	mbstowcs(data, tmp_data, size);

	return (convert_size == size) ? 1 : 0;
}



// example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "comport\ComPort.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ComPort com;
	uint8_t com_num = 0;
	
	if (com.open("mbed Serial Port", 921600))
	{
		while(1)
		{			
			if(com.isReadable())
			{
				//uint8_t c = 0;
				//com.read(c);
				//printf("%c",c);

				//char c[4096] = {NULL};
				//com.read(c);
				//printf("%s", c);

				wchar_t c[4096] = {NULL};
				com.read(c);
				printf("%ls", c);

			}

			if(!com.isOpen())
			{
				com.open("mbed Serial Port", 921600);
			}
			else
			{
				// process timer messages from calling thread - when hot disconnected this halts code
				//	should only be if opened
				MSG lMsg;
				GetMessage(&lMsg, NULL, 0, 0);
				DispatchMessage(&lMsg);
			}
		}
	}

	return 0;
}


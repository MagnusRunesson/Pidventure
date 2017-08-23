//
// main.c
//
#ifndef __main_h
#define __main_h

#include <circle/startup.h>
#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>
#include <circle/fs/fat/fatfs.h>
#include <circle/gpiopin.h>
#include <SDCard/emmc.h>
#include "Engine/Core/Types.h"

class CApp
{
public:
	CApp( void );
	~CApp( void );

	void Init();
	void Exit();
	void Update();

	void Log( char* _pszMessage );
	bool FileLoad( const char* _pszFileName, void** _ppReadData, int* _pReadBytes );

	uint32* m_pScreenBufferParty;
	uint8* m_pScreenBufferPartyUInt8;

	CGPIOPin m_testButton;

private:
	// do not change this order
	CMemorySystem		m_Memory;
	CActLED				m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CScreenDevice		m_Screen;
	CSerialDevice		m_Serial;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer				m_Timer;
	CLogger				m_Logger;

	CEMMCDevice			m_EMMC;
	CFATFileSystem		m_FileSystem;
};

#endif // __main_h

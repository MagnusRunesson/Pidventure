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
#include <circle/usb/dwhcidevice.h>
#include <circle/usb/usbkeyboard.h>
#include <SDCard/emmc.h>
#include "Engine/Core/Types.h"
#include "Engine/Platform/RPi_Circle/audio_rpi_circle.h"

void AppKeyStatusHandlerRaw( unsigned char ucModifiers, const unsigned char RawKeys[ 6 ]);

class CApp
{
public:
	CApp( void );
	~CApp( void );

	void Init();
	void Exit();
	void Update();
	void BlitScreen();

	void Log( const char* _pszMessage );
	void Log( char* _pszMessage );
	bool FileLoad( const char* _pszFileName, void** _ppReadData, int* _pReadBytes );
	void PadHardwareUpdate();
	void KeyStatusHandlerRaw( unsigned char ucModifiers, const unsigned char RawKeys[ 6 ]);

	uint32* m_pScreenBufferParty;
	uint8* m_pScreenBufferPartyUInt8;
	bool m_isLoggingActive;

	CUSBKeyboardDevice* m_pKeyboard;

	static uint32 m_keyboardJoypadEmulationRaise;
	static uint32 m_keyboardJoypadEmulationLower;

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
	CAudioRPiCircle		m_audio;

	// SD Card
	CEMMCDevice			m_EMMC;
	CFATFileSystem		m_FileSystem;

	// Keyboard
	CDWHCIDevice		m_DWHCI;


	CGPIOPin m_GPIO_PadUp;
	CGPIOPin m_GPIO_PadDown;
	CGPIOPin m_GPIO_PadLeft;
	CGPIOPin m_GPIO_PadRight;
	CGPIOPin m_GPIO_PadBtn0;
	CGPIOPin m_GPIO_PadBtn1;
	CGPIOPin m_GPIO_PadBtn2;
	CGPIOPin m_GPIO_PadBtn3;
	CGPIOPin m_GPIO_PadBtn4;
	CGPIOPin m_GPIO_PadBtn5;
	CGPIOPin m_GPIO_PadBtn6;
	CGPIOPin m_GPIO_PadBtn7;
};

#endif // __main_h

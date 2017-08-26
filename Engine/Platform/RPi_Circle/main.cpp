//
// main.c
//
#include <circle/startup.h>
#include <circle/string.h>

#include "Engine/Platform/RPi_Circle/main.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/IO/Joypad.h"

static const char FromKernel[] = "kernel";
#define PARTITION	"emmc1-1"

//#define FILENAME "kernel7.img"

extern void game_setup();
extern void game_loop();
extern void game_exit();
extern void game_debugTrigger( int );

extern uint32 g_JoypadHardwareBuffer;

void circleLog( char* _pszMessage, ... );

CApp* g_pApp;

typedef struct GPIOPinMapEntry
{
	uint32 hardwareMask;
	uint32 virtualMask;
};

const int NUM_GPIO_PINS = 12;
GPIOPinMapEntry g_GPIOPinMap[ NUM_GPIO_PINS ];

const int RPI_GPIO_UP		= 19;
const int RPI_GPIO_DOWN		= 26;
const int RPI_GPIO_LEFT		= 13;
const int RPI_GPIO_RIGHT	= 18; // 6;
const int RPI_GPIO_BTN0		= 20;
const int RPI_GPIO_BTN1		= 16;
const int RPI_GPIO_BTN2		= 12;
const int RPI_GPIO_BTN3		= 22;
const int RPI_GPIO_BTN4		= 23;
const int RPI_GPIO_BTN5		= 24;
const int RPI_GPIO_BTN6		= 27;
const int RPI_GPIO_BTN7		= 21;

CApp::CApp( void )
:	m_Screen( m_Options.GetWidth(), m_Options.GetHeight()),
	m_Timer( &m_Interrupt ),
	m_Logger( m_Options.GetLogLevel (), &m_Timer ),
	m_EMMC( &m_Interrupt, &m_Timer, &m_ActLED ),

	//
	m_DWHCI( &m_Interrupt, &m_Timer ),

	// GPIO pins for joypad
	m_GPIO_PadUp( RPI_GPIO_UP, GPIOModeInputPullUp ),
	m_GPIO_PadDown( RPI_GPIO_DOWN, GPIOModeInputPullUp ),
	m_GPIO_PadLeft( RPI_GPIO_LEFT, GPIOModeInputPullUp ),
	m_GPIO_PadRight( RPI_GPIO_RIGHT, GPIOModeInputPullUp ),
	m_GPIO_PadBtn0( RPI_GPIO_BTN0, GPIOModeInputPullUp ),
	m_GPIO_PadBtn1( RPI_GPIO_BTN1, GPIOModeInputPullUp ),
	m_GPIO_PadBtn2( RPI_GPIO_BTN2, GPIOModeInputPullUp ),
	m_GPIO_PadBtn3( RPI_GPIO_BTN3, GPIOModeInputPullUp ),
	m_GPIO_PadBtn4( RPI_GPIO_BTN4, GPIOModeInputPullUp ),
	m_GPIO_PadBtn5( RPI_GPIO_BTN5, GPIOModeInputPullUp ),
	m_GPIO_PadBtn6( RPI_GPIO_BTN6, GPIOModeInputPullUp ),
	m_GPIO_PadBtn7( RPI_GPIO_BTN7, GPIOModeInputPullUp )
{
	// DPad Up
	g_GPIOPinMap[ 0 ].virtualMask = PAD_KEYMASK_DPAD_UP;
	g_GPIOPinMap[ 0 ].hardwareMask = 0x40000;
}

CApp::~CApp()
{
}

void CApp::Init()
{
	bool bOK = TRUE;

	if( bOK )
	{
		bOK = m_Screen.Initialize();
	}

	if( bOK )
	{
		bOK = m_Serial.Initialize( 115200 );
	}

	if( bOK )
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice( m_Options.GetLogDevice (), FALSE );
		if( pTarget == 0 )
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize( pTarget );
	}

	if( bOK )
	{
		bOK = m_Interrupt.Initialize();
	}

	if( bOK )
	{
		bOK = m_Timer.Initialize();
	}

	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	if (bOK)
	{
		bOK = m_DWHCI.Initialize ();
	}

	// Initialize keyboard
	m_pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);
	if( m_pKeyboard == 0 )
	{
		m_Logger.Write (FromKernel, LogError, "Keyboard not found");
	}

	m_pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);

	//
	// Initialize screen stuff
	//
	screenBuffer = new float[ SCREEN_WIDTH*SCREEN_HEIGHT * 4 ];		// WxH, 4 channels (RGB, Depth)
	m_pScreenBufferParty = (uint32*)m_Screen.m_pFrameBuffer->GetBuffer();
	m_pScreenBufferPartyUInt8 = (uint8*)m_pScreenBufferParty;
	//return bOK;

	CDevice *pPartition = m_DeviceNameService.GetDevice (PARTITION, TRUE);
	if (pPartition == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Partition not found: %s", PARTITION);
	}

	if (!m_FileSystem.Mount (pPartition))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount partition: %s", PARTITION);
	}

	game_setup();


	/*
	// Create file and write to it
	unsigned hFile = m_FileSystem.FileCreate( FILENAME);
	if (hFile == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
	}

	for (unsigned nLine = 1; nLine <= 5; nLine++)
	{
		CString Msg;
		Msg.Format ("Hello File! (Line %u)\n", nLine);

		if (m_FileSystem.FileWrite (hFile, (const char *) Msg, Msg.GetLength ()) != Msg.GetLength ())
		{
			m_Logger.Write (FromKernel, LogError, "Write error");
			break;
		}
	}

	if (!m_FileSystem.FileClose (hFile))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}
	*/
}

void CApp::Exit()
{
	delete[] screenBuffer;
}

void CApp::KeyStatusHandlerRaw( unsigned char ucModifiers, const unsigned char RawKeys[ 6 ])
{
//	assert( s_pThis != 0 );

	CString Message;
	Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);

	for (unsigned i = 0; i < 6; i++)
	{
		if (RawKeys[i] != 0)
		{
			CString KeyCode;
			KeyCode.Format (" %02X", (unsigned) RawKeys[i]);

			Message.Append (KeyCode);
		}
	}

//	s_pThis->m_Logger.Write (FromKernel, LogNotice, Message);
	circleLog( Message );
}

const uint32 DST_SCALING = 6; // While it may look like this can be changed it is in fact hardcoded in a few places that the scaling is 6x
const uint32 DST_PHYSICAL_WIDTH = 800;
const uint32 DST_PHYSICAL_HEIGHT = 480;
const uint32 DST_STRIDE = (DST_PHYSICAL_WIDTH*(DST_SCALING-1))+(DST_PHYSICAL_WIDTH-(SCREEN_WIDTH*DST_SCALING));
const uint32 DST_SCREEN_CENTER_OFFSET =

(
	(
		(
			DST_PHYSICAL_HEIGHT - 
			(
				SCREEN_HEIGHT * DST_SCALING
			)
		) / 2
	) * DST_PHYSICAL_WIDTH
) +
(
	(
		DST_PHYSICAL_WIDTH -
		(
			SCREEN_WIDTH * DST_SCALING
		)
	) / 2
);


void CApp::Update()
{
	//circleLog( "Test button: %i, all buttons: 0x%08x", m_testButton.Read(), CGPIOPin::ReadAll());

	padUpdate();
	game_loop();

	int i;
	int w = m_Screen.m_pFrameBuffer->GetWidth();
	int h = m_Screen.m_pFrameBuffer->GetHeight();

#ifdef PROFILE_SCREEN_BLITTER
	static uint32 numIterations = 0;
	static uint32 timeCount = 0;

	unsigned t0 = CTimer::GetClockTicks();
#endif // PROFILE_SCREEN_BLITTER

	int x, y;
	int px, py;

	float* pSrc = screenBuffer;
	uint32* pDst0 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*0);
	uint32* pDst1 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*1);
	uint32* pDst2 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*2);
	uint32* pDst3 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*3);
	uint32* pDst4 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*4);
	uint32* pDst5 = m_pScreenBufferParty + DST_SCREEN_CENTER_OFFSET + (DST_PHYSICAL_WIDTH*5);
	
	for( y=0; y<SCREEN_HEIGHT; y++ )
	{
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
   			float sr = *pSrc++;
   			float sg = *pSrc++;
   			float sb = *pSrc++;
   			pSrc++; // Alpha channel
   			unsigned char r = (unsigned char)(sr*255.0f);
   			unsigned char g = (unsigned char)(sg*255.0f);
   			unsigned char b = (unsigned char)(sb*255.0f);
   			uint32 c = (r<<16) + (g<<8) + b;

			/*
   			for( py=0; py<4; py++ )
   				for( px=0; px<4; px++ )
   				{
   					int wrofs = ((((y*4)+py) * w) + ((x*4)+px)) * 4;
//	   				int wrofs = ((y*w)+x)*4;
		   			m_pScreenBufferPartyUInt8[ wrofs+0 ] = b;
		   			m_pScreenBufferPartyUInt8[ wrofs+1 ] = g;
		   			m_pScreenBufferPartyUInt8[ wrofs+2 ] = r;
		   			//m_pScreenBufferPartyUInt8[ wrofs+3 ] = a;
   				}
   				*/

   			*pDst0++ = c;
   			*pDst0++ = c;
   			*pDst0++ = c;
   			*pDst0++ = c;
   			*pDst0++ = c;
   			*pDst0++ = c;
   			*pDst1++ = c;
   			*pDst1++ = c;
   			*pDst1++ = c;
   			*pDst1++ = c;
   			*pDst1++ = c;
   			*pDst1++ = c;
   			*pDst2++ = c;
   			*pDst2++ = c;
   			*pDst2++ = c;
   			*pDst2++ = c;
   			*pDst2++ = c;
   			*pDst2++ = c;
   			*pDst3++ = c;
   			*pDst3++ = c;
   			*pDst3++ = c;
   			*pDst3++ = c;
   			*pDst3++ = c;
   			*pDst3++ = c;
   			*pDst4++ = c;
   			*pDst4++ = c;
   			*pDst4++ = c;
   			*pDst4++ = c;
   			*pDst4++ = c;
   			*pDst4++ = c;
   			*pDst5++ = c;
   			*pDst5++ = c;
   			*pDst5++ = c;
   			*pDst5++ = c;
   			*pDst5++ = c;
   			*pDst5++ = c;
		}

		pDst0 += DST_STRIDE;
		pDst1 += DST_STRIDE;
		pDst2 += DST_STRIDE;
		pDst3 += DST_STRIDE;
		pDst4 += DST_STRIDE;
		pDst5 += DST_STRIDE;
	}

#ifdef PROFILE_SCREEN_BLITTER
	unsigned t1 = CTimer::GetClockTicks();

	timeCount += (t1-t0);
	numIterations++;
	if( numIterations == 30 )
	{
		circleLog( "Time spent: %d", timeCount );
		numIterations = 0;
		timeCount = 0;
	}
#endif // PROFILE_SCREEN_BLITTER

	//m_Logger.Write( FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__ );

	// TODO: add your code here

	//return ShutdownHalt;
}

void CApp::Log( char* _pszMessage )
{
	m_Logger.Write( "debugLog", LogNotice, _pszMessage );
}

bool CApp::FileLoad( const char* _pszFileName, void** _ppReadData, int* _pReadBytes )
{
	unsigned hFile = m_FileSystem.FileOpen( _pszFileName );
	if( hFile == 0 )
	{
		m_Logger.Write( FromKernel, LogPanic, "Cannot open file: %s", _pszFileName );
	}

	unsigned fileSize = m_FileSystem.FileSize( hFile );
	char* buffer = new char[ fileSize ];

	unsigned nResult = m_FileSystem.FileRead( hFile, buffer, fileSize );

	if( nResult == FS_ERROR )
	{
		m_Logger.Write( FromKernel, LogError, "Read error" );
		return false;
	}

	if( !m_FileSystem.FileClose( hFile ))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}

	*_ppReadData = (void*)buffer;
	*_pReadBytes = (int)fileSize;

	return true;
}

void CApp::PadHardwareUpdate()
{
	unsigned pins = CGPIOPin::ReadAll();
	if( pins & 0x40000 )
		g_JoypadHardwareBuffer &= ~PAD_KEYMASK_DPAD_RIGHT;
	else
		g_JoypadHardwareBuffer |= PAD_KEYMASK_DPAD_RIGHT;
}


float* screenBuffer;


void circleLog( char* _pszMessage, ... )
{
	CString msg;

	va_list var;
	va_start( var, _pszMessage );

	msg.FormatV( _pszMessage, var );

	va_end( var );

	g_pApp->Log( msg );
}

void circleLog( const char* _pszMessage, ... )
{
	CString msg;

	va_list var;
	va_start( var, _pszMessage );

	msg.FormatV( _pszMessage, var );

	va_end( var );

	g_pApp->Log( msg );
}

bool circleFileLoad(const char* _pszFileName, void** _ppReadData, int* _pReadBytes)
{
	return g_pApp->FileLoad( _pszFileName, _ppReadData, _pReadBytes );
}

void circlePadHardwareInit()
{
}

void circlePadHardwareUpdate()
{
	g_pApp->PadHardwareUpdate();
}

void audioInit( int _frequency )
{
}

int main (void)
{
	// cannot return here because some destructors used in CKernel are not implemented

	CApp app;
	g_pApp = &app;


	app.Init();

	while(true)
	{
		app.Update();
	}

	app.Exit();

	reboot ();
	return EXIT_REBOOT;
}

//
// main.c
//
#include <circle/startup.h>
#include <circle/string.h>
#include <circle/memio.h>
#include <circle/spinlock.h>

#include "Engine/Core/Debug.h"
#include "Engine/Platform/RPi_Circle/main.h"
#include "Engine/Graphics/Screen.h"
#include "Engine/Graphics/Splash.h"
#include "Engine/IO/Joypad.h"
#include "Engine/IO/FileStream.h"
#include "Engine/Util/PgmSwitcher.h"

static const char FromKernel[] = "kernel";
#define PARTITION	"emmc1-1"

//#define FILENAME "kernel7.img"

extern void game_setup();
extern void game_loop();
extern void game_exit();
extern void game_debugTrigger( int );

extern uint32 g_JoypadHardwareBuffer;

void circleLog( char* _pszMessage, ... );
void circleLog( const char* _pszMessage, ... );

uint32 CApp::m_keyboardJoypadEmulationRaise;
uint32 CApp::m_keyboardJoypadEmulationLower;

CApp* g_pApp;

const int RPI_GPIO_DPAD_UP			= 19;
const int RPI_GPIO_DPAD_DOWN		= 26;
const int RPI_GPIO_DPAD_LEFT		= 13;
const int RPI_GPIO_DPAD_RIGHT		= 6;
const int RPI_GPIO_ACTION_UL		= 20;
const int RPI_GPIO_ACTION_UM		= 16;
const int RPI_GPIO_ACTION_UR		= 12;
const int RPI_GPIO_ACTION_LL		= 22;
const int RPI_GPIO_ACTION_LM		= 23;
const int RPI_GPIO_ACTION_LR		= 24;
const int RPI_GPIO_PGM_L			= 27;
const int RPI_GPIO_PGM_R			= 21;
const int RPI_GPIO_MASK_DPAD_UP		= 1 << RPI_GPIO_DPAD_UP;
const int RPI_GPIO_MASK_DPAD_DOWN	= 1 << RPI_GPIO_DPAD_DOWN;
const int RPI_GPIO_MASK_DPAD_LEFT	= 1 << RPI_GPIO_DPAD_LEFT;
const int RPI_GPIO_MASK_DPAD_RIGHT	= 1 << RPI_GPIO_DPAD_RIGHT;
const int RPI_GPIO_MASK_ACTION_UL	= 1 << RPI_GPIO_ACTION_UL;
const int RPI_GPIO_MASK_ACTION_UM	= 1 << RPI_GPIO_ACTION_UM;
const int RPI_GPIO_MASK_ACTION_UR	= 1 << RPI_GPIO_ACTION_UR;
const int RPI_GPIO_MASK_ACTION_LL	= 1 << RPI_GPIO_ACTION_LL;
const int RPI_GPIO_MASK_ACTION_LM	= 1 << RPI_GPIO_ACTION_LM;
const int RPI_GPIO_MASK_ACTION_LR	= 1 << RPI_GPIO_ACTION_LR;
const int RPI_GPIO_MASK_PGM_L		= 1 << RPI_GPIO_PGM_L;
const int RPI_GPIO_MASK_PGM_R		= 1 << RPI_GPIO_PGM_R;

CApp::CApp( void )
:	m_Screen( m_Options.GetWidth(), m_Options.GetHeight()),
	m_Timer( &m_Interrupt ),
	m_Logger( LogDebug, &m_Timer ),
	m_LoggerKernel( LogPanic, &m_Timer ),

	m_audio( &m_Interrupt ),
	m_EMMC( &m_Interrupt, &m_Timer, &m_ActLED ),

	//
	m_DWHCI( &m_Interrupt, &m_Timer ),

	// GPIO pins for joypad
	m_GPIO_PadUp( RPI_GPIO_DPAD_UP, GPIOModeInputPullUp ),
	m_GPIO_PadDown( RPI_GPIO_DPAD_DOWN, GPIOModeInputPullUp ),
	m_GPIO_PadLeft( RPI_GPIO_DPAD_LEFT, GPIOModeInputPullUp ),
	m_GPIO_PadRight( RPI_GPIO_DPAD_RIGHT, GPIOModeInputPullUp ),
	m_GPIO_PadBtn0( RPI_GPIO_ACTION_UL, GPIOModeInputPullUp ),
	m_GPIO_PadBtn1( RPI_GPIO_ACTION_UM, GPIOModeInputPullUp ),
	m_GPIO_PadBtn2( RPI_GPIO_ACTION_UR, GPIOModeInputPullUp ),
	m_GPIO_PadBtn3( RPI_GPIO_ACTION_LL, GPIOModeInputPullUp ),
	m_GPIO_PadBtn4( RPI_GPIO_ACTION_LM, GPIOModeInputPullUp ),
	m_GPIO_PadBtn5( RPI_GPIO_ACTION_LR, GPIOModeInputPullUp ),
	m_GPIO_PadBtn6( RPI_GPIO_PGM_L, GPIOModeInputPullUp ),
	m_GPIO_PadBtn7( RPI_GPIO_PGM_R, GPIOModeInputPullUp )
{
}

//CSpinLock m_waitVBLSpinLock;

volatile uint32 currentFrameCount;
volatile uint32 previousFrameCount;

void IRQParty( void* _pData )
{
	//m_waitVBLSpinLock.Acquire();
	currentFrameCount++;
	//m_waitVBLSpinLock.Release();
	
	write32( 0x3f600000, 0 );
}

void waitVBL()
{
	/*
	bool sameFrame = true;
	uint32 newFrameCount;
	
	while( previousFrameCount <= currentFrameCount ) {
		m_waitVBLSpinLock.Acquire();
		newFrameCount = currentFrameCount;
		m_waitVBLSpinLock.Release();
		sameFrame = previousFrameCount == newFrameCount;
	}
	
	previousFrameCount = currentFrameCount;
	return currentFrameCount;
	*/

	while( previousFrameCount >= currentFrameCount );
	previousFrameCount = currentFrameCount;
}

CApp::~CApp()
{
}

uint8 streambuffertest[ 4096 ];

void CApp::Init()
{
	currentFrameCount = 0;
	previousFrameCount = 0;
	
	m_isLoggingActive = false;
	bool bOK = TRUE;

	if( bOK )
	{
		bOK = m_Screen.Initialize();
	}

	//
	// Initialize screen stuff
	//
	screenBuffer = new float[ SCREEN_WIDTH*SCREEN_HEIGHT * 4 ];		// WxH, 4 channels (RGB, Depth)
	m_pScreenBufferParty = (uint32*)m_Screen.m_pFrameBuffer->GetBuffer();
	m_pScreenBufferPartyUInt8 = (uint8*)m_pScreenBufferParty;

	// Start by showing the splash screen, yay!
	splashRender();
	BlitScreen();

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

	m_Interrupt.ConnectIRQ( ARM_IRQ_SMI, IRQParty, (void*)0 );
	
	if( bOK )
	{
		bOK = m_Timer.Initialize();
	}

	m_audio.Start();

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

	m_pKeyboard->RegisterKeyStatusHandlerRaw( AppKeyStatusHandlerRaw );

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

	pgmSwitcherReset();
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
	
	fileStreamInit();

	/*
	debugLog("Refular file loading!");
	unsigned hFile = m_FileSystem.FileOpen( "stream.txt" );
	//debugLog("hFile: %i", hFile );
	
	unsigned rf0 = CTimer::GetClockTicks();
	unsigned nResult = m_FileSystem.FileRead( hFile, streambuffertest, 512 );
	unsigned rf1 = CTimer::GetClockTicks();

	//debugLog("Result: %i", nResult);
	m_FileSystem.FileClose( hFile );
	streambuffertest[ 10 ] = 0;
	debugLog( "Loaded regularly: %s", streambuffertest );

	int i;
	for( i=0; i<512; i++ )
		streambuffertest[ i ] = i;
	
	debugLog("-------->FileStreamOpen");
	int sh = fileStreamOpen( "stream.txt" );
	//debugLog("-------->FileStreamReadNextChunk");
	
	unsigned st0 = CTimer::GetClockTicks();
	fileStreamReadNextChunk( sh, streambuffertest );
	unsigned st1 = CTimer::GetClockTicks();

	streambuffertest[ 10 ] = 0;
	debugLog( "Streamed: %s", streambuffertest );
	
	debugLog("Read via FAT: %i, read via stream: %i", (rf1-rf0), (st1-st0));
	 */
}

void CApp::Exit()
{
	delete[] screenBuffer;
}

void AppKeyStatusHandlerRaw( unsigned char ucModifiers, const unsigned char RawKeys[ 6 ])
{
	g_pApp->KeyStatusHandlerRaw( ucModifiers, RawKeys );
}

void CApp::KeyStatusHandlerRaw( unsigned char ucModifiers, const unsigned char RawKeys[ 6 ])
{
//	assert( s_pThis != 0 );

	m_keyboardJoypadEmulationLower = m_keyboardJoypadEmulationRaise;	// Assume all have been released
	m_keyboardJoypadEmulationRaise = 0;

	CString Message;
	Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);

	for (unsigned i = 0; i < 6; i++)
	{
		uint8 rawKey = RawKeys[ i ];
		if( rawKey != 0 )
		{
			CString KeyCode;
			KeyCode.Format( " %02X", (unsigned)rawKey );
			Message.Append( KeyCode );

			switch( rawKey )
			{
				case 0x52:	// Up
					m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_DPAD_UP;
					break;

				case 0x51:	// Down
					m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_DPAD_DOWN;
					break;

				case 0x50:	// Left
					m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_DPAD_LEFT;
					break;

				case 0x4f:	// Right
					m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_DPAD_RIGHT;
					break;

				// Action buttons
				case 0x04:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_UL;	break;
				case 0x16:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_UM;	break;
				case 0x07:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_UR;	break;
				case 0x1d:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_LL;	break;
				case 0x1b:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_LM;	break;
				case 0x06:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_ACTION_LR;	break;

				// Program buttons
				case 0x2c:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_PGM_L;		break;
				case 0x28:	m_keyboardJoypadEmulationRaise |= PAD_KEYMASK_PGM_R;		break;

				// Debug triggers
				case 0x1e: game_debugTrigger( 1 ); break;
				case 0x1f: game_debugTrigger( 2 ); break;
				case 0x20: game_debugTrigger( 3 ); break;
				case 0x21: game_debugTrigger( 4 ); break;
				case 0x22: game_debugTrigger( 5 ); break;
				case 0x23: game_debugTrigger( 6 ); break;
				case 0x24: game_debugTrigger( 7 ); break;
				case 0x25: game_debugTrigger( 8 ); break;
				case 0x26: game_debugTrigger( 9 ); m_isLoggingActive = !m_isLoggingActive; break;
				case 0x27: game_debugTrigger( 0 ); break;
			}
		}
	}

//	s_pThis->m_Logger.Write (FromKernel, LogNotice, Message);
	//circleLog( (const char*)Message );
}

const uint32 DST_SCALING = 6; // While it may look like this can be changed it is in fact hardcoded in a few places that the scaling is 6x

const uint32 DST_PHYSICAL_WIDTH_x800 = 656;
const uint32 DST_PHYSICAL_HEIGHT_x800 = 512;
const uint32 DST_STRIDE_x800 = (DST_PHYSICAL_WIDTH_x800*(DST_SCALING-1))+(DST_PHYSICAL_WIDTH_x800-(SCREEN_WIDTH*DST_SCALING));
const uint32 DST_SCREEN_CENTER_OFFSET_x800 =

(
	(
		(
			DST_PHYSICAL_HEIGHT_x800 - 
			(
				SCREEN_HEIGHT * DST_SCALING
			)
		) / 2
	) * DST_PHYSICAL_WIDTH_x800
) +
(
	(
		DST_PHYSICAL_WIDTH_x800 -
		(
			SCREEN_WIDTH * DST_SCALING
		)
	) / 2
);

const uint32 DST_PHYSICAL_WIDTH_x640 = 640;
const uint32 DST_PHYSICAL_HEIGHT_x640 = 480;
const uint32 DST_STRIDE_x640 = (DST_PHYSICAL_WIDTH_x640*(DST_SCALING-1))+(DST_PHYSICAL_WIDTH_x640-(SCREEN_WIDTH*DST_SCALING));
const uint32 DST_SCREEN_CENTER_OFFSET_x640 =
(
	(
		(
			DST_PHYSICAL_HEIGHT_x640 - 
			(
				SCREEN_HEIGHT * DST_SCALING
			)
		) / 2
	) * DST_PHYSICAL_WIDTH_x640
) +
(
	(
		DST_PHYSICAL_WIDTH_x640 -
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

	waitVBL();
	BlitScreen();
}

void CApp::BlitScreen()
{
	int w = m_Screen.m_pFrameBuffer->GetWidth();
	int h = m_Screen.m_pFrameBuffer->GetHeight();

#ifdef PROFILE_SCREEN_BLITTER
	static uint32 numIterations = 0;
	static uint32 timeCount = 0;

	unsigned t0 = CTimer::GetClockTicks();
#endif // PROFILE_SCREEN_BLITTER

	int x, y;

	int o = (
		(
			(
				h - 
				(
					SCREEN_HEIGHT * DST_SCALING
				)
			) / 2
		) * w
	) +
	(
		(
			w -
			(
				SCREEN_WIDTH * DST_SCALING
			)
		) / 2
	);


	float* pSrc = screenBuffer;
	uint32* pDst0 = m_pScreenBufferParty + o + (w*0);
	uint32* pDst1 = m_pScreenBufferParty + o + (w*1);
	uint32* pDst2 = m_pScreenBufferParty + o + (w*2);
	uint32* pDst3 = m_pScreenBufferParty + o + (w*3);
	uint32* pDst4 = m_pScreenBufferParty + o + (w*4);
	uint32* pDst5 = m_pScreenBufferParty + o + (w*5);
	uint32 stride = (w * (DST_SCALING - 1)) + (w - (SCREEN_WIDTH * DST_SCALING));
	
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

		pDst0 += stride;
		pDst1 += stride;
		pDst2 += stride;
		pDst3 += stride;
		pDst4 += stride;
		pDst5 += stride;
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
}

void CApp::Log( const char* _pszMessage )
{
	if( m_isLoggingActive == false )
		return;

	m_Logger.Write( "debugLog", LogNotice, _pszMessage );
}

void CApp::Log( char* _pszMessage )
{
	if( m_isLoggingActive == false )
		return;
	
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
		m_Logger.Write( FromKernel, LogError, "Read error of file '%s'", _pszFileName );
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

void CApp::FileSave( const char* _pszFileName, void* _pContent, int _contentSize )
{
	unsigned hFile = m_FileSystem.FileCreate( _pszFileName );
	if (hFile == 0)
	{
		m_Logger.Write( FromKernel, LogPanic, "Cannot create file: %s", _pszFileName );
	}

	if( m_FileSystem.FileWrite( hFile, _pContent, _contentSize ) != _contentSize )
	{
		m_Logger.Write( FromKernel, LogError, "Write error" );
	}
	
	if (!m_FileSystem.FileClose (hFile))
	{
		m_Logger.Write( FromKernel, LogPanic, "Cannot close file" );
	}
}

#define CHECK_BUTTON( _buttonName ) \
	if( !(pins & RPI_GPIO_MASK_##_buttonName) ) \
	{ \
		/*circleLog(#_buttonName); */ \
		g_JoypadHardwareBuffer |= PAD_KEYMASK_##_buttonName; \
	} \

void CApp::PadHardwareUpdate()
{
	g_JoypadHardwareBuffer = 0;

	unsigned pins = CGPIOPin::ReadAll();
	CHECK_BUTTON( DPAD_RIGHT );
	CHECK_BUTTON( DPAD_LEFT );
	CHECK_BUTTON( DPAD_UP );
	CHECK_BUTTON( DPAD_DOWN );
	CHECK_BUTTON( ACTION_UL );
	CHECK_BUTTON( ACTION_UM );
	CHECK_BUTTON( ACTION_UR );
	CHECK_BUTTON( ACTION_LL );
	CHECK_BUTTON( ACTION_LM );
	CHECK_BUTTON( ACTION_LR );
	CHECK_BUTTON( PGM_L );
	CHECK_BUTTON( PGM_R );

	g_JoypadHardwareBuffer |= m_keyboardJoypadEmulationRaise;
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

void circleFileSave(const char* _pszFileName, void* _pContent, int _contentSize )
{
	g_pApp->FileSave( _pszFileName, _pContent, _contentSize );
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

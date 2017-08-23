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

extern void game_setup();
extern void game_loop();
extern void game_exit();
extern void game_debugTrigger( int );

void circleLog( char* _pszMessage, ... );

CApp* g_pApp;

CApp::CApp( void )
:	m_Screen( m_Options.GetWidth(), m_Options.GetHeight()),
	m_Timer( &m_Interrupt ),
	m_Logger( m_Options.GetLogLevel (), &m_Timer ),
	m_EMMC( &m_Interrupt, &m_Timer, &m_ActLED ),
	m_testButton( 18, GPIOModeInputPullUp )
{
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
}

void CApp::Exit()
{
	delete[] screenBuffer;
}

void CApp::Update()
{
	//circleLog( "Test button: %i, all buttons: 0x%08x", m_testButton.Read(), CGPIOPin::ReadAll());

	padUpdate();
	game_loop();

	int i;
	int w = m_Screen.m_pFrameBuffer->GetWidth();
	int h = m_Screen.m_pFrameBuffer->GetHeight();

	static uint32 numIterations = 0;
	static uint32 timeCount = 0;

	unsigned t0 = CTimer::GetClockTicks();

	int x, y;
	int px, py;
	for( y=0; y<SCREEN_HEIGHT; y++ )
	{
		for( x=0; x<SCREEN_WIDTH; x++ )
		{
   			int rdofs = ((y*SCREEN_WIDTH)+x)*4;
   			float sr = screenBuffer[ rdofs+0 ];
   			float sg = screenBuffer[ rdofs+1 ];
   			float sb = screenBuffer[ rdofs+2 ];
   			unsigned char r = (unsigned char)(sr*255.0f);
   			unsigned char g = (unsigned char)(sg*255.0f);
   			unsigned char b = (unsigned char)(sb*255.0f);
   			unsigned char a = 255;

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

			int wrofs = ((y*w)+x)*4;
			m_pScreenBufferPartyUInt8[ wrofs+0 ] = b;
			m_pScreenBufferPartyUInt8[ wrofs+1 ] = g;
			m_pScreenBufferPartyUInt8[ wrofs+2 ] = r;
		}
	}

	unsigned t1 = CTimer::GetClockTicks();

	timeCount += (t1-t0);
	numIterations++;
	if( numIterations == 30 )
	{
		circleLog( "Time spent: %d", timeCount );
		numIterations = 0;
		timeCount = 0;
	}

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

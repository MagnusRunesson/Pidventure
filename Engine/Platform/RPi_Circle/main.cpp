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

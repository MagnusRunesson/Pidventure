//
// main.c
//
#include <circle/startup.h>
#include "Engine/Platform/RPi_Circle/main.h"

static const char FromKernel[] = "kernel";

CApp::CApp( void )
:	m_Screen( m_Options.GetWidth(), m_Options.GetHeight()),
	m_Timer( &m_Interrupt ),
	m_Logger( m_Options.GetLogLevel (), &m_Timer )
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

	//return bOK;
}

void CApp::Exit()
{

}

void CApp::Update()
{
	//m_Logger.Write( FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__ );

	// TODO: add your code here

	//return ShutdownHalt;
}



float* screenBuffer;


void audioInit( int _frequency )
{
}

int main (void)
{
	// cannot return here because some destructors used in CKernel are not implemented

	CApp app;
	app.Init();

	while(true)
	{
		app.Update();
	}

	app.Exit();

	reboot ();
	return EXIT_REBOOT;
}

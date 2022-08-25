#include "application.h"
#define WINDOW_BAR_SIZE 16 // 16 pixel for the windows bar

namespace raster3d
{ 

// Callback
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ){
  	
	HDC          hdc{ nullptr };
    PAINTSTRUCT  ps;
	application* app{ nullptr };

	switch(message)
   {   
	case WM_CREATE:
		SetLastError(0);
        if ( !SetWindowLongPtr(hWnd, GWLP_USERDATA,(LONG_PTR) ((CREATESTRUCT*) lParam)->lpCreateParams) && GetLastError() ) 
		{
			MessageBox(NULL,"SetApplicationLongPtr() failed.\n" ,"ERROR",MB_OK|MB_ICONEXCLAMATION);
            ExitProcess(1);
        }
	 case WM_TIMER:
           InvalidateRect( hWnd, NULL, false );
           /////// START OPTIONAL SECTION
           UpdateWindow( hWnd );
           ValidateRect( hWnd, NULL );
           /////// END OPTIONAL SECTION
      break;
	  case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			app = (application*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (app && app->buffers())
			{
				app->buffers()->swap(hdc);
			}
			EndPaint( hWnd, &ps );
			break;
      case WM_CLOSE:
         DestroyWindow(hWnd);
      break;
      case WM_DESTROY:
         PostQuitMessage(0);
      break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Costructor
application::application(application_builder app_builder, const ivec2& size, uint frame_rate_cap)
{
	// Get win applicatiom module instance
	m_hInstance = GetModuleHandle (0);
	
	// Info class application
	WNDCLASSEX  WndCls;
    static char szAppName[] = "MainClass";

	//set info header
    WndCls.cbClsExtra    = 0;
    WndCls.cbWndExtra    = 0;
    WndCls.cbSize        = sizeof(WndCls);
    WndCls.hbrBackground = (HBRUSH)CreateSolidBrush(0);
    WndCls.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WndCls.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	WndCls.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );
    WndCls.hInstance     = m_hInstance;
    WndCls.lpfnWndProc   = WndProc;
    WndCls.lpszClassName = szAppName;
    WndCls.lpszMenuName  = NULL;
    WndCls.style         = 0;
	//regist class application
    RegisterClassEx(&WndCls);

	// Window adjust
	RECT rect;
	rect.top =    rect.left = 0;
	rect.bottom = rect.top + size.y - WINDOW_BAR_SIZE;
	rect.right =  rect.left + size.x;
	DWORD dwstyle = WS_MINIMIZEBOX | WS_SYSMENU | WS_POPUP | WS_CAPTION;
	AdjustWindowRect( &rect, dwstyle, true);

	//create window
	m_window=CreateWindowEx
	(    
		  WS_EX_CLIENTEDGE
		, szAppName
		, "Preview Application"
		, dwstyle
		, CW_USEDEFAULT // x
		, CW_USEDEFAULT // y
		, rect.right  - rect.left // width
		, rect.bottom - rect.top  // hight
		, NULL
		, NULL 
		, m_hInstance
		, (void*)this
	);

	// Create buffers context
	m_buffers = std::make_unique< buffers_context >(m_window);

	// Show Application
	ShowWindow(m_window,SW_SHOW);

	// Set focus on this new window
	SetForegroundWindow(m_window);
	SetFocus(m_window);	

	// Update the windows first time (local variables init)
	UpdateWindow(m_window);

	// Alloc application
	m_app = app_builder(*m_buffers, size, frame_rate_cap);

	// Start app
	m_app->start();
}

// Buffers
buffers_context* application::buffers()
{
	return m_buffers.get();
}


// Main loop
DWORD application::thread_main()
{
	// Start timer
	const  double ms_to_sleep = 1000.0 / static_cast<double>(m_app->frame_rate());
	auto   dttimer = timer_ms();
	double millipass = 0;
	double dt = 0;
	double sleep_time = 0;
	HDC    windows_dc = GetDC(m_window);
	//draw loop
	while (m_loop)
	{
		// Get timer pass
		millipass = dttimer;
		// Calc dt and sleep time
		sleep_time = ms_to_sleep - millipass;
		// Test time to sleep
		while (sleep_time > 0 && sleep_time < 1000)
		{
			Sleep(sleep_time > 10 ? 1 : 0);
			millipass = dttimer;
			sleep_time = ms_to_sleep - millipass;
		}
		// Compute dt
		dt = millipass;
		// Reset
		dttimer = timer_ms();
		// Update
		m_app->update(dt);
		m_buffers->swap(windows_dc);
	}

	return 0;
}

// Call main function
DWORD WINAPI application::thread_main_stub(LPVOID  app)
{
	// Call the real function
	return ((application*)app)->thread_main();
}

void application::exec()
{
	// Thread loop flag
	m_loop = true;
	// Window messages
    MSG message;	
    // Create a new thread to use as a timer
	HANDLE hTickThread = CreateThread( NULL, NULL, &thread_main_stub, this, NULL, NULL );
	// Update messages
	while( GetMessage(&message, NULL, 0, 0) )
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
	// Terminate app
	m_app->end();
	// Stop
	m_loop = false;
	// Wait
	WaitForSingleObject(hTickThread, INFINITE);
	// Bye thread
	TerminateThread( hTickThread, 0 );
}

}
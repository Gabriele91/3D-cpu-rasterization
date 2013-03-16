#include "Application.h"
//using names space
using namespace TinyRasterization;

//callback
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ){
  	
	HDC         hdc;
    PAINTSTRUCT ps;
	HPEN	    hpen;
	Application*    Applicationptr=NULL;
	BuffersContext* wbuffer=NULL;

	switch(message)
   {   
	case WM_CREATE:
		SetLastError(0);
        if ( !SetWindowLongPtr(hWnd, GWLP_USERDATA,(LONG) ((CREATESTRUCT*) lParam)->lpCreateParams) && GetLastError() ) {
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
			Applicationptr = (Application*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(Applicationptr)
				Applicationptr->getContext()->swap( hdc );
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
//costructor
Application::Application(ApplicationIstance* app):app(app),width(app->width),hight(app->hight){
	//get istance
	hInstance= GetModuleHandle (0);
	//info class application
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
    WndCls.hInstance     = hInstance;
    WndCls.lpfnWndProc   = WndProc;
    WndCls.lpszClassName = szAppName;
    WndCls.lpszMenuName  = NULL;
    WndCls.style         = 0;
	//regist class application
    RegisterClassEx(&WndCls);

	//create Application
	//window adjust
	RECT rect;
	rect.top =    rect.left = 0;
	rect.bottom = rect.top + hight + 7;
	rect.right =  rect.left + width;
	DWORD dwstyle = WS_MINIMIZEBOX | WS_SYSMENU | WS_POPUP | WS_CAPTION;
	AdjustWindowRect( &rect, dwstyle, true);
	//create window
	window=CreateWindowEx(  WS_EX_CLIENTEDGE,
							szAppName, 
							"Preview Application",
							dwstyle,
							CW_USEDEFAULT, //x
							CW_USEDEFAULT, //y
							rect.right - rect.left, //width
							rect.bottom - rect.top, //hight
							NULL, NULL, 
							hInstance,
							(void*)this);
	//create buffer
	context=new BuffersContext(window);
	//show Application
	ShowWindow(window,SW_SHOW);// Show The Application //mOstra finestra
	//SetForegroundWindow(window);// Slightly Higher Priority //priorità
	//SetFocus(window);	
	UpdateWindow( window );
	//start app
	app->start();
}
//return buffers
BuffersContext* Application::getContext(){
	return context;
}
//flip screen
DWORD WINAPI Application::threadUpdate(LPVOID  app){
	Application *self=(Application*)app;
	//start timer
	self->timer.start();	
	double msToSleep=1000.0/static_cast<double>(self->app->frameRate);
	double millipass=0;
	double dt=0;
	double sleepTime=0;
	//draw loop
	for(;;)
    {
		//get timer values
		millipass=static_cast<double>(self->timer.getGetCounter());
		//calc dt and sleep time
		sleepTime=msToSleep-(millipass/1000.0);
		while(sleepTime>0 && sleepTime<60000 ){
			Sleep(sleepTime>10?1:0);
			millipass=static_cast<double>(self->timer.getGetCounter());
			sleepTime=msToSleep-(millipass/1000.0);
		}
		//calc dt
		dt=millipass/1000.0;
		self->timer.reset();
		//update
		self->app->update(self->context,dt);	
		self->context->swap( GetDC(self->window) );
    }
}
void Application::exec(){
    MSG message;	
    // Create a new thread to use as a timer
	HANDLE hTickThread = CreateThread( NULL, NULL, &threadUpdate, this, NULL, NULL );
	while( GetMessage(&message, NULL, 0, 0) )
    {
        TranslateMessage(&message);
        DispatchMessage( &message);
    }
	//terminate app
	app->end();
	TerminateThread( hTickThread, 0 );
}
#ifndef WINDOW_H
#define WINDOW_H

#include <Config.h>
#include <BuffersContext.h>
#include <ApplicationIstance.h>
#include <Timer.h>

namespace TinyRasterization {

	class Application {
		
		uint width,hight;
		BuffersContext* context;
		ApplicationIstance *app;
		Timer timer;
		//pwin32 values
		HINSTANCE hInstance;
		HWND window;
		static DWORD WINAPI threadUpdate(LPVOID  app);

	public:
		//constructor
		Application(ApplicationIstance *app);
		//return buffers
		BuffersContext* getContext();
		//exec window
		void exec();
		void draw();

	};

};

#endif
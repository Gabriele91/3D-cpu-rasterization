#ifndef APPLICATION_H
#define APPLICATION_H

#include <Config.h>
#include <BuffersContext.h>
#include <ApplicationInstance.h>

namespace TinyRasterization {

	class Application {
		
		uint width,hight;
		BuffersContext* context;
		ApplicationInstance *app;
		//pwin32 values
		HINSTANCE hInstance;
		HWND Application;

	public:
		//constructor
		Application(ApplicationInstance *app,uint width,uint hight);
		//return buffers
		BuffersContext* getContext();
		//exec Application
		void exec();

	};

};

#endif
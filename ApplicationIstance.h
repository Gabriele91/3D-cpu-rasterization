#ifndef APPLICATIONISTANCE_H
#define APPLICATIONISTANCE_H 

#include <Config.h>
#include <Types.h>

namespace TinyRasterization {
	//class declaration
	class Application;
	class BuffersContext;
	//abstract class
	class ApplicationIstance {

		friend class Application;
		uint width,hight;
		uint frameRate;

	public:
		//constructor
		ApplicationIstance(uint width,uint hight,uint frameRate)
						  :width(width),hight(hight),frameRate(frameRate){}
		//
		DFORCEINLINE uint getWidth(){
			return width;
		}
		DFORCEINLINE uint getHight(){
			return hight;
		}
		//virtual methos
		virtual void start()=0;
		virtual void update(BuffersContext *app,float dt)=0;
		virtual void end()=0;

	};

};

#endif
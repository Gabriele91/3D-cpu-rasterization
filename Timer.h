#ifndef TIMER_H
#define TIMER_H

#include <Config.h>
#include <Types.h>
#include <TimeTicks.h>

namespace TinyRasterization {

	class Timer {

		bool started;
		bit64 sTime;

	public:

		Timer():started(false),sTime(0){}
		void start();
		bit64 getGetCounter();
		void reset();
		void end();

	};

};

#endif
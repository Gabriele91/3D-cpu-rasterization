#include "Timer.h"

using namespace TinyRasterization;

void Timer::start(){
	started=true;
	sTime=GetTimeTicks();
}
bit64 Timer::getGetCounter(){
	return started ? GetTimeTicks()-sTime : 0;
}
void Timer::reset(){
	sTime=GetTimeTicks();
}
void Timer::end(){
	started=false;
	sTime=0;
}
#include "TimeTicks.h"
//get time
extern TinyRasterization::bit64 TinyRasterization::GetTimeTicks(){
	bit64 val;
	#if defined(_MSC_VER)
		QueryPerformanceCounter( (LARGE_INTEGER *)&val );
	#else
		timeval timeVal;

		gettimeofday( &timeVal, NULL );

		val = (I64)timeVal.tv_sec * (1000*1000) + (I64)timeVal.tv_usec;
	#endif
	return val;
}
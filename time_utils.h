#pragma once
#include "config.h"
#include "types.h"

namespace raster3d 
{
	long long  get_ticks();
	double     get_time();
	double     get_time_ms();

	class timer
	{
	protected:

		double m_last;

	public:

		timer()				     : m_last(get_time()) {}
		timer(const timer & t)   : m_last(t.m_last)   {}
		operator double () const { return get_time() - m_last; }
	};

	class timer_ms
	{
	protected:

		double m_last;

	public:

		timer_ms()                  : m_last(get_time_ms()) {}
		timer_ms(const timer_ms& t) : m_last(t.m_last) {}
		operator double () const    { return get_time_ms() - m_last; }
	};
};

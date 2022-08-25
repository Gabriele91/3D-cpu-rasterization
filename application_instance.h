#pragma once
#include "config.h"
#include "types.h"
#include "math_utils.h"
#include "buffers_context.h"

namespace raster3d 
{
	// Forward declaration
	class application;

	// Instance 
	class application_instance
	{

	public:
		// Constructor
		application_instance(buffers_context& buffers, const ivec2& size, uint frame_rate_cap) 
		: m_buffers(buffers)
		, m_size(size)
		, m_frame_rate(frame_rate_cap)
		{}
		
		// Query
		buffers_context& buffers()   const { return m_buffers;    }
		const ivec2& size()          const { return m_size;       }
		const uint   frame_rate()    const { return m_frame_rate; }

		// Methos
		virtual void start()=0;
		virtual void update(float dt)=0;
		virtual void end()=0;

	private:
		
		buffers_context& m_buffers;
		const ivec2 m_size;
		const uint  m_frame_rate;
	};

};

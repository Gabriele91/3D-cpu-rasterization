#pragma once
#include "config.h"
#include "application_instance.h"
#include "buffers_context.h"
#include "time_utils.h"
#include <functional>

namespace raster3d 
{
	// Alias
	using application_builder = std::function< std::unique_ptr<application_instance>(buffers_context&, const ivec2&, uint) >;
	// Helper
	template < typename T >
	application_builder create_application_builder()
	{
		return [](buffers_context& buffers, const ivec2& size, uint frame_rate_cap) -> std::unique_ptr<application_instance>
		{
			return std::move
			(std::unique_ptr<application_instance>
			{
				static_cast<application_instance*>(new T(buffers, size, frame_rate_cap))
			});
		};
	}
	

	class application 
	{
	public:
		// Constructor
		application(application_builder, const ivec2& size, uint frame_rate_cap);
		
		// Buffers
		buffers_context* buffers();
		
		// Exec application
		void exec();

		// Draw
		void draw();

	protected:
		// Main loop
		DWORD thread_main();

		// App context
		std::unique_ptr<buffers_context>      m_buffers;
		std::unique_ptr<application_instance> m_app;
		std::atomic_bool				      m_loop;

		// Win32 stuffs
		HINSTANCE						 m_hInstance;
		HWND							 m_window;

		// Main loop stub
		static DWORD WINAPI thread_main_stub(LPVOID app);
	};

};
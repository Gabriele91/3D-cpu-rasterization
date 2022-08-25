#pragma once
#include "config.h"
#include "color.h"

namespace raster3d 
{
	class buffers_context 
	{

		HWND					 m_window      { nullptr };
		HDC						 m_hDC         { nullptr };
		HBITMAP					 m_hBitmap     { nullptr };
		BYTE*					 m_color_buffer{ nullptr };
		std::unique_ptr<float[]> m_zbuffer     { nullptr };
		uint					 m_width	   { 0 };
		uint					 m_hight       { 0 };


	public:

		// Contructor
		buffers_context(HWND  window);

		// Destructor
		virtual ~buffers_context();

		// Color buffer
		void  pixel(const color& color,uint x,uint y);
		color pixel(uint x,uint y) const;
		void  draw_zbuffer();
		void  clear_color();
		void  clear_color(const color& color);

		// ZBuffer
		void  depth(float zDf,uint x,uint y);
		float depth(uint x,uint y);
		void  clear_depth(float v = 0.0);
		
		// Swap buffers
		void swap(HDC hPaintDC);
		void swap();
		
	};

};
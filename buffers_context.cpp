#include "buffers_context.h"

namespace raster3d
{
buffers_context::buffers_context(HWND  window)
:m_window(window)
{
		// Get windows size
		RECT Rect;				  
		GetClientRect(window, &Rect);
		// Compute width and hight
		m_width = Rect.right - Rect.left;
		m_hight = Rect.bottom - Rect.top ;
		// Create a BITMAPINFO with minimal initilisation 
        // for the CreateDIBSection
        BITMAPINFO RGB32BitsBITMAPINFO; 
        ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
        RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		RGB32BitsBITMAPINFO.bmiHeader.biWidth = m_width;
		RGB32BitsBITMAPINFO.bmiHeader.biHeight = -m_hight;
		RGB32BitsBITMAPINFO.bmiHeader.biPlanes = 1;
		RGB32BitsBITMAPINFO.bmiHeader.biBitCount = 32;
		RGB32BitsBITMAPINFO.bmiHeader.biSizeImage = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biXPelsPerMeter = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biYPelsPerMeter = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biClrUsed = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biClrImportant = 0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbBlue = 0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbGreen = 0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbRed = 0;
		// Create context
		m_hDC = CreateCompatibleDC( GetDC(window) );
		// Pointer to pixels (will automatically have space allocated by CreateDIBSection
		// Create DIB section to always give direct access to pixels
		m_hBitmap = CreateDIBSection(  m_hDC
									 , (BITMAPINFO *)&RGB32BitsBITMAPINFO
									 , DIB_RGB_COLORS
									 , (void**)&m_color_buffer
									 , NULL
			                         , 0 );
		// Assign the bitmap section to the current dc
		SelectObject(m_hDC, m_hBitmap);
		//zbuffer
		m_zbuffer = std::make_unique<float[]>(m_width*m_hight);
}

// Destructor
buffers_context::~buffers_context()
{
	ReleaseDC(m_window, m_hDC); // Release our DC handle
	DeleteObject(m_hBitmap);    // delete bitmap object
	DeleteDC(m_hDC);			// delete DC
}

// Color buffer
void buffers_context::pixel(const color& color,uint x,uint y)
{
	if(x < m_width && y < m_hight)
	{
		//RGBA -> BRGA
		m_color_buffer[(x + y * m_width) * 4    ] = color.b;
		m_color_buffer[(x + y * m_width) * 4 + 1] = color.g;
		m_color_buffer[(x + y * m_width) * 4 + 2] = color.r;
		m_color_buffer[(x + y * m_width) * 4 + 3] = color.a;
	}
}

color buffers_context::pixel(uint x,uint y) const
{
	if (x < m_width && y < m_hight)
	{
		//BRGA -> RGBA
		return
		{
			m_color_buffer[(x + y * m_width) * 4 + 1],
			m_color_buffer[(x + y * m_width) * 4 + 2],
			m_color_buffer[(x + y * m_width) * 4 + 0],
			m_color_buffer[(x + y * m_width) * 4 + 3],
		};
	}
	return {};
}		
void buffers_context::clear_color()
{
	memset(m_color_buffer, 0, 4 * m_width * m_hight);
}

void buffers_context::clear_color(const color& color)
{
	for (size_t offset = 0; offset < m_width * m_hight; ++offset)
	{
		//RGBA -> BRGA	
		m_color_buffer[offset * 4]	   = color.b;
		m_color_buffer[offset * 4 + 1] = color.g;
		m_color_buffer[offset * 4 + 2] = color.r;
		m_color_buffer[offset * 4 + 3] = color.a;
	}
}

// ZBuffer
void  buffers_context::depth(float depth,uint x,uint y)
{
	if (x < m_width && y < m_hight) 
	{
		m_zbuffer[(x + y * m_width)] = depth;
	}
}

float buffers_context::depth(uint x,uint y)
{
	if(x< m_width && y< m_hight)
	{
		return m_zbuffer[(x + y * m_width)];
	}
	return 0.0f;
}

void  buffers_context::clear_depth(float v)
{
	for (uint y = 0; y < m_hight; ++y)
	for (uint x = 0; x < m_width; ++x)
	{
		m_zbuffer[(x + y * m_width)] = v;
	}
}
	
void buffers_context::draw_zbuffer()
{		
	for (uint y = 0; y < m_hight; ++y)
	for (uint x = 0; x < m_width; ++x)
	{
		float intens = m_zbuffer[(x + y * m_width)];
		m_color_buffer[(x + y * m_width) * 4]     = (byte)(intens * 255);
		m_color_buffer[(x + y * m_width) * 4 + 1] = (byte)(intens * 255);
		m_color_buffer[(x + y * m_width) * 4 + 2] = (byte)(intens * 255);
		m_color_buffer[(x + y * m_width) * 4 + 3] = (byte)(intens * 255);
	}
}

void buffers_context::swap(HDC hPaintDC)
{		        
	BitBlt( hPaintDC, 0, 0, m_width, m_hight, m_hDC, 0, 0, SRCCOPY );
}

void buffers_context::swap()
{		        
	BitBlt( GetDC(m_window), 0, 0, m_width, m_hight, m_hDC, 0, 0, SRCCOPY );
}

}
#ifndef BUFFERSCONTEXT_H
#define BUFFERSCONTEXT_H

#include <Config.h>
#include <Color.h>

namespace TinyRasterization {

	///BuffersContext class
	class BuffersContext {

	HWND  window;
	HDC hDC;
	HBITMAP hBitmap;
	BYTE  *colorBuffer;
	float *zbuffer;
	uint width,hight;


	public:
		///contructor
		BuffersContext(HWND  window);
		///virtual destructor
		virtual ~BuffersContext();
		///color buffer
		void  setPixel(const Color& color,uint x,uint y);
		Color getPixel(uint x,uint y) const;	
		void zbufferToColorBuffer();
		void clear();
		void clear(const Color& color);
		///zbuffer
		void  setZValue(float zDf,uint x,uint y);
		float getZValue(uint x,uint y);
		void  clearZbuffer(float v=0.0);
		
		void swap(HDC hPaintDC);
		void swap();
		
	};

};

#endif
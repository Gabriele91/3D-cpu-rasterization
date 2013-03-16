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
	BYTE *bBytes;
	uint width,hight;


	public:
		///contructor
		BuffersContext(HWND  window);
		///virtual destructor
		virtual ~BuffersContext();
		///virtual methods
		void setPixel(const Color& color,uint x,uint y);
		Color getPixel(uint x,uint y) const;	
		void clear();
		void clear(const Color& color);
		void swap(HDC hPaintDC);
		
	};

};

#endif
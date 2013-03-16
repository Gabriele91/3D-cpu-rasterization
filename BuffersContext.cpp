#include "BuffersContext.h"
//using name space
using namespace TinyRasterization;
///contructor
BuffersContext::BuffersContext(HWND  window):window(window){
		// Application info
		RECT Rect; // just a simple rect to hold the size of our Application
		GetClientRect(window, &Rect); // grab Application size
		//get width and hight
		width= Rect.right - Rect.left;
		hight= Rect.bottom - Rect.top ;
		// create a BITMAPINFO with minimal initilisation 
        // for the CreateDIBSection
        BITMAPINFO RGB32BitsBITMAPINFO; 
        ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
        RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        RGB32BitsBITMAPINFO.bmiHeader.biWidth=width;
        RGB32BitsBITMAPINFO.bmiHeader.biHeight=hight;
        RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
        RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;  
		RGB32BitsBITMAPINFO.bmiHeader.biSizeImage = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biXPelsPerMeter = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biYPelsPerMeter = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biClrUsed = 0;
		RGB32BitsBITMAPINFO.bmiHeader.biClrImportant = 0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbBlue=0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbGreen=0;
		RGB32BitsBITMAPINFO.bmiColors[0].rgbRed=0;
		// create enough room. all pixels * each color component 
		//create context
		hDC = CreateCompatibleDC( GetDC(window) );
		// Pointer to pixels (will automatically have space allocated by CreateDIBSection
		// Create DIB section to always give direct access to pixels
		hBitmap = CreateDIBSection( GetDC(window),
                                    (BITMAPINFO *)&RGB32BitsBITMAPINFO,
                                    DIB_RGB_COLORS, 
									(void**)&bBytes,
									NULL,0 );
		// assign the dib section to the dc
		SelectObject(hDC, hBitmap); 
}
///virtual destructor
BuffersContext::~BuffersContext(){
		ReleaseDC(window, hDC); // release our dc handle
		DeleteDC(hDC);			// delete dc
		DeleteObject(hBitmap);  // delete object
		delete [] bBytes;
}
///virtual methods
void BuffersContext::setPixel(const Color& color,uint x,uint y){			
	if(x<width && y<hight){
		bBytes[(x+y*width)*4]=color.r;
		bBytes[(x+y*width)*4+1]=color.g;
		bBytes[(x+y*width)*4+2]=color.b;
		bBytes[(x+y*width)*4+3]=color.a;
	}
}
Color BuffersContext::getPixel(uint x,uint y) const{
	if(x<width && y<hight)
		return Color(bBytes[(x+y*width)*4],
						bBytes[(x+y*width)*4+1],
						bBytes[(x+y*width)*4+2],
						bBytes[(x+y*width)*4+3]);
	return Color();
}		
void BuffersContext::clear(){
	memset(bBytes,0,4*width*hight);
}
void BuffersContext::clear(const Color& color){
	for(size_t offset=0;offset<width*hight;++offset)
		memcpy(bBytes+offset*4,
			  (byte*)color,
			  sizeof(byte)*4);
}
void BuffersContext::swap(HDC hPaintDC){		        
		BitBlt( hPaintDC, 0, 0,width,hight, hDC, 0, 0, SRCCOPY );
}
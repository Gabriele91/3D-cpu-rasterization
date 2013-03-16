#ifndef COLOR_H
#define COLOR_H

#include <Config.h>
#include <Types.h>

namespace TinyRasterization {

	class Color {

	public:
		//color composiction byte
		byte r,g,b,a;
		//constructors
		Color():r(0),g(0),b(0),a(0){}
		Color(byte r,byte g,byte b,byte a):r(r),g(g),b(b),a(a){}
		//
		DFORCEINLINE Color operator + (const Color& v){
			return Color(r+v.r,g+v.g,b+v.b,a+v.a); 
		}
		DFORCEINLINE Color operator - (const Color& v){ 
			return Color(r-v.r,g-v.g,b-v.b,a-v.a);
		}
		DFORCEINLINE operator const byte* () const { return (byte*) this; }
		DFORCEINLINE operator byte* () const { return (byte*) this; }
		DFORCEINLINE byte operator [](uint i){ return ((byte*)this)[i]; }
	};

};

#endif
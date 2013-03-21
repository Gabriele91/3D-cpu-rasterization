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
		DFORCEINLINE Color operator + (const Color& v) const {
			return Color(r+v.r,g+v.g,b+v.b,a+v.a); 
		}
		DFORCEINLINE Color operator - (const Color& v) const{ 
			return Color(r-v.r,g-v.g,b-v.b,a-v.a);
		}
		DFORCEINLINE Color operator * (float f) const{ 
			return Color(r*f,g*f,b*f,a*f);
		}
		DFORCEINLINE operator const byte* () const { return (byte*) this; }
		DFORCEINLINE operator byte* () const { return (byte*) this; }
		DFORCEINLINE byte operator [](uint i){ return ((byte*)this)[i]; }
	};

	class ColorFloat {

	public:
		//color composiction byte
		float r,g,b,a;
		//constructors
		ColorFloat():r(0),g(0),b(0),a(0){}
		ColorFloat(float r,float g,float b,float a):r(r),g(g),b(b),a(a){}
		//
		DFORCEINLINE ColorFloat operator + (const ColorFloat& v) const {
			return ColorFloat(r+v.r,g+v.g,b+v.b,a+v.a); 
		}
		DFORCEINLINE ColorFloat operator - (const ColorFloat& v) const{ 
			return ColorFloat(r-v.r,g-v.g,b-v.b,a-v.a);
		}
		DFORCEINLINE ColorFloat operator * (float f) const{ 
			return ColorFloat(r*f,g*f,b*f,a*f);
		}
		DFORCEINLINE Color toColor() const { return Color(r*255,g*255,b*255,a*255); }
		DFORCEINLINE float operator [](uint i){ return ((float*)this)[i]; }
	};

};

#endif
#pragma once
#include "config.h"
#include "types.h"

namespace raster3d 
{
	
template < typename type = byte >
struct color_t
{
	// Alias
	using value_type      = type;
	using pointer		  = type*;
	using const_pointer   = const type*;
	using reference       = type&;
	using const_reference = const type&;

	// Attributes
	value_type r, g, b, a;
		
	// Constructors
	color_t() :r(value_type(0)), g(value_type(0)), b(value_type(0)), a(value_type(0)) {}
	color_t(value_type r, value_type g, value_type b, value_type a) :r(r), g(g), b(b), a(a) {}
		
	// Math operators
	color_t operator + (const color_t& v) const
	{
		return { r + v.r, g + v.g, b + v.b, a + v.a };
	}
	color_t operator - (const color_t& v) const
	{
		return { r - v.r, g - v.g, b - v.b, a - v.a };
	}

	// Generic math operators
	template < typename right >
	color_t operator * (right f) const
	{ 
		return { value_type(r * f), value_type(g * f), value_type(b * f), value_type(a * f) };
	}

	// Cast operators
	operator const_pointer   () const { return (pointer)&r; }
	operator pointer         ()       { return (pointer)&r; }
	operator const_reference () const { return r; }
	operator reference       ()       { return r; }

	// Generic cast
	template < typename type_new >
	explicit operator color_t< type_new >() const
	{
		if constexpr (( std::is_floating_point_v<value_type> && std::is_floating_point_v<type_new>) ||
				      (!std::is_floating_point_v<value_type> &&!std::is_floating_point_v<type_new>))
		{
			return { (type_new)r, (type_new)g, (type_new)b, (type_new)a };
		}
		else if constexpr (!std::is_floating_point_v<value_type> && std::is_floating_point_v<type_new>)
		{
			return { 
					 (type_new)r / std::numeric_limits<value_type>::max(),
					 (type_new)g / std::numeric_limits<value_type>::max(),
					 (type_new)b / std::numeric_limits<value_type>::max(),
					 (type_new)a / std::numeric_limits<value_type>::max()
				   };
		}
		else if constexpr (std::is_floating_point_v<value_type> && !std::is_floating_point_v<type_new>)
		{
			return { 
					 (type_new)(r * std::numeric_limits<type_new>::max()),
					 (type_new)(g * std::numeric_limits<type_new>::max()),
					 (type_new)(b * std::numeric_limits<type_new>::max()),
					 (type_new)(a * std::numeric_limits<type_new>::max())
				   };

		}
		else
		{
			static_assert(std::is_floating_point_v<type_new>, "Unsupported color cast type");
			return {};
		}
	}

	// Access operators
	reference operator [](size_t i) { return ((pointer)&r)[i]; }
		
};

// Default integer color types
using color		      = color_t< byte >;
using color_ulong     = color_t< ulong >;
using color_ulonglong = color_t< ulonglong >;
using colot_float	  = color_t< float >;
using colot_double    = color_t< double >;

};
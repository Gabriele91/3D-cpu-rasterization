#pragma once
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cfloat>
#include <climits>
#include <cmath>
#include <list>
#include <array>
#include <string>
#include <sstream>
#include <unordered_map>
#include <type_traits>

#if defined(_WIN32)
	#define NOMINMAX
	#include <windows.h>
#endif

#ifdef _MSC_VER
	#define r3d_forceinline  __forceinline
	#define r3d_inline __inline
#elif __GNUC__>=4 && __GNUC_MINOR__ >=6
	#define r3d_forceinline __attribute__ ((always_inline)) 
	#define r3d_inline inline
#elif 
	#error "compiler not supported"
#endif

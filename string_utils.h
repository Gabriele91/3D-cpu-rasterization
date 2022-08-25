#pragma once
#include "config.h"

namespace raster3d
{
	std::string to_upper(std::string);
	std::string to_lower(std::string);
	int			to_int(const std::string&);
	float		to_float(const std::string&);
	double		to_double(const std::string&);
};


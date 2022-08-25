#include "string_utils.h"
#include <algorithm>

namespace raster3d
{

std::string to_upper(std::string inout)
{
	std::transform(inout.begin(), inout.end(), inout.begin(), std::toupper);
	return inout;
}

std::string to_lower(std::string inout)
{
	std::transform(inout.begin(), inout.end(), inout.begin(), std::tolower);
	return inout;
}

int to_int(const std::string& in)
{
	return atoi(in.c_str());
}

float to_float(const std::string& in)
{
	return (float)atof(in.c_str());
}

double to_double(const std::string& in)
{
	return atof(in.c_str());
}

}
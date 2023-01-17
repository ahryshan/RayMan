#pragma once

#include <fstream>
#include <string>

namespace RayMan::FileIO {
	void WriteImage(const std::string& filename, int width, int height, const uint32_t* buffer);
}
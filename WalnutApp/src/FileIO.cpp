#include "FileIO.h"


namespace  RayMan::FileIO 
{
	void ConvertRGBAtoRGB(const uint32_t* src, uint8_t* dst, size_t dstBufferSize) {
		const uint32_t* srcPtr = src;
		for (uint8_t* dstPtr; dstPtr < dst + dstBufferSize; dstPtr += 3) {
			uint8_t* croppedSrcPtr = (uint8_t*)srcPtr;
			croppedSrcPtr += 1;
			memcpy(dstPtr, croppedSrcPtr, 3);
		}
	}

	void WriteImage(const std::string& filename, int width, int height, const uint32_t* buffer) {
		std::ofstream file{filename, std::ios_base::out | std::ios_base::binary};
		file << "P6\n"
			<< width << " " << height << '\n'
			<< 255 << "\n";

		for (int y{height - 1}; y >= 0; y--) {
			for (int x{0}; x < width; x++) {
				uint8_t* pixelComponents{(uint8_t*)&buffer[x + y*width]};
				file << (char)(pixelComponents[0]) << (char)(pixelComponents[1]) << (char)(pixelComponents[2]);
			}
		}

		file.close();
	}
}

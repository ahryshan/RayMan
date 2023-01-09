#include "Renderer.h"
#include <Walnut/Random.h>

namespace RayMan {
	void Renderer::OnResize(uint32_t width, uint32_t height) {
		if (!m_FinalImage) {
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
			delete[] m_FinalImageData;
			m_FinalImageData = new uint32_t[width * height];
		}

		if (m_FinalImage->GetHeight() == height && m_FinalImage->GetWidth() == width)
			return;

		m_FinalImage->Resize(width, height);

		delete[] m_FinalImageData;
		m_FinalImageData = new uint32_t[width * height];
	}

	void Renderer::Render() {
		for (uint32_t y{0}; y < m_FinalImage->GetHeight(); y++)
		{
			for (uint32_t x{0}; x < m_FinalImage->GetWidth(); x++) {
				glm::vec2 coord{(float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight()};

				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
			}
		}

		m_FinalImage->SetData(m_FinalImageData);
	}

	uint32_t Renderer::PerPixel(glm::vec2 coord) const {
		uint8_t r = (uint8_t)(coord.x * 255.f);
		uint8_t g = (uint8_t)(coord.y * 255.f);

		return 0xff000000 | (g << 8) | r;
	}
}
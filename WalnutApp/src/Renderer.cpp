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
		for (uint32_t i{0}; i < m_FinalImage->GetHeight() * m_FinalImage->GetWidth(); i++) {
			m_FinalImageData[i] = Walnut::Random::UInt();
			m_FinalImageData[i] |= 0xff000000;
		}

		m_FinalImage->SetData(m_FinalImageData);
	}
}
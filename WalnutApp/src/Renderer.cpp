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
		float aspectRatio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();

		for (uint32_t y{0}; y < m_FinalImage->GetHeight(); y++) {
			for (uint32_t x{0}; x < m_FinalImage->GetWidth(); x++) {
				glm::vec2 coord{(float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight()};
				coord = coord * 2.f - 1.f; // normalize to -1 -> 1

				coord.x *= aspectRatio;

				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
			}
		}

		m_FinalImage->SetData(m_FinalImageData);
	}

	uint32_t Renderer::PerPixel(glm::vec2 coord) const {

		glm::vec3 rayDirection{coord, 1.f};
		rayDirection = glm::normalize(rayDirection);
		glm::vec3 rayOrigin{0, 0, -2.f};
		float radius = 0.5f;
		glm::vec3 lightDir{-2, -2, 2};
		lightDir = glm::normalize(lightDir);

		// ray/circle intersection formula for 2D:
		// 
		// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
		// where
		// a = ray origin (vec2)
		// b = ray direction (vec2)
		// r = radius
		// t = hit distance
		//
		// it's literally a quadratic equasion, 
		// so the terms below are a,b and c from the ax^2 + bx + c = 0
		// and not from the equasion above, given as reference.
		// except, we'll use t instead of x.

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		float discriminant = b * b - 4.f * a * c;

		if (discriminant < 0)
			return 0xff000000;

		float t0 = (-b - std::sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + std::sqrt(discriminant)) / (2.0f * a);

		float nearest = std::fmin(t0, t1);

		glm::vec3 hitPoint = rayOrigin + rayDirection * nearest;
		
		glm::vec3 normal = hitPoint - 0.0f;
		normal = glm::normalize(normal);

		float lightFactor = std::fmax(glm::dot(normal, -lightDir), 0);

		glm::vec3 color{1,0,1};
		color *= lightFactor;

		uint8_t colR{uint8_t((color.r) * 255.0f)};
		uint8_t colG{uint8_t((color.g) * 255.0f)};
		uint8_t colB{uint8_t((color.b) * 255.0f)};

		return 0xff000000 | (colB << 16) | (colG << 8) | colR;
	}
}
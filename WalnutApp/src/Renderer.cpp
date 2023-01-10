#include "Renderer.h"
#include <Walnut/Random.h>



namespace RayMan {
	namespace Utils {
		static uint32_t ConvertToRGBA(const glm::vec4& color) {
			uint8_t r = color.r * 255.0f;
			uint8_t g = color.g * 255.0f;
			uint8_t b = color.b * 255.0f;
			uint8_t a = color.a * 255.0f;
			return (a << 24) | (b << 16) | (g << 8) | r;
		}
	}

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
				auto color = PerPixel(coord);
				color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color); 
			}
		}

		m_FinalImage->SetData(m_FinalImageData);
	}

	glm::vec4 Renderer::PerPixel(glm::vec2 coord) const {

		glm::vec3 rayDirection{coord, -1.f};
		rayDirection = glm::normalize(rayDirection);
		glm::vec3 rayOrigin{0, 0, 2.f};
		float radius = m_Context.SphereRadius;

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
			return glm::vec4(0,0,0,1);

		float nearest = (-b - std::sqrt(discriminant)) / (2.0f * a);

		glm::vec3 hitPoint = rayOrigin + rayDirection * nearest;
		
		glm::vec3 normal = glm::normalize(hitPoint);

		glm::vec3 color{0};

		for (const auto& light : m_Context.Lights) {
			auto lightDir = light.Direction();
			auto normalizedLight{glm::normalize(lightDir)};
			float lightFactor = std::fmax(glm::dot(normal, normalizedLight), 0);
			color += m_Context.SphereColor * (light.Color() * lightFactor);
		}

		return {color, 1};
	}
}
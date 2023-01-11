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

	void Renderer::Render(const Scene& scene, const Camera& camera) {
		Ray ray;
		ray.Origin = camera.Position();
		for (uint32_t i{0}; i < camera.RayDirections().size(); i++) {
			ray.Direction = camera.RayDirections()[i];
			auto color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4{0.0f}, glm::vec4(1.0f));
			m_FinalImageData[i] = Utils::ConvertToRGBA(color);
		}

		m_FinalImage->SetData(m_FinalImageData);
	}

	glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray) const {
		glm::vec4 bgUpColor{0.2, 0.1, 0.3, 1.0f};
		glm::vec4 bgDownColor{0.0,0.0,0.1,1.0f};
		float t{glm::clamp(ray.Direction.y * 0.5f + 0.5f, 0.0f, 1.0f)};
		glm::vec4 bgColor{t * bgUpColor + (1 - t) * bgDownColor};

		if (scene.Spheres.size() == 0)
			return bgColor;

		const Sphere* closestSphere{nullptr};
		float closestHit{std::numeric_limits<float>::max()};
		for (const auto& sphere : scene.Spheres) {

			glm::vec3 origin{ray.Origin - sphere.Position};

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			float discriminant = b * b - 4.f * a * c;

			if (discriminant < 0)
				continue;

			float nearestT = (-b - std::sqrt(discriminant)) / (2.0f * a);
			if (nearestT < closestHit)
			{
				closestHit = nearestT;
				closestSphere = &sphere;
			}
		}

		if (!closestSphere)
			return bgColor;

		glm::vec3 origin = ray.Origin - closestSphere->Position;
		glm::vec3 hitPoint = origin + ray.Direction * closestHit;
		glm::vec3 normal = glm::normalize(hitPoint);

		glm::vec3 color{scene.AmbientLight};

		for (const auto& light : scene.DirectionalLights) {
			auto lightDir = light.Direction();
			auto normalizedLight{glm::normalize(lightDir)};
			float lightFactor = std::fmax(glm::dot(normal, normalizedLight), 0);
			color += closestSphere->Albedo * (light.Color() * lightFactor);
		}

		return {color, 1.0f};
	}
}
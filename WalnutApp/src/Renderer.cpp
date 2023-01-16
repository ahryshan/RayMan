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
		m_ActiveCamera = &camera;
		m_ActiveScene = &scene;

		for (uint32_t y{0}; y < m_FinalImage->GetHeight(); y++) {
			for (uint32_t x{0}; x < m_FinalImage->GetWidth(); x++) {
				auto color = RayGen(x, y);
				color = glm::clamp(color, glm::vec4{0.0f}, glm::vec4{1.0f});
				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
			}
		}

		m_FinalImage->SetData(m_FinalImageData);

		m_ActiveCamera = nullptr;
		m_ActiveScene = nullptr;
	}

	glm::vec4 Renderer::RayGen(uint32_t x, uint32_t y) {
		Ray ray;
		ray.Origin = m_ActiveCamera->Position();
		ray.Direction = m_ActiveCamera->RayDirections()[x + y * m_FinalImage->GetWidth()];

		glm::vec3 color{};
		float scalar{1.0f};

		int bounces{5};
		for (int i{0}; i < bounces; i++) {
			Renderer::HitPayload payload = TraceRay(ray);
			if (payload.HitDistance < 0.0f) {
				glm::vec3 bgUpColor{0.6, 0.7, 0.9};
				glm::vec3 bgDownColor{0.6, 0.7, 0.9};
				//glm::vec3 bgDownColor{0.0,0.0,0.1};
				float t{glm::clamp(ray.Direction.y * 0.5f + 0.5f, 0.0f, 1.0f)};
				color += glm::vec3{t * bgUpColor + (1 - t) * bgDownColor} * scalar;
				break;
			}

			const Sphere& closestSphere = m_ActiveScene->Spheres[payload.ObjectIndex];
			const Material& material{m_ActiveScene->Materials.at(closestSphere.MaterialIndex)};


			for (const auto& light : m_ActiveScene->DirectionalLights) {
				auto lightDir = light.Direction();
				auto normalizedLight{glm::normalize(lightDir)};
				float lightFactor = std::fmax(glm::dot(payload.WorlNormal, normalizedLight), 0);
					color += material.Albedo * (light.Color() * lightFactor) * scalar;
			}

			scalar *= 0.5f;

			ray.Origin = payload.WorldPosition + payload.WorlNormal * 0.0001f;
			ray.Direction = glm::reflect(ray.Direction, (payload.WorlNormal + (material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f))));
		}

		


		return {color, 1.0f};
	}

	Renderer::HitPayload Renderer::TraceRay(const Ray& ray) {
		glm::vec4 bgUpColor{0.2, 0.1, 0.3, 1.0f};
		glm::vec4 bgDownColor{0.0,0.0,0.1,1.0f};
		float t{glm::clamp(ray.Direction.y * 0.5f + 0.5f, 0.0f, 1.0f)};
		glm::vec4 bgColor{t * bgUpColor + (1 - t) * bgDownColor};

		if (m_ActiveScene->Spheres.size() == 0)
			return Miss(ray);

		int closestSphereIndex{-1};
		float closestHit{std::numeric_limits<float>::max()};
		for (uint32_t i{0}; i < m_ActiveScene->Spheres.size(); i++) {
			const Sphere& sphere = m_ActiveScene->Spheres[i];
			glm::vec3 origin{ray.Origin - sphere.Position};

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			float discriminant = b * b - 4.f * a * c;

			if (discriminant < 0)
				continue;

			float nearestT = (-b - std::sqrt(discriminant)) / (2.0f * a);
			if (nearestT > 0.0f && nearestT < closestHit) {
				closestHit = nearestT;
				closestSphereIndex = i;
			}
		}

		if (closestSphereIndex == -1)
			return Miss(ray);

		return ClosestHit(ray, closestHit, closestSphereIndex);
	}

	Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex) {
		Renderer::HitPayload payload;
		payload.HitDistance = hitDistance;
		payload.ObjectIndex = objectIndex;

		const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphere.Position;
		payload.WorldPosition = origin + ray.Direction * hitDistance;
		payload.WorlNormal = glm::normalize(payload.WorldPosition);

		payload.WorldPosition += closestSphere.Position;

		return payload;
	}

	Renderer::HitPayload Renderer::Miss(const Ray& ray) {
		HitPayload payload;
		payload.HitDistance = -1.0f;
		return payload;
	}
}
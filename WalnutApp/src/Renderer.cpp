#include "Renderer.h"

#include <execution>

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

			delete[] m_AccumulationData;
			m_AccumulationData = new glm::vec4[width * height]{};

			m_VerticalIter.resize(height);
			m_HorizontalIter.resize(width);
			for (uint32_t i{0}; i < width; i++)
				m_HorizontalIter[i] = i;
			for (uint32_t i{0}; i < height; i++)
				m_VerticalIter[i] = i;
		}

		if (m_FinalImage->GetHeight() == height && m_FinalImage->GetWidth() == width)
			return;

		m_FinalImage->Resize(width, height);

		delete[] m_FinalImageData;
		m_FinalImageData = new uint32_t[width * height];

		delete[] m_AccumulationData;
		m_AccumulationData = new glm::vec4[width * height];

		m_VerticalIter.resize(height);
		m_HorizontalIter.resize(width);
		for (uint32_t i{0}; i < width; i++)
			m_HorizontalIter[i] = i;
		for (uint32_t i{0}; i < height; i++)
			m_VerticalIter[i] = i;
	}

	void Renderer::Render(const Scene& scene, const Camera& camera) {
		if (m_Settings.Pause)
			return;

		m_ActiveCamera = &camera;
		m_ActiveScene = &scene;
		if (m_FrameIndex == 1)
			memset(m_AccumulationData, 0, m_FinalImage->GetHeight() * m_FinalImage->GetWidth() * sizeof(glm::vec4));

#define MT 1
#if MT
		std::for_each(std::execution::par,m_VerticalIter.begin(), m_VerticalIter.end(), [this](uint32_t y) {
			std::for_each(std::execution::par, m_HorizontalIter.begin(), m_HorizontalIter.end(), [this, y](uint32_t x) {
				glm::vec4 color = RayGen(x, y);

				m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

				glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
				accumulatedColor /= (float)m_FrameIndex;
				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
			});
		});
#else
		for (uint32_t y{0}; y < m_FinalImage->GetHeight(); y++) {
			for (uint32_t x{0}; x < m_FinalImage->GetWidth(); x++) {
				glm::vec4 color = RayGen(x, y);

				m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

				glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
				accumulatedColor /= (float)m_FrameIndex;
				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_FinalImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
			}
		}
#endif


		m_FinalImage->SetData(m_FinalImageData);

		if (m_Settings.Accumulate) {
			m_FrameIndex++;
		} else {
			m_FrameIndex = 1;
		}

		m_ActiveCamera = nullptr;
		m_ActiveScene = nullptr;
	}

	glm::vec4 Renderer::RayGen(uint32_t x, uint32_t y) {
		Ray ray;
		ray.Origin = m_ActiveCamera->Position();
		ray.Direction = m_ActiveCamera->RayDirections()[x + y * m_FinalImage->GetWidth()];
		if (m_Settings.Antialising)
			ray.Direction += Walnut::Random::Vec3(-0.001f, 0.001f);

		glm::vec3 color{};
		float scalar{1.0f};

		int bounces{3};
		for (int i{0}; i < bounces; i++) {

			Renderer::HitPayload payload = TraceRay(ray);
			if (payload.HitDistance < 0.0f) {
				glm::vec3 bgUpColor{0.6, 0.7, 0.9};
				glm::vec3 bgDownColor{0.6, 0.7, 0.9};
				//glm::vec3 bgDownColor{0.0,0.0,0.1};
				float t{glm::clamp(ray.Direction.y * 0.5f + 0.5f, 0.0f, 1.0f)};
				color += glm::vec3{t * bgUpColor + (1 - t) * bgDownColor} *scalar;
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
#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <Walnut/Image.h>

#include "DirectionalLight.h"
#include "Camera.h"
#include "Scene.h"
#include "Ray.h"

namespace RayMan {

	class Renderer {
	public:
		struct Settings {
			bool Accumulate = true;
		};

	public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render(const Scene& scene, const Camera& camera);

		inline const std::shared_ptr<Walnut::Image>& GetFinalImage() const { return m_FinalImage; }
		inline uint32_t FrameIndex() const { return m_FrameIndex; }
		inline void ResetFrameAccumulation() { m_FrameIndex = 1; }
		inline Settings& GetSettings() { return m_Settings; }

	private:
		struct HitPayload {
			float HitDistance;
			glm::vec3 WorldPosition;
			glm::vec3 WorlNormal;

			uint32_t ObjectIndex;
		};

		glm::vec4 RayGen(uint32_t x, uint32_t y);
		HitPayload TraceRay(const Ray& ray);
		HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex);
		HitPayload Miss(const Ray& ray);

	private:
		const Scene* m_ActiveScene;
		const Camera* m_ActiveCamera;

		std::shared_ptr<Walnut::Image> m_FinalImage{nullptr};
		uint32_t* m_FinalImageData{nullptr};
		glm::vec4* m_AccumulationData{nullptr};
		uint32_t m_FrameIndex{0};
		Settings m_Settings;

		std::vector<uint32_t> m_HorizontalIter, m_VerticalIter;
	};
}


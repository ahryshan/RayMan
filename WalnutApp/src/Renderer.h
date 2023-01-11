#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <Walnut/Image.h>

#include "DirectionalLight.h"
#include "Camera.h"
#include "Ray.h"

namespace RayMan {

	struct RenderContext {
		glm::vec3 SphereColor;
		float SphereRadius;
		std::vector<DirectionalLight> Lights;
	};

	class Renderer {
	public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render(const Camera& camera);

		inline const std::shared_ptr<Walnut::Image>& GetFinalImage() const { return m_FinalImage; }

		inline const RenderContext& Context() const { return m_Context; }
		inline RenderContext& Context() { return m_Context; }

	private:
		glm::vec4 TraceRay(const Ray& ray) const;

	private:
		std::shared_ptr<Walnut::Image> m_FinalImage{nullptr};
		uint32_t* m_FinalImageData{nullptr};
		RenderContext m_Context;
	};
}


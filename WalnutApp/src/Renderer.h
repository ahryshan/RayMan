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
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render(const Scene& scene, const Camera& camera);

		inline const std::shared_ptr<Walnut::Image>& GetFinalImage() const { return m_FinalImage; }

	private:
		glm::vec4 TraceRay(const Scene& scene, const Ray& ray) const;

	private:
		std::shared_ptr<Walnut::Image> m_FinalImage{nullptr};
		uint32_t* m_FinalImageData{nullptr};
	};
}


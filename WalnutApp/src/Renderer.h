#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <Walnut/Image.h>

namespace RayMan {
	struct DirectionalLight {
		glm::vec3 Color;
		glm::vec3 Direction;
	};

	struct RenderContext {
		glm::vec3 SphereColor;
		float SphereRadius;
		std::vector<DirectionalLight> Lights;
	};

	class Renderer {
	public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render();

		inline const std::shared_ptr<Walnut::Image>& GetFinalImage() const { return m_FinalImage; }

		inline const RenderContext& Context() const { return m_Context; }
		inline RenderContext& Context() { return m_Context; }

	private:
		glm::vec4 PerPixel(glm::vec2 coord) const;

	private:
		std::shared_ptr<Walnut::Image> m_FinalImage{nullptr};
		uint32_t* m_FinalImageData{nullptr};
		RenderContext m_Context;
	};
}


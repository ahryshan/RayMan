#pragma once

#include <memory>

#include <Walnut/Image.h>

namespace RayMan {
	class Renderer {
	public:
		Renderer() = default;

		void OnResize(uint32_t width, uint32_t height);
		void Render();

		inline const std::shared_ptr<Walnut::Image>& GetFinalImage() const { return m_FinalImage; }

	private:
		std::shared_ptr<Walnut::Image> m_FinalImage{nullptr};
		uint32_t* m_FinalImageData{nullptr};
	};
}


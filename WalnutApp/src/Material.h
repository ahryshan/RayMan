#pragma once

#include <glm/glm.hpp>

namespace RayMan {
	struct Material {
		glm::vec3 Albedo{1.0f};
		float Roughness{1.0f};
		float Metallic{0.0f};

		inline static Material Rough() { return Material(glm::vec3{1.0f}, 1.0f, 0.0f); }
		inline static Material Glossy() { return Material(glm::vec3{1.0f}, 0.1f, 0.0f); }
		inline static Material Reflective() { return Material(glm::vec3{1.0f}, 0.0f, 0.0f); }
	};
}
#pragma once

#include <glm/glm.hpp>

namespace RayMan {
	struct Sphere {
		glm::vec3 Position{0.0f};
		glm::vec3 Albedo{1.0f};
		float Radius{0.5f};
	};
}
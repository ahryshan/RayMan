#pragma once

#include <glm/glm.hpp>

#include "Material.h"

namespace RayMan {
	struct Sphere {
		glm::vec3 Position{0.0f};
		float Radius{0.5f};

		int MaterialIndex;
	};
}
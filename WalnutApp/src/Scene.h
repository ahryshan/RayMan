#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "DirectionalLight.h"
#include "Sphere.h"

namespace RayMan {
	struct Scene {
		std::vector<Sphere> Spheres;
		std::vector<DirectionalLight> DirectionalLights;
		glm::vec3 AmbientLight;
	};
}
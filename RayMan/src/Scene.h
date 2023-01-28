#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "DirectionalLight.h"
#include "Sphere.h"
#include "Material.h"

namespace RayMan {
	struct Scene {
		std::vector<Sphere> Spheres;
		std::vector<DirectionalLight> DirectionalLights;
		std::vector<Material> Materials;

		glm::vec3 SkyLightBaseColor;
		glm::vec3 SkyLightSecondaryColor;
	};
}
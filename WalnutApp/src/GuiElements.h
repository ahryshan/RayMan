#pragma once

#include "Sphere.h"
#include "DirectionalLight.h"

namespace RayMan::UI {
	void SphereConfiguration(Sphere& sphere, int maxMaterialIndex);

	void DirectionalLightConfiguration(DirectionalLight& light);

	void MaterialConfiguration(Material& material);
}

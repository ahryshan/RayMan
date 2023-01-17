#pragma once

#include <vector>

#include "Sphere.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Renderer.h"

namespace RayMan::UI {
	void RenderConfiguration(Renderer& renderer, std::vector<Renderer::RenderPipeline>& pipelines, int& selectedPipelineIndex, bool& staticRender);

	void SphereConfiguration(Sphere& sphere, const std::vector<Material>& materials);

	void DirectionalLightConfiguration(DirectionalLight& light);

	void MaterialConfiguration(Material& material);
}

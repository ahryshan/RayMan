#include "GuiElements.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace RayMan::UI {
	void SphereConfiguration(Sphere& sphere, int maxMaterialIndex) {
		ImGui::DragFloat3("Sphere Position", glm::value_ptr(sphere.Position), 0.1f, -100.0f, 100.0f, "%.2f");
		ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.01f, 0.05f, 100.0f, "%.2f");
		int newMaterialIndex{sphere.MaterialIndex};
		ImGui::DragInt("Material Index", &newMaterialIndex, 0.01f, 0, maxMaterialIndex);
		if (newMaterialIndex != sphere.MaterialIndex) {
			sphere.MaterialIndex = glm::clamp(newMaterialIndex, 0, maxMaterialIndex);
		}
	}

	void DirectionalLightConfiguration(DirectionalLight& light) {
		auto newRotation{light.Rotation()};

		ImGui::ColorEdit3("Light Color", glm::value_ptr(light.Color()));
		ImGui::DragFloat("Light Rotation Yaw", &newRotation[0], 1, 0, 0, "%.1f");
		ImGui::DragFloat("Light Rotation Pitch", &newRotation[1], 1, 0, 180, "%.1f");
		light.SetRotation(newRotation);
	}

	void MaterialConfiguration(Material& material) {
		ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
		ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f, "%.2f");

	}
}
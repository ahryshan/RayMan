#include "GuiElements.h"

#include <string>
#include <format>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "FileIO.h"

namespace RayMan::UI {

	template<typename T>
	void GenericCombo(std::vector<T> objects, int& currentSelection, const std::string& comboName, const std::string& itemName) {
		std::vector<std::string> options(objects.size());
		for (int i{0}; i < options.size(); i++) {
			options[i] = std::format("{} {}", itemName, i);
		}

		if (ImGui::BeginCombo(comboName.c_str(), options[currentSelection].c_str())) {
			for (int n{0}; n < options.size(); n++) {
				const bool isSelected = currentSelection == n;

				if (ImGui::Selectable(options[n].c_str()))
					currentSelection = n;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	void RenderConfiguration(Renderer& renderer, std::vector<Renderer::RenderPipeline>& pipelines, int& selectedPipelineIndex, bool& staticRender) {
		Renderer::Settings& settings{renderer.GetSettings()};

		ImGui::Checkbox("Accumulate", &settings.Accumulate);
		ImGui::Checkbox("Pause Rendering", &settings.Pause);
		ImGui::Checkbox("Antialising", &settings.Antialising);
		ImGui::Checkbox("Cast Shadows", &settings.CastShadows);
		ImGui::InputInt("Bounces", &settings.Bounce);
		if (ImGui::Button("Reset Render")) {
			renderer.ResetFrameAccumulation();
		}
		if (ImGui::Button("Export Image")) {
			::RayMan::FileIO::WriteImage("image.ppm", renderer.GetFinalImage()->GetWidth(), renderer.GetFinalImage()->GetHeight(), renderer.GetImageData());
		}
		ImGui::BeginHorizontal(1);

		bool pipelineConfigDisabled = staticRender;

		if (!staticRender && ImGui::Button("Render")) {
			staticRender = true;
		} else if (staticRender && ImGui::Button("Stop Render")) {
			staticRender = false;
		}

		if (pipelineConfigDisabled)
			ImGui::BeginDisabled();

		GenericCombo(pipelines, selectedPipelineIndex, "Render Pipeline", "Pipeline");

		if (pipelineConfigDisabled)
			ImGui::EndDisabled();

		ImGui::EndHorizontal();

		if (ImGui::TreeNode("Render Pipelines")) {
			for (int i{0}; i < pipelines.size(); i++) {
				ImGui::PushID(i);

				std::string nodeTitle{std::format("Pipeline {}", i)};
				if(ImGui::TreeNode(nodeTitle.c_str()))
				{
					if (pipelineConfigDisabled)
						ImGui::BeginDisabled();

					auto& pipeline = pipelines[i];
					ImGui::Checkbox("Antialising", &pipeline.Settings.Antialising);
					ImGui::Checkbox("Cast Shadows", &pipeline.Settings.CastShadows);
					ImGui::InputInt("Bounces", &pipeline.Settings.Bounce);
					ImGui::InputInt("Frame Limit", &pipeline.FrameLimit);

					if (pipelineConfigDisabled)
						ImGui::EndDisabled();

					ImGui::TreePop();
				}

				ImGui::PopID();
			}


			ImGui::TreePop();
		}
	}

	void SphereConfiguration(Sphere& sphere, const std::vector<Material>& materials) {
		ImGui::DragFloat3("Sphere Position", glm::value_ptr(sphere.Position), 0.1f, -100.0f, 100.0f, "%.2f");
		ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.01f, 0.05f, 100.0f, "%.2f");

		GenericCombo(materials, sphere.MaterialIndex, "Material", "Material");
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
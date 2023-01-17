#include <format>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "glm/gtc/type_ptr.hpp"

#include "Renderer.h"
#include "GuiElements.h"
#include "FileIO.h"

namespace RayMan {
	class RayManLayer : public Walnut::Layer {
	public:

		virtual void OnUIRender() override {
			ImGui::Begin("Render settings");
			UI::RenderConfiguration(m_Renderer, m_RenderPipelines, m_CurrentRenderPipeline, m_StaticRenderInProgress);
			ImGui::End();

			ImGui::Begin("Scene settings");

			if (ImGui::CollapsingHeader("Scene Properties")) {
				ImGui::ColorEdit3("Sky Sphere Up", glm::value_ptr(m_Scene.SkyLightBaseColor));
				ImGui::ColorEdit3("Sky Sphere Down", glm::value_ptr(m_Scene.SkyLightSecondaryColor));
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("Objects")) {
				int i{0};
				for (auto sphere{m_Scene.Spheres.begin()}; sphere < m_Scene.Spheres.end(); sphere++) {
					ImGui::PushID(i);
					std::string nodeTitle{std::format("Sphere #{}", i + 1)};
					if (ImGui::TreeNode(nodeTitle.c_str())) {
						UI::SphereConfiguration(*sphere, m_Scene.Materials);

						if (ImGui::Button("[X] Delete Sphere"))
							m_Scene.Spheres.erase(sphere);

						ImGui::TreePop();
						ImGui::NewLine();
					}

					ImGui::PopID();
					i++;
				}

				if (ImGui::Button("[+] Add Sphere")) {
					m_Scene.Spheres.push_back(Sphere{glm::vec3{0.0f}, 0.5f});
				}
				ImGui::NewLine();
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("Lighting")) {
				int i{0};
				for (auto light{m_Scene.DirectionalLights.begin()}; light < m_Scene.DirectionalLights.end(); light++) {
					ImGui::PushID(i);

					std::string nodeTitle{std::format("Light #{}", i + 1)};
					if (ImGui::TreeNode(nodeTitle.c_str())) {
						UI::DirectionalLightConfiguration(*light);

						if (ImGui::Button("[X] Delete Light")) {
							m_Scene.DirectionalLights.erase(light);
						}
						ImGui::TreePop();
						ImGui::NewLine();
					}

					ImGui::PopID();
					i++;
				}

				if (ImGui::Button("[+] Add Light")) {
					m_Scene.DirectionalLights.push_back(DirectionalLight{glm::vec3{1}});
				}

				ImGui::NewLine();
			}

			ImGui::Separator();


			if (ImGui::CollapsingHeader("Materials")) {
				int i{0};
				for (auto material{m_Scene.Materials.begin()}; material < m_Scene.Materials.end(); material++) {
					ImGui::PushID(i);

					std::string nodeTitle{std::format("Material #{}", i)};
					if (ImGui::TreeNode(nodeTitle.c_str())) {
						UI::MaterialConfiguration(*material);

						if (ImGui::Button("[X] Delete Material")) {
							m_Scene.Materials.erase(material);
						}
						ImGui::TreePop();
						ImGui::NewLine();
					}

					ImGui::PopID();
					i++;
				}

				if (ImGui::Button("[+] Add Material")) {
					m_Scene.Materials.push_back(Material());
				}

				ImGui::NewLine();
			}

			ImGui::End();

			ImGui::Begin("Render Stats");
			ImGui::Text("Last render time: %.3fms", m_LastRenderTime);
			ImGui::Text("Frame index: %d", m_Renderer.FrameIndex());
			ImGui::End();

			ImGui::Begin("Scene Stats");
			glm::vec3 cameraPos{m_Camera.Position()};
			ImGui::Text("Camera Position: %.2f %.2f %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
			glm::vec3 cameraDir{m_Camera.ForwardDirection()};
			ImGui::Text("Camera Forward Direction: %.2f %.2f %.2f", cameraDir.x, cameraDir.y, cameraDir.z);
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
			ImGui::Begin("Viewport");

			m_ViewportHeight = ImGui::GetContentRegionAvail().y;
			m_ViewportWidth = ImGui::GetContentRegionAvail().x;


			auto image = m_Renderer.GetFinalImage();
			if (image != nullptr)
				ImGui::Image(image->GetDescriptorSet(), ImVec2((float)image->GetWidth(), (float)image->GetHeight()), ImVec2{0,1}, ImVec2{1,0});
			ImGui::End();
			ImGui::PopStyleVar();

			Render();
		}

		virtual void OnAttach() override {
			DirectionalLight light{glm::vec3{1.0f}};
			light.SetRotation(glm::vec2{112.0f, 50.0f});

			m_Scene.Materials.push_back(Material::Rough());

			Material reflective{Material::Reflective()};
			reflective.Albedo = glm::vec3{1.0f, 0.0f, 1.0f};
			m_Scene.Materials.push_back(reflective);

			Material glossy{Material::Glossy()};
			glossy.Albedo = glm::vec3{0.15f, 0.2f, 1.0f};
			m_Scene.Materials.push_back(glossy);

			m_Scene.DirectionalLights.push_back(light);
			{
				Sphere sphere{};
				sphere.Position = {0.0f, 0.0f, 0.0f};
				sphere.Radius = 0.5f;
				sphere.MaterialIndex = 0;
				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere{};
				sphere.Position = {0.0f, -100.6f, 0.0f};
				sphere.Radius = 100.0f;
				sphere.MaterialIndex = 0;
				m_Scene.Spheres.push_back(sphere);
			}

			m_Scene.SkyLightBaseColor = {0.1f, 0.2f, 0.7f};
			m_Scene.SkyLightBaseColor = {0.4f, 0.5f, 0.9f};

			Renderer::RenderPipeline pipeline;
			Renderer::Settings settings;
			settings.Accumulate = true;
			settings.Antialising = true;
			settings.CastShadows = true;
			settings.Bounce = 10;
			pipeline.Settings = settings;
			pipeline.FrameLimit = 150;
			m_RenderPipelines.push_back(pipeline);
		}

		virtual void OnUpdate(float ts) override {
			if (m_Camera.OnUpdate(ts))
				m_Renderer.ResetFrameAccumulation();
		}

		void Render() {
			Walnut::Timer timer;

			m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Renderer.Render(m_Scene, m_Camera);
			if (m_StaticRenderInProgress) {
				m_Renderer.RenderWithPipeline(m_Scene, m_Camera, m_RenderPipelines[m_CurrentRenderPipeline]);
				if (m_Renderer.FrameIndex() == 1)
					m_StaticRenderInProgress = false;

				m_LastRenderTime += timer.ElapsedMillis();
			} else {
				m_Renderer.Render(m_Scene, m_Camera);
				m_LastRenderTime = timer.ElapsedMillis();
			}
		}

	private:
		float m_LastRenderTime{0};
		uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};
		Renderer m_Renderer;
		Camera m_Camera;
		Scene m_Scene;

		std::vector<Renderer::RenderPipeline> m_RenderPipelines;
		int m_CurrentRenderPipeline{0};
		bool m_StaticRenderInProgress{false};
	};
}

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayMan Raytracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayMan::RayManLayer>();
	app->SetMenubarCallback([app]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit")) {
				app->Close();
			}
			ImGui::EndMenu();
		}
		});
	return app;
}
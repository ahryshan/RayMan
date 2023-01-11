#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "glm/gtc/type_ptr.hpp"

#include "Renderer.h"

namespace RayMan {
	class RayManLayer : public Walnut::Layer {
	public:

		virtual void OnUIRender() override {
			ImGui::Begin("Scene settings");
			ImGui::Text("Last render time: %.3fms", m_LastRenderTime);

			ImGui::NewLine();

			ImGui::ColorEdit3("Ambient color", glm::value_ptr(m_Scene.AmbientLight));

			ImGui::NewLine();

			int i{0};
			for (auto& sphere{m_Scene.Spheres.begin()}; sphere < m_Scene.Spheres.end(); sphere++) {
				ImGui::PushID(i);

				ImGui::ColorEdit3("Sphere Color", glm::value_ptr(sphere->Albedo));
				ImGui::DragFloat3("Sphere Position", glm::value_ptr(sphere->Position), 0.1f, -100.0f, 100.0f, "%.2f");
				ImGui::DragFloat("Sphere Radius", &sphere->Radius, 0.01f, 0.05f, 100.0f, "%.2f");

				if (ImGui::Button("Delete Sphere"))
					m_Scene.Spheres.erase(sphere);

				ImGui::NewLine();

				ImGui::PopID();
				i++;
			}

			if (ImGui::Button("Add Sphere")) {
				m_Scene.Spheres.push_back(Sphere{glm::vec3{0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}, 0.5f});
			}

			ImGui::NewLine();

			i = 0;
			for (auto& light{m_Scene.DirectionalLights.begin()}; light < m_Scene.DirectionalLights.end(); light++) {
				ImGui::PushID(i);

				auto newRotation{light->Rotation()};

				ImGui::ColorEdit3("Light Color", glm::value_ptr(light->Color()));
				ImGui::DragFloat("Light Rotation Yaw", &newRotation[0], 1, 0, 0, "%.1f");
				ImGui::DragFloat("Light Rotation Pitch", &newRotation[1], 1, 0, 180, "%.1f");
				light->SetRotation(newRotation);

				if (ImGui::Button("Delete Light")) {
					m_Scene.DirectionalLights.erase(light);
				}

				ImGui::NewLine();

				ImGui::PopID();
				i++;
			}

			if (ImGui::Button("Add Light")) {
				m_Scene.DirectionalLights.push_back(DirectionalLight{glm::vec3{1}});
			}

			ImGui::End();

			ImGui::Begin("Camera Stats");
			glm::vec3 cameraPos{m_Camera.Position()};
			ImGui::Text("Camera Position: %.2f %.2f %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
			glm::vec3 cameraDir{m_Camera.ForwardDirection()};
			ImGui::Text("Camera Position: %.2f %.2f %.2f", cameraDir.x, cameraDir.y, cameraDir.z);
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
			m_Scene.DirectionalLights.push_back(DirectionalLight{glm::vec3{1,1,1}});
			{
				Sphere sphere{};
				sphere.Albedo = {0.8f, 0.3f, 0.3f};
				sphere.Position = {2.0f, 0.0f, 0.0f};
				sphere.Radius = 0.5f;
				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere{};
				sphere.Albedo = {0.3f, 0.8f, 0.3f};
				sphere.Position = {0.0f, 0.0f, 0.0f};
				sphere.Radius = 0.5f;
				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere{};
				sphere.Albedo = {0.3f, 0.3f, 0.8f};
				sphere.Position = {-2.0f, 0.0f, 0.0f};
				sphere.Radius = 0.5f;
				m_Scene.Spheres.push_back(sphere);
			}
			m_Scene.AmbientLight = glm::vec3{0.05f, 0.025f, 0.05f};
		}

		virtual void OnUpdate(float ts) override {
			m_Camera.OnUpdate(ts);
		}

		void Render() {
			Walnut::Timer timer;

			m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Renderer.Render(m_Scene, m_Camera);

			m_LastRenderTime = timer.ElapsedMillis();
		}

	private:
		float m_LastRenderTime{0};
		uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};
		Renderer m_Renderer;
		Camera m_Camera;
		Scene m_Scene;
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
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
			ImGui::Begin("Settings");
			ImGui::Text("Last render time: %.3fms", m_LastRenderTime);
			if (ImGui::Button("Render")) {
				Render();
			}
			ImGui::ColorEdit3("Sphere Color", &m_Renderer.Context().SphereColor[0]);

			int i{0};
			for (auto& light{m_Renderer.Context().Lights.begin()}; light < m_Renderer.Context().Lights.end(); light++) {
				ImGui::PushID(i);

				auto newRotation{light->Rotation()};

				ImGui::ColorEdit3("Light Color", glm::value_ptr(light->Color()));
				ImGui::DragFloat("Light Rotation Yaw", &newRotation[0], 1, 0, 0, "%.1f");
				ImGui::DragFloat("Light Rotation Pitch", &newRotation[1], 1, 0, 180, "%.1f");
				if (ImGui::Button("Remove light")) {
					m_Renderer.Context().Lights.erase(light);
				}

				if (newRotation != light->Rotation())
					light->SetRotation(newRotation);

				ImGui::PopID();
				i++;
			}
			if (ImGui::Button("Add Light")) {
				m_Renderer.Context().Lights.push_back(DirectionalLight{glm::vec3{1}});
			}
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
			m_Renderer.Context().Lights.push_back(DirectionalLight{glm::vec3{1,1,1}});
			m_Renderer.Context().SphereColor = glm::vec3{1,0,1};
			m_Renderer.Context().SphereRadius = 0.5f;
		}

		void Render() {
			Walnut::Timer timer;

			m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
			m_Renderer.Render();

			m_LastRenderTime = timer.ElapsedMillis();
		}

	private:
		float m_LastRenderTime{0};
		uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};
		Renderer m_Renderer;
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
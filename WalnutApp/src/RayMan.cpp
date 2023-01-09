#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

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
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
			ImGui::Begin("Viewport");

			m_ViewportHeight = ImGui::GetContentRegionAvail().y;
			m_ViewportWidth = ImGui::GetContentRegionAvail().x;


			auto image = m_Renderer.GetFinalImage();
			if (image != nullptr)
				ImGui::Image(image->GetDescriptorSet(), ImVec2((float)image->GetWidth(), (float)image->GetHeight()));
			ImGui::End();
			ImGui::PopStyleVar();
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
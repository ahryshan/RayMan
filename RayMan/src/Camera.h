#pragma once

#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>

namespace RayMan {
	class Camera {
	public:
		Camera() : Camera(45.0f, 0.01f, 100.0f) {}
		Camera(float verticalFOV, float nearClip, float farClip);

		bool OnUpdate(float ts);
		void OnResize(uint32_t width, uint32_t height);

		inline const glm::mat4& View() const { return m_View; }
		inline const glm::mat4& InverseView() const { return m_InverseView; }
		inline const glm::mat4& Projection() const { return m_Projection; }
		inline const glm::mat4& InversProjection() const { return m_InverseProjection; }

		inline const glm::vec3& Position() const { return m_Position; }
		inline const glm::vec3& ForwardDirection() const { return m_ForwardDirection; }

		inline const std::vector<glm::vec3>& RayDirections() const { return m_RayDirections; }

	private:
		void RecalculateProjection();
		void RecalculateView();
		void RecalculateRayDirections();

	private:
		glm::mat4
			m_View{1.0f},
			m_InverseView{1.0f},
			m_Projection{1.0f},
			m_InverseProjection{1.0f};

		glm::vec3
			m_Position{0.0f},
			m_ForwardDirection{0.0f};

		glm::vec2
			m_MouseLastPosition{0.0f};

		float
			m_NearClip{0.1f},
			m_FarClip{100.0f},
			m_VerticalFOV{45.0f};

		std::vector<glm::vec3> m_RayDirections;

		uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};
	};
}

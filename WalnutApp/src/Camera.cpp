#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Walnut/Input/Input.h>
#include <Walnut/Input/KeyCodes.h>

using namespace Walnut;

namespace RayMan {
	Camera::Camera(float verticalFOV, float nearClip, float farClip) : m_VerticalFOV{verticalFOV}, m_NearClip{nearClip}, m_FarClip{farClip} {
		m_ForwardDirection = glm::vec3{0,0,-1};
		m_Position = glm::vec3{0,0,3};
	}

	bool Camera::OnUpdate(float ts) {
		glm::vec2 mousePos = Input::GetMousePosition();
		glm::vec2 delta = (mousePos - m_MouseLastPosition);
		m_MouseLastPosition = mousePos;

		if (!Input::IsMouseButtonDown(MouseButton::Right)) {
			Input::SetCursorMode(CursorMode::Normal);
			return false;
		}

		Input::SetCursorMode(CursorMode::Locked);

		bool moved = false;

		constexpr glm::vec3 upDirection{0,1,0};
		glm::vec3 rightDirection{glm::cross(m_ForwardDirection, upDirection)};

		constexpr float speed{5};

		if (Input::IsKeyDown(KeyCode::W)) {
			m_Position += m_ForwardDirection * speed * ts;
			moved = true;
		} else if (Input::IsKeyDown(KeyCode::S)) {
			m_Position += -m_ForwardDirection * speed * ts;
			moved = true;
		}

		if (Input::IsKeyDown(KeyCode::D)) {
			m_Position += rightDirection * speed * ts;
			moved = true;
		} else if (Input::IsKeyDown(KeyCode::A)) {
			m_Position += -rightDirection * speed * ts;
			moved = true;
		}

		if (Input::IsKeyDown(KeyCode::Q)) {
			m_Position += upDirection * speed * ts;
			moved = true;
		} else if (Input::IsKeyDown(KeyCode::E)) {
			m_Position += -upDirection * speed * ts;
			moved = true;
		}

		constexpr float roatationSpeed{0.003f};

		if (delta.x != 0.0f || delta.y != 0.0f) {
			float pitchDelta = delta.y * roatationSpeed;
			float yawDelta = delta.x * roatationSpeed;

			glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
				glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));
			m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

			moved = true;
		}

		if (moved) {
			RecalculateView();
			RecalculateRayDirections();
		}

		return moved;
	}

	void Camera::OnResize(uint32_t width, uint32_t height) {
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		RecalculateView();
		RecalculateProjection();
		RecalculateRayDirections();
	}

	void Camera::RecalculateProjection() {
		m_Projection = glm::perspectiveFov(m_VerticalFOV, (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
		m_InverseProjection = glm::inverse(m_Projection);
	}

	void Camera::RecalculateView() {
		m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3{0,1,0});
		m_InverseView = glm::inverse(m_View);
	}

	void Camera::RecalculateRayDirections() {
		m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

		for (uint32_t x{0}; x < m_ViewportWidth; x++) {
			for (uint32_t y{0}; y < m_ViewportHeight; y++) {
				glm::vec2 coord{(float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight};
				coord = coord * 2.0f - 1.0f; // (-1;1)

				glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
				glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
				m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
			}
		}
	}
}

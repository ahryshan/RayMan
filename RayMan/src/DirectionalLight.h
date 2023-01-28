#pragma once

#include <glm/glm.hpp>

namespace RayMan {
	class DirectionalLight {
	public:
		DirectionalLight();
		DirectionalLight(glm::vec3 color) : DirectionalLight() {
			m_Color = color;
		}

		inline const glm::vec3& Color() const { return m_Color; }
		inline glm::vec3& Color() { return m_Color; }

		inline const glm::vec2& Rotation() const { return m_Rotation; }
		void SetRotation(const glm::vec2 rotation);

		inline const glm::vec3& Direction() const { return m_Direction; }

	private:
		glm::vec2 m_Rotation;
		glm::vec3 m_Color;
		glm::vec3 m_Direction;
	};
}


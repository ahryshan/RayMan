#include "DirectionalLight.h"

#include <glm/gtx/transform.hpp>

namespace RayMan {

	DirectionalLight::DirectionalLight() {
		m_Color = glm::vec3{1};
		m_Rotation = glm::vec3{0};
		m_Direction = {0,1,0};
	}

	void DirectionalLight::SetRotation(const glm::vec2 rotation) {
		m_Rotation = rotation;
		glm::mat4 transform{1};
		transform *= glm::rotate(glm::mat4(1), glm::radians(rotation.x), glm::vec3(0, 1, 0));
		transform *= glm::rotate(glm::mat4(1), glm::radians(rotation.y), glm::vec3(0, 0, 1));
		m_Direction = glm::normalize(transform[1]);
	}
}
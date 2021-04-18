#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FoxoCommons
{
	class Transform
	{
	public:
		glm::mat4 Recompose() const;
		void Decompose(const glm::mat4& matrix);
	public:
		glm::vec3 m_Pos = glm::vec3(0, 0, 0);
		glm::quat m_Quat = glm::quat(0, 0, 0, 0);
		glm::vec3 m_Sca = glm::vec3(1, 1, 1);
	};
}
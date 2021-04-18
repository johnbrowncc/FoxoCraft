#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace FoxoCommons
{
	glm::mat4 Transform::Recompose() const
	{
		glm::mat4 matrix = glm::mat4(1.f);
		matrix = glm::translate(matrix, m_Pos);
		matrix *= glm::toMat4(m_Quat);
		matrix = glm::scale(matrix, m_Sca);
		return matrix;
	}

	void Transform::Decompose(const glm::mat4& matrix)
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(matrix, m_Sca, m_Quat, m_Pos, skew, perspective);
	}
}
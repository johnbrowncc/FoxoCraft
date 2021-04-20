#pragma once

#include <glm/glm.hpp>

struct DebugData
{
	size_t chunksRendered;
	size_t chunksTotal;
	glm::vec3 playerPos;

	bool enableWireframe = false;

	void Draw();
};
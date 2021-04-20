#include "DebugInfo.h"

#include "Chunk.h"
#include <imgui.h>

void DebugData::Draw()
{
	if (ImGui::Begin("Debug"))
	{
		float cs = static_cast<float>(FoxoCraft::s_ChunkSize);

		float xf = playerPos.x;
		float yf = playerPos.y;
		float zf = playerPos.z;
		int xi = static_cast<int>(glm::floor(xf));
		int yi = static_cast<int>(glm::floor(yf));
		int zi = static_cast<int>(glm::floor(zf));
		int xc = static_cast<int>(glm::floor(xf / cs));
		int yc = static_cast<int>(glm::floor(yf / cs));
		int zc = static_cast<int>(glm::floor(zf / cs));
		int xl = xi - xc * FoxoCraft::s_ChunkSize;
		int yl = yi - yc * FoxoCraft::s_ChunkSize;
		int zl = zi - zc * FoxoCraft::s_ChunkSize;

		ImGui::Text("FoxoCraft");
		ImGui::Text("%i fps", static_cast<int>(ImGui::GetIO().Framerate));
		ImGui::Text("C: %i/%i", chunksRendered, chunksTotal);
		ImGui::Text("XYZ: %.3f / %.3f / %.3f", xf, yf, zf);
		ImGui::Text("Block: %i %i %i", xi, yi, zi);
		ImGui::Text("Chunk: %i %i %i in %i %i %i", xl, yl, zl, xc, yc, zc);

		ImGui::Separator();
		ImGui::Checkbox("Enable Wireframe", &enableWireframe);
	}
	ImGui::End();
}
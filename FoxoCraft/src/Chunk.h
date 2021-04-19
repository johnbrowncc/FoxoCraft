#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>
#include <glad/gl.h>

namespace FoxoCraft
{
	inline constexpr size_t s_ChunkSize = 32;
	inline constexpr size_t s_ChunkSize2 = s_ChunkSize * s_ChunkSize;
	inline constexpr size_t s_ChunkSize3 = s_ChunkSize * s_ChunkSize * s_ChunkSize;

	namespace Faces
	{
		inline constexpr size_t s_NumFaces = 6;
		inline constexpr size_t s_NumVerts = 6;
		inline constexpr size_t s_Count = 9 * s_NumVerts;

		// px py pz nx ny nz tx ty tz
		inline constexpr float data[s_Count * s_NumFaces] =
		{
			// left
			0, 0, 0, -1, 0, 0, 0, 0, 0,
			0, 0, 1, -1, 0, 0, 1, 0, 0,
			0, 1, 0, -1, 0, 0, 0, 1, 0,
			0, 1, 1, -1, 0, 0, 1, 1, 0,
			0, 1, 0, -1, 0, 0, 0, 1, 0,
			0, 0, 1, -1, 0, 0, 1, 0, 0,
			// right
			1, 0, 1, 1, 0, 0, 0, 0, 0,
			1, 0, 0, 1, 0, 0, 1, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 1, 0,
			1, 1, 0, 1, 0, 0, 1, 1, 0,
			1, 1, 1, 1, 0, 0, 0, 1, 0,
			1, 0, 0, 1, 0, 0, 1, 0, 0,
			// bottom
			0, 0, 0, 0, -1, 0, 0, 0, 0,
			1, 0, 0, 0, -1, 0, 1, 0, 0,
			0, 0, 1, 0, -1, 0, 0, 1, 0,
			1, 0, 1, 0, -1, 0, 1, 1, 0,
			0, 0, 1, 0, -1, 0, 0, 1, 0,
			1, 0, 0, 0, -1, 0, 1, 0, 0,
			// top
			0, 1, 1, 0, 1, 0, 0, 0, 0,
			1, 1, 1, 0, 1, 0, 1, 0, 0,
			0, 1, 0, 0, 1, 0, 0, 1, 0,
			1, 1, 0, 0, 1, 0, 1, 1, 0,
			0, 1, 0, 0, 1, 0, 0, 1, 0,
			1, 1, 1, 0, 1, 0, 1, 0, 0,
			// back
			1, 0, 0, 0, 0, -1, 0, 0, 0,
			0, 0, 0, 0, 0, -1, 1, 0, 0,
			1, 1, 0, 0, 0, -1, 0, 1, 0,
			0, 1, 0, 0, 0, -1, 1, 1, 0,
			1, 1, 0, 0, 0, -1, 0, 1, 0,
			0, 0, 0, 0, 0, -1, 1, 0, 0,
			// front
			0, 0, 1, 0, 0, 1, 0, 0, 0,
			1, 0, 1, 0, 0, 1, 1, 0, 0,
			0, 1, 1, 0, 0, 1, 0, 1, 0,
			1, 1, 1, 0, 0, 1, 1, 1, 0,
			0, 1, 1, 0, 0, 1, 0, 1, 0,
			1, 0, 1, 0, 0, 1, 1, 0, 0
		};

		const float* GetFacePointer(size_t faceIndex);
		void AppendFace(std::vector<float>& data, size_t faceIndex, glm::ivec3 ws, int textureIndex, int& count);
	};

	struct BlockFace
	{
		BlockFace(size_t index);

		size_t m_TextureIndex;
	};

	struct Block
	{
		Block(std::shared_ptr<BlockFace> top, std::shared_ptr<BlockFace> side, std::shared_ptr<BlockFace> bottom);

		std::shared_ptr<BlockFace> m_Top;
		std::shared_ptr<BlockFace> m_Side;
		std::shared_ptr<BlockFace> m_Bottom;
	};

	std::shared_ptr<BlockFace> GetBlockFace(const std::string& id);
	std::shared_ptr<Block> GetBlock(const std::string& id);

	void RegisterBlockFace(const std::string& id, std::shared_ptr<BlockFace> face);
	void RegisterBlock(const std::string& id, std::shared_ptr<Block> block);

	struct World;

	struct Chunk
	{
		glm::ivec3 m_Pos = glm::ivec3(0, 0, 0);
		World* m_World = nullptr;
		std::array<std::shared_ptr<Block>, s_ChunkSize3> m_Data;
		GLint m_Count = 0;
		GLuint m_Vao = 0;
		GLuint m_Vbo = 0;

		Chunk(glm::ivec3 pos, World* world);
		~Chunk();

		inline size_t IndexLS(glm::ivec3 ls)
		{
			return ls.z * s_ChunkSize2 + ls.y * s_ChunkSize + ls.x;
		}

		bool InBoundsLS(glm::ivec3 ls);

		glm::ivec3 WSLS(glm::ivec3 ws);

		std::shared_ptr<Block> GetBlockLSUS(glm::ivec3 ls);

		std::shared_ptr<Block> GetBlockLS(glm::ivec3 ls);

		std::shared_ptr<Block> GetBlockWSEX(glm::ivec3 ws);

		void SetBlockLS(glm::ivec3 ls, std::shared_ptr<Block> block);

		void Generate();

		void BuildMeshV2();

		void Render();
	};

	struct World
	{
		std::vector<std::shared_ptr<Chunk>> m_Chunks;

		void AddChunks();

		std::shared_ptr<Block> GetBlockWS(glm::ivec3 ws);

		void Render();
	};
}
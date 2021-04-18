#include <glad/gl.h>
#include <spdlog/spdlog.h>
#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "FoxoCommons/Window.h"
#include "FoxoCommons/Shader.h"
#include "FoxoCommons/Transform.h"
#include "FoxoCommons/Util.h"
#include "FoxoCommons/Texture.h"

#include "Log.h"
#include "OpenSimplexNoise.h"
#include "stb_image.h"

struct BlockFace
{
	BlockFace(size_t index)
		: m_TextureIndex(index)
	{
	}

	size_t m_TextureIndex;
};

std::unordered_map<std::string, std::shared_ptr<BlockFace>> s_BlockFaces;

std::shared_ptr<BlockFace> GetBlockFace(const std::string& id)
{
	auto result = s_BlockFaces.find(id);

	if (result != s_BlockFaces.end())
		return result->second;

	return nullptr;
}

namespace Faces
{
	static constexpr size_t s_NumFaces = 6;
	static constexpr size_t s_NumVerts = 6;
	static constexpr size_t s_Count = 9 * s_NumVerts;

	// px py pz nx ny nz tx ty tz
	static float data[s_Count * s_NumFaces] =
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

	float* GetFacePointer(size_t faceIndex)
	{
		return data + faceIndex * s_Count;
	}

	void AppendFace(std::vector<float>& data, size_t faceIndex, int x, int y, int z, int textureIndex, int& count)
	{
		float* facePtr = GetFacePointer(faceIndex);

		for (size_t i = 0; i < s_Count; i += 9)
		{
			data.push_back(facePtr[i + 0] + x);
			data.push_back(facePtr[i + 1] + y);
			data.push_back(facePtr[i + 2] + z);
			data.push_back(facePtr[i + 3]);
			data.push_back(facePtr[i + 4]);
			data.push_back(facePtr[i + 5]);
			data.push_back(facePtr[i + 6]);
			data.push_back(facePtr[i + 7]);
			data.push_back(facePtr[i + 8] + textureIndex);
		}

		count += 6;
	}
};

struct Camera
{
	FoxoCommons::Transform transform;

	void Update(GLFWwindow* window, double deltaTime, float dx, float dy)
	{
		if (dx != 0.0f || dy != 0.0f)
		{
			glm::mat4 matrix = transform.Recompose();
			matrix = glm::rotate(matrix, glm::radians(dx * -0.1f), glm::vec3(glm::inverse(matrix) * glm::vec4(0, 1, 0, 0)));
			matrix = glm::rotate(matrix, glm::radians(dy * -0.1f), glm::vec3(1, 0, 0));
			transform.Decompose(matrix);
		}

		float speed = 10;

		glm::vec4 movement = glm::vec4(0.0f);

		if (glfwGetKey(window, GLFW_KEY_W)) --movement.z;
		if (glfwGetKey(window, GLFW_KEY_S)) ++movement.z;
		if (glfwGetKey(window, GLFW_KEY_A)) --movement.x;
		if (glfwGetKey(window, GLFW_KEY_D)) ++movement.x;
		if (glfwGetKey(window, GLFW_KEY_Q)) --movement.y;
		if (glfwGetKey(window, GLFW_KEY_E)) ++movement.y;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) ++movement.w;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) --movement.w;

		if (glm::length2(movement) > 0)
		{
			glm::mat4 matrix = transform.Recompose();

			movement = glm::normalize(movement) * speed * static_cast<float>(deltaTime);

			matrix = glm::translate(matrix, glm::vec3(movement));
			matrix = glm::translate(matrix, glm::vec3(glm::inverse(matrix) * glm::vec4(0, 1, 0, 0)) * movement.w);
			transform.Decompose(matrix);
		}
	}
};

struct Storage
{
	glm::vec2 mouseLast = glm::vec2();
	glm::vec2 mouseCurrent = glm::vec2();
	glm::vec2 mouseDelta = glm::vec2();
} s_Storage;

static constexpr size_t s_ChunkSize = 32;
static constexpr size_t s_ChunkSize2 = s_ChunkSize * s_ChunkSize;
static constexpr size_t s_ChunkSize3 = s_ChunkSize * s_ChunkSize * s_ChunkSize;

struct ChunkData
{
	int m_X, m_Y, m_Z;

	std::array<std::shared_ptr<BlockFace>, s_ChunkSize3> blocks;

	ChunkData(int x, int y, int z)
	{
		m_X = x;
		m_Y = y;
		m_Z = z;

		for (size_t i = 0; i < blocks.size(); ++i)
		{
			blocks[i] = nullptr;
		}
	}

	size_t Index(int x, int y, int z)
	{
		return z * s_ChunkSize2 + y * s_ChunkSize + x;
	}

	bool InBounds(int x, int y, int z)
	{
		if (x < 0) return false;
		if (y < 0) return false;
		if (z < 0) return false;
		if (x >= s_ChunkSize) return false;
		if (y >= s_ChunkSize) return false;
		if (z >= s_ChunkSize) return false;

		return true;
	}

	std::shared_ptr<BlockFace> GetBlock(int x, int y, int z)
	{
		if (!InBounds(x, y, z)) return nullptr;
		return blocks[Index(x, y, z)];
	}

	void SetBlock(int x, int y, int z, std::shared_ptr<BlockFace> block)
	{
		if (!InBounds(x, y, z)) return;
		blocks[Index(x, y, z)] = block;
	}

	void Generate()
	{
		OpenSimplexNoise noise = OpenSimplexNoise(0);

		std::shared_ptr<BlockFace> grass = ::GetBlockFace("core.grass");

		for (int z = 0; z < s_ChunkSize; ++z)
		{
			int wz = z + m_Z * s_ChunkSize;

			for (int x = 0; x < s_ChunkSize; ++x)
			{
				int wx = x + m_X * s_ChunkSize;

				int height = FoxoCommons::Map<float>((float) noise.Evaluate(wx / 16.f, wz / 16.f), -1.f, 1.f, 5.f, 20.f);

				for (int y = 0; y < height; ++y)
				{
					SetBlock(x, y, z, grass);
				}
			}
		}
	}

#if 0
	void BuildMeshV1(std::vector<float>& vector, int& count)
	{
		for (int z = 0; z < s_ChunkSize; ++z)
		{
			for (int y = 0; y < s_ChunkSize; ++y)
			{
				for (int x = 0; x < s_ChunkSize; ++x)
				{
					std::shared_ptr<BlockFace> block = GetBlock(x, y, z);

					if (!block) continue;

					for(int i = 0; i < Faces::s_NumFaces; ++i)
						Faces::AppendFace(vector, i, x, y, z, block->m_TextureIndex, count);
				}
			}
		}
	}
#endif

	void BuildMeshV2(std::vector<float>& vector, int& count)
	{
		for (int z = 0; z < s_ChunkSize; ++z)
		{
			int wz = z + m_Z * s_ChunkSize;

			for (int y = 0; y < s_ChunkSize; ++y)
			{
				int wy = y + m_Y * s_ChunkSize;

				for (int x = 0; x < s_ChunkSize; ++x)
				{
					int wx = x + m_X * s_ChunkSize;

					std::shared_ptr<BlockFace> block = GetBlock(x, y, z);
					if (!block) continue;

					if (!GetBlock(x - 1, y, z)) Faces::AppendFace(vector, 0, wx, wy, wz, block->m_TextureIndex, count);
					if (!GetBlock(x + 1, y, z)) Faces::AppendFace(vector, 1, wx, wy, wz, block->m_TextureIndex, count);
					if (!GetBlock(x, y - 1, z)) Faces::AppendFace(vector, 2, wx, wy, wz, block->m_TextureIndex, count);
					if (!GetBlock(x, y + 1, z)) Faces::AppendFace(vector, 3, wx, wy, wz, block->m_TextureIndex, count);
					if (!GetBlock(x, y, z - 1)) Faces::AppendFace(vector, 4, wx, wy, wz, block->m_TextureIndex, count);
					if (!GetBlock(x, y, z + 1)) Faces::AppendFace(vector, 5, wx, wy, wz, block->m_TextureIndex, count);
				}
			}
		}
	}
};

static int Run()
{
	FoxoCommons::Window window = FoxoCommons::Window(1280, 720, "VoxelGame");

	if (!window.GetHandle()) return -1;

	window.MakeContextCurrent();
	window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		spdlog::error("Failed to load opengl");
		return -1;
	}

	spdlog::info(glGetString(GL_VENDOR));
	spdlog::info(glGetString(GL_RENDERER));
	spdlog::info(glGetString(GL_VERSION));
	spdlog::info(glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	FoxoCommons::Texture2DArray texture;
	{
		s_BlockFaces["core.grass"] = std::make_shared<BlockFace>(0);

		int w, h, c;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pixels = stbi_load("res/grass.png", &w, &h, &c, 4);

		if (pixels)
		{
			texture = FoxoCommons::Texture2DArray(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA8, w, h, 1, true);
			texture.SubImage(0, 0, 0, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			texture.GenerateMipmaps();
		}

		stbi_image_free(pixels);
	}

	ChunkData chunk = ChunkData(1, 0, 0);
	chunk.Generate();
	std::vector<float> data;
	int count = 0;
	chunk.BuildMeshV2(data, count);

	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, data.size() * sizeof(float), data.data(), GL_NONE);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, 9 * sizeof(float));
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribBinding(vao, 2, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	FoxoCommons::Program program;

	{
		std::optional<std::string> vertSrc = FoxoCommons::ReadTextFile("res/chunk.vert");
		std::optional<std::string> fragSrc = FoxoCommons::ReadTextFile("res/chunk.frag");

		if (vertSrc && fragSrc)
		{
			std::vector<FoxoCommons::Shader> shaders;
			shaders.reserve(2);
			shaders.emplace_back(GL_VERTEX_SHADER, vertSrc.value());
			shaders.emplace_back(GL_FRAGMENT_SHADER, fragSrc.value());

			program = FoxoCommons::Program(shaders);
		}
		else
		{
			FE_LOG_ERROR("Failed to load shaders");
		}
	}

	window.SetUserPointer(&s_Storage);

	glfwSetCursorPosCallback(window.GetHandle(), [](GLFWwindow* win, double x, double y)
	{
		Storage* ptr = static_cast<Storage*>(glfwGetWindowUserPointer(win));
		ptr->mouseCurrent.x = static_cast<float>(x);
		ptr->mouseCurrent.y = static_cast<float>(y);
	});

	double lastTime, currentTime, deltaTime;

	lastTime = glfwGetTime();
	deltaTime = 1.;

	Camera camera;

	while (!window.ShouldClose())
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		s_Storage.mouseLast = s_Storage.mouseCurrent;
		glfwPollEvents();
		s_Storage.mouseDelta = s_Storage.mouseCurrent - s_Storage.mouseLast;

		camera.Update(window.GetHandle(), deltaTime, s_Storage.mouseDelta.x, s_Storage.mouseDelta.y);

		auto [w, h] = window.GetSize();

		glViewport(0, 0, w, h);
		glClearColor(0.7f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture.Bind(0);
		program.Bind();
		program.UniformMat4f("u_Projection", glm::infinitePerspective(glm::radians(70.f), window.GetAspect(), 0.01f));
		program.UniformMat4f("u_View", glm::inverse(camera.transform.Recompose()));
		program.UniformMat4f("u_Model", glm::mat4(1.0f));
		program.Uniform1i("u_Albedo", 0);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, count);

		window.SwapBuffers();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	return 0;
}

int main()
{
	FoxoEngine::CreateLogger();
	int status = Run();
	FoxoEngine::DestroyLogger();
	return status;
}
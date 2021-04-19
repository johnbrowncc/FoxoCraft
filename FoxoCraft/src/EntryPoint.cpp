#include <glad/gl.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <filesystem>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <FoxoCommons/Window.h>
#include <FoxoCommons/Shader.h>
#include <FoxoCommons/Transform.h>
#include <FoxoCommons/Util.h>
#include <FoxoCommons/Texture.h>

#include "Log.h"
#include "OpenSimplexNoise.h"
#include "stb_image.h"

#include "Chunk.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <limits>

struct Player
{
	FoxoCommons::Transform m_Transform;
	FoxoCommons::Transform m_TransformExtra;
	float vel = 0;
	bool canJump = false;

	Player()
	{
		m_Transform.m_Pos.y = 80;
	}

	void Update(GLFWwindow* window, double deltaTime, glm::vec2 mouseDelta, bool mouseLocked, FoxoCraft::World& world)
	{
		if (!mouseLocked) return;

		if (mouseDelta.x != 0.f)
		{
			glm::mat4 matrix = m_Transform.Recompose();
			matrix = glm::rotate(matrix, glm::radians(mouseDelta.x * -0.1f), glm::vec3(glm::inverse(matrix) * glm::vec4(0, 1, 0, 0)));
			m_Transform.Decompose(matrix);

			matrix = m_TransformExtra.Recompose();
			matrix = glm::rotate(matrix, glm::radians(mouseDelta.y * -0.1f), glm::vec3(1, 0, 0));
			m_TransformExtra.Decompose(matrix);
		}

		vel += -10 * static_cast<float>(deltaTime);

		float speed = 4;
		glm::vec3 movement = glm::vec3(0.0f);
		
		if (glfwGetKey(window, GLFW_KEY_W)) --movement.z;
		if (glfwGetKey(window, GLFW_KEY_S)) ++movement.z;
		if (glfwGetKey(window, GLFW_KEY_A)) --movement.x;
		if (glfwGetKey(window, GLFW_KEY_D)) ++movement.x;
		
		

		if (glfwGetKey(window, GLFW_KEY_SPACE) && canJump)
		{
			vel = 5;
			canJump = false;
		}

		//if (glfwGetKey(window, GLFW_KEY_SPACE)) ++movement.y;
		//if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) --movement.y;
		
		if (glm::length2(movement) > 0)
		{
			movement = glm::normalize(movement) * speed * static_cast<float>(deltaTime);
		}

		glm::mat4 matrix = m_Transform.Recompose();
		matrix = glm::translate(matrix, glm::vec3(movement));
		FoxoCommons::Transform nextTransform;
		nextTransform.Decompose(matrix);
		nextTransform.m_Pos.y += vel * static_cast<float>(deltaTime);

		glm::vec3& currentPos = m_Transform.m_Pos;
		glm::vec3& nextPos = nextTransform.m_Pos;

		if (!world.GetBlockWS(glm::vec3(nextPos.x, currentPos.y, currentPos.z)))
		{
			currentPos.x = nextPos.x;
		}

		if (!world.GetBlockWS(glm::vec3(currentPos.x, nextPos.y, currentPos.z)))
		{
			currentPos.y = nextPos.y;
		}
		else
		{
			vel = 0;
			canJump = true;
		}

		if (!world.GetBlockWS(glm::vec3(currentPos.x, currentPos.y, nextPos.z)))
		{
			currentPos.z = nextPos.z;
		}
		
	}
};

struct Camera
{
	FoxoCommons::Transform transform;

	Camera()
	{
		transform.m_Pos.y = 80;
	}

	void Update(GLFWwindow* window, double deltaTime, float dx, float dy, bool mouseLocked, FoxoCraft::World& world)
	{
		if (!mouseLocked) return;

		if (dx != 0.0f || dy != 0.0f)
		{
			glm::mat4 matrix = transform.Recompose();
			matrix = glm::rotate(matrix, glm::radians(dx * -0.1f), glm::vec3(glm::inverse(matrix) * glm::vec4(0, 1, 0, 0)));
			matrix = glm::rotate(matrix, glm::radians(dy * -0.1f), glm::vec3(1, 0, 0));
			transform.Decompose(matrix);
		}

		constexpr float defaultMovementSpeed = 10;
		constexpr float accelerMovementSpeed = 50;

		float speed = defaultMovementSpeed;

		glm::vec4 movement = glm::vec4(0.0f);

		if (glfwGetKey(window, GLFW_KEY_W)) --movement.z;
		if (glfwGetKey(window, GLFW_KEY_S)) ++movement.z;
		if (glfwGetKey(window, GLFW_KEY_A)) --movement.x;
		if (glfwGetKey(window, GLFW_KEY_D)) ++movement.x;
		if (glfwGetKey(window, GLFW_KEY_Q)) --movement.y;
		if (glfwGetKey(window, GLFW_KEY_E)) ++movement.y;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) ++movement.w;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) --movement.w;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) speed = accelerMovementSpeed;

		if (glm::length2(movement) > 0)
		{
			

			glm::mat4 matrix = transform.Recompose();

			movement = glm::normalize(movement) * speed * static_cast<float>(deltaTime);

			matrix = glm::translate(matrix, glm::vec3(movement));
			matrix = glm::translate(matrix, glm::vec3(glm::inverse(matrix) * glm::vec4(0, 1, 0, 0)) * movement.w);

			FoxoCommons::Transform newTransform;
			newTransform.Decompose(matrix);

			FoxoCraft::Block* block = world.GetBlockWS(newTransform.m_Pos);

			if (!block)
			{
				transform = newTransform;
			}
		}
	}
};

struct Storage
{
	glm::vec2 mouseLast = glm::vec2();
	glm::vec2 mouseCurrent = glm::vec2();
	glm::vec2 mouseDelta = glm::vec2();
} s_Storage;

struct ModLoader
{
	static void Load(FoxoCommons::Texture2DArray& gpuTexture)
	{
		struct TextureInfo
		{
			std::string m_FileName;
			std::string m_Id;

			TextureInfo(std::string_view filename, std::string_view id)
			{
				m_FileName = filename;
				m_Id = id;
			}

			int w = 0, h = 0, c = 0;
			stbi_uc* pixels = nullptr;
		};

		std::vector<TextureInfo> textures;

		// Discover modlist
		std::vector<std::string> mods;
		for (const auto& entry : std::filesystem::directory_iterator("FoxoCraft/mods"))
		{
			mods.push_back(entry.path().filename().u8string());
		}

		// Load textures for each mod
		for (const auto& mod : mods)
		{
			for (const auto& entry : std::filesystem::directory_iterator("FoxoCraft/mods/" + mod + "/textures"))
			{
				std::string texturePath = entry.path().u8string();
				std::string textureName = entry.path().filename().u8string();
				textureName = textureName.substr(0, textureName.find_last_of("."));

				textures.emplace_back(texturePath, mod + '.' + textureName);
			}
		}

		stbi_set_flip_vertically_on_load(true);

		for (auto& texture : textures)
		{
			texture.pixels = stbi_load(texture.m_FileName.data(), &texture.w, &texture.h, &texture.c, 4);
		}

		int largestWidth = 0, largestHeight = 0;

		for (auto& texture : textures)
		{
			if (texture.w > largestWidth) largestWidth = texture.w;
			if (texture.h > largestHeight) largestHeight = texture.h;
		}

		gpuTexture = FoxoCommons::Texture2DArray(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA8, largestWidth, largestHeight, textures.size(), true);

		for (size_t i = 0; i < textures.size(); ++i)
		{
			auto& texture = textures[i];

			FoxoCraft::RegisterBlockFace(texture.m_Id, FoxoCraft::BlockFace(i));

			if (texture.pixels)
			{
				gpuTexture.SubImage(0, 0, i, texture.w, texture.h, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels);
				stbi_image_free(texture.pixels);
			}
		}

		gpuTexture.GenerateMipmaps();
	}
};

static int Run()
{
	FoxoCommons::Window window = FoxoCommons::Window(1280, 720, "FoxoCraft", []()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	});

	if (!window.GetHandle()) return -1;

	window.MakeContextCurrent();

	if (!gladLoadGL(glfwGetProcAddress))
	{
		spdlog::error("Failed to load opengl");
		return -1;
	}

	spdlog::info(glGetString(GL_VENDOR));
	spdlog::info(glGetString(GL_RENDERER));
	spdlog::info(glGetString(GL_VERSION));
	spdlog::info(glGetString(GL_SHADING_LANGUAGE_VERSION));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window.GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	FoxoCommons::Texture2DArray texture;
	ModLoader::Load(texture);

	FoxoCraft::RegisterBlock("core.grass", FoxoCraft::Block(FoxoCraft::GetBlockFace("core.grass"), FoxoCraft::GetBlockFace("core.grass_side"), FoxoCraft::GetBlockFace("core.dirt")));
	FoxoCraft::RegisterBlock("core.dirt", FoxoCraft::Block(FoxoCraft::GetBlockFace("core.dirt"), FoxoCraft::GetBlockFace("core.dirt"), FoxoCraft::GetBlockFace("core.dirt")));
	FoxoCraft::RegisterBlock("core.wood", FoxoCraft::Block(FoxoCraft::GetBlockFace("core.wood"), FoxoCraft::GetBlockFace("core.wood"), FoxoCraft::GetBlockFace("core.wood")));
	FoxoCraft::RegisterBlock("core.stone", FoxoCraft::Block(FoxoCraft::GetBlockFace("core.stone"), FoxoCraft::GetBlockFace("core.stone"), FoxoCraft::GetBlockFace("core.stone")));
	FoxoCraft::LockModify(); // prevent further changes to structures

	int64_t seed = FoxoCommons::GenerateValue(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
	FE_LOG_INFO("Using seed: {}", seed);
	FoxoCraft::World world = FoxoCraft::World(seed);
	world.AddChunks();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

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

	//Camera camera;
	Player player;

	bool mouseLocked = false;

	while (!window.ShouldClose())
	{
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		s_Storage.mouseLast = s_Storage.mouseCurrent;
		glfwPollEvents();
		s_Storage.mouseDelta = s_Storage.mouseCurrent - s_Storage.mouseLast;

		// Lock the mouse
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		{
			if (!mouseLocked)
			{
				mouseLocked = true;
				window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			}
		}
		else if (glfwGetKey(window.GetHandle(), GLFW_KEY_ESCAPE))
		{
			if (mouseLocked)
			{
				mouseLocked = false;
				window.SetInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static bool enableWireframe = false;

		if (ImGui::Begin("Debug"))
		{
			ImGui::Checkbox("Enable Wireframe", &enableWireframe);
		}
		ImGui::End();

		//camera.Update(window.GetHandle(), deltaTime, s_Storage.mouseDelta.x, s_Storage.mouseDelta.y, mouseLocked, world);
		player.Update(window.GetHandle(), deltaTime, s_Storage.mouseDelta, mouseLocked, world);

		auto [w, h] = window.GetSize();

		glViewport(0, 0, w, h);
		glClearColor(0.7f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(enableWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		texture.Bind(0);
		program.Bind();
		program.UniformMat4f("u_Projection", glm::infinitePerspective(glm::radians(90.f), window.GetAspect(), 0.01f));
		//program.UniformMat4f("u_View", glm::inverse(camera.transform.Recompose()));
		FoxoCommons::Transform t = player.m_Transform;
		t.m_Pos.y += 1.6f;

		program.UniformMat4f("u_View", glm::inverse(t.Recompose() * player.m_TransformExtra.Recompose()));
		program.UniformMat4f("u_Model", glm::mat4(1.0f));
		program.Uniform1i("u_Albedo", 0);

		world.Render();


		if (enableWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.SwapBuffers();
	}

	return 0;
}

int main()
{
	FoxoEngine::CreateLogger();
	int status = Run();
	FoxoEngine::DestroyLogger();
	return status;
}
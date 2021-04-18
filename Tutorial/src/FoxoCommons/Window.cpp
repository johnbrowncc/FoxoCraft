#include "Window.h"

#include <spdlog/spdlog.h>

namespace FoxoCommons
{
	static size_t s_Count = 0;

	Window::Window(int width, int height, std::string_view title)
	{
		if (s_Count == 0)
		{
			glfwSetErrorCallback([](int error_code, const char* description)
			{
				spdlog::error("glfw error {:#x}: {}", error_code, description);
			});

			if (!glfwInit())
			{
				spdlog::error("failed to initialize glfw");
			}
		}

		++s_Count;

		m_Handle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

		if (!m_Handle)
		{
			spdlog::error("failed to create window");
		}
	}

	Window::~Window()
	{
		if (m_Handle) glfwDestroyWindow(m_Handle);
		m_Handle = nullptr;

		--s_Count;

		if (s_Count == 0)
		{
			glfwTerminate();
			glfwSetErrorCallback(nullptr);
		}
	}

	Window::Window(Window&& other) noexcept
	{
		Window::operator=(std::move(other));
	}

	Window& Window::operator=(Window&& other) noexcept
	{
		Window::~Window();
		m_Handle = other.m_Handle;
		other.m_Handle = nullptr;

		return *this;
	}
}
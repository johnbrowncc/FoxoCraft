#pragma once

#include <utility>
#include <string_view>

#include <GLFW/glfw3.h>

namespace FoxoCommons
{
	class Window final
	{
	public:
		Window() = default;
		Window(int width, int height, std::string_view title);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		inline GLFWwindow* GetHandle() const
		{
			return m_Handle;
		}

		inline void MakeContextCurrent() const
		{
			glfwMakeContextCurrent(m_Handle);
		}

		inline void SwapBuffers() const
		{
			glfwSwapBuffers(m_Handle);
		}

		inline bool ShouldClose() const
		{
			return glfwWindowShouldClose(m_Handle);
		}

		inline std::pair<int, int> GetSize()
		{
			int w, h;
			glfwGetFramebufferSize(m_Handle, &w, &h);
			return { w, h };
		}

		inline float GetAspect()
		{
			auto [w, h] = GetSize();
			return static_cast<float>(w) / static_cast<float>(h);
		}

		inline void SetUserPointer(void* pointer)
		{
			glfwSetWindowUserPointer(m_Handle, pointer);
		}
		
		inline void SetInputMode(int mode, int value)
		{
			glfwSetInputMode(m_Handle, mode, value);
		}
	private:
		GLFWwindow* m_Handle = nullptr;
	};
}
#include "Shader.h"

#include <spdlog/spdlog.h>

namespace FoxoCommons
{
	Shader::Shader(GLenum type, std::string_view source)
	{
		m_Handle = glCreateShader(type);

		const char* data = source.data();
		glShaderSource(m_Handle, 1, &data, nullptr);

		glCompileShader(m_Handle);

		GLint status;

		glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE)
		{
			glGetShaderiv(m_Handle, GL_INFO_LOG_LENGTH, &status);

			std::unique_ptr<GLchar[]> message = std::make_unique<GLchar[]>(status);

			glGetShaderInfoLog(m_Handle, status, &status, message.get());

			spdlog::error("Shader compilation failed: {}", message.get());
		}
	}

	Shader::~Shader()
	{
		if (m_Handle != 0) glDeleteShader(m_Handle);
	}

	Shader::Shader(Shader&& other) noexcept
	{
		Shader::operator=(std::move(other));
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		Shader::~Shader();
		m_Handle = other.m_Handle;
		other.m_Handle = 0;
		return *this;
	}

	Program::Program(const std::vector<Shader>& shaders)
	{
		m_Handle = glCreateProgram();

		for (const Shader& shader : shaders)
			glAttachShader(m_Handle, shader.m_Handle);

		glLinkProgram(m_Handle);

		for (const Shader& shader : shaders)
			glDetachShader(m_Handle, shader.m_Handle);

		GLint status;

		glGetProgramiv(m_Handle, GL_LINK_STATUS, &status);

		if (status != GL_TRUE)
		{
			glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &status);

			std::unique_ptr<GLchar[]> message = std::make_unique<GLchar[]>(status);

			glGetProgramInfoLog(m_Handle, status, &status, message.get());

			spdlog::error("Program linking failed: {}", message.get());
		}
	}

	Program::~Program()
	{
		if(m_Handle != 0) glDeleteProgram(m_Handle);
	}

	Program::Program(Program&& other) noexcept
	{
		Program::operator=(std::move(other));
	}

	Program& Program::operator=(Program&& other) noexcept
	{
		Program::~Program();
		m_Handle = other.m_Handle;
		other.m_Handle = 0;
		return *this;
	}
}
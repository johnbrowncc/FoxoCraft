#pragma once

#include <string_view>
#include <memory>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace FoxoCommons
{
	class Shader final
	{
	public:
		Shader(GLenum type, std::string_view source);
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;
	private:
		GLuint m_Handle = 0;

		friend class Program;
	};

	class Program final
	{
	public:
		Program() = default;
		Program(const std::vector<Shader>& shaders);
		~Program();

		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

		Program(Program&& other) noexcept;
		Program& operator=(Program&& other) noexcept;

		inline void Bind() const
		{
			glUseProgram(m_Handle);
		}

		inline GLint GetUniformLocation(std::string_view name) const
		{
			return glGetUniformLocation(m_Handle, name.data());
		}
		
		inline void Uniform1f(std::string_view name, float v0) const
		{
			glUniform1f(GetUniformLocation(name), v0);
		}

		inline void Uniform2f(std::string_view name, const glm::vec2& v0) const
		{
			glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void Uniform3f(std::string_view name, const glm::vec3& v0) const
		{
			glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void Uniform4f(std::string_view name, const glm::vec4& v0) const
		{
			glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void Uniform1i(std::string_view name, int v0) const
		{
			glUniform1i(GetUniformLocation(name), v0);
		}

		inline void Uniform2i(std::string_view name, const glm::ivec2& v0) const
		{
			glUniform2iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void Uniform3i(std::string_view name, const glm::ivec3& v0) const
		{
			glUniform3iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void Uniform4i(std::string_view name, const glm::ivec4& v0) const
		{
			glUniform4iv(GetUniformLocation(name), 1, glm::value_ptr(v0));
		}

		inline void UniformMat2f(std::string_view name, const glm::mat2& v0) const
		{
			glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
		}

		inline void UniformMat3f(std::string_view name, const glm::mat3& v0) const
		{
			glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
		}

		inline void UniformMat4f(std::string_view name, const glm::mat4& v0) const
		{
			glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v0));
		}
	public:
		GLuint m_Handle = 0;
	};
}
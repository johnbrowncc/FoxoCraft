#pragma once

#include <string>

#include <glad/gl.h>

namespace FoxoCommons
{
	class Texture2D final
	{
	public:
		Texture2D() = default;
		Texture2D(GLenum minFilter, GLenum magFilter, GLenum wrapMode, GLenum internalFormat, GLsizei width, GLsizei height, bool mipmaps);
		~Texture2D();

		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;

		Texture2D(Texture2D&& other) noexcept;
		Texture2D& operator=(Texture2D&& other) noexcept;

		inline void Bind(GLuint unit) const
		{
			glBindTextureUnit(unit, m_Handle);
		}

		inline void SubImage(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) const
		{
			glTextureSubImage2D(m_Handle, 0, xoffset, yoffset, width, height, format, type, pixels);
		}

		inline void GenerateMipmaps() const
		{
			glGenerateTextureMipmap(m_Handle);
		}

		inline GLuint GetHandle() const
		{
			return m_Handle;
		}
	private:
		GLuint m_Handle = 0;
	};

	class Texture2DArray final
	{
	public:
		Texture2DArray() = default;
		Texture2DArray(GLenum minFilter, GLenum magFilter, GLenum wrapMode, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool mipmaps);
		~Texture2DArray();

		Texture2DArray(const Texture2DArray&) = delete;
		Texture2DArray& operator=(const Texture2DArray&) = delete;

		Texture2DArray(Texture2DArray&& other) noexcept;
		Texture2DArray& operator=(Texture2DArray&& other) noexcept;

		inline void Bind(GLuint unit) const
		{
			glBindTextureUnit(unit, m_Handle);
		}

		inline void SubImage(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, void* pixels) const
		{
			glTextureSubImage3D(m_Handle, 0, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		}

		inline void GenerateMipmaps() const
		{
			glGenerateTextureMipmap(m_Handle);
		}

		inline GLuint GetHandle() const
		{
			return m_Handle;
		}
	private:
		GLuint m_Handle = 0;
	};
}
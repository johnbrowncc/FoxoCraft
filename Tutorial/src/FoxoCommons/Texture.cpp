#include "Texture.h"

#include <memory>

#include <glm/glm.hpp>

namespace FoxoCommons
{
	Texture2D::Texture2D(GLenum minFilter, GLenum magFilter, GLenum wrapMode, GLenum internalFormat, GLsizei width, GLsizei height, bool mipmaps)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		int maxLevel = 0;

		if (mipmaps)
		{
			maxLevel = static_cast<int>(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))));
			float aniso = 0.0f;

			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			glTextureParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		}

		glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, maxLevel);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, magFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_R, wrapMode);

		glTextureStorage2D(m_Handle, maxLevel + 1, internalFormat, width, height);
	}

	Texture2D::~Texture2D()
	{
		if(m_Handle != 0) glDeleteTextures(1, &m_Handle);
	}

	Texture2D::Texture2D(Texture2D&& other) noexcept
	{
		Texture2D::operator=(std::move(other));
	}

	Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
	{
		Texture2D::~Texture2D();
		m_Handle = other.m_Handle;
		other.m_Handle = 0;
		return *this;
	}

	Texture2DArray::Texture2DArray(GLenum minFilter, GLenum magFilter, GLenum wrapMode, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, bool mipmaps)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_Handle);

		int maxLevel = 0;

		if (mipmaps)
		{
			maxLevel = static_cast<int>(glm::floor(glm::log2(static_cast<float>(glm::max(width, height)))));
			float aniso = 0.0f;

			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			glTextureParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		}

		glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, maxLevel);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, magFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_R, wrapMode);

		glTextureStorage3D(m_Handle, maxLevel + 1, internalFormat, width, height, depth);
	}

	Texture2DArray::~Texture2DArray()
	{
		if (m_Handle != 0) glDeleteTextures(1, &m_Handle);
	}

	Texture2DArray::Texture2DArray(Texture2DArray&& other) noexcept
	{
		Texture2DArray::operator=(std::move(other));
	}

	Texture2DArray& Texture2DArray::operator=(Texture2DArray&& other) noexcept
	{
		Texture2DArray::~Texture2DArray();
		m_Handle = other.m_Handle;
		other.m_Handle = 0;
		return *this;
	}
}
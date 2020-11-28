#include "pch/pch.h"

#include "openglrendertarget.h"

bool COpenGLRenderTarget::Setup(unsigned width, unsigned height, Renderer::TextureUsage usage)
{
	GLenum format = GL_NONE, attachment = GL_NONE;

	switch (usage) {
	case Renderer::TEXTURE_RENDERTARGET:
		format = GL_RGBA8;
		attachment = GL_COLOR_ATTACHMENT0;
		break;
	case Renderer::TEXTURE_DEPTHSTENCIL:
	case Renderer::TEXTURE_DEPTHSTENCIL_READONLY:
		format = GL_R32F;
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	default:
		global_log.Error("OpenGL: Unhandled TextureUsage case for RenderTarget generation!");
		break;
	}

	glCreateBuffers(1, &this->bo);
	glNamedBufferStorage(this->bo, 4*width*height, nullptr, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &this->texid);
	glTextureBuffer(this->texid, format, this->bo);

	this->view = reinterpret_cast< void * >(new GLuint);
	glCreateFramebuffers(1, reinterpret_cast< GLuint * >(this->view));

	glNamedFramebufferTexture(reinterpret_cast< GLuint * >(this->view)[0], attachment, this->texid, 0);

	return true;
}

void COpenGLRenderTarget::Release()
{
	glDeleteFramebuffers(1, reinterpret_cast< GLuint * >(this->view));
	delete reinterpret_cast< GLuint * >(this->view);

	glDeleteTextures(1, &this->texid);
	glDeleteBuffers(1, &this->bo);
}

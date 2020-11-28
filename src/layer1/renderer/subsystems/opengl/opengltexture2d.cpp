#include "pch/pch.h"

#include "opengltexture2d.h"

static const GLenum gladdressmode[] = {
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER
};

static const GLenum glcomparisonfunc[] = {
	GL_NEVER,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_LESS,
	GL_LEQUAL,
	GL_GREATER,
	GL_GEQUAL,
	GL_ALWAYS
};

bool COpenGLTexture2D::SetData(unsigned char *data, unsigned width, unsigned height, Renderer::SamplerStateDesc samplerstatedesc)
{
/*
	glCreateBuffers(1, &this->bo);
	glNamedBufferStorage(this->bo, 4*width*height, data, 0);
*/
	this->buffer = reinterpret_cast< void * >(new GLuint);
	glCreateTextures(GL_TEXTURE_2D, 1, reinterpret_cast< GLuint * >(this->buffer));
	//glTextureBuffer(reinterpret_cast< GLuint * >(this->buffer)[0], GL_RGBA8, this->bo);
	glTextureStorage2D(reinterpret_cast< GLuint * >(this->buffer)[0], 1, GL_RGBA8, width, height);
	glTextureSubImage2D(reinterpret_cast< GLuint * >(this->buffer)[0], 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	GLenum min_filter, mag_filter;

	switch (samplerstatedesc.Filter) {
	case Renderer::FILTER_DEFAULT:
	case Renderer::FILTER_NEAREST:
		min_filter = GL_NEAREST_MIPMAP_NEAREST;
		mag_filter = GL_NEAREST;
		break;
	case Renderer::FILTER_BILINEAR:
		min_filter = GL_LINEAR_MIPMAP_NEAREST;
		mag_filter = GL_LINEAR;
		break;
	case Renderer::FILTER_TRILINEAR:
	case Renderer::FILTER_ANISOTROPIC:
		min_filter = GL_LINEAR_MIPMAP_LINEAR;
		mag_filter = GL_LINEAR;
		break;
	default:
		min_filter = GL_NEAREST_MIPMAP_NEAREST;
		mag_filter = GL_NEAREST;
		break;
	}

	if (samplerstatedesc.MaxAniso < 1)
		samplerstatedesc.MaxAniso = 1;

	GLuint *psampler = new GLuint;
	glCreateSamplers(1, psampler);
	glSamplerParameteri(psampler[0], GL_TEXTURE_WRAP_S, gladdressmode[samplerstatedesc.AddressModeU]);
	glSamplerParameteri(psampler[0], GL_TEXTURE_WRAP_T, gladdressmode[samplerstatedesc.AddressModeV]);
	glSamplerParameteri(psampler[0], GL_TEXTURE_WRAP_R, gladdressmode[samplerstatedesc.AddressModeW]);
	glSamplerParameteri(psampler[0], GL_TEXTURE_COMPARE_FUNC, glcomparisonfunc[samplerstatedesc.ComparisonFunc]);
	glSamplerParameteri(psampler[0], GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(psampler[0], GL_TEXTURE_MAG_FILTER, mag_filter);
	glSamplerParameterf(psampler[0], GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerstatedesc.MaxAniso);
	glSamplerParameterf(psampler[0], GL_TEXTURE_LOD_BIAS, samplerstatedesc.MipLODBias);
	glSamplerParameterf(psampler[0], GL_TEXTURE_MIN_LOD, samplerstatedesc.MinLOD);
	glSamplerParameterf(psampler[0], GL_TEXTURE_MAX_LOD, samplerstatedesc.MaxLOD);

	this->sampler = psampler;

	return true;
}

void COpenGLTexture2D::Release()
{
	glDeleteSamplers(1, reinterpret_cast< GLuint * >(this->sampler));
	delete reinterpret_cast< GLuint * >(this->sampler);

	glDeleteTextures(1, reinterpret_cast< GLuint * >(this->buffer));
	delete reinterpret_cast< GLuint * >(this->buffer);

	//glDeleteBuffers(1, &this->bo);
}

/*******************************************************************************
 * Framebuffer.cpp                                                             *
 * Copyright (C) 2010 Cyril Bazhenov                                           *
 *******************************************************************************
 * This library is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation; either version 2.1 of the License, or (at     *
 * your option) any later version.                                             *
 ******************************************************************************/

#include <Framebuffer.h>

#include <GL/glew.h>

#include <stdio.h>
#define logPrint printf

namespace framework
{

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &_fbo);
}

Framebuffer::~Framebuffer()
{
	if (_fbo)
		glDeleteFramebuffers(1, &_fbo);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::setRenderTargets(unsigned int flag)
{
#define ADD_RT(N) \
	if (flag & RTF_COLOR##N) { \
		index = N + 1; \
		rt[N] = GL_COLOR_ATTACHMENT##N; \
	}
	GLuint rt[8];
	size_t index = 0;

	ADD_RT(0);
	ADD_RT(1);
	ADD_RT(2);
	ADD_RT(3);
	ADD_RT(4);
	ADD_RT(5);
	ADD_RT(6);
	ADD_RT(7);

	if (index > 1) {
		glDrawBuffers(index, rt);
	}
#undef ADD_RT
}

Texture* Framebuffer::createTexture(GLuint type, unsigned int width, unsigned int height,
									GLuint format,
									GLuint internalFormat,
									GLuint byteType)
{
	Texture* ret = new Texture(type);
	ret->create2D(NULL, width, height, format, internalFormat, byteType);
	return ret;
}

void Framebuffer::attach(Texture *texture, GLuint attachment)
{
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getID(), 0);
}

void Framebuffer::attachLayer(Texture *texture, GLenum attachment, GLint layer)
{
	glFramebufferTextureLayer( GL_FRAMEBUFFER, attachment, texture->getID(), 0, layer );
}

RenderBuffer* Framebuffer::createRenderBuffer(GLuint type, unsigned int width, unsigned int height)
{
	RenderBuffer* ret = new RenderBuffer(type, width, height);
	return ret;
}

void Framebuffer::attach(RenderBuffer *buffer, GLuint attacment)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attacment, GL_RENDERBUFFER, buffer->getID());
}

bool Framebuffer::checkStatus()
{
	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch ( status )
	{
		case GL_FRAMEBUFFER_COMPLETE :
			return true;

		case GL_FRAMEBUFFER_UNDEFINED :
			logPrint( "Framebuffer: undefined\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
			logPrint( "Framebuffer incomplete: Attachment is not complete\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
			logPrint( "Framebuffer incomplete: No image is attached to FBO\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :
			logPrint( "Framebuffer incomplete: Draw buffer\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER :
			logPrint( "Framebuffer incomplete: Read buffer\n" );
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED :
			logPrint( "Unsupported by FBO implementation\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE :
			logPrint( "Framebuffer incomplete: Multisample\n" );
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :
			logPrint( "Framebuffer incomplete: Layer targets\n" );
			break;
	};
	return false;
}

}

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
	ret->bind();
	ret->copyData(NULL, width, height, format, internalFormat, byteType);
	ret->unbind();
	return ret;
}

void Framebuffer::attach(Texture *texture, GLuint attachment)
{
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getID(), 0);
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

}

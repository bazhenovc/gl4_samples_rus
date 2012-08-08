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
{}

Framebuffer::~Framebuffer()
{}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::create(int w, int h)
{
	// Create FBO
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	// Create depth renderbuffer
	glGenRenderbuffers(1, &_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
	                          GL_DEPTH_ATTACHMENT,
	                          GL_RENDERBUFFER,
	                          _depthRenderbuffer);

	// Create color texture
	glGenTextures(1, &_colorAttachment);
	glBindTexture(GL_TEXTURE_2D, _colorAttachment);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                       GL_TEXTURE_2D, _colorAttachment,
	                       0);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		logPrint("Failed to create FBO :(");
	}

	glViewport(0, 0, w, h);
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


}

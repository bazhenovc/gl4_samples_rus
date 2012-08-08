/*******************************************************************************
 * Framebuffer.h                                                               *
 * Copyright (C) 2010 Cyril Bazhenov                                           *
 *******************************************************************************
 * This library is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation; either version 2.1 of the License, or (at     *
 * your option) any later version.                                             *
 ******************************************************************************/

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h>

#include "Texture.h"
#include "RenderBuffer.h"

#include <list>

namespace framework
{

enum RenderTargetFlag
{
	RTF_COLOR0 = (1 << 0),
	RTF_COLOR1 = (1 << 1),
	RTF_COLOR2 = (1 << 2),
	RTF_COLOR3 = (1 << 3),
	RTF_COLOR4 = (1 << 4),
	RTF_COLOR5 = (1 << 5),
	RTF_COLOR6 = (1 << 6),
	RTF_COLOR7 = (1 << 7)
};

/**
 *	Framebuffer object
*/
class Framebuffer
{
public:

	Framebuffer();
	~Framebuffer();

	/**
	 *	Get color attachment
	*/
	inline GLuint colorAttachment() const {
		return _colorAttachment;
	}

	/**
	 * Bind framebuffer
	*/
	void bind();

	/**
	 * Unbind framebuffer
	*/
	void unbind();

	/**
		Set render targets
	*/
	void setRenderTargets(unsigned int flag);

	/**
		Create texture for attaching.
	*/
	Texture* createTexture(GLuint type, unsigned int width, unsigned int height,
						   GLuint format = GL_RGBA,
						   GLuint internalFormat = GL_RGBA,
						   GLuint byteType = GL_UNSIGNED_BYTE);

	/**
		Attach a texture as a render target
	*/
	void attach(Texture* texture, GLuint attachment);

	/**
		Create renderbuffer for attaching
	*/
	RenderBuffer* createRenderBuffer(GLuint type, unsigned int width, unsigned int height);

	/**
		Attach a renderbuffer as a render target
	*/
	void attach(RenderBuffer* buffer, GLuint attacment);

private:
	//! fbo ID
	GLuint _fbo;

	//! Depth renderbuffer ID
	GLuint _depthRenderbuffer;

	//! Color texture ID
	GLuint _colorAttachment;
};

}
#endif

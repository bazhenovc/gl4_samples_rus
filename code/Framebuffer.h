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

namespace framework
{

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
	 * Create FBO
	*/
	void create(int w, int h);

	/**
	 * Bind framebuffer
	*/
	void bind();

	/**
	 * Unbind framebuffer
	*/
	void unbind();

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

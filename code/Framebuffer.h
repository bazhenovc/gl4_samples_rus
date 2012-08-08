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

	/**
		Set render targets
	*/
	void setRenderTargets(unsigned int flag);

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

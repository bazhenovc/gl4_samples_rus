
#include "RenderBuffer.h"

namespace framework
{

RenderBuffer::RenderBuffer(GLuint t, unsigned int w, unsigned int h)
	: type(t), width(w), height(h)
{
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &id);
}

}

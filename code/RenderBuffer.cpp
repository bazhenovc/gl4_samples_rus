
#include "RenderBuffer.h"

namespace framework
{

RenderBuffer::RenderBuffer()
	: type(0), width(0), height(0)
{
}

void RenderBuffer::create(GLuint t, unsigned int w, unsigned int h, unsigned int samples)
{
	if ( !id )
		glGenRenderbuffers(1, &id);
	
	type	= t;
	width	= w;
	height	= h;
	glBindRenderbuffer(GL_RENDERBUFFER, id);

	if ( samples > 1 )
		glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
	else
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, type, width, height );
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &id);
}

}

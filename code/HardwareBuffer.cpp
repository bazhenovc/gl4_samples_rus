#include "HardwareBuffer.h"

#include <GL/glew.h>

namespace framework
{

HardwareBuffer::HardwareBuffer(GLuint type)
	: _type(type), _id(0), _size(0), _ptrsize(0)
{
	glGenBuffers(1, &_id);
}

HardwareBuffer::~HardwareBuffer(void)
{
	glDeleteBuffers(1, &_id);
}

void HardwareBuffer::bind(void)
{
	glBindBuffer(_type, _id);
}

void HardwareBuffer::unbind(void)
{
	glBindBuffer(_type, 0);
}

void HardwareBuffer::copyData(const void* data, size_t size, size_t ptrsize, GLuint usage)
{
	_size = size;
	_ptrsize = ptrsize;
	glBufferData(_type, size * ptrsize, data, usage);
}

void HardwareBuffer::copySubData(const void* data, size_t size, size_t ptrsize, size_t offset)
{
	_size = size;
	_ptrsize = ptrsize;
	glBufferSubData(_type, size * ptrsize, offset, data);
}

void* HardwareBuffer::map(BufferMap bm)
{
	return glMapBuffer(_type, bm);
}

bool HardwareBuffer::unmap(void)
{
	return glUnmapBuffer(_type);
}
}

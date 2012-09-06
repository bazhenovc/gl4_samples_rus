#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <GL/glew.h>

namespace framework
{

class RenderBuffer
{
public:

	RenderBuffer();
	~RenderBuffer();

	inline GLuint getID() const
	{
		return id;
	}

	inline GLuint getType() const
	{
		return type;
	}

	inline unsigned int getWidth() const
	{
		return width;
	}

	inline unsigned int getHeight() const
	{
		return height;
	}

	void create(GLuint type, unsigned int width, unsigned int height, unsigned int samples = 0);

private:

	GLuint id;
	GLuint type;
	unsigned int width;
	unsigned int height;
};

}

#endif // RENDERBUFFER_H

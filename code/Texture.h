#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

namespace framework
{

class Texture
{
public:

	Texture(GLuint type);
	~Texture();

	void bind();
	void unbind();

	void bind(int stage);
	void unbind(int stage);

	inline unsigned int getID() const
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

	void copyData(const void* data, unsigned int w, unsigned int h,
				  GLuint format = GL_RGBA,
				  GLuint internalFormat = GL_RGBA,
				  GLuint byteType = GL_UNSIGNED_BYTE);

	bool loadDDS(const char* filename);

private:

	GLuint id;
	GLuint type;
	unsigned int width;
	unsigned int height;
};

}

#endif // TEXTURE_H

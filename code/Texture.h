#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include "HardwareBuffer.h"

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

	void bindImage(int stage, GLenum format, GLenum access, int level = 0, int layer = -1);
	void unbindImage(int stage);

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

	void create2D(const void* data, unsigned int w, unsigned int h,
				  GLuint format = GL_RGBA,
				  GLuint internalFormat = GL_RGBA,
				  GLuint byteType = GL_UNSIGNED_BYTE,
				  unsigned int levels = 0);

	void subData2D(const void* data, unsigned int x, unsigned int y,
				  unsigned int w, unsigned int h,
				  GLuint format = GL_RGBA, GLuint byteType = GL_UNSIGNED_BYTE);

	void create3D(const void* data, unsigned int w, unsigned int h, unsigned int d,
				  GLuint format = GL_RGBA,
				  GLuint internalFormat = GL_RGBA,
				  GLuint byteType = GL_UNSIGNED_BYTE,
				  unsigned int levels = 0);

	void subData3D(const void* data, unsigned int x, unsigned int y, unsigned int z,
				  unsigned int w, unsigned int h, unsigned int d,
				  GLuint format = GL_RGBA, GLuint byteType = GL_UNSIGNED_BYTE);

	void createBuffer(HardwareBuffer *buf, GLenum internalFormat);

	void generateMipmaps();

	void setFilter(GLenum minFilter, GLenum magFilter);
	void setWrap(GLenum s, GLenum t, GLenum r = GL_REPEAT);
	void setAnisotropy(int level);

	bool loadDDS(const char* filename, bool genMipmaps = true);
	bool loadImage(const char* filename, bool genMipmaps = true);

	bool load2DLayerDDS(const char *filename, int layer);
	bool load2DLayerImage(const char *filename, int layer);

private:
	static void _initIL();

private:

	GLuint id;
	GLuint type;
	unsigned int width;
	unsigned int height;
};

}

#endif // TEXTURE_H

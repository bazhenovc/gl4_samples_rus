
#include "Texture.h"

#include "libdds.h"
#include "libdds_opengl.h"

#include "stdio.h"

namespace framework
{

Texture::Texture(GLuint t)
	: type(t)
{
	id = 0;
}

Texture::~Texture()
{
	if (id)
		glDeleteTextures(1, &id);
}

void Texture::bind()
{
	glBindTexture(type, id);
}

void Texture::unbind()
{
	glBindTexture(type, 0);
}

void Texture::bind(int stage)
{
	glBindMultiTextureEXT( stage, type, id );
}

void Texture::unbind(int stage)
{
	glBindMultiTextureEXT( stage, type, 0 );
}

void Texture::copyData(const void *data, unsigned int w, unsigned int h,
					   GLuint format, GLuint internalFormat,
					   GLuint byteType)
{
	if (!id) {
		glGenTextures(1, &id);
	}
	width = w;
	height = h;
	glTexImage2D(type, 0, internalFormat, w, h, 0, format, byteType, data);
}

bool Texture::loadDDS(const char *filename)
{
	DDS_GL_TextureInfo texInfo;
	int err = ddsGL_load(filename, &texInfo);
	if (DDS_OK != err) {
		printf("Failed to load texture: %s\n", filename);
		return false;
	}

	id = texInfo.id;
	width = texInfo.width;
	height = texInfo.height;
	return true;
}

}

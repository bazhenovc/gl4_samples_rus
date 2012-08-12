
#include "Texture.h"

#include "libdds.h"
#include "libdds_opengl.h"
/*
#include <IL/il.h>
#include <IL/ilu.h>
*/
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
	glBindMultiTextureEXT( GL_TEXTURE0 + stage, type, id );
}

void Texture::unbind(int stage)
{
	glBindMultiTextureEXT( GL_TEXTURE0 + stage, type, 0 );
}

void Texture::create2D(const void *data, unsigned int w, unsigned int h,
					   GLuint format, GLuint internalFormat,
					   GLuint byteType)
{
	if (!id) {
		glGenTextures(1, &id);
	}
	width = w;
	height = h;
	
	bind();
	glTexImage2D(type, 0, internalFormat, w, h, 0, format, byteType, data);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	unbind();
}

void Texture::create3D(const void *data, unsigned int w, unsigned int h, unsigned int d,
					   GLuint format, GLuint internalFormat,
					   GLuint byteType)
{
	if (!id) {
		glGenTextures(1, &id);
	}
	width = w;
	height = h;
	
	bind();
	glTexImage3D(type, 0, internalFormat, w, h, d, 0, format, byteType, data);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	unbind();
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

bool Texture::loadImage(const char *filename)
{/*
	static bool ilinit = false;
	if (!ilinit) {
		ilinit = true;
		ilInit();
		iluInit();
	}
	ILuint ilid = 0;

	ilGenImages(1, &ilid);
	ilBindImage(ilid);

	ilLoadImage(filename);

	if (IL_NO_ERROR != ilGetError()) {
		ilDeleteImages(1, &id);
		return false;
	}

	create2D( ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
			  ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE) );

	ilDeleteImages(1, &id);

	return true;*/
	return false;
}

}

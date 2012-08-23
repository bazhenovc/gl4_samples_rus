
#include "Texture.h"

#include "libdds.h"
#include "libdds_opengl.h"
/*
#include <IL/il.h>
#include <IL/ilu.h>
*/
#include "stdio.h"
#include "System.h"

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
	//glActiveTexture( GL_TEXTURE0 + stage );
	//glBindTexture( type, id );
	glBindMultiTextureEXT( GL_TEXTURE0 + stage, type, id );
}

void Texture::unbind(int stage)
{
	//glActiveTexture( GL_TEXTURE0 + stage );
	//glBindTexture( type, 0 );
	glBindMultiTextureEXT( GL_TEXTURE0 + stage, type, 0 );
}

void Texture::bindImage(int stage, GLenum format, GLenum access, int level, int layer)
{
	glBindImageTexture( stage, id, level, layer >= 0, layer >= 0 ? layer : 0, access, format );
}

void Texture::unbindImage(int stage)
{
	glBindImageTexture( stage, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI );
}

void Texture::setFilter(GLenum minFilter, GLenum magFilter)
{
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::setWrap(GLenum s, GLenum t, GLenum r)
{
	glTexParameteri(type, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, t);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, r);
}

void Texture::setAnisotropy(int level)
{
	glTexParameteri(type, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
}

void Texture::create2D(const void *data, unsigned int w, unsigned int h,
					   GLuint format, GLuint internalFormat,
					   GLuint byteType, unsigned int levels)
{
	if (!id) {
		glGenTextures(1, &id);
	}
	width = w;
	height = h;
	
	bind();

	if ( !data )
	{
		//glTexStorage2D( type, levels+1, internalFormat, w, h );
		for (unsigned int i = 0; i <= levels; ++i)
		{
			glTexImage2D( type, i, internalFormat, w, h, 0, format, byteType, NULL );
			w = DDS_MAX( w>>1, 1 );
			h = DDS_MAX( h>>1, 1 );
		}
		setFilter( levels > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR, GL_LINEAR );
	}
	else
	{
		assert( levels > 0 && "levels parameter not supported" );
		glTexImage2D(type, 0, internalFormat, w, h, 0, format, byteType, data);
		setFilter( GL_LINEAR, GL_LINEAR );
	}
	setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	unbind();
}

void Texture::subData2D(const void *data, unsigned int x, unsigned int y,
					   unsigned int w, unsigned int h,
					   GLuint format, GLuint byteType)
{
	bind();
	glTexSubImage2D(type, 0, x, y, w, h, format, byteType, data);
	unbind();
}

void Texture::create3D(const void *data, unsigned int w, unsigned int h, unsigned int d,
					   GLuint format, GLuint internalFormat,
					   GLuint byteType, unsigned int levels)
{
	if (!id) {
		glGenTextures(1, &id);
	}
	width = w;
	height = h;
	
	bind();
	
	if ( !data )
	{
		//glTexStorage3D( type, levels+1, internalFormat, w, h, d );
		for (unsigned int i = 0; i <= levels; ++i)
		{
			glTexImage3D( type, i, internalFormat, w, h, d, 0, format, byteType, NULL );
			w = DDS_MAX( w>>1, 1 );
			h = DDS_MAX( h>>1, 1 );
			if ( type == GL_TEXTURE_3D )	d = DDS_MAX( d>>1, 1 );
		}
		setFilter( levels > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR, GL_LINEAR );
	}
	else
	{
		assert( levels > 0 && "levels parameter not supported" );
		glTexImage3D( type, 0, internalFormat, w, h, d, 0, format, byteType, data );
		setFilter( GL_LINEAR, GL_LINEAR );
	}
	setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	unbind();
}

void Texture::subData3D(const void *data, unsigned int x, unsigned int y, unsigned int z,
					   unsigned int w, unsigned int h, unsigned int d,
					   GLuint format, GLuint byteType)
{
	bind();
	glTexSubImage3D(type, 0, x, y, z, w, h, d, format, byteType, data);
	unbind();
}

void Texture::createBuffer(HardwareBuffer *buf, GLenum internalFormat)
{
	if (!id) {
		glGenTextures(1, &id);
		//bind();
		//unbind();
	}
	glTextureBufferEXT( id, type, internalFormat, buf->getID() );
}

void Texture::generateMipmaps()
{
	glGenerateMipmap( type );
}

bool Texture::loadDDS(const char *filename)
{
	DDS_GL_TextureInfo texInfo;
	System::clearGLErrors();
	int err = ddsGL_loadToGL(filename, &texInfo);
	System::checkGLError();
	if (DDS_OK != err) {
		printf("Failed to load texture: %s\n", filename);
		return false;
	}

	id = texInfo.id;
	width = texInfo.width;
	height = texInfo.height;
	return true;
}

bool Texture::load2DLayerDDS(const char *filename, int layer)
{
	if ( !id )
		return false;

	DDS_GL_TextureInfo texInfo;
	int err = ddsGL_load(filename, &texInfo);
	if (DDS_OK != err) {
		printf("Failed to load texture: %s\n", filename);
		return false;
	}

	bind();

    size_t offset = 0;
    dds_uint mipWidth = texInfo.width,
            mipHeight = texInfo.height,
            mipSize;
    /* Upload mipmaps to video memory */
    size_t mip;
    for (mip = 0; mip < texInfo.num_mipmaps; mip++)
	{
        mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * texInfo.block_size;
		glCompressedTexSubImage3D( type, mip, 0, 0, layer,
					texInfo.width, texInfo.height, 1, texInfo.format,
					mipSize, texInfo.data + offset );

        mipWidth = DDS_MAX (mipWidth >> 1, 1);
        mipHeight = DDS_MAX (mipHeight >> 1, 1);

        offset += mipSize;
    }
    ddsGL_free (&texInfo);

	unbind();

	return true;
}

void Texture::_initIL()
{/*
	static bool ilinit = false;
	if (!ilinit) {
		ilinit = true;
		ilInit();
		iluInit();
	}*/
}

bool Texture::loadImage(const char *filename)
{/*
	_initIL();

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

bool Texture::load2DLayerImage(const char *filename, int layer)
{
	if ( !id )
		return false;
	/*
	_initIL();

	ILuint ilid = 0;

	ilGenImages(1, &ilid);
	ilBindImage(ilid);

	ilLoadImage(filename);

	if (IL_NO_ERROR != ilGetError()) {
		ilDeleteImages(1, &id);
		return false;
	}

	subData3D(	ilGetData(), 0, 0, layer,
				ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1,
				ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE) );

	ilDeleteImages(1, &id);

	return true;*/
	return false;
}

}

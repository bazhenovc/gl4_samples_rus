/*
 * libdds
 * Copyright (c) 2010 Cyril Bazhenov bazhenovc@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include <libdds_opengl.h>
#include <libdds.h>

#include <GL/glew.h>

dds_uint ddsGL_loadToGL (const char* filename, DDS_GL_TextureInfo* texture) {
	dds_uint  err = ddsGL_load( filename, texture );

	if ( err != DDS_OK )
		return err;

    /* Generate new texture */
    glGenTextures (1, &texture->id);
    glBindTexture (GL_TEXTURE_2D, texture->id);
    /* Filtering */
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->num_mipmaps > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    size_t offset = 0;
    dds_uint mipWidth = texture->width,
            mipHeight = texture->height,
            mipSize;
    /* Upload mipmaps to video memory */
    size_t mip;
    for (mip = 0; mip < texture->num_mipmaps; mip++) {
        mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * texture->block_size;

        glCompressedTexImage2D (GL_TEXTURE_2D, mip, texture->format,
                mipWidth, mipHeight, 0, mipSize,
				texture->data + offset);

        mipWidth = DDS_MAX (mipWidth >> 1, 1);
        mipHeight = DDS_MAX (mipHeight >> 1, 1);

        offset += mipSize;
    }
    ddsGL_free (texture);
    return DDS_OK;
}

dds_uint ddsGL_load (const char* filename, DDS_GL_TextureInfo* texture) {
    DDSTextureInfo * textureInfo = new DDSTextureInfo;
    dds_int error = dds_load (filename, textureInfo);
    if (error != DDS_OK) {
        return error;
    }
    texture->width = textureInfo->surface.width;
    texture->height = textureInfo->surface.height;
    texture->num_mipmaps = textureInfo->surface.mip_level;
	texture->id = 0;
    switch (textureInfo->surface.format.fourcc) {
        case DDS_FOURCC_DXT1:
            texture->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            texture->internal_format = 3;
			texture->block_size = 8;
            break;
        case DDS_FOURCC_DXT3:
            texture->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            texture->internal_format = 4;
			texture->block_size = 16;
            break;
        case DDS_FOURCC_DXT5:
            texture->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            texture->internal_format = 4;
			texture->block_size = 16;
            break;
        case DDS_FOURCC_ATI1:
		case DDS_FOURCC_BC4U:
			texture->format = GL_COMPRESSED_RED_RGTC1_EXT;
			texture->internal_format = 1;
			texture->block_size = 8;
			break;
		case DDS_FOURCC_BC4S:
			texture->format = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT;
			texture->internal_format = 1;
			texture->block_size = 8;
			break;
        case DDS_FOURCC_BC5U:
            texture->format = GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
            texture->internal_format = 2;
			texture->block_size = 16;
            break;
        case DDS_FOURCC_BC5S:
            texture->format = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT;
            texture->internal_format = 2;
			texture->block_size = 16;
            break;
        default:
            dds_free (textureInfo);
            return DDS_BAD_COMPRESSION;
    }
	texture->_info = textureInfo;
	texture->data  = textureInfo->texels;
	texture->data_size = textureInfo->buffer_size;
    return DDS_OK;
}

void ddsGL_free (DDS_GL_TextureInfo* texture) {
	if ( texture->_info ) {
		dds_free( (DDSTextureInfo *)texture->_info );
		delete (DDSTextureInfo *)texture->_info;
		texture->_info = 0;
		texture->data = 0;
	}
}

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <gl/glew.h>
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include <string>
#include <assert.h>

namespace framework
{

struct RenderState
{
	enum
	{
		MAX_COLOR_BUFFERS	= 8
	};

	struct ColorBuffer
	{
		GLenum	blendSrcRGB,
				blendSrcA,
				blendDstRGB,
				blendDstA,
				blendModeRGB,
				blendModeA;
		bool	colorMaskR	: 1,
				colorMaskG	: 1,
				colorMaskB	: 1,
				colorMaskA	: 1;
		bool	blending	: 1,
				enabled		: 1;

		ColorBuffer(): blendSrcRGB(GL_ONE), blendSrcA(GL_ONE), blendDstRGB(GL_ZERO), blendDstA(GL_ZERO),
			blendModeRGB(GL_FUNC_ADD), blendModeA(GL_FUNC_ADD), colorMaskR(1), colorMaskG(1),
			colorMaskB(1), colorMaskA(1), blending(0), enabled(0) {}
	};

	struct Stencil
	{
		GLenum			sFail,
						dFail,
						dPPass,
						func;
		unsigned char	funcRef,
						funcMask,
						mask;
		bool			test;

		Stencil(): sFail(GL_KEEP), dFail(GL_KEEP), dPPass(GL_KEEP), func(GL_ALWAYS), funcRef(0), funcMask(-1), mask(-1), test(false) {}
	};

	struct Depth
	{
		GLenum			func;
		bool			write	: 1,
						test	: 1;

		Depth(): func(GL_LEQUAL), write(true), test(true) {}
	};


	ColorBuffer		colorBuffers[MAX_COLOR_BUFFERS];
	Stencil			stencil;
	Depth			depth;
	GLenum			cullFace,
					frontFace,
					polygonMode;
	glm::vec2		polygonOffsets,
					depthRange;
	bool			depthClamp		: 1,
					dither			: 1,
					polygonOffset	: 1,
					cubeMapSeamless	: 1,
					smooth			: 1,
					framebufferSRGB	: 1,
					culling			: 1;

	RenderState(): cullFace(GL_BACK), frontFace(GL_CCW), polygonMode(GL_FILL), polygonOffsets(0.f), smooth(0),
		depthClamp(1), dither(0), polygonOffset(0), cubeMapSeamless(0), culling(0), framebufferSRGB(0),
		depthRange(0.f,1.f)
	{
		colorBuffers[0].enabled = true;
	}

	void apply();
};


class Material
{
private:
	struct MtrTexture
	{
		Texture *	texture;
		std::string	uniform;
		int			stage;

		MtrTexture(): texture(NULL), uniform(), stage(-1) {}
		MtrTexture(Texture *t, const char *u, int s): texture(t), uniform(u), stage(s) {}
	};

	std::vector<MtrTexture>	_textures;
	std::vector<char>		_uboData;
	std::string				_uboName;
	GLuint					_ubo;
	size_t					_uboSize;
	bool					_uboChanged;

public:
	Material();
	~Material();

	void apply(Shader *shader, GLint uboBindingIndex = 0);
	void createUB(const char *name, size_t size, const void *data = NULL);

	bool addTexture(Texture *tex, const char *uniform, int stage = -1);
	bool addTexture(const char *filename, const char *uniform, int stage, GLenum type = GL_TEXTURE_2D);

	template <typename T>
	T & getData()
	{
		assert( sizeof(T) == _uboSize );
		_uboChanged = true;
		return *(T*)(&_uboData[0]);
	}


	// helper for copy data to UBO
	static size_t ub_alignedCopy(void *dst, const void *src, size_t lineSize, size_t numLines, size_t alignPow)
	{
		size_t	size	= lineSize & (1<<(alignPow-1));
		size_t	aligned = size == 0 ? lineSize : lineSize + ((1<<alignPow) - size);
		size = 0;

		for (size_t i = 0; i < numLines; ++i)
		{
			memcpy( ((char *)dst) + size, src, lineSize );
			size += aligned;
			src   = ((const char *)src) + lineSize;
		}
		return size;
	}
};

}

#endif	// _MATERIAL_H_
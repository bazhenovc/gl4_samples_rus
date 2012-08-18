#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <gl/glew.h>
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include <string>

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
	glm::vec2		polygonOffsets;
	bool			depthClamp		: 1,
					dither			: 1,
					polygonOffset	: 1,
					cubeMapSeamless	: 1,
					smooth			: 1,
					framebufferSRGB	: 1,
					culling			: 1;

	RenderState(): cullFace(GL_BACK), frontFace(GL_CCW), polygonMode(GL_FILL), polygonOffsets(), smooth(0),
		depthClamp(1), dither(0), polygonOffset(0), cubeMapSeamless(0), culling(0), framebufferSRGB(0)
	{
		colorBuffers[0].enabled = true;
	}

	void apply();
};


class Material
{
public:
	struct Data
	{
		glm::vec3	diffuseColor,
					ambientColor,
					specularColor;

		Data(): diffuseColor(1.f), ambientColor(1.f), specularColor(1.f) {}
	};

private:
	struct MtrTexture
	{
		Texture *	texture;
		std::string	uniform;
		int			stage;

		MtrTexture(): texture(NULL), unifomr(), stage(-1) {}
		MtrTexture(Texture *t, const char *u, int s): texture(t), uniform(u), stage(s) {}
	};

	Data	_data;

public:
	Material();
	~Material();

	void apply(Shader *shader);

	bool addTexture(Texture *tex, const char *uniform, int stage);
	bool addTexture(const char *filename, const char *uniform, int stage);

	Data &			getData()			{ return _data; }
	Data const &	getData()	const	{ return _data; }
};

}

#endif	// _MATERIAL_H_
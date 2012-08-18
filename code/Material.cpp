#include "Material.h"
#include <map>

namespace framework
{
	
void RenderState::apply()
{
	for (int i = 0; i < MAX_COLOR_BUFFERS; ++i)
	{
		ColorBuffer const &		cb = colorBuffers[i];

		if ( !cb.enabled )
			continue;

		glColorMaski( i, cb.colorMaskR, cb.colorMaskG, cb.colorMaskB, cb.colorMaskA );

		if ( cb.blending )
		{
			glEnablei( i, GL_BLEND);
			glBlendEquationSeparatei( i, cb.blendModeRGB, cb.blendModeA );
			glBlendFuncSeparatei( i, cb.blendSrcRGB, cb.blendSrcA, cb.blendDstRGB, cb.blendDstA );
		}
		else
			glDisable(GL_BLEND);
	}


	if ( culling )
	{
		glEnable(GL_CULL_FACE);
		glCullFace(cullFace);
		glFrontFace(frontFace);
	}
	else
		glDisable(GL_CULL_FACE);


	glDepthMask( depth.write );

	if ( depth.test ) {
		glDepthFunc( depth.func );
		glEnable(GL_DEPTH_TEST);
	}
	else
		glDisable(GL_DEPTH_TEST);


	if ( stencil.test ) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc( GL_FRONT_AND_BACK, stencil.func, stencil.funcRef, stencil.funcMask );
		glStencilMask( GL_FRONT_AND_BACK, stencil.mask );
	}
	else
		glDisable(GL_STENCIL_TEST);

	switch ( polygonMode )
	{
		case GL_FILL :
			polygonOffset ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
			smooth ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
			break;

		case GL_LINE :
			polygonOffset ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
			smooth ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
			break;

		case GL_POINT :
			smooth ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
			break;
	};

	if ( polygonOffset )
		glPolygonOffset( polygonOffsets.x, polygonOffsets.y );

	depthClamp ?		glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
	dither ?			glEnable(GL_DITHER) : glDisable(GL_DITHER);
	cubeMapSeamless ?	glEnable(GL_CUBE_MAP_SEAMLESS) : glDisable(GL_CUBE_MAP_SEAMLESS);
}


class TextureManager
{
private:
	typedef std::map< std::string, Texture *>	texmap_t;
	typedef texmap_t::value_type				value_t;

	texmap_t		_textures;

public:
	~TextureManager()
	{
		// TODO: delete textures
	}

	bool load(const char *filename, Texture *&tex)
	{
		std::string	name(filename);
		// TODO: одинаковое форматирование путей к фалу

		texmap_t::const_iterator	iter = _textures.find( name );

		if ( iter != _textures.end() ) {
			tex = (*iter).second;
			return true;
		}

		tex = new Texture(GL_TEXTURE_2D);

		if ( tex->loadDDS( filename ) ) {
			_textures.insert( value_t( name, tex ) );
			return true;
		}

		return false;
	}
};

static TextureManager	texMngr;


Material::Material()
{
}

Material::~Material()
{
}

void Material::apply(Shader *shader)
{
}

bool Material::addTexture(Texture *tex, const char *uniform, int stage)
{
	_textures.push_back( MtrTexture( tex, uniform ,stage ) );
	return true;
}

bool Material::addTexture(const char *filename, const char *uniform, int stage)
{
	Texture	*	tex = NULL;

	if ( texMngr.load( filename, tex ) )
	{
		_textures.push_back( MtrTexture( tex, uniform, stage ) );
		return true;
	}
	return false;
}

}
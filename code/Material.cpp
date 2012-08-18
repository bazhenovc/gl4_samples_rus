#include "Material.h"
#include <map>
#include <algorithm>

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
		glStencilFunc( stencil.func, stencil.funcRef, stencil.funcMask );
		glStencilMask( stencil.mask );
		glStencilOp( stencil.sFail, stencil.dFail, stencil.dPPass );
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

	glPolygonMode( GL_FRONT_AND_BACK, polygonMode );
	glDepthRangef( depthRange.x, depthRange.y );

	depthClamp ?		glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
	dither ?			glEnable(GL_DITHER) : glDisable(GL_DITHER);
	cubeMapSeamless ?	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
		struct FreeTexture {
			void operator () (value_t &val) const {
				delete val.second;
				val.second = NULL;
			}
		};

		std::for_each( _textures.begin(), _textures.end(), FreeTexture() );
	}

	bool load(const char *filename, Texture *&tex, GLenum type)
	{
		std::string	name(filename);

		texmap_t::const_iterator	iter = _textures.find( name );

		if ( iter != _textures.end() ) {
			tex = (*iter).second;
			return true;
		}

		tex = new Texture(type);

		if ( tex->loadDDS( filename ) ) {
			_textures.insert( value_t( name, tex ) );
			return true;
		}

		return false;
	}
};

static TextureManager	texMngr;


Material::Material(): _ubo(0), _uboSize(0), _uboChanged(false)
{
}

Material::~Material()
{
	if ( _ubo ) {
		glDeleteBuffers( 1, &_ubo );
		_ubo = 0;
	}
}

void Material::apply(Shader *shader, GLint uboBindingIndex)
{
	if ( _uboChanged )
	{
		glBindBuffer( GL_UNIFORM_BUFFER, _ubo );
		glBufferData( GL_UNIFORM_BUFFER, _uboSize, &_uboData[0], GL_STREAM_DRAW );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	}

	shader->bindUB( _uboName.c_str(), uboBindingIndex, _ubo );

	for (size_t i = 0; i < _textures.size(); ++i)
	{
		_textures[i].texture->bind( _textures[i].stage );
		shader->setUniformInt( _textures[i].uniform.c_str(), _textures[i].stage );
	}
}

void Material::createUB(const char *name, size_t size, const void *data)
{
	if ( !_ubo ) {
		glGenBuffers( 1, &_ubo );
	}

	_uboSize	= size;
	_uboData.resize( size );
	_uboName	= name;
	_uboChanged	= true;

	if ( data ) {
		memcpy( &_uboData[0], data, size );
	}
}

bool Material::addTexture(Texture *tex, const char *uniform, int stage)
{
	if ( stage == -1 ) {
		stage = _textures.size();
	}

	_textures.push_back( MtrTexture( tex, uniform, stage ) );
	return true;
}

bool Material::addTexture(const char *filename, const char *uniform, int stage, GLenum type)
{
	Texture	*	tex = NULL;

	if ( texMngr.load( filename, tex, type ) )
	{
		return addTexture( tex, uniform, stage );
	}
	return false;
}

}
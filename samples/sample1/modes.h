enum EViewIndex
{
	VIEW_COLOR	= 0,
	VIEW_NORMAL,
	VIEW_TESS,
};


class View
{
private:
	Shader		*	_viewColor,
				*	_viewNormal,
				*	_viewTess;
	Texture		*	_colorTarget,
				*	_normalTarget,
				*	_depthTarget;
	Framebuffer *	_fbo;

public:
	View():
		_viewColor(NULL), _viewNormal(NULL), _viewTess(NULL),
		_colorTarget(NULL), _normalTarget(NULL), _fbo(NULL), _depthTarget(NULL)
	{}

	~View()
	{
		delete _viewColor;
		delete _viewNormal;
		delete _viewTess;
		delete _colorTarget;
		delete _normalTarget;
		delete _depthTarget;
		delete _fbo;
	}

	void init()
	{
		program->load( _viewColor,	"shaders/view_color.prg" );
		program->load( _viewNormal,	"shaders/view_normal.prg" );
		program->load( _viewTess,	"shaders/view_tesslevel.prg" );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );
		
		if ( !_normalTarget )
			_normalTarget = new Texture( GL_TEXTURE_2D );
		
		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_fbo )
			_fbo = new Framebuffer();
		
		_colorTarget->bind();
		_colorTarget->copyData(  NULL, scrWidth, scrHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
		_normalTarget->bind();
		_normalTarget->copyData( NULL, scrWidth, scrHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
		_depthTarget->bind();
		_depthTarget->copyData(  NULL, scrWidth, scrHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT );
		_depthTarget->unbind();

		_fbo->bind();
		_fbo->attach( _colorTarget, GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _normalTarget, GL_COLOR_ATTACHMENT1 );
		_fbo->attach( _depthTarget, GL_DEPTH_ATTACHMENT );
		_fbo->unbind();
	}

	void bind()
	{
		_fbo->bind();
		_fbo->setRenderTargets( RTF_COLOR0 | RTF_COLOR1 );
		glViewport( 0, 0, scrWidth, scrHeight );
	}

	void unbind()
	{
		_fbo->unbind();
	}

	void draw(int i)
	{
		Shader *	shader = NULL;

		if ( i == VIEW_COLOR )		shader = _viewColor;
		if ( i == VIEW_NORMAL )		shader = _viewNormal;
		if ( i == VIEW_TESS )		shader = _viewTess;

		program->bind( shader );

		_colorTarget->bind(  TEX_DIFFUSE );
		_normalTarget->bind( TEX_NORMAL );
		_depthTarget->bind(  TEX_DEPTH );

		fullScreenQuad->draw();
		
		_depthTarget->unbind(  TEX_DEPTH );
		_normalTarget->unbind( TEX_NORMAL );
		_colorTarget->unbind(  TEX_DIFFUSE );
	}
};


class Mode
{
public:
	virtual ~Mode() {}
	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void draw(int) = 0;
};


class Part1 : public Mode
{
private:
	Shader *	_shader;

public:
	Part1(): _shader(NULL)
	{}

	~Part1()
	{
		unload();
	}

	void load()
	{
		program->load( _shader, "shaders/tess_quad_disp.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );
	}

	void unload()
	{
		delete _shader;		_shader = NULL;
	}

	void draw(int)
	{
		program->bind( _shader );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );

		gridMesh->draw();
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_shaderTessTri,
			*	_shaderTessQuad;
	int			_patchSize;

public:
	Part2(): _shaderTessTri(NULL), _shaderTessQuad(NULL), _patchSize(0)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		program->load( _shaderTessTri,  "shaders/tess_tri_rnd_level.prg" );
		program->load( _shaderTessQuad, "shaders/tess_quad_rnd_level.prg" );
	}

	void unload()
	{
		delete _shaderTessTri;		_shaderTessTri = NULL;
		delete _shaderTessQuad;		_shaderTessQuad = NULL;
		_patchSize = 0;
	}

	void draw(int i)
	{
		// i:	0 - triangles,		2 - quads
		//		1 - correct mode,	3 - incorrect mode
		Shader *	shader	= (i & 2) ? _shaderTessQuad : _shaderTessTri;
		bool		regenerate	= false;

		if ( shader == _shaderTessQuad && _patchSize != 4 ) {
			_patchSize = 4;
			regenerate = true;
		}
		if ( shader == _shaderTessTri && _patchSize != 3 ) {
			_patchSize = 3;
			regenerate = true;
		}
		if ( regenerate ) {
			gridMesh->createGrid( gridSize, 1.f / float(gridSize), _patchSize );
		}
		
		program->bind( shader );
		shader->setUniformInt( shader->getLoc("unIncorrectMode"), !!(i & 1) );
		
		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );

		gridMesh->draw();
	}
};


class Part3 : public Mode
{
private:
	Shader		*	_tessShader,
				*	_genShader;
	Framebuffer *	_fbo;
	Texture		*	_renderTarget;

public:
	Part3(): _tessShader(NULL), _genShader(NULL), _fbo(NULL), _renderTarget(NULL)
	{}

	~Part3()
	{
		unload();
	}

	void load()
	{
		program->load( _tessShader, "shaders/tess_quad_level_tex.prg" );
		program->load( _genShader,  "shaders/gen_normal_and_tesslvl.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );

		if ( !_fbo )
			_fbo = new Framebuffer();
		
		if ( !_renderTarget )
			_renderTarget = new Texture( GL_TEXTURE_2D );

		_renderTarget->bind();
		_renderTarget->copyData( NULL, heightMap->getWidth(), heightMap->getHeight(),
								 GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );
		_renderTarget->unbind();

		_fbo->bind();
		_fbo->attach( _renderTarget, GL_COLOR_ATTACHMENT0 );
		_fbo->unbind();
	}

	void unload()
	{
		delete _tessShader;		_tessShader = NULL;
		delete _genShader;		_genShader = NULL;
		delete _renderTarget;	_renderTarget = NULL;
		delete _fbo;			_fbo = NULL;
	}

	void draw(int i)
	{
		if ( i == 0 )
		{
			program->bind( _tessShader );
			
			diffuseMap->bind( TEX_DIFFUSE );
			heightMap->bind(  TEX_HEIGHT );
			_renderTarget->bind( TEX_NORMAL );

			gridMesh->draw();

			_renderTarget->unbind( TEX_NORMAL );

		}
		else
		{
			// generate normal and tess level map
			_fbo->bind();
			
			glDepthMask( GL_FALSE );
			glDisable( GL_DEPTH_TEST );

			glDrawBuffer( GL_COLOR_ATTACHMENT0 );
			glViewport( 0, 0, heightMap->getWidth(), heightMap->getHeight() );

			program->bind( _genShader );

			heightMap->bind( TEX_HEIGHT );

			fullScreenQuad->draw();

			_fbo->unbind();

			glDepthMask( GL_TRUE );
			glEnable( GL_DEPTH_TEST );
			glDrawBuffer( GL_BACK );
			glViewport( 0, 0, scrWidth, scrHeight );
		}
	}
};


class Part4 : public Mode
{
private:
	Shader *	_shader;

public:
	Part4(): _shader(NULL)
	{}

	~Part4()
	{
		unload();
	}

	void load()
	{
		program->load( _shader, "shaders/tess_quad_distance_lods.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );
	}

	void unload()
	{
		delete _shader;		_shader = NULL;
	}

	void draw(int)
	{
		program->bind( _shader );
		
		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		
		gridMesh->draw();
	}
};


class Part5 : public Mode
{
private:
	Shader *	_shader;

public:
	Part5(): _shader(NULL)
	{}

	~Part5()
	{
		unload();
	}

	void load()
	{
		program->load( _shader, "shaders/tess_tri_screenspace_lods.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 3 );
	}

	void unload()
	{
		delete _shader;		_shader = NULL;
	}

	void draw(int)
	{
		program->bind( _shader );
		
		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		
		gridMesh->draw();
	}
};


class Part6 : public Mode
{
private:
	Shader	*	_shaderDist,
			*	_shaderScreenSpace;

public:
	Part6(): _shaderDist(NULL), _shaderScreenSpace(NULL)
	{}

	~Part6()
	{
		unload();
	}

	void load()
	{
		program->load( _shaderDist, "shaders/tess_quad_distance_lods_culling.prg" );
		program->load( _shaderScreenSpace, "shaders/tess_quad_screenspace_lods_culling.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );
	}

	void unload()
	{
		delete _shaderDist;				_shaderDist = NULL;
		delete _shaderScreenSpace;		_shaderScreenSpace = NULL;
	}

	void draw(int i)
	{
		Shader *	shader = (i & 1) ? _shaderDist : _shaderScreenSpace;

		program->bind( shader );
		
		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		
		gridMesh->draw();
	}
};
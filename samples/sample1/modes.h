enum EViewIndex
{
	VIEW_COLOR	= 0,
	VIEW_NORMAL,
	VIEW_TESS,
	VIEW_COLOR_MIX_TESS,
	VIEW_FOG,
};


class View
{
private:
	Shader		*	_viewColor,
				*	_viewNormal,
				*	_viewTess,
				*	_viewColorMixTess,
				*	_viewFog;
	Texture		*	_colorTarget,
				*	_normalTarget,
				*	_depthTarget;
	Framebuffer *	_fbo;
	glm::mat4		_ortho;

public:
	View():
		_viewColor(NULL), _viewNormal(NULL), _viewTess(NULL), _viewColorMixTess(NULL),
		_colorTarget(NULL), _normalTarget(NULL), _depthTarget(NULL), _fbo(NULL), _viewFog(NULL)
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
		delete _viewColorMixTess;
		delete _viewFog;
	}

	void init()
	{
		_ortho = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );

		program->load( _viewColor,	"shaders/view_color.prg" );
		program->load( _viewNormal,	"shaders/view_normal.prg" );
		program->load( _viewTess,	"shaders/view_tesslevel.prg" );
		program->load( _viewColorMixTess, "shaders/view_color_mix_tesslvl.prg" );
		program->load( _viewFog,		"shaders/view_color_fog.prg" );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_normalTarget )
			_normalTarget = new Texture( GL_TEXTURE_2D );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_fbo )
			_fbo = new Framebuffer();

		_colorTarget->create2D(  NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_normalTarget->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_depthTarget->create2D(  NULL, sys.getWndSize().x, sys.getWndSize().y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT );

		_fbo->bind();
		_fbo->attach( _colorTarget, GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _normalTarget, GL_COLOR_ATTACHMENT1 );
		_fbo->attach( _depthTarget, GL_DEPTH_ATTACHMENT );
		_fbo->checkStatus();
		_fbo->unbind();
	}

	void bind()
	{
		_fbo->bind();
		_fbo->setRenderTargets( RTF_COLOR0 | RTF_COLOR1 );
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().y );

		const float		depthClear = 1.0f;
		const glm::vec4	colorClear = glm::vec4( 0.f, 0.8f, 1.0f, 1.f );
		const glm::vec4	normalClear = glm::vec4( 1.f, 1.f, 1.f, 1.f );

		glClearBufferfv( GL_DEPTH, 0, &depthClear );
		glClearBufferfv( GL_COLOR, 0, glm::value_ptr(colorClear) );
		glClearBufferfv( GL_COLOR, 1, glm::value_ptr(normalClear) );
	}

	void unbind()
	{
		_fbo->unbind();
	}

	void draw(int i)
	{
		Shader *	shader = NULL;

		if ( i == VIEW_COLOR )			shader = _viewColor;		else
		if ( i == VIEW_NORMAL )			shader = _viewNormal;		else
		if ( i == VIEW_TESS )			shader = _viewTess;			else
		if ( i == VIEW_COLOR_MIX_TESS )	shader = _viewColorMixTess;	else
		if ( i == VIEW_FOG )			shader = _viewFog;			else
										return;

		program->getStates().mvp = _ortho;

		program->bind( shader );

		glDisable(GL_DEPTH_TEST);
		_colorTarget->bind(  TEX_DIFFUSE );
		_normalTarget->bind( TEX_NORMAL );
		_depthTarget->bind(  TEX_DEPTH );

		fullScreenQuad->draw();

		_depthTarget->unbind(  TEX_DEPTH );
		_normalTarget->unbind( TEX_NORMAL );
		_colorTarget->unbind(  TEX_DIFFUSE );
		glEnable(GL_DEPTH_TEST);
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
		normalMap->bind(  TEX_NORMAL );

		gridMesh->draw();
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_shader;

public:
	Part2(): _shader(NULL)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		program->load( _shader,  "shaders/tess_tri_rnd_level.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 3 );
	}

	void unload()
	{
		delete _shader;		_shader = NULL;
	}

	void draw(int i)
	{
		program->bind( _shader );
		_shader->setUniformInt( _shader->getLoc("unIncorrectMode"), !!(i & 1) );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		normalMap->bind(  TEX_NORMAL );

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
		program->load( _genShader,  "shaders/gen_tesslvl.prg" );
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );

		if ( !_fbo )
			_fbo = new Framebuffer();

		if ( !_renderTarget )
			_renderTarget = new Texture( GL_TEXTURE_2D );

		_renderTarget->bind();
		_renderTarget->create2D( NULL, gridSize+1, gridSize+1,
								 GL_RED, GL_R8, GL_UNSIGNED_BYTE );
		_renderTarget->unbind();

		_fbo->bind();
		_fbo->attach( _renderTarget, GL_COLOR_ATTACHMENT0 );

		// generate normal and tess level map
		glDepthMask( GL_FALSE );
		glDisable( GL_DEPTH_TEST );

		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		glViewport( 0, 0, _renderTarget->getWidth(), _renderTarget->getHeight() );
		glClear(GL_COLOR_BUFFER_BIT);

		program->getStates().mvp = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );
		program->bind( _genShader );

		heightMap->bind( TEX_HEIGHT );

		fullScreenQuad->draw();

		_fbo->unbind();

		glDepthMask( GL_TRUE );
		glEnable( GL_DEPTH_TEST );
		glDrawBuffer( GL_BACK );
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().y );
	}

	void unload()
	{
		delete _tessShader;		_tessShader = NULL;
		delete _genShader;		_genShader = NULL;
		delete _renderTarget;	_renderTarget = NULL;
		delete _fbo;			_fbo = NULL;
	}

	void draw(int)
	{
		program->bind( _tessShader );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		normalMap->bind(  TEX_NORMAL );
		_renderTarget->bind( TEX_TESS_LEVEL );

		gridMesh->draw();

		_renderTarget->unbind( TEX_TESS_LEVEL );
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
		normalMap->bind(  TEX_NORMAL );

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
		normalMap->bind(  TEX_NORMAL );

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
		Shader *	shader = (i & 1) ? _shaderScreenSpace : _shaderDist;

		program->bind( shader );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		normalMap->bind(  TEX_NORMAL );

		gridMesh->draw();
	}
};


class Part7 : public Mode
{
private:
	Shader *	_shader;

public:
	Part7(): _shader(NULL)
	{}

	~Part7()
	{
		unload();
	}

	void load()
	{
		program->load( _shader, "shaders/tess_quad_normal_to_camera.prg" );
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
		normalMap->bind(  TEX_NORMAL );

		gridMesh->draw();
	}
};

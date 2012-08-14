enum EViewIndex
{
	VIEW_COLOR	= 0,
	VIEW_NORMAL,
	VIEW_TESS,
	VIEW_COLOR_MIX_TESS,
};


class View
{
private:
	Shader		*	_viewColor,
				*	_viewNormal,
				*	_viewTess,
				*	_viewColorMixTess;
	Texture		*	_colorTarget,
				*	_normalTarget,
				*	_depthTarget;
	Mesh		*	_fullScreenQuad;
	Framebuffer *	_fbo;
	glm::mat4		_ortho;

public:
	View():
		_viewColor(NULL), _viewNormal(NULL), _viewTess(NULL), _viewColorMixTess(NULL),
		_colorTarget(NULL), _normalTarget(NULL), _depthTarget(NULL), _fbo(NULL), _fullScreenQuad(NULL)
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
		delete _fullScreenQuad;
	}

	void init()
	{
		_ortho = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );
		
		_fullScreenQuad	= new Mesh();
		_fullScreenQuad->makeQuad();

		program->load( _viewColor,	"shaders/view_color.prg" );
		program->load( _viewNormal,	"shaders/view_normal.prg" );
		program->load( _viewTess,	"shaders/view_tesslevel.prg" );
		program->load( _viewColorMixTess, "shaders/view_color_mix_tesslvl.prg" );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_normalTarget )
			_normalTarget = new Texture( GL_TEXTURE_2D );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_fbo )
			_fbo = new Framebuffer();

		_colorTarget->create2D(  NULL, scrWidth, scrHeight, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_normalTarget->create2D( NULL, scrWidth, scrHeight, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_depthTarget->create2D(  NULL, scrWidth, scrHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT );

		_fbo->bind();
		_fbo->attach( _colorTarget,  GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _normalTarget, GL_COLOR_ATTACHMENT1 );
		_fbo->attach( _depthTarget,  GL_DEPTH_ATTACHMENT );
		_fbo->checkStatus();
		_fbo->unbind();
	}

	void bind()
	{
		_fbo->bind();
		_fbo->setRenderTargets( RTF_COLOR0 | RTF_COLOR1 );
		glViewport( 0, 0, scrWidth, scrHeight );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
										return;

		program->getStates().mvp = _ortho;

		program->bind( shader );

		glDisable(GL_DEPTH_TEST);
		_colorTarget->bind(  TEX_DIFFUSE );
		_normalTarget->bind( TEX_NORMAL );
		_depthTarget->bind(  TEX_DEPTH );

		_fullScreenQuad->draw();

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
	Shader *	_shaders[6];
	Mesh *		_grids[2];
	int			_lastIdx,
				_lastTime;
	float		_partTime;

public:
	Part1(): _lastIdx(0), _partTime(0.f), _lastTime(0)
	{
		memset( _shaders, 0, sizeof(_shaders) );
		_grids[0] = NULL;
		_grids[1] = NULL;
	}

	~Part1()
	{
		unload();
	}

	void load()
	{
		const int	gridSize = 15;

		program->load( _shaders[0], "shaders/tess_tri_tessmode.prg", "#define SPACING equal_spacing" );
		program->load( _shaders[1], "shaders/tess_tri_tessmode.prg", "#define SPACING fractional_even_spacing" );
		program->load( _shaders[2], "shaders/tess_tri_tessmode.prg", "#define SPACING fractional_odd_spacing" );

		program->load( _shaders[3], "shaders/tess_quad_tessmode.prg", "#define SPACING equal_spacing" );
		program->load( _shaders[4], "shaders/tess_quad_tessmode.prg", "#define SPACING fractional_even_spacing" );
		program->load( _shaders[5], "shaders/tess_quad_tessmode.prg", "#define SPACING fractional_odd_spacing" );

		_grids[0] = new Mesh();
		_grids[0]->createGrid( gridSize, 1.f / float(gridSize), 3 );

		_grids[1] = new Mesh();
		_grids[1]->createGrid( gridSize, 1.f / float(gridSize), 4 );

		program->getStates().gridScale = 100.f;
		program->getStates().maxTessLevel = 6.f;
		cam.setPosition( glm::vec3(-50.f, -50.f, -50.f) );
	}

	void unload()
	{
		delete _shaders[0];		_shaders[0] = NULL;
		delete _shaders[1];		_shaders[1] = NULL;
		delete _grids[0];		_grids[0] = NULL;
		delete _grids[1];		_grids[1] = NULL;
	}

	void draw(int i)
	{
		if ( _lastIdx != i ) {
			_lastIdx  = i&1;
			_partTime = 0.f;
		}

		const float		timeForShader = 15.f * 2.f;

		_partTime += float(lastTime - _lastTime) * 0.001f;
		_lastTime  = lastTime;

		float		level = sin( fmod( _partTime, timeForShader ) ) * 0.5f + 0.5f;

		if ( level > timeForShader * 3.f ) {
			_partTime = 0.f;
			level = 0.f;
		}

		program->getStates().detailLevel = level;

		program->bind( _shaders[ (i&1) /*+ int(_partTime/timeForShader + 0.5f) % 3*/ ] );

		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		_grids[ i>2 ]->draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_pnTriangles,
			*	_bezierPatches;
	Texture *	_diffuseMap,
			*	_heightMap,
			*	_normalMap;
	Mesh	*	_grid3,
			*	_grid16;

public:
	Part2(): _pnTriangles(NULL), _bezierPatches(NULL),
			_diffuseMap(NULL), _heightMap(NULL), _normalMap(NULL),
			_grid3(NULL), _grid16(NULL)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		program->load( _pnTriangles,    "shaders/tess_pn-triangles.prg" );
		program->load( _bezierPatches,  "shaders/tess_bezier.prg" );

		_grid3	= new Mesh();
		_grid16	= new Mesh();
		
		const int	gridSize = 127;

		_grid3->createGrid(  gridSize, 1.f / float(gridSize),  3 );
		_grid16->createGrid( gridSize, 1.f / float(gridSize), 16 );
		
		_diffuseMap		= new Texture( GL_TEXTURE_2D );
		_heightMap		= new Texture( GL_TEXTURE_2D );
		_normalMap		= new Texture( GL_TEXTURE_2D );

		_diffuseMap->loadDDS( "textures/grass.dds" );
		_heightMap->loadDDS(  "textures/height2_bc4.dds" );
		_normalMap->loadDDS(  "textures/normal2.dds" );

		_diffuseMap->bind();
		_diffuseMap->setWrap( GL_REPEAT, GL_REPEAT );
		_diffuseMap->setAnisotropy( 16 );
		_diffuseMap->unbind();

		_heightMap->bind();
		_heightMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		_heightMap->unbind();

		_normalMap->bind();
		_normalMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		_normalMap->unbind();

		if ( currPart != 1 ) {
			program->getStates() = Program::States();
			cam.setPosition( glm::vec3(-1590.f, -135.f, -1830.f) );
		}
	}

	void unload()
	{
		delete _pnTriangles;		_pnTriangles = NULL;
		delete _bezierPatches;		_bezierPatches = NULL;
		delete _grid3;				_grid3 = NULL;
		delete _grid16;				_grid16 = NULL;
		delete _diffuseMap;			_diffuseMap = NULL;
		delete _heightMap;			_heightMap = NULL;
		delete _normalMap;			_normalMap = NULL;
	}

	void draw(int i)
	{
		Mesh	*	grid	= i&1 ? _grid16 : _grid3;
		Shader	*	shader	= i&1 ? _bezierPatches : _pnTriangles;

		program->bind( shader );

		_diffuseMap->bind( TEX_DIFFUSE );
		_heightMap->bind(  TEX_HEIGHT );
		_normalMap->bind(  TEX_NORMAL );
		
		glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		grid->draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_CULL_FACE );
	}
};


class Part3 : public Mode
{
private:

public:
	Part3()
	{}

	~Part3()
	{
		unload();
	}

	void load()
	{
	}

	void unload()
	{
	}

	void draw(int)
	{
	}
};


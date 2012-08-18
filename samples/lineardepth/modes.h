
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
	Shader		*	_viewColor,
				*	_viewDepth;
	Shader *		_terrainShader;
	Texture		*	_colorTarget,
				*	_depthTarget;
	Framebuffer *	_fbo;
	glm::mat4		_ortho;

public:
	Part1(): _terrainShader(NULL), _viewColor(NULL), _viewDepth(NULL),
			 _colorTarget(NULL), _depthTarget(NULL), _fbo(NULL)
	{}

	~Part1()
	{
		unload();
	}

	void load()
	{
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );
		
		_ortho = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );

		program->load( _viewColor,		"shaders/view_color.prg" );
		program->load( _viewDepth,		"shaders/view_lineardepth.prg" );
		program->load( _terrainShader,	"shaders/tess_quad_lineardepth.prg" );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );
		
		_colorTarget->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_depthTarget->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_fbo )
			_fbo = new Framebuffer();

		_fbo->bind();
		_fbo->attach( _colorTarget, GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _depthTarget, GL_DEPTH_ATTACHMENT );
		_fbo->checkStatus();
		_fbo->unbind();
	}

	void unload()
	{
		delete _terrainShader;		_terrainShader = NULL;
		delete _viewColor;			_viewColor = NULL;
		delete _viewDepth;			_viewDepth = NULL;
		delete _colorTarget;		_colorTarget = NULL;
		delete _depthTarget;		_depthTarget = NULL;
		delete _fbo;				_fbo = NULL;
	}

	void draw(int i)
	{
		_fbo->bind();
		_fbo->setRenderTargets( RTF_COLOR0 );
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().y );
		
		const float		depthClear = 1.0f;
		const glm::vec4	colorClear = glm::vec4( 0.f, 0.8f, 1.0f, 0.f );

		glClearBufferfv( GL_DEPTH, 0, &depthClear );
		glClearBufferfv( GL_COLOR, 0, glm::value_ptr(colorClear) );

		program->bind( _terrainShader );
		
		_terrainShader->setUniformFloat(  "unFarPlane",  3000.f );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		normalMap->bind(  TEX_NORMAL );
		
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		glEnable( GL_CULL_FACE );

		if ( updateQuery ) primitivesQuery->begin( GL_PRIMITIVES_GENERATED );
		gridMesh->draw();
		if ( updateQuery ) { primitivesQuery->end();  updateQuery = false; }
	
		glDisable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		_fbo->unbind();


		glDisable(GL_DEPTH_TEST);
		program->getStates().mvp = _ortho;

		program->bind( (i&1) ? _viewDepth : _viewColor );

		_colorTarget->bind( TEX_DIFFUSE );
		_depthTarget->bind( TEX_DEPTH );

		fullScreenQuad->draw();
		glEnable(GL_DEPTH_TEST);
	}
};


class Part2 : public Mode
{
private:
	Shader		*	_viewColor,
				*	_viewDepth;
	Shader *		_terrainShader;
	Texture		*	_colorTarget,
				*	_depthTarget;
	Framebuffer *	_fbo;
	glm::mat4		_ortho,
					_proj1,
					_proj2;

public:
	Part2(): _terrainShader(NULL), _viewColor(NULL), _viewDepth(NULL),
			 _colorTarget(NULL), _depthTarget(NULL), _fbo(NULL)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		gridMesh->createGrid( gridSize, 1.f / float(gridSize), 4 );
		
		_ortho = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );
		_proj1 = glm::perspective<float>( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 0.5f, 201.0f );
		_proj2 = glm::perspective<float>( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 200.0f, 3000.0f );

		program->load( _viewColor,		"shaders/view_color_64bit.prg" );
		program->load( _viewDepth,		"shaders/view_lineardepth_64bit.prg" );
		program->load( _terrainShader,	"shaders/tess_quad_lineardepth_64bit.prg" );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D_ARRAY );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D_ARRAY );
		
		_colorTarget->create3D( NULL, sys.getWndSize().x, sys.getWndSize().y, 2, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_depthTarget->create3D( NULL, sys.getWndSize().x, sys.getWndSize().y, 2, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT );

		if ( !_colorTarget )
			_colorTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_depthTarget )
			_depthTarget  = new Texture( GL_TEXTURE_2D );

		if ( !_fbo )
			_fbo = new Framebuffer();

		_fbo->bind();
		_fbo->attach( _colorTarget, GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _depthTarget, GL_DEPTH_ATTACHMENT );
		_fbo->checkStatus();
		_fbo->unbind();
	}

	void unload()
	{
		delete _terrainShader;		_terrainShader = NULL;
		delete _viewColor;			_viewColor = NULL;
		delete _viewDepth;			_viewDepth = NULL;
		delete _colorTarget;		_colorTarget = NULL;
		delete _depthTarget;		_depthTarget = NULL;
		delete _fbo;				_fbo = NULL;
	}

	void draw(int i)
	{
		_fbo->bind();
		_fbo->setRenderTargets( RTF_COLOR0 );
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().y );
		
		const float		depthClear = 1.0f;
		const glm::vec4	colorClear = glm::vec4( 0.f, 0.8f, 1.0f, 0.f );

		glClearBufferfv( GL_DEPTH, 0, &depthClear );
		glClearBufferfv( GL_COLOR, 0, glm::value_ptr(colorClear) );

		program->bind( _terrainShader );
		
		_terrainShader->setUniformMatrix( "unMVPMatrices[0]", _proj1 * cam.toMVMatrix() );
		_terrainShader->setUniformMatrix( "unMVPMatrices[1]", _proj2 * cam.toMVMatrix() );
		_terrainShader->setUniformVector( "unFarPlanes", glm::vec2( 200.f, 3000.f ) );

		diffuseMap->bind( TEX_DIFFUSE );
		heightMap->bind(  TEX_HEIGHT );
		normalMap->bind(  TEX_NORMAL );
		
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		glEnable( GL_CULL_FACE );

		if ( updateQuery ) primitivesQuery->begin( GL_PRIMITIVES_GENERATED );
		gridMesh->draw();
		if ( updateQuery ) { primitivesQuery->end();  updateQuery = false; }
	
		glDisable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		_fbo->unbind();


		glDisable(GL_DEPTH_TEST);
		program->getStates().mvp = _ortho;

		program->bind( (i&1) ? _viewDepth : _viewColor );

		_colorTarget->bind( TEX_DIFFUSE );
		_depthTarget->bind( TEX_DEPTH );

		fullScreenQuad->draw();
		glEnable(GL_DEPTH_TEST);
	}
};

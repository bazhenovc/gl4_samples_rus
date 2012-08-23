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

		_colorTarget->create2D(  NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_normalTarget->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );
		_depthTarget->create2D(  NULL, sys.getWndSize().x, sys.getWndSize().y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT );

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
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().x );

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
		program->load( _shaders[2], "shaders/tess_tri_tessmode.prg", "#define SPACING fractional_even_spacing" );
		program->load( _shaders[4], "shaders/tess_tri_tessmode.prg", "#define SPACING fractional_odd_spacing" );

		program->load( _shaders[1], "shaders/tess_quad_tessmode.prg", "#define SPACING equal_spacing" );
		program->load( _shaders[3], "shaders/tess_quad_tessmode.prg", "#define SPACING fractional_even_spacing" );
		program->load( _shaders[5], "shaders/tess_quad_tessmode.prg", "#define SPACING fractional_odd_spacing" );

		_grids[0] = new Mesh();
		_grids[0]->createGrid( gridSize, 1.f / float(gridSize), 3 );

		_grids[1] = new Mesh();
		_grids[1]->createGrid( gridSize, 1.f / float(gridSize), 4 );

		if ( currPart != 0 ) {
			program->getStates() = Program::States();
			program->getStates().gridScale = 100.f;
			program->getStates().maxTessLevel = 6.f;
			cam.init( 60.0f, float(sys.getWndSize().y) / float(sys.getWndSize().x), 1.f, 3000.0f,
						glm::vec3(	-program->getStates().gridScale * 0.5f,
									-program->getStates().gridScale * 0.9f,
									-program->getStates().gridScale * 1.5f ) );
		}
	}

	void unload()
	{
		delete _shaders[0];		_shaders[0] = NULL;
		delete _shaders[1];		_shaders[1] = NULL;
		delete _shaders[2];		_shaders[2] = NULL;
		delete _shaders[3];		_shaders[3] = NULL;
		delete _shaders[4];		_shaders[4] = NULL;
		delete _shaders[5];		_shaders[5] = NULL;
		delete _grids[0];		_grids[0] = NULL;
		delete _grids[1];		_grids[1] = NULL;
	}

	void draw(int i)
	{
		const glm::vec4		clearColor(0.f);
		glClearBufferfv( GL_COLOR, 0, glm::value_ptr(clearColor) );

		program->bind( _shaders[ i%6 ] );

		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		_grids[ i&1 ]->draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_pnTriangles,
			*	_bezier16Patches;
	Texture *	_diffuseMap,
			*	_heightMap,
			*	_normalMap;
	Mesh	*	_grid3,
			*	_grid16;

public:
	Part2(): _pnTriangles(NULL), _bezier16Patches(NULL),
			_diffuseMap(NULL), _heightMap(NULL), _normalMap(NULL),
			_grid3(NULL), _grid16(NULL)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		program->load( _pnTriangles,     "shaders/tess_pn-triangles.prg" );
		program->load( _bezier16Patches, "shaders/tess_bezier16.prg" );

		_grid3	= new Mesh();
		_grid16	= new Mesh();
		
		const int	gridSize = 127;

		_grid3->createGrid(  gridSize, 1.f / float(gridSize),  3 );
		_grid16->createGrid( gridSize, 1.f / float(gridSize), 16 );
		
		_diffuseMap		= new Texture( GL_TEXTURE_2D );
		_heightMap		= new Texture( GL_TEXTURE_2D );
		_normalMap		= new Texture( GL_TEXTURE_2D );

		_diffuseMap->loadDDS( "textures/grass.dds" );
		_heightMap->loadDDS(  "textures/height.dds" );
		_normalMap->loadDDS(  "textures/normal.dds" );

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
			cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y),
					  1.f, 3000.0f,
					  glm::vec3(	-program->getStates().gridScale * 0.5f,
									 program->getStates().heightScale * 0.1f,
									-program->getStates().gridScale * 0.5f )
					 );
		}
	}

	void unload()
	{
		delete _pnTriangles;		_pnTriangles = NULL;
		delete _bezier16Patches;	_bezier16Patches = NULL;
		delete _grid3;				_grid3 = NULL;
		delete _grid16;				_grid16 = NULL;
		delete _diffuseMap;			_diffuseMap = NULL;
		delete _heightMap;			_heightMap = NULL;
		delete _normalMap;			_normalMap = NULL;
	}

	void draw(int i)
	{
		Mesh	*	grid	= (i&1) ? _grid16 : _grid3;
		Shader	*	shader	= (i&1) ? _bezier16Patches : _pnTriangles;

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

/*
class Part2_1 : public Mode
{
private:
	Shader	*	_terrainShader;
	Texture *	_diffuseMap,
			*	_heightMap,
			*	_normalMap;
	Mesh	*	_grid;

public:
	Part2_1(): _terrainShader(NULL), _grid(NULL),
			_diffuseMap(NULL), _heightMap(NULL), _normalMap(NULL)
	{}

	~Part2_1()
	{
		unload();
	}

	void load()
	{
		program->load( _terrainShader,	"shaders/tess_quad_tiling.prg" );

		_grid	= new Mesh();
		
		const int	gridSize = 127;

		_grid->createGrid( gridSize, 1.f / float(gridSize), 4 );
		
		_diffuseMap		= new Texture( GL_TEXTURE_2D_ARRAY );
		_heightMap		= new Texture( GL_TEXTURE_2D );
		_normalMap		= new Texture( GL_TEXTURE_2D );

		_heightMap->loadDDS(  "textures/height.dds" );
		_normalMap->loadDDS(  "textures/normal.dds" );

		_diffuseMap->create3D( NULL, 512, 512, 16, GL_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, glm::log2(512.f) );
		_diffuseMap->load2DLayerDDS( "textures/h0_a0.dds", 0 );
		_diffuseMap->load2DLayerDDS( "textures/h0_a1.dds", 1 );
		_diffuseMap->load2DLayerDDS( "textures/h1_a0.dds", 2 );
		_diffuseMap->load2DLayerDDS( "textures/h2_a0.dds", 3 );
		_diffuseMap->load2DLayerDDS( "textures/h3_a0.dds", 4 );
		_diffuseMap->load2DLayerDDS( "textures/h4_a0.dds", 5 );
		_diffuseMap->load2DLayerDDS( "textures/h5_a0.dds", 6 );
		_diffuseMap->load2DLayerDDS( "textures/h5_a1.dds", 7 );
		_diffuseMap->load2DLayerDDS( "textures/h5_a2.dds", 8 );
		_diffuseMap->load2DLayerDDS( "textures/h5_a3.dds", 9 );
		_diffuseMap->load2DLayerDDS( "textures/h6_a0.dds", 10 );
		_diffuseMap->load2DLayerDDS( "textures/h6_a1.dds", 11 );
		_diffuseMap->load2DLayerDDS( "textures/h6_a2.dds", 12 );
		_diffuseMap->load2DLayerDDS( "textures/h7_a0.dds", 13 );
		_diffuseMap->load2DLayerDDS( "textures/h7_a1.dds", 14 );

		_diffuseMap->bind();
		_diffuseMap->generateMipmaps();
		_diffuseMap->setWrap( GL_REPEAT, GL_REPEAT, GL_CLAMP_TO_EDGE );
		//_diffuseMap->setFilter( GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR );
		_diffuseMap->setAnisotropy( 16 );
		_diffuseMap->unbind();

		_heightMap->bind();
		_heightMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		_heightMap->unbind();

		_normalMap->bind();
		_normalMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		_normalMap->unbind();

		if ( currPart != 2 ) {
			program->getStates() = Program::States();
			cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y),
					  1.f, 3000.0f,
					  glm::vec3(	-program->getStates().gridScale * 0.5f,
									 program->getStates().heightScale * 0.1f,
									-program->getStates().gridScale * 0.5f )
					 );
		}
	}

	void unload()
	{
		delete _terrainShader;		_terrainShader = NULL;
		delete _grid;				_grid = NULL;
		delete _diffuseMap;			_diffuseMap = NULL;
		delete _heightMap;			_heightMap = NULL;
		delete _normalMap;			_normalMap = NULL;
	}

	void draw(int)
	{
		program->bind( _terrainShader );

		_diffuseMap->bind( TEX_DIFFUSE );
		_heightMap->bind(  TEX_HEIGHT );
		_normalMap->bind(  TEX_NORMAL );
		
		glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );

		_grid->draw();

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_CULL_FACE );
	}
};*/



class Part3 : public Mode
{
private:
	struct Model
	{
		Shader	*		shader;
		SceletalMesh *	mesh;
		glm::vec3		pos;
		glm::quat		orient;
		float			scale;
		bool			paused;

		Model(): shader(NULL), mesh(NULL), scale(1.f), paused(false) {}
		~Model() { delete shader;  delete mesh; }
	};

	Model	animModel;

public:
	Part3()
	{}

	~Part3()
	{
		unload();
	}

	static void onTrackEnd(SceletalMesh * mesh)
	{
		size_t	curr		= mesh->getCurrentTrack();
		size_t	max_tracks	= mesh->getTrackInfo().size();

		mesh->activeTrack( curr+1 == max_tracks ? 0 : curr+1 );
	}

	void load()
	{
		program->load( animModel.shader, "shaders/tess_mesh_pn-triangles.prg" );	

		animModel.mesh = new SceletalMesh();
		
#if 0
		animModel.mesh->loadMD5Mesh( "pinky/pinky.md5mesh" );
		animModel.mesh->loadMD5Anim( "pinky/pinky_idle1.md5anim" );
		animModel.mesh->loadMD5Anim( "pinky/pinky_run.md5anim" );
		animModel.mesh->loadMD5Anim( "pinky/pinky_attack.md5anim" );
#endif

#if 1
		animModel.mesh->loadMD5Mesh( "qshambler/qshambler.md5mesh" );
		animModel.mesh->loadMD5Anim( "qshambler/qshambler_idle02.md5anim" );
		animModel.mesh->loadMD5Anim( "qshambler/qshambler_walk.md5anim" );
		animModel.mesh->loadMD5Anim( "qshambler/qshambler_attack01.md5anim" );
		animModel.mesh->loadMD5Anim( "qshambler/qshambler_attack02.md5anim" );

		Material *	mtr = 0;
		
		mtr = new Material();
		mtr->addTexture( "textures/grass.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( 0u, mtr );
		animModel.mesh->setMaterial( 1u, mtr );
#endif

#if 0
		animModel.mesh->loadMD5Mesh( "Boblamp/boblampclean.md5mesh" );
		animModel.mesh->loadMD5Anim( "Boblamp/boblampclean.md5anim" );

		Material *	mtr = 0;
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_body.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( "guard1_body", mtr );
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_face.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( "guard1_face", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_helmet.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( "guard1_helmet", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/iron_grill.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( "iron_grill", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/round_grill.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( "round_grill", mtr );
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_body.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		animModel.mesh->setMaterial( 5, mtr );
#endif

		animModel.mesh->setCallback( &onTrackEnd );
		animModel.mesh->activeTrack( 1 );


		if ( currPart != 3 ) {
			animModel.pos = glm::vec3( 0.f, 0.f, 0.f );

			program->getStates() = Program::States();
			cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 1.f, 3000.0f );
		}
	}

	void unload()
	{
		animModel = Model();
	}

	void draw(int i)
	{
		if ( input.isKeyClick('u') )	animModel.paused = !animModel.paused;

		if ( !animModel.paused )
			animModel.mesh->update( sys.getTimeDelta() * 0.0005f );

		program->getStates().mvp = cam.buildMVPMatrix( animModel.pos );	// TODO: rotation, scale

		program->bind( animModel.shader );
	
		glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );

		animModel.mesh->draw( animModel.shader, true );
		
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_CULL_FACE );
	}
};
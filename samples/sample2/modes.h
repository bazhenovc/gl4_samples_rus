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
		_colorTarget->bind(  TEX_DIFFUSE );		glBindSampler( TEX_DIFFUSE, 0 );
		_normalTarget->bind( TEX_NORMAL );		glBindSampler( TEX_NORMAL, 0 );
		_depthTarget->bind(  TEX_DEPTH );		glBindSampler( TEX_DEPTH, 0 );

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
	virtual void getModeInfo(int, std::string &) = 0;	// TODO: return information of mode (keys, ...)
};


class Part1 : public Mode
{
private:
	Shader *	_shaders[6];
	Mesh *		_grids[2];
	float		_gridScale;

public:
	Part1(): _gridScale(100.f)
	{
		memset( _shaders, 0, sizeof(_shaders) );
		memset( _grids,   0, sizeof(_grids) );
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
			_gridScale = 100.f;
			program->getStates() = Program::States();
			program->getStates().maxTessLevel = 6.f;
			cam.init( 60.0f, float(sys.getWndSize().y) / float(sys.getWndSize().x), 1.f, 500.0f,
						glm::vec3(	-_gridScale * 0.5f,
									-_gridScale * 0.9f,
									-_gridScale * 1.5f ) );
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
		Shader *	shader = _shaders[ i%6 ];

		const glm::vec4		clearColor(0.f);
		glClearBufferfv( GL_COLOR, 0, glm::value_ptr(clearColor) );

		program->bind( shader );
		shader->setUniformFloat( "unGridScale", _gridScale );

		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		_grids[ i&1 ]->draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
	
	void getModeInfo(int i, std::string &info)
	{
		if ( i >= count_of(_shaders) )
			return;
	
		static const char *	spacing[] = { "equal", "fractional_even", "fractional_odd" };
		static const char	partInfo[] = "";
		
		info += partInfo;
		info += "\nPatch: ";
		info += i&1 ? "quad" : "triangle";
		info += "\nSpacing: ";
		info += spacing[i>>1];
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_pnTriangles,
			*	_phong3Patches,
			*	_bezier16Patches;
	Texture *	_diffuseMap,
			*	_heightMap,
			*	_normalMap;
	Mesh	*	_grid3,
			*	_grid16;
	float		_gridScale;
	float		_heightScale;
	float		_fPositionBlend;

public:
	Part2(): _pnTriangles(NULL), _bezier16Patches(NULL), _phong3Patches(NULL),
			_diffuseMap(NULL), _heightMap(NULL), _normalMap(NULL),
			_grid3(NULL), _grid16(NULL), _gridScale(2000.f),
			_heightScale(800.f), _fPositionBlend(1.0)
	{}

	~Part2()
	{
		unload();
	}

	void load()
	{
		program->load( _pnTriangles,     "shaders/tess_pn-triangles.prg" );
		program->load( _phong3Patches,	 "shaders/tess_phong.prg" );
		program->load( _bezier16Patches, "shaders/tess_bezier16.prg" );

		_grid3	= new Mesh();
		_grid16	= new Mesh();
		
		 const int	gridSize = 255;
		_grid3->createGrid(  gridSize, 1.f / float(gridSize),  3 );
		_grid16->createGrid( gridSize, 1.f / float(gridSize), 16 );

		_diffuseMap		= new Texture( GL_TEXTURE_2D );
		_heightMap		= new Texture( GL_TEXTURE_2D );
		_normalMap		= new Texture( GL_TEXTURE_2D );

		_diffuseMap->loadDDS( "textures/grass.dds" );
		_heightMap->loadDDS(  "textures/height_256.dds" );
		_normalMap->loadDDS(  "textures/normal_256.dds" );

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
			_gridScale	 = 2000.f;
			_heightScale = _gridScale * 0.125f;

			program->getStates() = Program::States();
			cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y),
					  1.f, 2000.0f,
					  glm::vec3(	-_gridScale * 0.5f,
									-_heightScale * 0.3f,
									-_gridScale * 0.5f )
					 );
		}
	}

	void unload()
	{
		delete _phong3Patches;		_phong3Patches = NULL;
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
		if ( i >= 3 )
			return;

		// [ ]
		if ( input.isKeyClick('[') )	_heightScale--;
		if ( input.isKeyClick(']') )	_heightScale++;

		// v b
		if ( input.isKeyClick('b') )	_fPositionBlend += 0.05f;
		if ( input.isKeyClick('v') )	_fPositionBlend -= 0.05f;
		_fPositionBlend = glm::clamp( _fPositionBlend, 0.f, 1.f );

		
		Mesh	*	grid	= i == 0 ? _grid16 : _grid3;
		Shader	*	shader	= NULL;
		
		switch ( i )
		{
			case 0 :	shader = _bezier16Patches;	break;
			case 1 :	shader = _pnTriangles;		break;
			case 2 :	shader = _phong3Patches;	break;
		};

		program->bind( shader );
		shader->setUniformFloat( "unGridScale",		_gridScale );
		shader->setUniformFloat( "unHeightScale",	_heightScale );
		shader->setUniformFloat( "unPositionBlend", _fPositionBlend );

		_diffuseMap->bind( TEX_DIFFUSE );
		_heightMap->bind(  TEX_HEIGHT );
		_normalMap->bind(  TEX_NORMAL );
		
		glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		grid->draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_CULL_FACE );
	}
	
	void getModeInfo(int i, std::string &info)
	{
		if ( i >= 3 )
			return;
	
		static const char *	shaderInfo[] = { "Bezier patches", "PN-Triangles", "Phong patches" };
		static const char	partInfo[] = "";
		
		info += partInfo;
		info += "\nCurrent mode: ";
		info += shaderInfo[i];
	}
};


class Part3 : public Mode
{
private:
	struct Model
	{
		SceletalMesh *	mesh;
		glm::vec3		pos;
		glm::quat		orient;
		float			scale;
		bool			paused;

		Model(): mesh(NULL), scale(1.f), paused(false) {}
		~Model() { delete mesh; }
	};
	
	Shader	*	_shaders[2];
	MultiMesh *	_scene;
	Model		_animModel;
	float		_fPositionBlend;

public:
	Part3():
		_fPositionBlend(1.f), _scene(NULL)
	{
		memset( _shaders, 0, sizeof(_shaders) );
	}

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
		program->load( _shaders[0], "shaders/tess_mesh_pn-triangles.prg" );	
		program->load( _shaders[1], "shaders/tess_mesh_phong.prg" );	

		_animModel.mesh = new SceletalMesh();
		
		
		if ( !_scene ) {
			_scene = new MultiMesh();
			_scene->load( "meshes/SMUT.3ds", "SMUT_textures/" );
		}

#if 0
		_animModel.mesh->loadMD5Mesh( "pinky/pinky.md5mesh" );
		_animModel.mesh->loadMD5Anim( "pinky/pinky_idle1.md5anim" );
		_animModel.mesh->loadMD5Anim( "pinky/pinky_run.md5anim" );
		_animModel.mesh->loadMD5Anim( "pinky/pinky_attack.md5anim" );
#endif

#if 1
		_animModel.mesh->loadMD5Mesh( "qshambler/qshambler.md5mesh" );
		_animModel.mesh->loadMD5Anim( "qshambler/qshambler_idle02.md5anim" );
		_animModel.mesh->loadMD5Anim( "qshambler/qshambler_walk.md5anim" );
		_animModel.mesh->loadMD5Anim( "qshambler/qshambler_attack01.md5anim" );
		_animModel.mesh->loadMD5Anim( "qshambler/qshambler_attack02.md5anim" );

		Material *	mtr = 0;
		
		mtr = new Material();
		mtr->addTexture( "textures/grass.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( 0u, mtr );
		_animModel.mesh->setMaterial( 1u, mtr );
#endif

#if 0
		_animModel.mesh->loadMD5Mesh( "Boblamp/boblampclean.md5mesh" );
		_animModel.mesh->loadMD5Anim( "Boblamp/boblampclean.md5anim" );

		Material *	mtr = 0;
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_body.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( "guard1_body", mtr );
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_face.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( "guard1_face", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_helmet.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( "guard1_helmet", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/iron_grill.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( "iron_grill", mtr );

		mtr = new Material();
		mtr->addTexture( "Boblamp/round_grill.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( "round_grill", mtr );
		
		mtr = new Material();
		mtr->addTexture( "Boblamp/guard1_body.dds", "unDiffuseMap", 0, GL_TEXTURE_2D, Sampler() );
		_animModel.mesh->setMaterial( 5, mtr );
#endif

		_animModel.mesh->setCallback( &onTrackEnd );
		_animModel.mesh->activeTrack( 1 );


		if ( currPart != 2 ) {
			_animModel.pos = glm::vec3( 0.f, 0.f, 0.f );

			program->getStates() = Program::States();
			cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 1.f, 3000.0f );
		}
	}

	void unload()
	{
		_animModel = Model();
		delete _shaders[0];		_shaders[0] = NULL;
		delete _shaders[1];		_shaders[1] = NULL;
	}

	void draw(int i)
	{
		static const glm::quat	scene_rotation = glm::rotate( glm::quat(), 90.f, glm::vec3(1.f, 0.f, 0.f) );
		static const glm::quat	mesh_rotation  = glm::rotate( glm::quat(), 90.f, glm::vec3(0.f, 0.f, 1.f) ) *
												 glm::rotate( glm::quat(), 90.f, glm::vec3(0.f, 1.f, 0.f) );

		// u
		if ( input.isKeyClick('u') )	_animModel.paused = !_animModel.paused;

		// v b
		if ( input.isKeyClick('b') )	_fPositionBlend += 0.05f;
		if ( input.isKeyClick('v') )	_fPositionBlend -= 0.05f;
		_fPositionBlend = glm::clamp( _fPositionBlend, 0.f, 1.f );

		if ( !_animModel.paused )
			_animModel.mesh->update( sys.getTimeDelta() * 0.0005f );	// x2 slow


		Shader *	shader = _shaders[ i % count_of(_shaders) ];

		program->bind( shader );
		shader->setUniformFloat( "unPositionBlend", _fPositionBlend );
	
		//glEnable( GL_CULL_FACE );
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		
		shader->setUniformMatrix( "unMVPMatrix", cam.buildMVPMatrix( _animModel.pos, mesh_rotation, 0.1f ) );
		_animModel.mesh->draw( shader, true );
		
		shader->setUniformMatrix( "unMVPMatrix", cam.buildMVPMatrix( glm::vec3(0.f), scene_rotation, 10.f ) );
		_scene->draw( shader, true );
		
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_CULL_FACE );
	}
	
	void getModeInfo(int i, std::string &info)
	{
		static const char	partInfo[] = "";
		
		info += partInfo;
	}
};
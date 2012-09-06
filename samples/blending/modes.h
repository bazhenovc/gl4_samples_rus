


class Mode
{
public:
	virtual ~Mode() {}
	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void draw(int) = 0;
};



class Part0 : public Mode
{
private:
	Shader		*	_sceneShader,
				*	_blendShader1,
				*	_blendShader2,
				*	_viewColor,
				*	_viewDepth,
				*	_copyDepth;
	MultiMesh	*	_blendMesh;
	Texture		*	_colorTarget,
				*	_depthTarget1,
				*	_depthTarget2;
	Framebuffer *	_fbo;
	bool			_enableViewDepth;

public:
	Part0():
		_sceneShader(NULL), _blendMesh(NULL), _blendShader1(NULL), _blendShader2(NULL),
		_colorTarget(NULL), _depthTarget1(NULL), _depthTarget2(NULL), _fbo(NULL),
		_viewColor(NULL), _viewDepth(NULL), _copyDepth(NULL), _enableViewDepth(false)
	{}

	~Part0()
	{
		unload();
	}

	void load()
	{
		program->load( _sceneShader,		"shaders/scene.prg" );
		program->load( _blendShader1,	"shaders/blend.prg" );
		program->load( _blendShader2,	"shaders/blend2.prg" );
		program->load( _viewColor,		"shaders/view_color.prg" );
		program->load( _viewDepth,		"shaders/view_depth.prg" );
		program->load( _copyDepth,		"shaders/copy_depth.prg" );

		_blendMesh = new MultiMesh();
		_blendMesh->load( "meshes/blend.3ds" );

		_blendMesh->getMaterial(0)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
		_blendMesh->getMaterial(1)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.7f, 0.2f, 0.0f, 1.0f );
		_blendMesh->getMaterial(2)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.5f, 0.5f, 0.0f, 1.0f );
		_blendMesh->getMaterial(3)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.2f, 0.7f, 0.0f, 1.0f );
		_blendMesh->getMaterial(4)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
		_blendMesh->getMaterial(5)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.0f, 0.7f, 0.2f, 1.0f );
		_blendMesh->getMaterial(6)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.0f, 0.5f, 0.5f, 1.0f );
		_blendMesh->getMaterial(7)->getData< MultiMesh::MaterialParams >().diffuse = glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f );

		_colorTarget	= new Texture(GL_TEXTURE_2D);
		_depthTarget1	= new Texture(GL_TEXTURE_2D);
		_depthTarget2	= new Texture(GL_TEXTURE_2D);
		_fbo			= new Framebuffer();

		_colorTarget->create2D(  NULL, sys.getWndSize().x, sys.getWndSize().y, GL_RGBA, GL_RGBA8,  GL_UNSIGNED_BYTE );
		_depthTarget1->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_DEPTH_STENCIL, GL_DEPTH32F_STENCIL8, GL_FLOAT_32_UNSIGNED_INT_24_8_REV );
		_depthTarget2->create2D( NULL, sys.getWndSize().x, sys.getWndSize().y, GL_DEPTH_STENCIL, GL_DEPTH32F_STENCIL8, GL_FLOAT_32_UNSIGNED_INT_24_8_REV );
		
		_fbo->bind();
		_fbo->attach( _colorTarget,   GL_COLOR_ATTACHMENT0 );
		_fbo->attach( _depthTarget1,  GL_DEPTH_STENCIL_ATTACHMENT );
		_fbo->checkStatus();
		_fbo->unbind();
	}

	void unload()
	{
		delete _fbo;			_fbo = NULL;
		delete _sceneShader;	_sceneShader = NULL;
		delete _blendMesh;		_blendMesh = NULL;
		delete _blendShader1;	_blendShader1 = NULL;
		delete _blendShader2;	_blendShader2 = NULL;
		delete _colorTarget;	_colorTarget = NULL;
		delete _depthTarget1;	_depthTarget1 = NULL;
		delete _depthTarget2;	_depthTarget2 = NULL;
		delete _viewColor;		_viewColor = NULL;
		delete _viewDepth;		_viewDepth = NULL;
		delete _copyDepth;		_copyDepth = NULL;
	}

	void draw(int i)
	{
		static const glm::quat	q = glm::rotate( glm::quat(), 90.f, glm::vec3(1.f, 0.f, 0.f) );
		static const glm::mat4	ortho = glm::ortho( -1.f, 1.f, -1.f, 1.f, -1.f, 1.f );

		if ( input.isKeyClick('c') )	_enableViewDepth = !_enableViewDepth;

		// first pass
		_fbo->bind();
		_fbo->attach( _depthTarget1, GL_DEPTH_STENCIL_ATTACHMENT );
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		glViewport( 0, 0, _colorTarget->getWidth(), _colorTarget->getHeight() );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
		program->getStates().mvp		 = cam.buildMVPMatrix( glm::vec3(0.f), q, 10.f );
		program->bind( _sceneShader );
		scene->draw( _sceneShader );

		program->getStates().mvp		 = cam.toMatrixScale( 2.f );
		
		_depthTarget1->bind( TEX_DEPTH );
		glBindSampler( TEX_DEPTH, 0 );

		// without blending
		if ( i == 0 )
		{
			program->bind( _blendShader1 );
			_blendMesh->draw( _blendShader1 );
		}
		else
		// simple blending
		if ( i == 1 )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glDepthMask( GL_FALSE );

			program->bind( _blendShader1 );
			_blendMesh->draw( _blendShader1 );

			glDepthMask( GL_TRUE );
			glDisable( GL_BLEND );
		}
		else
		// order-independent transparency
		if ( i == 2 )
		{
			Texture *	depthTargets[2] = { _depthTarget2, _depthTarget1 };
			const int	max_iterations = 12;

			glStencilMask( 0xFF );
			glEnable( GL_BLEND );
			glEnable( GL_STENCIL_TEST );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

			for (int j = 0; j < max_iterations; ++j)
			{
				_fbo->attach( depthTargets[ j&1 ], GL_DEPTH_STENCIL_ATTACHMENT );
				glClearBufferfi( GL_DEPTH_STENCIL, 0, 0.f, 0 );
			
				glDepthFunc( (j == max_iterations-1) ? GL_LEQUAL : GL_GEQUAL );
				glStencilFunc( GL_ALWAYS, 1, 0 );
				
				program->getStates().mvp	 = cam.toMatrixScale( 2.f );
				program->bind( _blendShader2 );
			
				depthTargets[ (j+1)&1 ]->bind( TEX_DEPTH );
				glBindSampler( TEX_DEPTH, 0 );

				_blendMesh->draw( _blendShader2 );

	
				// copy depth
				program->getStates().mvp = ortho;
				program->bind( _copyDepth );
			
				glDrawBuffer( GL_NONE );
				glDepthFunc( GL_ALWAYS );
				glStencilFunc( GL_NOTEQUAL, 1, 255 );
			
				fullScreenQuad->draw();
			
				glDrawBuffer( GL_COLOR_ATTACHMENT0 );
			}
			
			glDisable( GL_STENCIL_TEST );
			glDepthFunc( GL_LEQUAL );
			glDisable( GL_BLEND );
			depthTargets[ max_iterations&1 ]->bind( TEX_DEPTH );
		}

		_fbo->unbind();


		// second pass
		glViewport( 0, 0, sys.getWndSize().x, sys.getWndSize().y );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		program->getStates().mvp = ortho;
		program->bind( _enableViewDepth ? _viewDepth : _viewColor );
		
		glDisable( GL_DEPTH_TEST );
		_colorTarget->bind( TEX_DIFFUSE );		glBindSampler( TEX_DIFFUSE, 0 );

		fullScreenQuad->draw();

		_depthTarget2->unbind( TEX_DEPTH );
		_colorTarget->unbind( TEX_DIFFUSE );
		glEnable( GL_DEPTH_TEST );

	}
};

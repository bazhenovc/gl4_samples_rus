
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Program.h"
#include "Texture.h"
#include "Framebuffer.h"
#include <vector>
#include <string>

using namespace framework;


static const char *	sampleShaders[] = {
	"",
	"",
	"",
	"tess_tri_screenspace_lods.prg",
	"tess_quad_distance_lods_culling.prg",
	"tess_quad_screenspace_lods_culling.prg",
	"view_color.prg",
	"view_normal.prg",
	"view_tesslevel.prg"
};


class Mode
{
public:
	virtual ~Mode() {}
	virtual void load() = 0;
	virtual void draw(int) = 0;
};

Mode *		currentMode		= NULL;
Mesh *		gridMesh		= NULL;
Mesh *		fullScreenQuad	= NULL;
Texture *	diffuseMap		= NULL,
		*	heightMap		= NULL;
Program		program;
int			scrWidth		= 800,
			scrHeight		= 600;


class Part1 : public Mode
{
private:
	Shader *	_shader;

public:
	Part1(): _shader(NULL)
	{}

	~Part1()
	{
		delete _shader;
	}

	void load()
	{
		program.load( _shader, "tess_quad_disp.prg" );
	}

	void draw(int)
	{
		program.bind( _shader );

		glActiveTexture( GL_TEXTURE0 );
		diffuseMap->bind();

		glActiveTexture( GL_TEXTURE1 );
		heightMap->bind();

		gridMesh->draw();
	}
};


class Part2 : public Mode
{
private:
	Shader	*	_shaderTessTri,
			*	_shaderTessQuad;

public:
	Part2(): _shaderTessTri(NULL), _shaderTessQuad(NULL)
	{}

	~Part2()
	{
		delete _shaderTessTri;
		delete _shaderTessQuad;
	}

	void load()
	{
		program.load( _shaderTessTri,  "tess_tri_rnd_level.prg" );
		program.load( _shaderTessQuad, "tess_quad_rnd_level.prg" );
	}

	void draw(int i)
	{
		// i:	0 - triangles,		2 - quads
		//		1 - correct mode,	3 - incorrect mode
		Shader *	shader	= (i & 2) ? _shaderTessQuad : _shaderTessTri;

		program.bind( shader );
		shader->setUniformInt( shader->getLoc("unIncorrectMode"), !!(i & 1) );

		glActiveTexture( GL_TEXTURE0 );
		diffuseMap->bind();

		glActiveTexture( GL_TEXTURE1 );
		heightMap->bind();

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
	Part3(): _tessShader(NULL), _genShader(NULL), _fbo(NULL)
	{}

	~Part3()
	{
		delete _tessShader;
		delete _genShader;
		delete _fbo;
	}

	void load()
	{
		program.load( _tessShader, "tess_quad_level_tex.prg" );
		program.load( _genShader,  "gen_normal_and_tesslvl.prg" );

		if ( !_fbo ) {
			_fbo = new Framebuffer();
		}
		if ( !_renderTarget ) {
			_renderTarget = new Texture( GL_TEXTURE_2D );
			_renderTarget->copyData( NULL, scrWidth, scrHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );

			_fbo->bind();
			_fbo->attach( _renderTarget, GL_COLOR_ATTACHMENT0 );
			_fbo->unbind();
		}
	}

	void draw(int i)
	{
		if ( i == 0 )
		{
			program.bind( _tessShader );

			glActiveTexture( GL_TEXTURE0 );
			diffuseMap->bind();

			glActiveTexture( GL_TEXTURE1 );
			heightMap->bind();

			glActiveTexture( GL_TEXTURE2 );
			_renderTarget->bind();

			gridMesh->draw();

			_renderTarget->unbind();

		}
		else
		{
			_fbo->bind();

			glDepthMask( GL_FALSE );
			glDepthFunc( GL_NEVER );

			glDrawBuffer( GL_COLOR_ATTACHMENT0 );
			glViewport( 0, 0, scrWidth, scrHeight );

			program.bind( _genShader );

			glActiveTexture( GL_TEXTURE1 );
			heightMap->bind();

			fullScreenQuad->draw();

			_fbo->unbind();

			glDepthMask( GL_TRUE );
			glDepthFunc( GL_LEQUAL );
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
		delete _shader;
	}

	void load()
	{
		program.load( _shader, "tess_quad_distance_lods.prg" );
	}

	void draw(int i)
	{
	}
};



void init()
{
}

void shutdown()
{
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glutSwapBuffers();
}

void reshape(int w, int h)
{}

void keyDown(unsigned char key, int, int)
{
	if (27 == key)
		exit(0);
}

int main(int argc, char** argv)
{
	atexit(shutdown);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("basic");
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyDown);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

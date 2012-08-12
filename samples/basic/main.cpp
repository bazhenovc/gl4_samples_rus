
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Framebuffer.h"

#include <stdio.h>

using namespace framework;

#ifndef RESOURCE_PATH
#	ifdef _DEBUG
#		define RESOURCE_PATH		"../../../"
#	else
#		define RESOURCE_PATH		"../"
#	endif
#endif


// globals
Mesh* mesh = 0;
Mesh * fsQuad = 0;
Shader* shader = 0;
Texture* texture = 0;
Framebuffer * fbo = 0;
Texture * target = 0;
GLuint renTarget = 0;

FPSCamera cam;
glm::vec2 mousePos;

bool keys[512];


void init()
{
	memset(keys, 0, 512 * sizeof(bool));

	cam.init(60.0f, 800.0f / 600.0f, 0.1f, 3000.0f);

	mesh = new Mesh;
	mesh->fromFile( RESOURCE_PATH"media/torus.e2m" );

	fsQuad = new Mesh();
	fsQuad->makeQuad();

	shader = new Shader;
	shader->loadShaders(RESOURCE_PATH"media/shaders/basic.prg");

	texture = new Texture(GL_TEXTURE_2D);
	texture->loadDDS(RESOURCE_PATH"media/textures/rockwall.dds");

	fbo = new Framebuffer();
	target = new Texture(GL_TEXTURE_2D);
	target->copyData( NULL, 800, 600, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE );

	/*glGenTextures( 1, &renTarget );
	glBindTexture( GL_TEXTURE_2D, renTarget );
	*/
	target->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	target->unbind();

	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture( GL_TEXTURE_2D, 0 );*/

	fbo->bind();
	fbo->attach( target, GL_COLOR_ATTACHMENT0 );
	//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renTarget, 0 );
	//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->getID(), 0 );
	fbo->checkStatus();
	fbo->unbind();

	glClearColor( 1.f, 0.f, 0.f, 1.f );
}

void shutdown()
{
	delete mesh;
	delete shader;
	delete texture;
	delete fbo;
}

void display()
{
	cam.move(	(keys['w'] - keys['s']) * 0.1f,
				(keys['d'] - keys['a']) * 0.1f,
				(keys['q'] - keys['e']) * 0.1f );
	//*
	fbo->bind();
	{
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		glViewport( 0, 0, 800, 600 );
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		texture->bind(0);

		shader->bind();
		shader->setUniformMatrix("mvp", cam.toMatrix());
		shader->setTexture("diffuse", 0);
		{
			mesh->draw();
		}
		shader->unbind();

		texture->unbind();
	}
	fbo->unbind();
	glFinish();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindMultiTextureEXT( GL_TEXTURE0, GL_TEXTURE_2D, renTarget );
	target->bind(0);
	shader->bind();
	shader->setUniformMatrix("mvp", glm::ortho(-1.f,1.f,-1.f,1.f,-1.f,1.f) );
	shader->setTexture("diffuse", 0);
	{
		fsQuad->draw();
	}
	target->unbind(0);
	//glBindMultiTextureEXT( GL_TEXTURE0, GL_TEXTURE_2D, 0 );

	/*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture->bind(0);
	shader->bind();
	shader->setUniformMatrix("mvp", cam.toMatrix());
	shader->setTexture("diffuse", 0);
	{
		mesh->draw();
	}
	shader->unbind();
	texture->unbind();

	//*/
	glutSwapBuffers();
}

void reshape(int w, int h)
{}

void keyDown(unsigned char key, int, int)
{
	if (27 == key)
		exit(0);

	if (key < 512)
		keys[key] = true;
}

void keyUp(unsigned char key, int, int)
{
	if (key < 512)
		keys[key] = false;
}

void mouseDown(int button, int state, int x, int y)
{
	mousePos = glm::vec2((float)x, (float)y);
}

void mouseMotion(int x, int y)
{
	glm::vec2 newMousePos = glm::vec2((float)x, (float)y);

	glm::vec2 delta = newMousePos - mousePos;

	cam.rotate( delta.x * 0.0001f, delta.y * 0.0001f );

	mousePos = newMousePos;
}

int main(int argc, char** argv)
{
	atexit(shutdown);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("basic");
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMotion);

	glewExperimental = GL_TRUE;
	glewInit();

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

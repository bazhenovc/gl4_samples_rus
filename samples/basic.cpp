
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

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
Shader* shader = 0;
Texture* texture = 0;

glm::mat4 mvp;
Camera cam;
glm::vec2 mousePos;

bool keys[512];

void init()
{
	memset(keys, 0, 512 * sizeof(bool));

	mvp = glm::perspective(60.0f, 800.0f / 600.0f, 0.00001f, 3000.0f);
	cam.pos = glm::vec3(0, 0, -10);
	cam.target = glm::vec3(0, 0, -1);

	mesh = new Mesh;
	mesh->fromFile( RESOURCE_PATH"media/torus.e2m" );

	shader = new Shader;
	shader->loadShaders(RESOURCE_PATH"media/shaders/basic.prg");

	texture = new Texture(GL_TEXTURE_2D);
	texture->loadDDS("media/textures/rockwall.dds");
}

void shutdown()
{
	delete mesh;
	delete shader;
	delete texture;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (keys['w'])
		cam.move(1);
	if (keys['s'])
		cam.move(-1);
	if (keys['a'])
		cam.rotate(1);
	if (keys['d'])
		cam.rotate(-1);

	texture->bind();

	shader->bind();
	shader->setUniformMatrix("mvp", mvp * cam.toMatrix());
	shader->setTexture("diffuse", 0);
	{
		mesh->draw();
	}
	shader->unbind();

	texture->unbind();

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

	cam.rotate(-delta.x / 2);
	cam.rotateUp(-delta.y / 2);

	mousePos = newMousePos;
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
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMotion);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

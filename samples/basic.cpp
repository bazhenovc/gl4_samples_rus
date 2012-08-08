
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "mathlib/Matrix4.hpp"

using namespace framework;

#ifdef _DEBUG
#	define RESOURCE_PATH		"../../../"
#else
#	define RESOURCE_PATH		"../"
#endif


// globals
Mesh* mesh = 0;
Shader* shader = 0;
Matrix4 mvp;

void init()
{
	mvp = Matrix4::perspective(60.0f, 800.0f / 600.0f, 0.00001f, 3000.0f);
	mvp *= Matrix4::translate(0, 0, -10);
	mesh = new Mesh;
	mesh->fromFile( RESOURCE_PATH"media/torus.e2m" );
	shader = new Shader;
	shader->loadShaders( RESOURCE_PATH"data/shaders/tess_quad_disp.prg" );
}

void shutdown()
{
	delete mesh;
	delete shader;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvp *= Matrix4::rotate(1, 1, 1, 1);

	shader->bind();
	shader->setUniformMatrix("mvp", mvp);
	{
		mesh->bind();
		mesh->draw();
		mesh->unbind();
	}
	shader->unbind();

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

	init();

	glutMainLoop();

	return 0;
}

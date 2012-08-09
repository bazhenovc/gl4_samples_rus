#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "mathlib/Matrix4.hpp"

using namespace framework;

class Mode;
class View;
class Program;

Mode *		currentMode		= NULL;
View *		currentView		= NULL;
Mesh *		gridMesh		= NULL;
Mesh *		fullScreenQuad	= NULL;
Texture *	diffuseMap		= NULL,
		*	heightMap		= NULL;
Program *	program			= NULL;
int			scrWidth		= 800,
			scrHeight		= 600;
int			gridSize		= 100;
int			modeIndex		= 0;
int			viewIndex		= 0;

#include "program.h"
#include "modes.h"

Mode *		allModes[] = {	new Part1(), new Part2(), new Part3(),
							new Part4(), new Part5(), new Part6() };


void init()
{
	setResourceDirectory( "media" );

	gridMesh		= new Mesh();

	fullScreenQuad	= new Mesh();
	fullScreenQuad->makeQuad();


	diffuseMap		= new Texture( GL_TEXTURE_2D );
	heightMap		= new Texture( GL_TEXTURE_2D );

	diffuseMap->bind();
	diffuseMap->loadDDS( "textures/rockwall.dds" );
	diffuseMap->unbind();

	heightMap->bind();
	heightMap->loadDDS( "textures/height_map.dds" );
	heightMap->unbind();

	program			= new Program();

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();
}

void shutdown()
{
	for (size_t i = 0; i < count_of(allModes); ++i)
	{
		delete allModes[i];
		allModes[i] = NULL;
	}

	delete gridMesh;
	delete fullScreenQuad;
	delete diffuseMap;
	delete heightMap;
	delete program;
	delete currentView;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentView->bind();
	currentMode->draw( modeIndex );
	currentView->unbind();

	currentView->draw( viewIndex );

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	scrWidth	= w;
	scrHeight	= h;
	currentMode->load();
	currentView->init();
}

void keyDown(unsigned char key, int, int)
{
	if ( key == 27 )	exit(0);

	// + -
	if ( key == '+' )	program->getStates().maxTessLevel++;	
	if ( key == '-' )	program->getStates().maxTessLevel--;

	// < >
	if ( key == ',' )	program->getStates().detailLevel += 50.f;
	if ( key == '.' )	program->getStates().detailLevel -= 50.f;

	// [ ]
	if ( key == '[' )	program->getStates().heightScale += 10.f;
	if ( key == ']' )	program->getStates().heightScale -= 10.f;

	// ( )
	if ( key == '9' && modeIndex > 0 )	modeIndex--;
	if ( key == '0' && modeIndex < 6 )	modeIndex++;

	// 1..8
	if ( key >= '1' && key <= '8' )		modeIndex = key - '1';

	if ( key == 'r' )	currentMode->load();			// reload

	if ( key == 'c' )	viewIndex = VIEW_COLOR;		// view color map
	if ( key == 'n' )	viewIndex = VIEW_NORMAL;	// view normal map
	if ( key == 't' )	viewIndex = VIEW_TESS;		// view tess level map
}

void specialKeyDown(int key, int, int)
{
	// F1...F6
	if ( key >= GLUT_KEY_F1 && key < GLUT_KEY_F1 + count_of(allModes) )
	{
		currentMode->unload();
		currentMode = allModes[ key - GLUT_KEY_F1 ];
		currentMode->load();
		modeIndex = 0;
	}
}

int main(int argc, char** argv)
{
	atexit(shutdown);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(scrWidth, scrHeight);
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("Sample1");
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyDown);
	glutSpecialUpFunc(specialKeyDown);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

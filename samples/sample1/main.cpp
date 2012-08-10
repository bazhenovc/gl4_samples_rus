#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"

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
FPSCamera	cam;
glm::vec2	mousePos;
bool		wireframe		= false;

bool keys[512];

#include "program.h"
#include "modes.h"

int			viewIndex  = VIEW_COLOR;
Mode *		allModes[] = {	new Part1(), new Part2(), new Part3(),
							new Part4(), new Part5(), new Part6() };

// temp
Mesh* mesh = 0;
Shader* shader = 0;


void init()
{
	setResourceDirectory( "media" );
	
	memset(keys, 0, 512 * sizeof(bool));
	
	// -34.8, -7.9, -42.4
	cam.init( 60.0f, 800.0f / 600.0f, 0.1f, 3000.0f, glm::vec3(-35.f, -8.f, -42.f) );

	gridMesh		= new Mesh();

	fullScreenQuad	= new Mesh();
	fullScreenQuad->makeQuad();


	diffuseMap		= new Texture( GL_TEXTURE_2D );
	heightMap		= new Texture( GL_TEXTURE_2D );

	diffuseMap->bind();
	diffuseMap->loadDDS( "textures/rockwall.dds" );
	diffuseMap->unbind();

	heightMap->bind();
	heightMap->loadDDS( "textures/height-map.dds" );
	heightMap->unbind();

	program			= new Program();

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();

	// temp:
	mesh = new Mesh;
	mesh->fromFile( "torus.e2m" );

	shader = new Shader;
	shader->loadShaders("shaders/basic.prg");
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

	// temp:
	delete mesh;
	delete shader;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// + -
	if ( keys['+'] )	program->getStates().maxTessLevel++;
	if ( keys['-'] )		program->getStates().maxTessLevel--;

	// < >
	if ( keys[','] )	program->getStates().detailLevel += 50.f;
	if ( keys['.'] )	program->getStates().detailLevel -= 50.f;

	// [ ]
	if ( keys['['] )	program->getStates().heightScale += 10.f;
	if ( keys[']'] )	program->getStates().heightScale -= 10.f;

	// ( )
	if ( keys['9'] && modeIndex > 0 )	modeIndex--;
	if ( keys['0'] && modeIndex < 6 )	modeIndex++;

	if ( keys['r'] )	currentMode->load();			// reload

	if ( keys['c'] )	viewIndex = VIEW_COLOR;		// view color map
	if ( keys['n'] )	viewIndex = VIEW_NORMAL;	// view normal map
	if ( keys['t'] )	viewIndex = VIEW_TESS;		// view tess level map

	if ( keys['p'] )	wireframe = !wireframe;
	
	cam.move( (keys['w'] - keys['s']) * 0.1f, (keys['d'] - keys['a']) * 0.1f, 0.f );

	program->getStates().mvp = cam.toMatrix();

	currentView->bind();
	glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );

	currentMode->draw( modeIndex );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
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
	
	if (key < 512)
		keys[key] = true;

	// 1..8
	if ( key >= '1' && key <= '8' )		modeIndex = key - '1';
}

void keyUp(unsigned char key, int, int)
{
	if (key < 512)
		keys[key] = false;
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(scrWidth, scrHeight);
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("Sample1");
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialUpFunc(specialKeyDown);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseDown);
	glutMotionFunc(mouseMotion);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

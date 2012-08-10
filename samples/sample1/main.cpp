#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Input.h"

using namespace framework;

class Mode;
class View;
class Program;

Mode *		currentMode		= NULL;
View *		currentView		= NULL;
Mesh *		gridMesh		= NULL;
Mesh *		fullScreenQuad	= NULL;
Texture *	diffuseMap		= NULL,
		*	heightMap		= NULL,
		*	normalMap		= NULL;
Program *	program			= NULL;
Input		input;
int			scrWidth		= 800,
			scrHeight		= 600;
int			gridSize		= 100;
int			modeIndex		= 0;
FPSCamera	cam;
bool		wireframe		= false;

#include "program.h"
#include "modes.h"

int			viewIndex  = VIEW_COLOR;
Mode *		allModes[] = {	new Part1(), new Part2(), new Part3(),
							new Part4(), new Part5(), new Part6() };


void init()
{
	setResourceDirectory( "media" );
	
	cam.init( 60.0f, 800.0f / 600.0f, 0.1f, 3000.0f, glm::vec3(-270.f, -50.f, -400.f) );

	gridMesh		= new Mesh();

	fullScreenQuad	= new Mesh();
	fullScreenQuad->makeQuad();


	diffuseMap		= new Texture( GL_TEXTURE_2D );
	heightMap		= new Texture( GL_TEXTURE_2D );
	normalMap		= new Texture( GL_TEXTURE_2D );

	diffuseMap->loadDDS( "textures/rockwall.dds" );
	heightMap->loadDDS(  "textures/rockwall_height.dds" );
	normalMap->loadDDS(  "textures/rockwall_normal.dds" );

	program			= new Program();

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();

	glClearColor( 0.f, 0.8f, 1.0f, 1.f );
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

void reload(int i)
{
	currentMode->unload();
	currentMode = allModes[ i ];
	currentMode->load();
	modeIndex = 0;

	static char	buf[512];
	sprintf( buf, "Sample1, part%i", i+1 );
	glutSetWindowTitle( buf );
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( input.isKey(27) )		exit(0);

	// + -
	if ( input.isKey('=') )		program->getStates().maxTessLevel += 0.05f;
	if ( input.isKey('-') )		program->getStates().maxTessLevel -= 0.05f;

	// < >
	if ( input.isKey(',') )		program->getStates().detailLevel -= 2.f;
	if ( input.isKey('.') )		program->getStates().detailLevel += 2.f;

	// [ ]
	if ( input.isKey('[') )		program->getStates().heightScale += 0.1f;
	if ( input.isKey(']') )		program->getStates().heightScale -= 0.1f;

	// ( )
	if ( input.isKeyClick('9') && modeIndex > 0 )	modeIndex--;
	if ( input.isKeyClick('0') && modeIndex < 6 )	modeIndex++;

	if ( input.isKeyClick('r') )	{ currentMode->unload();  currentMode->load(); }			// reload

	if ( input.isKeyClick('c') )	viewIndex = VIEW_COLOR;		// view color map
	if ( input.isKeyClick('n') )	viewIndex = VIEW_NORMAL;	// view normal map
	if ( input.isKeyClick('t') )	viewIndex = VIEW_TESS;		// view tess level map

	if ( input.isKeyClick('p') )	wireframe = !wireframe;
	
	// 1..4
	if ( input.isKey('1') )			modeIndex = 0;
	if ( input.isKey('2') )			modeIndex = 1;
	if ( input.isKey('3') )			modeIndex = 2;
	if ( input.isKey('4') )			modeIndex = 3;

	// F1..F6
	if ( input.isKeyClick(GLUT_KEY_F1) )	reload( 0 );
	if ( input.isKeyClick(GLUT_KEY_F2) )	reload( 1 );
	if ( input.isKeyClick(GLUT_KEY_F3) )	reload( 2 );
	if ( input.isKeyClick(GLUT_KEY_F4) )	reload( 3 );
	if ( input.isKeyClick(GLUT_KEY_F5) )	reload( 4 );
	if ( input.isKeyClick(GLUT_KEY_F6) )	reload( 5 );
	
	cam.rotate( input.mouseDelta() * 0.0001f );
	input.resetMouseDelta();

	cam.move(	(input.isKey('w') - input.isKey('s')) * 0.25f,
				(input.isKey('d') - input.isKey('a')) * 0.25f,
				(input.isKey('q') - input.isKey('e')) * 0.25f );

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

int main(int argc, char** argv)
{
	atexit(shutdown);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(scrWidth, scrHeight);
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("Sample1, part1");
	glutIdleFunc(display);
	glutReshapeFunc(reshape);

	input.init();

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);

	init();

	glutMainLoop();

	return 0;
}

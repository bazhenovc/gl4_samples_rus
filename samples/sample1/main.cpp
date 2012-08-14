#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Input.h"
#include "Query.h"

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
Query *		primitivesQuery	= NULL;
Input		input;
int			scrWidth		= 800,
			scrHeight		= 600;
int			gridSize		= 127;
int			modeIndex		= 0;
int			currPart		= 0;
int			frameCounter	= 0;
int			lastTime		= 0;
FPSCamera	cam;
bool		wireframe		= false;
bool		updateQuery		= true;

#include "program.h"
#include "modes.h"

int			viewIndex  = VIEW_COLOR;
Mode *		allModes[] = {	new Part1(), new Part2(), new Part3(),
							new Part4(), new Part5(), new Part6() };


void init()
{
	setResourceDirectory( "media" );
	
	cam.init( 60.0f, 800.0f / 600.0f, 0.1f, 3000.0f, glm::vec3(-1590.f, -135.f, -1830.f) );

	gridMesh		= new Mesh();

	fullScreenQuad	= new Mesh();
	fullScreenQuad->makeQuad();


	diffuseMap		= new Texture( GL_TEXTURE_2D );
	heightMap		= new Texture( GL_TEXTURE_2D );
	normalMap		= new Texture( GL_TEXTURE_2D );

	diffuseMap->loadDDS( "textures/grass.dds" );
	heightMap->loadDDS(  "textures/height.dds" );
	normalMap->loadDDS(  "textures/normal.dds" );

	diffuseMap->bind();
	glTexParameteri(diffuseMap->getType(), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(diffuseMap->getType(), GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(diffuseMap->getType(), GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	diffuseMap->unbind();

	heightMap->bind();
	glTexParameteri(heightMap->getType(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(heightMap->getType(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	heightMap->unbind();

	normalMap->bind();
	glTexParameteri(normalMap->getType(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(normalMap->getType(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	normalMap->unbind();

	program			= new Program();

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();

	primitivesQuery	= new Query();
	//primitivesQuery->begin( GL_PRIMITIVES_GENERATED );
	//primitivesQuery->end();

	glEnable( GL_DEPTH_CLAMP );
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_FRONT );
	glClearColor( 0.f, 0.8f, 1.0f, 1.f );
	glSwapInterval( 0 );

	lastTime = glutGet( GLUT_ELAPSED_TIME );
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
	delete primitivesQuery;
}

void loadMode(int i)
{
	currentMode->unload();
	currentMode = allModes[ i ];
	currentMode->load();
	currPart = i;
	modeIndex = 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( input.isKey(27) )		exit(0);

	// + -
	if ( input.isKey('=') )		program->getStates().maxTessLevel += 0.025f;
	if ( input.isKey('-') )		program->getStates().maxTessLevel -= 0.025f;

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
	if ( input.isKeyClick('m') )	viewIndex = VIEW_COLOR_MIX_TESS;

	if ( input.isKeyClick('p') )	wireframe = !wireframe;
	
	// 1..4
	if ( input.isKey('1') )			modeIndex = 0;
	if ( input.isKey('2') )			modeIndex = 1;
	if ( input.isKey('3') )			modeIndex = 2;
	if ( input.isKey('4') )			modeIndex = 3;

	// F1..F6
	if ( input.isSpecKeyClick(GLUT_KEY_F1) )	loadMode( 0 );
	if ( input.isSpecKeyClick(GLUT_KEY_F2) )	loadMode( 1 );
	if ( input.isSpecKeyClick(GLUT_KEY_F3) )	loadMode( 2 );
	if ( input.isSpecKeyClick(GLUT_KEY_F4) )	loadMode( 3 );
	if ( input.isSpecKeyClick(GLUT_KEY_F5) )	loadMode( 4 );
	if ( input.isSpecKeyClick(GLUT_KEY_F6) )	loadMode( 5 );
	
	cam.rotate( input.mouseDelta() * 0.1f );
	input.resetMouseDelta();

	const int	new_time	= glutGet( GLUT_ELAPSED_TIME );
	const float	time_delta	= float( new_time - lastTime ) * 0.025f;
	lastTime = new_time;

	cam.move(	(input.isKey('w') - input.isKey('s')) * time_delta,
				(input.isKey('d') - input.isKey('a')) * time_delta,
				(input.isSpecKey(0x70) - input.isKey(' ')) * time_delta );

	program->getStates().mvp = cam.toMatrix();

	currentView->bind();
	glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL );
	glEnable( GL_CULL_FACE );

	if ( updateQuery ) primitivesQuery->begin( GL_PRIMITIVES_GENERATED );
	currentMode->draw( modeIndex );
	if ( updateQuery ) { primitivesQuery->end();  updateQuery = false; }
	
	glDisable( GL_CULL_FACE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	currentView->unbind();

	currentView->draw( viewIndex );

	glutSwapBuffers();
	++frameCounter;
}

void reshape(int w, int h)
{
	scrWidth	= w;
	scrHeight	= h;
	currentMode->load();
	currentView->init();
}

void timerFunc(int id)
{
	unsigned long long	res = 0;

	if ( primitivesQuery->isResultReady() ) {
		res = primitivesQuery->getResult();
		updateQuery = true;
	}

	static char	buf[512];
	sprintf( buf, "Sample1, part%i  Fps:%i, vertices: %i / %i", currPart+1, frameCounter,
			 gridMesh->getIndexBuffer()->getSize(), res );
	glutSetWindowTitle( buf );
	frameCounter = 0;
	glutTimerFunc( 1000, timerFunc, id );
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
	glutTimerFunc( 1000, timerFunc, 1 );

	input.init();

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutMainLoop();

	return 0;
}

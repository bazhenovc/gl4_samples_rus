#include "System.h"

using namespace framework;

class Mode;
class View;
class Program;

Mode *		currentMode		= NULL;
Mesh *		fullScreenQuad	= NULL;
MultiMesh *	scene			= NULL;
Program *	program			= NULL;
System		sys;
Input &		input			= *sys.getInput();
int			modeIndex		= 0;
int			currPart		= 0;
FPSCamera	cam;
bool		wireframe		= false;
float		camSpeed		= 0.025f;

#include "program.h"
#include "modes.h"

Mode *		allModes[] = { new Part0() };


void init()
{
	sys.setCurrentDirectory( "samples/blending/media" );
	
	scene = new MultiMesh();
	scene->load( "meshes/SMUT.3ds", "textures/" );

	fullScreenQuad	= new Mesh();
	fullScreenQuad->makeQuad();

	program			= new Program();

	currentMode		= allModes[0];
	
	currentMode->load();


	glEnable( GL_DEPTH_CLAMP );
	glEnable( GL_DEPTH_TEST );
	//glCullFace( GL_FRONT );
	glDisable( GL_CULL_FACE );

	sys.swapInterval( 0 );
	cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 0.1f, 500.0f );

	glClearColor( 0.f, 0.3f, 1.f, 1.f );
	glClearDepthf( 1.f );
}

void shutdown()
{
	for (size_t i = 0; i < count_of(allModes); ++i)
	{
		delete allModes[i];
		allModes[i] = NULL;
	}

	delete fullScreenQuad;
	delete scene;
}

void loadMode(int i)
{
	if ( i >= count_of(allModes) )
		return;

	currentMode->unload();
	currentMode = allModes[ i ];
	currentMode->load();
	currPart = i;
	//modeIndex = 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( input.isKey(27) )			exit(0);

	// + -
	if ( input.isKeyClick('=') )	program->getStates().maxTessLevel++;
	if ( input.isKeyClick('-') )		program->getStates().maxTessLevel--;

	// < >
	if ( input.isKeyClick(',') )	program->getStates().detailLevel--;
	if ( input.isKeyClick('.') )	program->getStates().detailLevel++;

	// up, down
	if ( input.isSpecKeyClick(GLUT_KEY_UP) )	camSpeed += 0.01f;
	if ( input.isSpecKeyClick(GLUT_KEY_DOWN) )	camSpeed -= 0.01f;

	// ( )
	if ( input.isKeyClick('9') && modeIndex > 0 )	modeIndex--;
	if ( input.isKeyClick('0') && modeIndex < 7 )	modeIndex++;

	if ( input.isKeyClick('r') )	{ currentMode->unload();  currentMode->load(); }			// reload
	if ( input.isKeyClick('p') )	wireframe = !wireframe;
	
	// 1..4
	if ( input.isKey('1') )			modeIndex = 0;
	if ( input.isKey('2') )			modeIndex = 1;
	if ( input.isKey('3') )			modeIndex = 2;
	if ( input.isKey('4') )			modeIndex = 3;
	if ( input.isKey('5') )			modeIndex = 4;
	if ( input.isKey('6') )			modeIndex = 5;
	if ( input.isKey('7') )			modeIndex = 6;
	if ( input.isKey('8') )			modeIndex = 7;

	// F1..F7
	if ( input.isSpecKeyClick(GLUT_KEY_F1) )	loadMode( 0 );
	if ( input.isSpecKeyClick(GLUT_KEY_F2) )	loadMode( 1 );
	if ( input.isSpecKeyClick(GLUT_KEY_F3) )	loadMode( 2 );
	if ( input.isSpecKeyClick(GLUT_KEY_F4) )	loadMode( 3 );
	if ( input.isSpecKeyClick(GLUT_KEY_F5) )	loadMode( 4 );
	if ( input.isSpecKeyClick(GLUT_KEY_F6) )	loadMode( 5 );
	if ( input.isSpecKeyClick(GLUT_KEY_F7) )	loadMode( 6 );
	
	cam.rotate( input.mouseDelta() * 0.2f );

	const float	time_delta	= sys.getTimeDelta() * camSpeed;
	
	cam.move(	(input.isKey('w') - input.isKey('s')) * time_delta,
				(input.isKey('d') - input.isKey('a')) * time_delta,
				(input.isKey('q') - input.isKey('e')) * time_delta );

	program->getStates().mvp		 = cam.toMatrixScale( 10.f );

	currentMode->draw( modeIndex );
}


void reshape(int w, int h)
{
	//scrWidth	= w;
	//scrHeight	= h;
	currentMode->load();
}


void timerFunc()
{
	static char	buf[512];
	sprintf( buf, "Blending, part%i  Fps:%i", currPart+1, sys.getFPS() );
	glutSetWindowTitle( buf );
}


int main(int argc, char** argv)
{
	sys.setOnTimer( timerFunc );
	sys.initGLUT( argc, argv, display, init, "Blending", 1024, 768 );
	shutdown();
	return 0;
}

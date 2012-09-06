#include "System.h"

using namespace framework;

class Mode;
class View;
class Program;

Mode *		currentMode		= NULL;
View *		currentView		= NULL;
Program *	program			= NULL;
Query *		primitivesQuery	= NULL;
System		sys;
Input &		input			= *sys.getInput();
int			modeIndex		= 0;
int			currPart		= -1;
FPSCamera	cam;
bool		wireframe		= false;
bool		updateQuery		= true;

#include "program.h"
#include "modes.h"

int			viewIndex  = VIEW_COLOR;
Mode *		allModes[] = {	new Part1(), new Part2(), new Part3() };


void init()
{
	sys.setCurrentDirectory( "samples/sample2/media" );
	
	program			= new Program();

	cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y), 1.f, 3000.0f );

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();
	currPart		= 0;

	primitivesQuery	= new Query();

	glEnable( GL_DEPTH_CLAMP );
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_FRONT );
	glClearColor( 0.f, 0.8f, 1.0f, 1.f );
	sys.swapInterval( 0 );
}

void shutdown()
{
	for (size_t i = 0; i < count_of(allModes); ++i)
	{
		delete allModes[i];
		allModes[i] = NULL;
	}

	delete program;
	delete currentView;
	delete primitivesQuery;
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
	if ( input.isKey('5') )			modeIndex = 4;
	if ( input.isKey('6') )			modeIndex = 5;

	// F1..F6
	if ( input.isSpecKeyClick(GLUT_KEY_F1) )	loadMode( 0 );
	if ( input.isSpecKeyClick(GLUT_KEY_F2) )	loadMode( 1 );
	if ( input.isSpecKeyClick(GLUT_KEY_F3) )	loadMode( 2 );
	if ( input.isSpecKeyClick(GLUT_KEY_F4) )	loadMode( 3 );
	if ( input.isSpecKeyClick(GLUT_KEY_F5) )	loadMode( 4 );
	if ( input.isSpecKeyClick(GLUT_KEY_F6) )	loadMode( 5 );
	
	cam.rotate( input.mouseDelta() * 0.2f );

	const float	time_delta	= sys.getTimeDelta() * 0.025f;

	cam.move(	(input.isKey('w') - input.isKey('s')) * time_delta,
				(input.isKey('d') - input.isKey('a')) * time_delta,
				(input.isKey('q') - input.isKey('e')) * time_delta );

	program->getStates().mvp  = cam.toMatrix();

	currentView->bind();
	
	if ( updateQuery ) primitivesQuery->begin( GL_PRIMITIVES_GENERATED );
	currentMode->draw( modeIndex );
	if ( updateQuery ) { primitivesQuery->end();  updateQuery = false; }
	
	currentView->unbind();

	currentView->draw( viewIndex );
}

void reshape(int w, int h)
{
	//scrWidth	= w;
	//scrHeight	= h;
	currentMode->unload();
	currentMode->load();
	currentView->init();
}


void timerFunc()
{
	static unsigned int		vertices = 0;
	static char				buf[512];

	if ( primitivesQuery->isResultReady() ) {
		vertices = primitivesQuery->getResult();
		updateQuery = true;
	}

	sprintf( buf, "Sample2, part%i, mode:%i  Fps:%i, vertices: %i", currPart+1, modeIndex+1, sys.getFPS(), vertices );
	glutSetWindowTitle( buf );
}


int main(int argc, char** argv)
{
	sys.setOnTimer( timerFunc );
	sys.initGLUT( argc, argv, display, init, "Sample2, part1", 1024, 768 );
	shutdown();
	return 0;
}

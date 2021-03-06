#include "System.h"

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
System		sys;
Input &		input			= *sys.getInput();
int			gridSize		= 127;
int			modeIndex		= 0;
int			currPart		= 0;
FPSCamera	cam;
bool		wireframe		= false;
bool		updateQuery		= true;
float		camSpeed		= 0.025f;

#include "program.h"
#include "modes.h"

int			viewIndex  = VIEW_COLOR;
Mode *		allModes[] = {	new Part1(), new Part2(), new Part3(),
							new Part4(), new Part5(), new Part6(), new Part7() };


void init()
{
	sys.setCurrentDirectory( "media" );

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
	diffuseMap->setWrap( GL_REPEAT, GL_REPEAT );
	diffuseMap->setAnisotropy( 16 );
	diffuseMap->unbind();

	heightMap->bind();
	heightMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	heightMap->unbind();

	normalMap->bind();
	normalMap->setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	normalMap->unbind();

	program			= new Program();

	currentMode		= allModes[0];
	currentView		= new View();

	currentView->init();
	currentMode->load();

	primitivesQuery	= new Query();

	glEnable( GL_DEPTH_CLAMP );
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_FRONT );
	sys.swapInterval( 0 );
	
	cam.init( 60.0f, float(sys.getWndSize().x) / float(sys.getWndSize().y),
			  1.f, 3000.0f,
			  glm::vec3(	-program->getStates().gridScale * 0.5f,
							 program->getStates().heightScale * 0.1f,
							-program->getStates().gridScale * 0.5f )
			 );
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

	// [ ]
	if ( input.isKeyClick('[') )	program->getStates().heightScale++;
	if ( input.isKeyClick(']') )	program->getStates().heightScale--;

	// left, right
	if ( input.isSpecKeyClick(GLUT_KEY_LEFT) )	program->getStates().gridScale -= 10.f;
	if ( input.isSpecKeyClick(GLUT_KEY_RIGHT))	program->getStates().gridScale += 10.f;

	// up, down
	if ( input.isSpecKeyClick(GLUT_KEY_UP) )	camSpeed += 0.01f;
	if ( input.isSpecKeyClick(GLUT_KEY_DOWN) )	camSpeed -= 0.01f;

	// ( )
	if ( input.isKeyClick('9') && modeIndex > 0 )	modeIndex--;
	if ( input.isKeyClick('0') && modeIndex < 6 )	modeIndex++;

	if ( input.isKeyClick('r') )	{ currentMode->unload();  currentMode->load(); }			// reload

	if ( input.isKeyClick('c') )	viewIndex = VIEW_COLOR;		// view color map
	if ( input.isKeyClick('n') )	viewIndex = VIEW_NORMAL;	// view normal map
	if ( input.isKeyClick('t') )	viewIndex = VIEW_TESS;		// view tess level map
	if ( input.isKeyClick('m') )	viewIndex = VIEW_COLOR_MIX_TESS;
	if ( input.isKeyClick('f') )	viewIndex = VIEW_FOG;

	if ( input.isKeyClick('p') )	wireframe = !wireframe;
	
	// 1..4
	if ( input.isKey('1') )			modeIndex = 0;
	if ( input.isKey('2') )			modeIndex = 1;
	if ( input.isKey('3') )			modeIndex = 2;
	if ( input.isKey('4') )			modeIndex = 3;

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

	program->getStates().mvp	 = cam.toMatrix();

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
}

void reshape(int w, int h)
{
	//scrWidth	= w;
	//scrHeight	= h;
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

	sprintf( buf, "Sample1, part%i, mode%i,  Fps:%i, vertices: %i / %i", currPart+1, modeIndex+1, sys.getFPS(),
			 gridMesh->getIndexBuffer()->getSize(), vertices );
	glutSetWindowTitle( buf );
}

int main(int argc, char** argv)
{
	sys.setOnTimer( timerFunc );
	sys.initGLUT( argc, argv, display, init, "Sample1, part1", 1024, 768 );
	shutdown();
	return 0;
}

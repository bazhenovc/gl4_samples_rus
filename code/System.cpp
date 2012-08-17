#include "System.h"

#if defined( _WIN32 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( WIN64 )
#	define PLATFORM_WINDOWS
#	include <Windows.h>
#	include <GL/wglew.h>
#else
#	include <gl/glxew.h>
#endif

#include <string>

#ifdef PLATFORM_WINDOWS
#	define glSwapInterval	wglSwapIntervalEXT
#else
#	define glSwapInterval	glXSwapIntervalEXT
#endif

namespace framework
{
	Input				System::_input;
	int					System::_lastTime		= 0;
	int					System::_frameCounter	= 0;
	int					System::_fpsTimer		= 0;
	int					System::_lastFPS		= 0;
	float				System::_timeDelta		= 0.f;
	System::idleFunc_t	System::_idleFunc		= NULL;
	System::idleFunc_t	System::_onTimerFunc	= NULL;
	glm::ivec2			System::_windowSize		= glm::ivec2( 800, 600 );


	void System::initGLUT(int argc, char** argv, void (* idleFunc)( void ), void (* initFunc)( void ), const char *title, int w, int h)
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
		glutInitWindowSize(w, h);
		glutInitContextVersion(4, 2);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

		glutCreateWindow(title);
		glutIdleFunc(_idle);
		//glutReshapeFunc(reshape);

		_input.init();

		glewExperimental = GL_TRUE;
		glewInit();

		initFunc();

		_idleFunc	= idleFunc;
		_windowSize	= glm::ivec2(w,h);

		_lastTime = glutGet( GLUT_ELAPSED_TIME );
		glutMainLoop();
	}
	
	void System::setOnTimer( void (* onTimer)(void) )
	{
		_onTimerFunc = onTimer;
	}

	void System::_idle()
	{
		// update time
		const int	last = _lastTime;
		_lastTime = glutGet( GLUT_ELAPSED_TIME );
		const int	delta = _lastTime - last;
		_timeDelta = float(delta);

		_idleFunc();

		glutSwapBuffers();
		
		_input.resetMouseDelta();
		++_frameCounter;
		_fpsTimer += delta;

		if ( _fpsTimer >= 1000 )
		{
			_lastFPS		= int( float(_frameCounter) * 1000.f / float(_fpsTimer) + 0.5f );
			_fpsTimer		= 0;
			_frameCounter	= 0;

			if ( _onTimerFunc )
				_onTimerFunc();
		}
	}

	bool System::setCurrentDirectory(const char *dirName, int maxSearchDepth)
	{
	#ifdef PLATFORM_WINDOWS
		std::string	path = dirName;
		for (int i = 0; i < maxSearchDepth; ++i)
		{
			int i_code = ::GetFileAttributes( path.c_str() );
			if ( (i_code != -1) && (FILE_ATTRIBUTE_DIRECTORY & i_code) )
			{
				::SetCurrentDirectory( path.c_str() );
				return true;
			}
			path = "../" + path;
		}
	#else
	#	error TODO: set current directory...
	#endif
		return false;
	}

	void System::swapInterval(int i)
	{
		glSwapInterval( i );
	}
}
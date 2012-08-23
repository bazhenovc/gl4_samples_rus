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

#define logPrint printf


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
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
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

		_idleFunc	= idleFunc;
		_windowSize	= glm::ivec2(w,h);

		initFunc();

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
		logPrint( "can't set current directory from: %s\n", dirName );
		return false;
	}

	void System::swapInterval(int i)
	{
		glSwapInterval( i );
	}
	
	void System::clearGLErrors()
	{
		while ( glGetError() != GL_NO_ERROR ) {}
	}

	bool System::checkGLError()
	{
		GLenum	err = glGetError();

		if ( err == GL_NO_ERROR )
			return true;

		static const char *	errors[] = { "GL_INVALID_ENUM",
										 "GL_INVALID_VALUE",
										 "GL_INVALID_OPERATION",
										 "GL_OUT_OF_MEMORY",
										 "GL_INVALID_FRAMEBUFFER_OPERATION" };

		switch ( err )
		{
			case GL_INVALID_ENUM:					err = 0;	break;
			case GL_INVALID_VALUE:					err = 1;	break;
			case GL_INVALID_OPERATION:				err = 2;	break;
			case GL_OUT_OF_MEMORY:					err = 3;	break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:	err = 4;	break;
			default:	logPrint("GL error: %i", err );			return false;
		}

		logPrint("GL error: %s\n", errors[err] );
		return false;
	}
}
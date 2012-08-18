#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Input.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Query.h"

namespace framework
{

class System
{
private:
	typedef void (* idleFunc_t)( void );

	static Input		_input;
	static int			_lastTime;
	static int			_frameCounter;
	static int			_lastFPS;
	static int			_fpsTimer;
	static glm::ivec2	_windowSize;
	static float		_timeDelta;
	static idleFunc_t	_idleFunc;
	static idleFunc_t	_onTimerFunc;


	static void _idle(void);

public:
	static Input *				getInput()			{ return &_input; }
	static int					getLastTime()		{ return _lastTime; }
	static float				getTimeDelta()		{ return _timeDelta; }
	static int					getFPS()			{ return _lastFPS; }
	static glm::ivec2 const &	getWndSize()		{ return _windowSize; }

	static void initGLUT(int argc, char** argv, void (* idleFunc)( void ), void (* initFunc)( void ),
						 const char *title, int w, int h);

	static void setOnTimer( void (* onTimer)(void) );
	
	static bool setCurrentDirectory(const char *dirName, int maxSearchDepth = 4);

	static void swapInterval(int i);

	//static void clearGLErrors();
	//static bool checkGLError();
};

}

#endif	// _SYSTEM_H_
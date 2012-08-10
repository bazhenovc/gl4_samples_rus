#ifndef _INPUT_SYSTEM_H_
#define _INPUT_SYSTEM_H_

#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace framework
{

template <typename T, size_t I>
inline size_t count_of(const T (&)[I]) {
	return I;
}

class Input
{
private:
	enum EState
	{
		NOT_PRESSED	= 0x0,
		UP			= 0x1,
		DOWN		= 0x3,
		PRESSED		= 0x4,
	};

	static unsigned char	_keys[256];
	static glm::vec2		_mouseDelta;
	static glm::vec2		_mousePos;

public:
	static void _keyDown(unsigned char key, int, int)
	{
		if (key < count_of(_keys))
			_keys[key] = DOWN;
	}

	static void _keyUp(unsigned char key, int, int)
	{
		if ( key < count_of(_keys) )
			_keys[key] = UP;
	}

	static void _specialDown(int key, int, int)
	{
		if (key < count_of(_keys))
			_keys[key] = DOWN;
	}

	static void _specialUp(int key, int, int)
	{
		if ( key < count_of(_keys) )
			_keys[key] = UP;
	}

	static void _mouseDown(int button, int state, int x, int y)
	{
		_mousePos = glm::vec2( (float)x, (float)y );
	}

	static void _mouseMotion(int x, int y)
	{
		glm::vec2 newMousePos = glm::vec2( (float)x, (float)y );

		_mouseDelta = newMousePos - _mousePos;
		_mousePos	= newMousePos;
	}

	static bool isKey(unsigned char key) {
		return _keys[ key ] > UP ;
	}

	static bool isKeyClick(unsigned char key) {
		return ( _keys[key] == DOWN && (_keys[key] = PRESSED) );
	}

	static glm::vec2 const & mousePos() {
		return _mousePos;
	}

	static glm::vec2 const & mouseDelta() {
		return _mouseDelta;
	}

	static void resetMouseDelta() {
		_mouseDelta = glm::vec2(0.f,0.f);
	}

	static void init() {
		glutSpecialFunc( _specialDown );
		glutSpecialUpFunc( _specialUp );
		glutKeyboardFunc( _keyDown );
		glutKeyboardUpFunc( _keyUp );
		glutMouseFunc( _mouseDown );
		glutMotionFunc( _mouseMotion );
	}
};
}
#endif	// _INPUT_SYSTEM_H_
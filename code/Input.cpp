#include "Input.h"

namespace framework
{
	unsigned char	Input::_keys[256]		= {0};
	unsigned char	Input::_specKeys[128]	= {0};
	glm::vec2		Input::_mouseDelta		= glm::vec2(0.f,0.f);
	glm::vec2		Input::_mousePos		= glm::vec2(0.f,0.f);

	

	void Input::init()
	{
		glutSpecialFunc( _specialDown );
		glutSpecialUpFunc( _specialUp );
		glutKeyboardFunc( _keyDown );
		glutKeyboardUpFunc( _keyUp );
		glutMouseFunc( _mouseDown );
		glutMotionFunc( _mouseMotion );
	}
}
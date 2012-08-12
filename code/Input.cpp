#include "Input.h"

namespace framework
{
	unsigned char	Input::_keys[256]		= {0};
	unsigned char	Input::_specKeys[128]	= {0};
	glm::vec2		Input::_mouseDelta		= glm::vec2(0.f,0.f);
	glm::vec2		Input::_mousePos		= glm::vec2(0.f,0.f);
}
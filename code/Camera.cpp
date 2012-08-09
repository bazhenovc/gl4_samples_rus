
#include "Camera.h"
#include "mathlib/Math.hpp"

namespace framework
{

void Camera::move(float factor)
{
	glm::vec3 dir = glm::normalize(target - pos);
	pos += dir * factor;
	target += dir * factor;
}

void Camera::rotate(float factor)
{
	target = glm::rotateY(target, factor);
}

void Camera::rotateUp(float factor)
{
	target = glm::rotateX(target, factor);
}

}

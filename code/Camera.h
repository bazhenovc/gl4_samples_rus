#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace framework
{

class Camera
{
public:

	// to build a lookAt matrix
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 up;

	Camera()
		: pos(0, 0, 0), target(0, 0, -1), up(0, 1, 0)
	{}
	~Camera() {}

	inline glm::mat4 toMatrix() const
	{
		return glm::lookAt(pos, target, up);
	}

	// factor > 0 - go forward, < 0 - backward
	void move(float factor);
	// factor > 0 - rotate right, < 0 - left
	void rotate(float factor);
	// factor > 0 - rotate up, < 0 - down
	void rotateUp(float factor);
};

}

#endif // CAMERA_HH

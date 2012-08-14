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



class FPSCamera
{
private:
	glm::quat	_orientation;
	glm::vec3	_position;
	glm::mat4	_proj,
				_view;

public:
	FPSCamera() {}

	inline void init(float fovY, float aspect, float nearPlane, float farPlane, const glm::vec3 &pos = glm::vec3(0.f)) {
		_proj = glm::perspective( fovY, aspect, nearPlane, farPlane );
		_position = pos;
	}

	inline glm::mat4 toMatrix() const {
		return _proj * _view * glm::translate( _position );
	}

	inline glm::mat4 toMVMatrix() const {
		return _view * glm::translate( _position );
	}

	inline glm::mat3 toNormalMatrix() const {
		return glm::transpose( glm::inverse( glm::mat3( _view * glm::translate( _position ) ) ) );
	}

	inline glm::vec3 const & position() const {
		return _position;
	}

	inline void setPosition(const glm::vec3 &pos) {
		_position = pos;
	}

	inline glm::vec3 getDirection() const {
		const float	x = _orientation.x;
		const float y = _orientation.y;
		const float z = _orientation.z;
		const float w = _orientation.w;
		return glm::vec3( 2.0f * x * z + 2.0f * y * w,
						  2.0f * z * y - 2.0f * x * w,
						  1.0f - 2.0f * x * x - 2.0f * y * y );
	}

	inline void rotateRad(float x, float y) {
		bool		changed = false;
		glm::quat &	q = _orientation;

		if ( y != 0.f ) {
			q = glm::quat( cos(y*0.5f), sin(y*0.5f), 0.f, 0.f ) * q;
			changed = true;
		}

		if ( x != 0.f ) {
			q = q * glm::quat( cos(x*0.5f), 0.f, sin(x*0.5f), 0.f );
			changed = true;
		}

		if ( changed ) {
			q = glm::normalize( q );
			_view = glm::mat4_cast( _orientation );
		}
	}

	inline void rotate(const glm::vec2 &v) {
		rotate( v.x, v.y );
	}

	inline void rotate(float x, float y) {
		rotateRad( glm::radians(x), glm::radians(y) );
	}

	inline void move(float x, float y, float z) {
		_position += glm::vec3( _view[0][0], _view[1][0], _view[2][0] ) * -y;
		_position += glm::vec3( 0.f, 1.f, 0.f ) * z;
		_position += glm::vec3( _view[0][2], _view[1][2], _view[2][2] ) * x;
	}
};

}

#endif // CAMERA_HH

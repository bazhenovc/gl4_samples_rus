#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>

#include <mathlib/Vector2.hpp>
#include <mathlib/Vector3.hpp>
#include <mathlib/Vector4.hpp>
#include <mathlib/Matrix3.hpp>
#include <mathlib/Matrix4.hpp>

namespace framework
{
/**
 OpenGL shader implementation
 */
class Shader
{
private:
	/// Linked program
	GLuint _prog;

public:
	/**
	 Constructor
	 \brief
	 Does not initialize the shader
	 */
	Shader(void);

	/**
	 Destructor
	 \brief
	 Releases shader
	 */
	virtual ~Shader(void);

	/**
	 Bind shader
	 */
	void bind(void);

	/**
	 Unbind shader
	 */
	void unbind(void);

	/**
	 Set attribute location
	 */
	void setAttributeLocation(GLuint id, const char* name);

	/**
	 Set fragment data location
	 */
	void setFragmentDataLocation(GLuint id, const char* name);

	/** Get uniform location
	 * @par name Name
	 * @return location */
	GLuint getUniformLocation(const char* name);

	/** Get attrib location
	 * @par name Name
	 * @return location */
	GLuint getAttrLocation(const char* name);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(const char* name, const Vector2& value);

	/// Uniform variables handling method.
	/// @param loc Var location
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(int loc, const Vector2& value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(const char* name, const Vector3& value);

	/// Uniform variables handling method.
	/// @param loc Var location
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(int loc, const Vector3& value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(const char* name, const Vector4& value);

	/// Uniform variables handling method.
	/// @param loc Var location
	/// @param value Var
	/// @return Assignment result
	bool setUniformVector(int loc, const Vector4& value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformFloat(const char* name, const float value);

	/// Uniform variables handling method.
	/// @param loc Var location
	/// @param value Var
	/// @return Assignment result
	bool setUniformFloat(int loc, const float value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformInt(const char* name, const int value);

	/// Uniform variables handling method.
	/// @param loc Var location
	/// @param value Var
	/// @return Assignment result
	bool setUniformInt(int loc, const int value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformMatrix(const char* name, const Matrix3& value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformMatrix(const char* name, const Matrix4& value);

	/// Uniform variables handling method.
	/// @param name Var name
	/// @param value Var
	/// @return Assignment result
	bool setUniformMatrix(const char* name, const float value[16]);

	/// Get named vector
	/// @param name Uniform name
	/// @return Vector for name.
	Vector4 getUniformVector(const char* name);

	/// Get vector for location
	/// @param loc Vector location
	/// @return Vector
	Vector4 getUniformVector(int loc);

	/// Get location for uniform name
	/// @param name Uniform name
	/// @return Uniform loc
	int getLoc(const char* name);

	/// Set texture
	/// @param name Texture name
	/// @param texUnit Texture unit
	/// @return Assignment result
	bool setTexture(const char* name, GLuint texUnit);

	/// Set texture
	/// @param loc Texture location
	/// @param texUnit Texture unit
	/// @return Assignment result
	bool setTexture(int loc, GLuint texUnit);

	// Attach shader from file
	bool attachShaderFromFile(const char *fileName, GLenum shaderType);

	// Attach shader source
	bool attachShaderSrc(const char *src, GLenum shaderType);

	// Link program
	bool link();

	// Load shaders from single file
	bool loadShaders(const char *fileName);
};
}

#endif

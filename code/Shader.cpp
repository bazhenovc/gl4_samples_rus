#include "Shader.h"
#include <GL/glew.h>

#include <stdio.h>
#include <malloc.h>

#define logPrint printf

namespace
{

static char* filetobuf(const char *file)
{
	FILE *fptr;
	long length;
	char *buf;
	size_t unused = 0;

	fptr = fopen(file, "r");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*) malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	unused = fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

}

namespace framework
{

Shader::Shader(void)
	: _vert(0), _frag(0), _geom(0), _prog(0)
{ }

Shader::~Shader()
{
	if (_prog) {
		if (_vert) {
			glDetachShader(_prog, _vert);
			glDeleteShader(_vert);
		}
		if (_frag) {
			glDetachShader(_prog, _frag);
			glDeleteShader(_frag);
		}
		if (_geom) {
			glDetachShader(_prog, _geom);
			glDeleteShader(_geom);
		}
		glDeleteProgram(_prog);
	}
}

void Shader::bind()
{
	glUseProgram(_prog);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::setAttributeLocation(GLuint id, const char* name)
{
	glBindAttribLocation(_prog, id, name);
}

void Shader::setFragmentDataLocation(GLuint id, const char* name)
{
	glBindFragDataLocation(_prog, id, name);
}

GLuint Shader::getUniformLocation(const char* name)
{
	return glGetUniformLocation(_prog, name);
}

GLuint Shader::getAttrLocation(const char* name)
{
	return glGetAttribLocation(_prog, name);
}

bool Shader::setUniformVector(const char* name, const Vector2& value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform2fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformVector(int loc, const Vector2& value)
{
	if (loc < 0) {
		return false;
	}
	glUniform2fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformVector(const char* name, const Vector3& value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform3fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformVector(int loc, const Vector3& value)
{
	if (loc < 0) {
		return false;
	}
	glUniform3fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformVector(const char* name, const Vector4& value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform4fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformVector(int loc, const Vector4& value)
{
	if (loc < 0) {
		return false;
	}
	glUniform4fv(loc, 1, value.ptr());
	return true;
}

bool Shader::setUniformFloat(const char* name, const float value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform1f(loc, value);
	return true;
}

bool Shader::setUniformFloat(int loc, const float value)
{
	if (loc < 0) {
		return false;
	}
	glUniform1f(loc, value);
	return true;
}

bool Shader::setUniformInt(const char* name, const int value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform1i(loc, value);
	return true;
}

bool Shader::setUniformInt(int loc, const int value)
{
	if (loc < 0) {
		return false;
	}
	glUniform1i(loc, value);
	return true;
}

bool Shader::setUniformMatrix(const char* name, const Matrix3& value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniformMatrix3fv(loc, 1, GL_FALSE, value.ptr());
	return true;
}

bool Shader::setUniformMatrix(const char* name, const Matrix4& value)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value.ptr());
	return true;
}

bool Shader::setUniformMatrix(const char* name, const float value[16])
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value);
	return true;
}

Vector4 Shader::getUniformVector(const char* name)
{
	float v[4] = {0};
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return Vector4(v);
	}
	glGetUniformfv(_prog, loc, v);
	return Vector4(v);
}

Vector4 Shader::getUniformVector(int loc)
{
	float v[4] = {0};
	if (loc < 0) {
		return Vector4(v);
	}
	glGetUniformfv(_prog, loc, v);
	return Vector4(v);
}

int Shader::getLoc(const char* name)
{
	return glGetUniformLocation(_prog, name);
}

bool Shader::setTexture(const char* name, GLuint texUnit)
{
	int loc = glGetUniformLocation(_prog, name);
	if (loc < 0) {
		return false;
	}
	glUniform1i(loc, texUnit);
	return false;
}

bool Shader::setTexture(int loc, GLuint texUnit)
{
	if (loc < 0) {
		return false;
	}
	glUniform1i(loc, texUnit);
	return true;
}

bool Shader::fromFile(const char* vrt, const char* frg, const char* geo)
{
	char* vsrc = filetobuf(vrt);
	char* fsrc = filetobuf(frg);
	char* gsrc = filetobuf(geo);
	if (!vsrc && !fsrc && !gsrc) {
		logPrint("Error: none of given files exist: %s %s %s\n", vrt, frg, geo);
		return false;
	}
	_prog = glCreateProgram();
	if (!_prog) {
		logPrint("Error: failed to create program");
		return false;
	}
	if (vsrc) {
		_vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(_vert, 1, (const GLchar**) & vsrc, 0);
		glCompileShader(_vert);
		free(vsrc);
		glAttachShader(_prog, _vert);
		logPrint("GLSL compiled: %s\n", vrt);
	}
	if (fsrc) {
		_frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(_frag, 1, (const GLchar**) & fsrc, 0);
		glCompileShader(_frag);
		free(fsrc);
		glAttachShader(_prog, _frag);
		logPrint("GLSL compiled: %s\n", frg);
	}
	if (gsrc) {
		_geom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(_geom, 1, (const GLchar**) & gsrc, 0);
		glCompileShader(_geom);
		free(gsrc);
		glAttachShader(_prog, _geom);
		logPrint("GLSL compiled: %s\n", geo);
	}
	glLinkProgram(_prog);
	// Load program log
	GLint logLen;
	GLsizei chars;
	GLchar* infoLog;
	glGetProgramiv(_prog, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 1) {
		infoLog = (GLchar*) malloc(logLen * sizeof ( GLchar));
		glGetProgramInfoLog(_prog, logLen, &chars, infoLog);
		logPrint("GLSL compilation error: %s\n", infoLog);
		free(infoLog);
		return false;
	}
	//logPrint("GLSL compiled\n");
	//Log::getSingleton().printLn("GLSL compiled");
	return true;
}
}


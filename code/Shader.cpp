#include "Shader.h"
#include <GL/glew.h>

#include <stdio.h>
#include <malloc.h>
#include <string>

#define logPrint printf
#define COUNTOF( x )	(sizeof(x)/sizeof(x[0]))

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
	: _prog(0)
{ }

Shader::~Shader()
{
	if (_prog) {
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
	
bool Shader::attachShaderFromFile(const char *fileName, GLenum shaderType)
{
	char* src = filetobuf( fileName );
	if ( !src ) {
		// TODO: error to log
		return false;
	}
	if ( !attachShaderSrc( src, shaderType ) ) {
		free(src);
		return false;
	}
	free(src);
	return true;
}

bool Shader::attachShaderSrc(const char *src, GLenum shaderType)
{
	if ( !_prog ) {
		_prog = glCreateProgram();
		if (!_prog) {
			logPrint("Error: failed to create program");
			return false;
		}
	}
	GLuint	shader = glCreateShader( shaderType );
	glShaderSource( shader, 1, (const GLchar **) &src, 0 );
	glCompileShader( shader );
	glAttachShader( _prog, shader );
	glDeleteShader( shader );
	return true;
}

bool Shader::link()
{
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
	return true;
}
	
bool Shader::loadShaders(const char *fileName)
{
	struct SConfigShaderType {
		const char *	name;
		GLenum			type;
	};
	static const SConfigShaderType	shaderTypes[] = {	{ "--vertex",		GL_VERTEX_SHADER },
														{ "--tesscontrol",	GL_TESS_CONTROL_SHADER },
														{ "--tesseval",		GL_TESS_EVALUATION_SHADER },
														{ "--geometry",		GL_GEOMETRY_SHADER },
														{ "--fragment",		GL_FRAGMENT_SHADER },
														{ "--eof", 0 } };
	char *	data = filetobuf( fileName );
	std::string	src = data;
	free(data);

	size_t	offsets[ COUNTOF(shaderTypes) ] = {0};

	for (int i = 0; i < COUNTOF(shaderTypes); ++i) {
		offsets[i] = src.find( shaderTypes[i].name, 0 );
		if ( offsets[i] != size_t(-1) ) {
			src[ offsets[i] ] = '\0';
			src[ (offsets[i] += strlen(shaderTypes[i].name)) ] = '\0';
		}
	}

	for (int i = 0; i < COUNTOF(shaderTypes); ++i) {
		if ( offsets[i] != size_t(-1) && shaderTypes[i].type != 0 )
			attachShaderSrc( src.c_str() + offsets[i]+1, shaderTypes[i].type );
	}

	return link();
}

}


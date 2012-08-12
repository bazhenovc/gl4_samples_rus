#include "Query.h"

namespace framework
{

Query::Query(): _id(0), _target(0)
{
	glGenQueries( 1, &_id );
}

Query::~Query()
{
	glDeleteQueries( 1, &_id );
}

void Query::begin(GLenum target)
{
	glBeginQuery( _target = target, _id );
}

void Query::end()
{
	glEndQuery( _target );
}

bool Query::isResultReady()
{
	GLint	ready = 0;
	glGetQueryObjectiv( _id, GL_QUERY_RESULT_AVAILABLE, &ready );
	return ready == GL_TRUE;
}

unsigned int Query::getResult()
{
	GLuint	result = 0;
	glGetQueryObjectuiv( _id, GL_QUERY_RESULT, &result );
	return result;
}

unsigned long long Query::getResult64()
{
	GLuint64	result = 0;
	glGetQueryObjectui64v( _id, GL_QUERY_RESULT, &result );
	return result;
}

}
#ifndef _QUERY_H_
#define _QUERY_H_

#include <GL/glew.h>

namespace framework
{
class Query
{
private:
	GLuint	_id;
	GLenum	_target;

public:
	Query();
	~Query();

	void begin(GLenum target);
	void end();

	bool isResultReady();
	unsigned int  getResult();
	unsigned long long getResult64();
};
}

#endif	// _QUERY_H_
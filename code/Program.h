#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "Shader.h"

namespace framework
{
/*
	Helper for program loading, and uniform setting
*/
class Program
{
public:
	struct States
	{
		Matrix4		mvp;
		float		gridScale;
		float		maxTessLevel;
		float		heightScale;
		float		detailLevel;

		States(): gridScale(100.f), maxTessLevel(12.f), heightScale(10.f), detailLevel(500.f) {}
	};

private:
	States		_states;

public:
	States &		getStates();
	States const &	getStates()	const;

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader) const;

	bool load(Shader *&shader, const char *fileName) const;

	void bind(Shader *shader) const;
};
}

#endif	// _PROGRAM_H_

#include "Program.h"

namespace framework
{
	
Program::States & Program::getStates()
{
	return _states;
}

Program::States const & Program::getStates() const
{
	return _states;
}

void Program::setConstUniforms(Shader *shader) const
{
	shader->setTexture( shader->getLoc("unDiffuseMap"), 0 );
	shader->setTexture( shader->getLoc("unHeightMap"), 1 );
	shader->setTexture( shader->getLoc("unNormalMap"), 2 );
}

void Program::setUniforms(Shader *shader) const
{
	shader->setUniformMatrix( "unMVPMatrix",					_states.mvp );
	shader->setUniformFloat( shader->getLoc("unGridScale"),		_states.gridScale );
	shader->setUniformFloat( shader->getLoc("unMaxTessLevel"),	_states.maxTessLevel );
	shader->setUniformFloat( shader->getLoc("unHeightScale"),	_states.heightScale );
	shader->setUniformFloat( shader->getLoc("unDetailLevel"),	_states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName) const
{
	if ( !shader )
		shader = new Shader();

	if ( !shader->loadShaders( fileName ) )
		return false;

	setConstUniforms( shader );
	return true;
}

void Program::bind(Shader *shader) const
{
	glPatchParameteri( GL_PATCH_VERTICES, shader->getPatchSize() );
	shader->bind();
	setUniforms( shader );
}

}
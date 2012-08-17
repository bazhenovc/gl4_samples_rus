
enum ETextureType
{
	TEX_DIFFUSE	= 0,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_DEPTH,
	TEX_TESS_LEVEL,
};


/*
	Helper for program loading, and uniform setting
*/
class Program
{
public:
	struct States
	{
		glm::mat4	mvp;
		glm::mat4	invProj;
		glm::mat3	norm;
		float		gridScale;
		float		maxTessLevel;
		float		heightScale;
		float		detailLevel;

		States(): gridScale(10000.f), maxTessLevel(12.f), heightScale(-1000.f), detailLevel(20.f) {}
	};

private:
	States		_states;

public:
	States &		getStates()			{ return _states; }
	States const &	getStates()	const	{ return _states; }

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader);

	bool load(Shader *&shader, const char *fileName) const;

	void bind(Shader *shader);
};





void Program::setConstUniforms(Shader *shader) const
{
	shader->setTexture( shader->getLoc("unDiffuseMap"),		TEX_DIFFUSE );
	shader->setTexture( shader->getLoc("unHeightMap"),		TEX_HEIGHT );
	shader->setTexture( shader->getLoc("unNormalMap"),		TEX_NORMAL );
	shader->setTexture( shader->getLoc("unDepthMap"),		TEX_DEPTH );
	shader->setTexture( shader->getLoc("unTessLevelMap"),	TEX_TESS_LEVEL );
}

void Program::setUniforms(Shader *shader)
{
	if ( _states.maxTessLevel < 1.f )	_states.maxTessLevel = 1.f;
	if ( _states.detailLevel < 0.f )	_states.detailLevel = 0.f;

	shader->setUniformMatrix( "unMVPMatrix",						 _states.mvp );
	shader->setUniformMatrix( "unProjInvMatrix",					 _states.invProj );
	shader->setUniformMatrix( "unNormalMatrix",					 _states.norm );
	shader->setUniformFloat( shader->getLoc("unGridScale"),		 _states.gridScale );
	shader->setUniformFloat( shader->getLoc("unMaxTessLevel"),	 _states.maxTessLevel );
	shader->setUniformFloat( shader->getLoc("unHeightScale"),	-_states.heightScale );
	shader->setUniformFloat( shader->getLoc("unDetailLevel"),	 _states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName) const
{
	if ( !shader )
		shader = new Shader();

	if ( !shader->loadShaders( fileName ) )
		return false;

	return true;
}

void Program::bind(Shader *shader)
{
	glPatchParameteri( GL_PATCH_VERTICES, shader->getPatchSize() );
	shader->bind();
	setConstUniforms( shader );
	setUniforms( shader );
}


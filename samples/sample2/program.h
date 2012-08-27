
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
		float		detailLevel;
		float		maxTessLevel;

		States(): maxTessLevel(12.f), detailLevel(20.f) {}
	};

private:
	States		_states;

public:
	States &		getStates()			{ return _states; }
	States const &	getStates()	const	{ return _states; }

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader);

	bool load(Shader *&shader, const char *fileName, const char *preprocessor = NULL) const;

	void bind(Shader *shader);
};




void Program::setConstUniforms(Shader *shader) const
{
	shader->setTexture( "unDiffuseMap",		TEX_DIFFUSE );
	shader->setTexture( "unHeightMap",		TEX_HEIGHT );
	shader->setTexture( "unNormalMap",		TEX_NORMAL );
	shader->setTexture( "unDepthMap",		TEX_DEPTH );
}

void Program::setUniforms(Shader *shader)
{
	if ( _states.maxTessLevel < 1.f )	_states.maxTessLevel = 1.f;

	shader->setUniformMatrix( "unMVPMatrix",		 _states.mvp );
	shader->setUniformFloat( "unMaxTessLevel",	 _states.maxTessLevel );
	shader->setUniformFloat( "unDetailLevel",	 _states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName, const char *preprocessor) const
{
	if ( !shader )
		shader = new Shader();

	if ( !shader->loadShaders( fileName, preprocessor ) )
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



enum ETextureType
{
	TEX_DIFFUSE	= 0,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_DEPTH,
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
		float		maxTessLevel;
		float		detailLevel;

		States(): maxTessLevel(12.f), detailLevel(20.f) {}
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
	shader->setTexture( "unDiffuseMap",		TEX_DIFFUSE );
	shader->setTexture( "unHeightMap",		TEX_HEIGHT );
	shader->setTexture( "unNormalMap",		TEX_NORMAL );
	shader->setTexture( "unDepthMap",		TEX_DEPTH );
}

void Program::setUniforms(Shader *shader)
{
	if ( _states.maxTessLevel < 1.f )	_states.maxTessLevel = 1.f;
	if ( _states.detailLevel < 0.f )	_states.detailLevel = 0.f;

	shader->setUniformMatrix( "unMVPMatrix",		 _states.mvp );
	shader->setUniformVector( "unViewport",		glm::vec2( sys.getWndSize() ) );
	shader->setUniformFloat( "unMaxTessLevel",	 _states.maxTessLevel );
	shader->setUniformFloat( "unDetailLevel",	 _states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName) const
{
	if ( !shader )
		shader = new Shader();

	return shader->loadShaders( fileName );
}

void Program::bind(Shader *shader)
{
	glPatchParameteri( GL_PATCH_VERTICES, shader->getPatchSize() );
	shader->bind();
	setConstUniforms( shader );
	setUniforms( shader );
}


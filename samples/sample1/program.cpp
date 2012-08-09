#if defined( _WIN32 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( WIN64 )
#	define PLATFORM_WINDOWS
#	include <Windows.h>
#endif

#include <string>

/*
	Helper for program loading, and uniform setting
*/
class Program
{
public:
	struct States
	{
		glm::mat4		mvp;
		float		gridScale;
		float		maxTessLevel;
		float		heightScale;
		float		detailLevel;

		States(): gridScale(100.f), maxTessLevel(12.f), heightScale(10.f), detailLevel(500.f) {}
	};

private:
	States		_states;

public:
	States &		getStates()			{ return _states; }
	States const &	getStates()	const	{ return _states; }

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader) const;

	bool load(Shader *&shader, const char *fileName) const;

	void bind(Shader *shader) const;
};




/*
	System function: set resource directory
*/
bool setResourceDirectory(const char *dirName, int maxSearchDepth = 4)
{
#ifdef PLATFORM_WINDOWS
	std::string	path = dirName;
	for (int i = 0; i < maxSearchDepth; ++i)
	{
		int i_code = ::GetFileAttributes( path.c_str() );
		if ( (i_code != -1) && (FILE_ATTRIBUTE_DIRECTORY & i_code) )
		{
			::SetCurrentDirectory( path.c_str() );
			return true;
		}
		path = "../" + path;
	}
#endif
	return false;
}


template <typename T, size_t I>
inline size_t count_of(const T (&)[I]) {
	return I;
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

